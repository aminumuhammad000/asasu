const axios = require('axios');

const API_URL = 'http://localhost:5000/api';

async function test() {
    console.log('🚀 Starting Backend Tests...');

    try {
        // 1. Create a Claim (Adding file simulation)
        console.log('\n--- Test 1: Create Claim ---');
        const claimData = {
            id: 'ASU-TEST01',
            agentName: 'Test Agent',
            agentId: 'AGT001',
            email: 'test@example.com',
            clientCount: 10,
            fileName: 'test.xlsx',
            fileData: 'https://res.cloudinary.com/demo/image/upload/sample.jpg',
            date: new Date().toLocaleString(),
            timestamp: Date.now(),
            status: 'Pending'
        };
        const createRes = await axios.post(`${API_URL}/claims`, claimData);
        const claimId = createRes.data._id;
        console.log('✅ Claim Created:', claimId);

        // 2. Get All Claims
        console.log('\n--- Test 2: Get Claims ---');
        const getRes = await axios.get(`${API_URL}/claims`);
        console.log('✅ Claims Found:', getRes.data.length);

        // 3. Update Status to Rejected (Rejecting)
        console.log('\n--- Test 3: Reject Claim ---');
        await axios.patch(`${API_URL}/claims/${claimId}`, { status: 'Rejected' });
        const updatedRes = await axios.get(`${API_URL}/claims`);
        const updatedClaim = updatedRes.data.find(c => c._id === claimId);
        console.log('✅ Claim Status:', updatedClaim.status);

        // 4. Delete Claim
        console.log('\n--- Test 4: Delete Claim ---');
        await axios.delete(`${API_URL}/claims/${claimId}`);
        const deleteRes = await axios.get(`${API_URL}/claims`);
        const found = deleteRes.data.find(c => c._id === claimId);
        if (!found) console.log('✅ Claim Deleted Successfully');

        console.log('\n✨ All backend tests passed!');
    } catch (err) {
        console.error('❌ Test Failed:', err.message);
        if (err.response) console.error('Response:', err.response.data);
    }
}

test();
