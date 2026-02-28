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
    description: String,
    date: String,
    timestamp: Number,
    status: { type: String, default: 'Open' },
    responses: [Object] // Array of response objects if needed in future
});

const Claim = mongoose.model('Claim', ClaimSchema);
const Ticket = mongoose.model('Ticket', TicketSchema);

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
        const claims = await Claim.find().sort({ timestamp: -1 });
        res.json(claims);
    } catch (err) {
        res.status(500).json({ error: err.message });
    }
});

app.post('/api/claims', async (req, res) => {
    try {
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
        const tickets = await Ticket.find().sort({ timestamp: -1 });
        res.json(tickets);
    } catch (err) {
        res.status(500).json({ error: err.message });
    }
});

app.post('/api/tickets', async (req, res) => {
    try {
        const ticket = new Ticket(req.body);
        await ticket.save();
        res.status(201).json(ticket);
    } catch (err) {
        res.status(500).json({ error: err.message });
    }
});

app.patch('/api/tickets/:id', async (req, res) => {
    try {
        const ticket = await Ticket.findByIdAndUpdate(req.params.id, req.body, { new: true });
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

// Serve Frontend (Optional fallback/API health check)
app.get('/', (req, res) => {
    res.send('🚀 ASASU Realty API is running!');
});

const PORT = process.env.PORT || 5000;
app.listen(PORT, () => console.log(`🚀 Server running on http://localhost:${PORT} & https://api.asasurealty-commission.com.ng`));
