const mongoose = require('mongoose');
require('dotenv').config();

const MONGODB_URI = process.env.MONGODB_URI || 'mongodb://localhost:27017/asasu_db';

const ClaimSchema = new mongoose.Schema({
    id: String, agentName: String, agentId: String, email: String, clientCount: Number,
    notes: String, fileName: String, fileSize: String, date: String, timestamp: Number,
    status: { type: String, default: 'Pending' }, fileData: String
});

const TicketSchema = new mongoose.Schema({
    id: String, type: String, agentId: String, description: String, date: String,
    timestamp: Number, status: { type: String, default: 'Open' }
});

const Claim = mongoose.model('Claim', ClaimSchema);
const Ticket = mongoose.model('Ticket', TicketSchema);

async function seed() {
    console.log('🌱 Seeding database...');
    await mongoose.connect(MONGODB_URI);

    // Clear existing
    await Claim.deleteMany({});
    await Ticket.deleteMany({});

    const claims = [
        {
            id: 'ASU-123456',
            agentName: 'John Doe',
            agentId: 'AGT-001',
            email: 'john@example.com',
            clientCount: 15,
            fileName: 'commissions_feb.xlsx',
            fileSize: '45 KB',
            date: new Date().toLocaleString(),
            timestamp: Date.now(),
            status: 'Pending',
            fileData: 'https://res.cloudinary.com/demo/image/upload/v123456/sample.jpg'
        },
        {
            id: 'ASU-789012',
            agentName: 'Jane Smith',
            agentId: 'AGT-002',
            email: 'jane@example.com',
            clientCount: 22,
            fileName: 'asasu_sales_v2.csv',
            fileSize: '12 KB',
            date: new Date().toLocaleString(),
            timestamp: Date.now() - 86400000,
            status: 'Paid',
            fileData: 'https://res.cloudinary.com/demo/image/upload/v123456/sample.jpg'
        }
    ];

    const tickets = [
        {
            id: 'TKT-999001',
            type: 'Payment',
            agentId: 'AGT-001',
            description: 'My commission for ASU-123456 is still pending.',
            date: new Date().toLocaleString(),
            timestamp: Date.now(),
            status: 'Open'
        }
    ];

    await Claim.insertMany(claims);
    await Ticket.insertMany(tickets);

    console.log('✅ Database seeded with 2 claims and 1 ticket!');
    process.exit();
}

seed().catch(err => {
    console.error(err);
    process.exit(1);
});
