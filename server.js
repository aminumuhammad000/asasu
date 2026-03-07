const express = require('express');
const mongoose = require('mongoose');
const path = require('path');
const nodemailer = require('nodemailer');
const cors = require('cors');
const multer = require('multer');
const cloudinary = require('cloudinary').v2;
require('dotenv').config();

// Cloudinary Config
cloudinary.config({
    cloud_name: process.env.CLOUDINARY_CLOUD_NAME,
    api_key: process.env.CLOUDINARY_API_KEY,
    api_secret: process.env.CLOUDINARY_API_SECRET
});

// Multer Storage (Memory)
const storage = multer.memoryStorage();
const upload = multer({
    storage,
    limits: { fileSize: 10 * 1024 * 1024 } // 10MB limit
});

const app = express();
app.use(cors());
app.use(express.json());

// MongoDB Connection
mongoose.connect(process.env.MONGODB_URI || 'mongodb://localhost:27017/asasu_db')
    .then(() => console.log('✅ MongoDB Connected'))
    .catch(err => console.error('❌ MongoDB Connection Error:', err));

// Schemas
const ClaimSchema = new mongoose.Schema({
    id: String,
    agentName: String,
    agentId: String,
    email: String,
    clientCount: Number,
    notes: String,
    fileName: String,
    fileSize: String,
    date: String,
    timestamp: Number,
    status: { type: String, default: 'Pending' },
    fileData: String // Cloudinary URL
});

const TicketSchema = new mongoose.Schema({
    id: String,
    type: String,
    agentId: String,
    agentEmail: String,
    description: String,
    date: String,
    timestamp: Number,
    status: { type: String, default: 'Open' },
    responses: [Object], // Array of response objects if needed in future
    adminResponse: String
});

const MessageSchema = new mongoose.Schema({
    id: String,
    agentId: String,
    text: String,
    date: String,
    timestamp: Number,
    sender: { type: String, default: 'Admin' }
});

const Claim = mongoose.model('Claim', ClaimSchema);
const Ticket = mongoose.model('Ticket', TicketSchema);
const Message = mongoose.model('Message', MessageSchema);

// Email Transporter
const transporter = nodemailer.createTransport({
    service: 'gmail',
    auth: {
        user: process.env.EMAIL_USER,
        pass: process.env.EMAIL_PASS
    }
});

// API Routes

// File Upload Endpoint
app.post('/api/upload', upload.single('file'), async (req, res) => {
    try {
        if (!req.file) {
            return res.status(400).json({ error: 'No file uploaded' });
        }

        // Validate File Type
        const fileName = req.file.originalname.toLowerCase();
        if (!fileName.endsWith('.xlsx') && !fileName.endsWith('.xls') && !fileName.endsWith('.csv')) {
            return res.status(400).json({ error: 'Only Excel or CSV files allowed' });
        }

        // Upload to Cloudinary as "raw"
        const result = await new Promise((resolve, reject) => {
            const uploadStream = cloudinary.uploader.upload_stream(
                { resource_type: 'raw', folder: 'asasu_claims' },
                (error, result) => {
                    if (error) reject(error);
                    else resolve(result);
                }
            );
            uploadStream.end(req.file.buffer);
        });

        res.json({ secure_url: result.secure_url });
    } catch (err) {
        console.error('Upload Error:', err);
        res.status(500).json({ error: err.message });
    }
});

app.get('/api/claims', async (req, res) => {
    try {
        const page = parseInt(req.query.page) || 1;
        const limit = parseInt(req.query.limit) || 10;
        const skip = (page - 1) * limit;

        const query = {};
        if (req.query.agentId) {
            query.agentId = req.query.agentId;
        }
        if (req.query.search) {
            const s = req.query.search;
            query.$or = [
                { id: { $regex: s, $options: 'i' } },
                { agentName: { $regex: s, $options: 'i' } },
                { agentId: { $regex: s, $options: 'i' } }
            ];
        }

        const total = await Claim.countDocuments(query);
        const claims = await Claim.find(query).sort({ timestamp: -1 }).skip(skip).limit(limit);

        res.json({
            data: claims,
            pagination: {
                total,
                page,
                limit,
                pages: Math.ceil(total / limit)
            }
        });
    } catch (err) {
        res.status(500).json({ error: err.message });
    }
});

app.post('/api/claims', async (req, res) => {
    try {
        if (!req.body.email) {
            return res.status(400).json({ error: 'Agent email is compulsory' });
        }
        const claim = new Claim(req.body);
        await claim.save();

        // Send Email Notification
        const mailOptions = {
            from: process.env.EMAIL_USER,
            to: process.env.ADMIN_EMAIL,
            subject: `New Commission Claim: ${claim.id}`,
            text: `Agent: ${claim.agentName} (${claim.agentId})\nClients: ${claim.clientCount}\nFile: ${claim.fileName}\nURL: ${claim.fileData}\nDate: ${claim.date}`
        };

        if (process.env.EMAIL_USER && process.env.EMAIL_PASS) {
            transporter.sendMail(mailOptions, (err, info) => {
                if (err) console.error('❌ Email Error:', err);
                else console.log('📧 Email Sent:', info.response);
            });
        }

        res.status(201).json(claim);
    } catch (err) {
        res.status(500).json({ error: err.message });
    }
});

app.patch('/api/claims/:id', async (req, res) => {
    try {
        const oldClaim = await Claim.findById(req.params.id);
        const claim = await Claim.findByIdAndUpdate(req.params.id, req.body, { new: true });

        if (!claim) {
            return res.status(404).json({ error: 'Claim not found' });
        }

        // Send Email Notification if status changed
        if (req.body.status && req.body.status !== oldClaim.status) {
            let statusMessage = '';
            let subject = `Claim Update: ${claim.id}`;

            switch (claim.status) {
                case 'Paid':
                    statusMessage = `Good news! Your commission claim (${claim.id}) has been processed and paid.`;
                    subject = `🎉 Commission Paid: ${claim.id}`;
                    break;
                case 'Rejected':
                    statusMessage = `Your commission claim (${claim.id}) has been rejected. Please contact support for more details.`;
                    subject = `❌ Claim Rejected: ${claim.id}`;
                    break;
                case 'Reviewed':
                    statusMessage = `Your commission claim (${claim.id}) has been reviewed and is currently being processed.`;
                    subject = `ℹ️ Claim Reviewed: ${claim.id}`;
                    break;
                default:
                    statusMessage = `The status of your commission claim (${claim.id}) has been updated to: ${claim.status}.`;
            }

            const mailOptions = {
                from: process.env.EMAIL_USER,
                to: claim.email,
                subject: subject,
                text: `Hello ${claim.agentName},\n\n${statusMessage}\n\nClaim Details:\nID: ${claim.id}\nDate: ${claim.date}\nStatus: ${claim.status}\n\nThank you,\nASASU Realty Team`
            };

            if (process.env.EMAIL_USER && process.env.EMAIL_PASS && claim.email) {
                transporter.sendMail(mailOptions, (err, info) => {
                    if (err) console.error('❌ Status Email Error:', err);
                    else console.log('📧 Status Notification Sent:', info.response);
                });
            }
        }

        res.json(claim);
    } catch (err) {
        console.error('Update Error:', err);
        res.status(500).json({ error: err.message });
    }
});

app.delete('/api/claims/:id', async (req, res) => {
    try {
        await Claim.findByIdAndDelete(req.params.id);
        res.json({ message: 'Claim deleted' });
    } catch (err) {
        res.status(500).json({ error: err.message });
    }
});

app.get('/api/tickets', async (req, res) => {
    try {
        const page = parseInt(req.query.page) || 1;
        const limit = parseInt(req.query.limit) || 10;
        const skip = (page - 1) * limit;

        const query = {};
        if (req.query.agentId) {
            query.agentId = req.query.agentId;
        }

        const total = await Ticket.countDocuments(query);
        const tickets = await Ticket.find(query).sort({ timestamp: -1 }).skip(skip).limit(limit);

        res.json({
            data: tickets,
            pagination: {
                total,
                page,
                limit,
                pages: Math.ceil(total / limit)
            }
        });
    } catch (err) {
        res.status(500).json({ error: err.message });
    }
});

app.post('/api/tickets', async (req, res) => {
    try {
        if (!req.body.agentEmail) {
            return res.status(400).json({ error: 'Agent email is compulsory' });
        }
        const ticket = new Ticket(req.body);
        await ticket.save();
        res.status(201).json(ticket);
    } catch (err) {
        res.status(500).json({ error: err.message });
    }
});

app.patch('/api/tickets/:id', async (req, res) => {
    try {
        const { status, adminResponse } = req.body;
        const updateData = {};
        if (status) updateData.status = status;
        if (adminResponse) updateData.adminResponse = adminResponse;

        const ticket = await Ticket.findByIdAndUpdate(req.params.id, updateData, { new: true });
        res.json(ticket);
    } catch (err) {
        res.status(500).json({ error: err.message });
    }
});

app.delete('/api/tickets/:id', async (req, res) => {
    try {
        await Ticket.findByIdAndDelete(req.params.id);
        res.json({ message: 'Ticket deleted' });
    } catch (err) {
        res.status(500).json({ error: err.message });
    }
});
// Admin Login
app.post('/api/admin/login', (req, res) => {
    const { password } = req.body;
    if (password === process.env.ADMIN_PASSWORD) {
        res.json({ success: true });
    } else {
        res.status(401).json({ success: false, error: 'Invalid password' });
    }
});

// Admin Custom Email
app.post('/api/admin/send-email', async (req, res) => {
    const { password, to, subject, message } = req.body;

    // Simple password check
    if (password !== process.env.ADMIN_PASSWORD) {
        return res.status(401).json({ error: 'Unauthorized access' });
    }

    if (!to || !subject || !message) {
        return res.status(400).json({ error: 'Recipient, subject, and message are required' });
    }

    const mailOptions = {
        from: `ASASU Realty Support <${process.env.EMAIL_USER}>`,
        to: to,
        subject: subject,
        text: message
    };

    try {
        if (process.env.EMAIL_USER && process.env.EMAIL_PASS) {
            await transporter.sendMail(mailOptions);
            console.log(`📧 Custom Email Sent to ${to}`);
            res.json({ success: true, message: 'Email sent successfully' });
        } else {
            res.status(500).json({ error: 'Email service not configured' });
        }
    } catch (err) {
        console.error('❌ Custom Email Error:', err);
        res.status(500).json({ error: err.message });
    }
});

// Admin Broadcast Email
app.post('/api/admin/broadcast-email', async (req, res) => {
    const { password, subject, message } = req.body;

    if (password !== process.env.ADMIN_PASSWORD) {
        return res.status(401).json({ error: 'Unauthorized access' });
    }

    try {
        // Collect all unique agent emails from claims
        const agents = await Claim.distinct('email');
        const recipients = agents.filter(email => email && email.includes('@'));

        if (recipients.length === 0) {
            return res.status(400).json({ error: 'No agent emails found' });
        }

        const mailOptions = {
            from: `ASASU Realty <${process.env.EMAIL_USER}>`,
            bcc: recipients.join(','),
            subject: subject,
            text: message
        };

        await transporter.sendMail(mailOptions);
        console.log(`📧 Broadcast Email Sent to ${recipients.length} agents`);
        res.json({ success: true, count: recipients.length });
    } catch (err) {
        console.error('❌ Broadcast Error:', err);
        res.status(500).json({ error: err.message });
    }
});

// Admin Stats
app.get('/api/admin/stats', async (req, res) => {
    try {
        const claims = await Claim.find();
        const tickets = await Ticket.find();

        const pending = claims.filter(c => c.status === 'Pending').length;
        const openT = tickets.filter(t => t.status === 'Open').length;

        const now = new Date();
        const currentMonth = now.getMonth();
        const currentYear = now.getFullYear();

        const monthlyCount = claims.filter(c => {
            if (!c.timestamp) return false;
            const d = new Date(c.timestamp);
            return d.getMonth() === currentMonth && d.getFullYear() === currentYear && c.status === 'Paid';
        }).length;

        const totalCommission = claims.filter(c => c.status !== 'Rejected' && c.status !== 'Paid')
            .reduce((acc, c) => acc + (Number(c.clientCount || 0) * 1000), 0);

        res.json({
            pendingCount: pending,
            openTicketsCount: openT,
            processedThisMonth: monthlyCount,
            totalCommission: totalCommission
        });
    } catch (err) {
        res.status(500).json({ error: err.message });
    }
});

// Admin Update Password
app.post('/api/admin/update-password', async (req, res) => {
    const { currentPassword, newPassword } = req.body;

    if (currentPassword !== process.env.ADMIN_PASSWORD) {
        return res.status(401).json({ error: 'Incorrect current password' });
    }

    if (!newPassword || newPassword.length < 5) {
        return res.status(400).json({ error: 'New password too short' });
    }

    // UPDATE ENV VARIABLE IN MEMORY (Note: This doesn't persist to .env file automatically in many node setups without extra logic)
    process.env.ADMIN_PASSWORD = newPassword;
    console.log('✅ Admin password updated');
    res.json({ success: true });
});

// Admin Send Direct Message
app.post('/api/admin/send-message', async (req, res) => {
    const { password, agentId, text } = req.body;

    if (password !== process.env.ADMIN_PASSWORD) {
        return res.status(401).json({ error: 'Unauthorized' });
    }

    try {
        const message = new Message({
            id: 'MSG-' + Date.now().toString().slice(-6),
            agentId,
            text,
            date: new Date().toLocaleString(),
            timestamp: Date.now()
        });
        await message.save();
        res.status(201).json(message);
    } catch (err) {
        res.status(500).json({ error: err.message });
    }
});

// Agent Get Messages
app.get('/api/messages/:agentId', async (req, res) => {
    try {
        const messages = await Message.find({ agentId: req.params.agentId }).sort({ timestamp: -1 });
        res.json(messages);
    } catch (err) {
        res.status(500).json({ error: err.message });
    }
});

// Serve Frontend (Optional fallback/API health check)
app.get('/', (req, res) => {
    res.send('🚀 ASASU Realty API is running!');
});

const PORT = process.env.PORT || 5000;
app.listen(PORT, () => console.log(`🚀 Server running on http://localhost:${PORT} & https://api.asasurealty-commission.com.ng`));
