<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ASASU Realty - Agent Commission Portal</title>
    <script src="https://cdn.tailwindcss.com"></script>
    <link href="https://fonts.googleapis.com/css2?family=Plus+Jakarta+Sans:wght@400;500;600;700;800&display=swap" rel="stylesheet">
    <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.4.0/css/all.min.css">
    <script src="https://cdnjs.cloudflare.com/ajax/libs/xlsx/0.18.5/xlsx.full.min.js"></script>
    <style>
        .glass-effect { background: rgba(255, 255, 255, 0.9); backdrop-filter: blur(10px); }
        .file-drop-zone {
            background-image: url("data:image/svg+xml,%3csvg width='100%25' height='100%25' xmlns='http://www.w3.org/2000/svg'%3e%3crect width='100%25' height='100%25' fill='none' rx='16' ry='16' stroke='%23CBD5E1FF' stroke-width='2' stroke-dasharray='12%2c 12' stroke-dashoffset='0' stroke-linecap='square'/%3e%3c/svg%3e");
            transition: all 0.3s ease;
        }
        .file-drop-zone.dragover {
            background-image: url("data:image/svg+xml,%3csvg width='100%25' height='100%25' xmlns='http://www.w3.org/2000/svg'%3e%3crect width='100%25' height='100%25' fill='none' rx='16' ry='16' stroke='%230EA5E9FF' stroke-width='3' stroke-dasharray='12%2c 12' stroke-dashoffset='0' stroke-linecap='square'/%3e%3c/svg%3e");
            background-color: #f0f9ff;
            transform: scale(1.02);
        }
        .success-checkmark { width: 80px; height: 80px; border-radius: 50%; display: block; stroke-width: 2; stroke: #fff; stroke-miterlimit: 10; margin: 0 auto; box-shadow: inset 0px 0px 0px #10b981; animation: fill .4s ease-in-out .4s forwards, scale .3s ease-in-out .9s both; }
        .checkmark__circle { stroke-dasharray: 166; stroke-dashoffset: 166; stroke-width: 2; stroke-miterlimit: 10; stroke: #10b981; fill: none; animation: stroke 0.6s cubic-bezier(0.65, 0, 0.45, 1) forwards; }
        .checkmark__check { transform-origin: 50% 50%; stroke-dasharray: 48; stroke-dashoffset: 48; animation: stroke 0.3s cubic-bezier(0.65, 0, 0.45, 1) 0.8s forwards; }
        @keyframes stroke { 100% { stroke-dashoffset: 0; } }
        @keyframes scale { 0%, 100% { transform: none; } 50% { transform: scale3d(1.1, 1.1, 1); } }
        @keyframes fill { 100% { box-shadow: inset 0px 0px 0px 50px #10b981; } }
        .loader { border: 3px solid #f3f3f3; border-radius: 50%; border-top: 3px solid #0ea5e9; width: 24px; height: 24px; animation: spin 1s linear infinite; }
        @keyframes spin { 0% { transform: rotate(0deg); } 100% { transform: rotate(360deg); } }
        .ticket-card { transition: all 0.3s ease; }
        .ticket-card:hover { transform: translateY(-2px); box-shadow: 0 10px 25px -5px rgba(0, 0, 0, 0.1); }
        .modal { display: none; position: fixed; top: 0; left: 0; width: 100%; height: 100%; background: rgba(0,0,0,0.5); z-index: 100; backdrop-filter: blur(4px); }
        .modal.active { display: flex; align-items: center; justify-content: center; }
        .animate-pulse-slow { animation: pulse 3s cubic-bezier(0.4, 0, 0.6, 1) infinite; }
    </style>
<base target="_blank">
</head>
<body class="font-sans text-slate-800 bg-slate-50 min-h-screen">

    <!-- Navigation -->
    <nav class="fixed w-full z-50 glass-effect border-b border-slate-200/50">
        <div class="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8">
            <div class="flex justify-between h-16 items-center">
                <div class="flex items-center gap-3">
                    <div class="w-10 h-10 bg-gradient-to-br from-blue-600 to-orange-500 rounded-xl flex items-center justify-center text-white font-bold text-xl shadow-lg">
                        A
                    </div>
                    <div>
                        <h1 class="text-xl font-bold text-slate-900 tracking-tight">ASASU Realty</h1>
                        <p class="text-xs text-slate-500 font-medium">Commission Portal</p>
                    </div>
                </div>
                <div class="flex items-center gap-4">
                    <button onclick="showTicketModal()" class="px-4 py-2 bg-orange-100 text-orange-700 rounded-lg hover:bg-orange-200 transition-colors font-medium flex items-center gap-2 text-sm">
                        <i class="fas fa-headset"></i>
                        <span class="hidden sm:inline">Support / Tickets</span>
                        <span id="ticketBadge" class="bg-orange-500 text-white text-xs rounded-full w-5 h-5 flex items-center justify-center hidden">0</span>
                    </button>
                    <button onclick="toggleView()" id="viewToggle" class="hidden px-4 py-2 rounded-lg bg-slate-100 hover:bg-slate-200 transition-colors text-sm font-medium text-slate-700">
                        <i class="fas fa-lock"></i>
                        <span>Admin</span>
                    </button>
                </div>
            </div>
        </div>
    </nav>

    <!-- Main Content -->
    <main class="pt-24 pb-12 px-4 sm:px-6 lg:px-8 max-w-7xl mx-auto">

        <!-- Agent View -->
        <div id="agentView" class="animate-fade-in">
            <div class="text-center mb-12">
                <h2 class="text-4xl md:text-5xl font-extrabold text-slate-900 mb-4 leading-tight">
                    Submit Your <span class="text-transparent bg-clip-text bg-gradient-to-r from-blue-600 to-orange-500">Commission Claims</span>
                </h2>
                <p class="text-lg text-slate-600 max-w-2xl mx-auto">
                    Upload your client list Excel file. We'll process your commission within 24-48 hours.
                </p>
            </div>

            <div class="max-w-2xl mx-auto">
                <div class="bg-white rounded-2xl shadow-xl border border-slate-100 overflow-hidden">
                    <div class="p-8">
                        <!-- Progress Steps -->
                        <div class="flex justify-between mb-8 relative">
                            <div class="absolute top-1/2 left-0 w-full h-1 bg-slate-100 -z-10 transform -translate-y-1/2"></div>
                            <div class="flex flex-col items-center gap-2">
                                <div class="w-10 h-10 rounded-full bg-blue-600 text-white flex items-center justify-center font-bold shadow-lg" id="step1">1</div>
                                <span class="text-xs font-semibold text-blue-600">Upload</span>
                            </div>
                            <div class="flex flex-col items-center gap-2">
                                <div class="w-10 h-10 rounded-full bg-slate-200 text-slate-400 flex items-center justify-center font-bold" id="step2">2</div>
                                <span class="text-xs font-semibold text-slate-400" id="step2Text">Details</span>
                            </div>
                            <div class="flex flex-col items-center gap-2">
                                <div class="w-10 h-10 rounded-full bg-slate-200 text-slate-400 flex items-center justify-center font-bold" id="step3">3</div>
                                <span class="text-xs font-semibold text-slate-400" id="step3Text">Submit</span>
                            </div>
                        </div>

                        <!-- Upload Zone -->
                        <div id="uploadZone" class="file-drop-zone rounded-2xl p-8 text-center cursor-pointer hover:bg-slate-50 transition-all" onclick="document.getElementById('fileInput').click()">
                            <input type="file" id="fileInput" class="hidden" accept=".xlsx,.xls,.csv" onchange="handleFileSelect(event)">

                            <div id="uploadIcon" class="mb-4">
                                <div class="w-20 h-20 mx-auto bg-blue-50 rounded-full flex items-center justify-center mb-4 animate-bounce">
                                    <i class="fas fa-cloud-upload-alt text-4xl text-blue-600"></i>
                                </div>
                            </div>

                            <div id="filePreview" class="hidden">
                                <div class="w-20 h-20 mx-auto bg-green-50 rounded-full flex items-center justify-center mb-4">
                                    <i class="fas fa-file-excel text-4xl text-green-600"></i>
                                </div>
                                <h3 class="text-lg font-bold text-slate-900 mb-1" id="fileName">filename.xlsx</h3>
                                <p class="text-sm text-slate-500 mb-4" id="fileSize">2.4 MB</p>
                                <button onclick="event.stopPropagation(); resetUpload()" class="text-sm text-red-500 hover:text-red-600 font-medium underline">
                                    Remove file
                                </button>
                            </div>

                            <div id="uploadText">
                                <h3 class="text-lg font-bold text-slate-900 mb-2">Drop your Excel file here</h3>
                                <p class="text-slate-500 mb-4">or click to browse from your computer</p>
                                <div class="flex justify-center gap-2 text-xs text-slate-400">
                                    <span class="px-3 py-1 bg-slate-100 rounded-full">.XLSX</span>
                                    <span class="px-3 py-1 bg-slate-100 rounded-full">.XLS</span>
                                    <span class="px-3 py-1 bg-slate-100 rounded-full">.CSV</span>
                                </div>
                            </div>
                        </div>

                        <!-- Agent Details Form -->
                        <div id="agentForm" class="mt-6 hidden animate-fade-in">
                            <div class="space-y-4">
                                <div>
                                    <label class="block text-sm font-semibold text-slate-700 mb-2">Agent Full Name *</label>
                                    <input type="text" id="agentName" class="w-full px-4 py-3 rounded-xl border border-slate-200 focus:border-blue-500 focus:ring-2 focus:ring-blue-200 outline-none transition-all" placeholder="Enter your full name">
                                </div>
                                <div>
                                    <label class="block text-sm font-semibold text-slate-700 mb-2">Agent ID / Phone Number *</label>
                                    <input type="text" id="agentId" class="w-full px-4 py-3 rounded-xl border border-slate-200 focus:border-blue-500 focus:ring-2 focus:ring-blue-200 outline-none transition-all" placeholder="e.g., AGT-2024-001 or 08012345678">
                                </div>
                                <div>
                                    <label class="block text-sm font-semibold text-slate-700 mb-2">Email Address (for notifications)</label>
                                    <input type="email" id="agentEmail" class="w-full px-4 py-3 rounded-xl border border-slate-200 focus:border-blue-500 focus:ring-2 focus:ring-blue-200 outline-none transition-all" placeholder="your@email.com">
                                </div>
                                <div>
                                    <label class="block text-sm font-semibold text-slate-700 mb-2">Number of Clients in File *</label>
                                    <input type="number" id="clientCount" class="w-full px-4 py-3 rounded-xl border border-slate-200 focus:border-blue-500 focus:ring-2 focus:ring-blue-200 outline-none transition-all" placeholder="Approximate number">
                                </div>
                                <div>
                                    <label class="block text-sm font-semibold text-slate-700 mb-2">Notes for Admin (Optional)</label>
                                    <textarea id="agentNotes" rows="3" class="w-full px-4 py-3 rounded-xl border border-slate-200 focus:border-blue-500 focus:ring-2 focus:ring-blue-200 outline-none transition-all" placeholder="Any special instructions..."></textarea>
                                </div>
                            </div>
                        </div>

                        <!-- Submit Button -->
                        <button id="submitBtn" onclick="submitClaim()" disabled class="w-full mt-6 bg-slate-200 text-slate-400 font-bold py-4 rounded-xl cursor-not-allowed transition-all flex items-center justify-center gap-2">
                            <span>Submit Commission Claim</span>
                            <i class="fas fa-arrow-right"></i>
                        </button>

                        <!-- Success Message -->
                        <div id="successMessage" class="hidden text-center py-8">
                            <svg class="success-checkmark" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 52 52">
                                <circle class="checkmark__circle" cx="26" cy="26" r="25" fill="none"/>
                                <path class="checkmark__check" fill="none" d="M14.1 27.2l7.1 7.2 16.7-16.8"/>
                            </svg>
                            <h3 class="text-2xl font-bold text-slate-900 mb-2 mt-4">Claim Submitted!</h3>
                            <p class="text-slate-600 mb-2">Reference: <span id="refNumber" class="font-mono font-bold text-blue-600">ASU-2024-001</span></p>
                            <p class="text-slate-600 mb-6">We'll process your commission within 24-48 hours. Check your email for confirmation.</p>
                            <button onclick="resetForm()" class="px-6 py-3 bg-blue-600 text-white rounded-xl font-semibold hover:bg-blue-700 transition-colors">
                                Submit Another Claim
                            </button>
                        </div>
                    </div>

                    <div class="bg-slate-50 px-8 py-4 border-t border-slate-100">
                        <div class="flex items-start gap-3 text-sm text-slate-600">
                            <i class="fas fa-info-circle text-blue-500 mt-0.5"></i>
                            <p>Ensure your Excel has: Schedule date your client appears, the serial number, and the correct %. Kindly note that <b>any Excel file with altered percentages will be automatically rejected, and the inflated percentage will not be paid.</b></p>
                        </div>
                    </div>
                </div>

                <!-- Recent Submissions -->
                <div class="mt-8 bg-white rounded-2xl shadow-lg border border-slate-100 p-6">
                    <h3 class="font-bold text-slate-900 mb-4 flex items-center gap-2">
                        <i class="fas fa-history text-blue-500"></i>
                        Your Recent Submissions
                    </h3>
                    <div id="submissionsList" class="space-y-3">
                        <!-- Populated by JavaScript -->
                    </div>
                </div>
            </div>
        </div>

        <!-- Admin View -->
        <div id="adminView" class="hidden animate-fade-in">
            <div class="flex justify-between items-center mb-8">
                <div>
                    <h2 class="text-3xl font-bold text-slate-900">Admin Dashboard</h2>
                    <p class="text-slate-600">Manage claims, tickets, and process payments</p>
                </div>
                <div class="flex gap-3">
                    <button onclick="exportData()" class="px-4 py-2 bg-blue-600 text-white rounded-lg hover:bg-blue-700 font-medium flex items-center gap-2">
                        <i class="fas fa-download"></i>
                        Export Data
                    </button>
                </div>
            </div>

            <!-- Stats -->
            <div class="grid grid-cols-1 md:grid-cols-4 gap-6 mb-8">
                <div class="bg-white p-6 rounded-2xl shadow-lg border border-slate-100">
                    <div class="flex justify-between items-start mb-4">
                        <div class="p-3 bg-blue-50 rounded-xl">
                            <i class="fas fa-file-upload text-blue-600 text-xl"></i>
                        </div>
                        <span id="pendingCount" class="text-2xl font-bold text-slate-900">0</span>
                    </div>
                    <p class="text-sm text-slate-500">Pending Claims</p>
                </div>
                <div class="bg-white p-6 rounded-2xl shadow-lg border border-slate-100">
                    <div class="flex justify-between items-start mb-4">
                        <div class="p-3 bg-orange-50 rounded-xl">
                            <i class="fas fa-ticket-alt text-orange-600 text-xl"></i>
                        </div>
                        <span id="openTickets" class="text-2xl font-bold text-slate-900">0</span>
                    </div>
                    <p class="text-sm text-slate-500">Open Tickets</p>
                </div>
                <div class="bg-white p-6 rounded-2xl shadow-lg border border-slate-100">
                    <div class="flex justify-between items-start mb-4">
                        <div class="p-3 bg-green-50 rounded-xl">
                            <i class="fas fa-check-circle text-green-600 text-xl"></i>
                        </div>
                        <span id="processedCount" class="text-2xl font-bold text-slate-900">0</span>
                    </div>
                    <p class="text-sm text-slate-500">Processed This Month</p>
                </div>
                <div class="bg-white p-6 rounded-2xl shadow-lg border border-slate-100">
                    <div class="flex justify-between items-start mb-4">
                        <div class="p-3 bg-purple-50 rounded-xl">
                            <i class="fas fa-money-bill-wave text-purple-600 text-xl"></i>
                        </div>
                        <span id="totalCommission" class="text-2xl font-bold text-slate-900">₦0</span>
                    </div>
                    <p class="text-sm text-slate-500">Total Commission Due</p>
                </div>
            </div>

            <!-- Tabs -->
            <div class="bg-white rounded-2xl shadow-xl border border-slate-100 overflow-hidden">
                <div class="border-b border-slate-100">
                    <div class="flex">
                        <button onclick="switchTab('claims')" id="tabClaims" class="flex-1 px-6 py-4 text-sm font-bold text-blue-600 border-b-2 border-blue-600 bg-blue-50">
                            Commission Claims
                        </button>
                        <button onclick="switchTab('tickets')" id="tabTickets" class="flex-1 px-6 py-4 text-sm font-bold text-slate-500 hover:text-slate-700">
                            Support Tickets <span id="ticketTabBadge" class="ml-2 bg-orange-500 text-white text-xs px-2 py-0.5 rounded-full hidden">0</span>
                        </button>
                    </div>
                </div>

                <!-- Claims Tab -->
                <div id="claimsPanel" class="p-6">
                    <div class="flex justify-between items-center mb-4">
                        <input type="text" id="searchClaims" placeholder="Search by agent name..." class="px-4 py-2 rounded-lg border border-slate-200 text-sm focus:outline-none focus:border-blue-500 w-64" onkeyup="filterClaims()">
                    </div>
                    <div class="overflow-x-auto">
                        <table class="w-full">
                            <thead class="bg-slate-50">
                                <tr>
                                    <th class="px-4 py-3 text-left text-xs font-bold text-slate-500 uppercase">Agent</th>
                                    <th class="px-4 py-3 text-left text-xs font-bold text-slate-500 uppercase">File</th>
                                    <th class="px-4 py-3 text-left text-xs font-bold text-slate-500 uppercase">Clients</th>
                                    <th class="px-4 py-3 text-left text-xs font-bold text-slate-500 uppercase">Submitted</th>
                                    <th class="px-4 py-3 text-left text-xs font-bold text-slate-500 uppercase">Status</th>
                                    <th class="px-4 py-3 text-right text-xs font-bold text-slate-500 uppercase">Action</th>
                                </tr>
                            </thead>
                            <tbody id="adminClaimsTable" class="divide-y divide-slate-100">
                                <!-- Populated by JS -->
                            </tbody>
                        </table>
                    </div>
                </div>

                <!-- Tickets Tab -->
                <div id="ticketsPanel" class="hidden p-6">
                    <div class="space-y-4" id="adminTicketsList">
                        <!-- Populated by JS -->
                    </div>
                </div>
            </div>
        </div>
    </main>

    <!-- Ticket Modal -->
    <div id="ticketModal" class="modal">
        <div class="bg-white rounded-2xl shadow-2xl w-full max-w-2xl mx-4 max-h-[90vh] overflow-y-auto">
            <div class="p-6 border-b border-slate-100 flex justify-between items-center">
                <div>
                    <h3 class="text-xl font-bold text-slate-900">Help & Support</h3>
                    <p class="text-sm text-slate-500">Create a ticket for commission issues</p>
                </div>
                <button onclick="closeTicketModal()" class="text-slate-400 hover:text-slate-600">
                    <i class="fas fa-times text-xl"></i>
                </button>
            </div>

            <div class="p-6">
                <!-- New Ticket Form -->
                <div id="newTicketForm" class="mb-6">
                    <h4 class="font-semibold text-slate-900 mb-4">Create New Ticket</h4>
                    <div class="space-y-4">
                        <div>
                            <label class="block text-sm font-medium text-slate-700 mb-1">Ticket Type</label>
                            <select id="ticketType" class="w-full px-4 py-2 rounded-lg border border-slate-200 focus:border-blue-500 outline-none">
                                <option value="payment">Missing Commission Payment</option>
                                <option value="calculation">Wrong Commission Calculation</option>
                                <option value="status">Check Claim Status</option>
                                <option value="technical">Technical Issue</option>
                                <option value="other">Other</option>
                            </select>
                        </div>
                        <div>
                            <label class="block text-sm font-medium text-slate-700 mb-1">Your Agent ID / Phone</label>
                            <input type="text" id="ticketAgentId" class="w-full px-4 py-2 rounded-lg border border-slate-200 focus:border-blue-500 outline-none" placeholder="So we can look up your account">
                        </div>
                        <div>
                            <label class="block text-sm font-medium text-slate-700 mb-1">Description *</label>
                            <textarea id="ticketDescription" rows="4" class="w-full px-4 py-2 rounded-lg border border-slate-200 focus:border-blue-500 outline-none" placeholder="Describe your issue in detail..."></textarea>
                        </div>
                        <button onclick="submitTicket()" class="w-full bg-orange-500 text-white font-bold py-3 rounded-lg hover:bg-orange-600 transition-colors">
                            Submit Ticket
                        </button>
                    </div>
                </div>

                <!-- My Tickets -->
                <div>
                    <h4 class="font-semibold text-slate-900 mb-4">Your Tickets</h4>
                    <div id="myTicketsList" class="space-y-3">
                        <!-- Populated by JS -->
                    </div>
                </div>
            </div>
        </div>
    </div>

    <!-- Toast -->
    <div id="toast" class="fixed bottom-4 right-4 transform translate-y-20 opacity-0 transition-all duration-300 z-50">
        <div class="bg-slate-900 text-white px-6 py-4 rounded-xl shadow-2xl flex items-center gap-3">
            <i id="toastIcon" class="fas fa-check-circle text-green-400 text-xl"></i>
            <div>
                <p class="font-semibold" id="toastTitle">Success</p>
                <p class="text-sm text-slate-300" id="toastMessage">Operation completed</p>
            </div>
        </div>
    </div>

    <script>
        // Data Storage (In-memory for demo - would be database in production)
        let claims = JSON.parse(localStorage.getItem('asasu_claims') || '[]');
        let tickets = JSON.parse(localStorage.getItem('asasu_tickets') || '[]');
        let currentFile = null;
        let isAdmin = false;

        // Initialize
        document.addEventListener('DOMContentLoaded', () => {
            updateStats();
            renderSubmissions();
            renderAdminTables();
            renderTickets();
            checkTicketBadge();

            // Setup drag and drop
            setupDragDrop();

            // Show admin toggle (in real app, this would be password protected)
            document.getElementById('viewToggle').classList.remove('hidden');
        });

        // Drag and Drop
        function setupDragDrop() {
            const zone = document.getElementById('uploadZone');
            ['dragenter', 'dragover', 'dragleave', 'drop'].forEach(e => {
                zone.addEventListener(e, (ev) => { ev.preventDefault(); ev.stopPropagation(); }, false);
            });
            ['dragenter', 'dragover'].forEach(e => {
                zone.addEventListener(e, () => zone.classList.add('dragover'), false);
            });
            ['dragleave', 'drop'].forEach(e => {
                zone.addEventListener(e, () => zone.classList.remove('dragover'), false);
            });
            zone.addEventListener('drop', (e) => {
                if (e.dataTransfer.files.length) handleFile(e.dataTransfer.files[0]);
            }, false);
        }

        function handleFileSelect(e) {
            if (e.target.files.length) handleFile(e.target.files[0]);
        }

        function handleFile(file) {
            const valid = ['.xlsx', '.xls', '.csv'];
            const ext = '.' + file.name.split('.').pop().toLowerCase();
            if (!valid.includes(ext)) {
                showToast('Error', 'Please upload Excel or CSV files only', 'error');
                return;
            }
            currentFile = file;
            document.getElementById('uploadIcon').classList.add('hidden');
            document.getElementById('uploadText').classList.add('hidden');
            document.getElementById('filePreview').classList.remove('hidden');
            document.getElementById('fileName').textContent = file.name;
            document.getElementById('fileSize').textContent = formatSize(file.size);
            document.getElementById('agentForm').classList.remove('hidden');
            updateStep(2);
            validateForm();
        }

        function resetUpload() {
            currentFile = null;
            document.getElementById('fileInput').value = '';
            document.getElementById('uploadIcon').classList.remove('hidden');
            document.getElementById('uploadText').classList.remove('hidden');
            document.getElementById('filePreview').classList.add('hidden');
            document.getElementById('agentForm').classList.add('hidden');
            updateStep(1);
        }

        function formatSize(bytes) {
            if (bytes === 0) return '0 Bytes';
            const k = 1024;
            const sizes = ['Bytes', 'KB', 'MB'];
            const i = Math.floor(Math.log(bytes) / Math.log(k));
            return parseFloat((bytes / Math.pow(k, i)).toFixed(2)) + ' ' + sizes[i];
        }

        function updateStep(step) {
            for (let i = 1; i <= 3; i++) {
                const el = document.getElementById(`step${i}`);
                const txt = document.getElementById(`step${i}Text`);
                if (i < step) {
                    el.className = 'w-10 h-10 rounded-full bg-green-500 text-white flex items-center justify-center font-bold shadow-lg';
                    el.innerHTML = '<i class="fas fa-check"></i>';
                    if (txt) txt.className = 'text-xs font-semibold text-green-600';
                } else if (i === step) {
                    el.className = 'w-10 h-10 rounded-full bg-blue-600 text-white flex items-center justify-center font-bold shadow-lg';
                    el.innerHTML = i;
                    if (txt) txt.className = 'text-xs font-semibold text-blue-600';
                } else {
                    el.className = 'w-10 h-10 rounded-full bg-slate-200 text-slate-400 flex items-center justify-center font-bold';
                    el.innerHTML = i;
                    if (txt) txt.className = 'text-xs font-semibold text-slate-400';
                }
            }
        }

        // Form validation
        ['agentName', 'agentId', 'clientCount'].forEach(id => {
            document.getElementById(id).addEventListener('input', validateForm);
        });

        function validateForm() {
            const name = document.getElementById('agentName').value;
            const id = document.getElementById('agentId').value;
            const count = document.getElementById('clientCount').value;
            const btn = document.getElementById('submitBtn');

            if (currentFile && name && id && count) {
                btn.disabled = false;
                btn.className = 'w-full mt-6 bg-gradient-to-r from-blue-600 to-orange-500 text-white font-bold py-4 rounded-xl hover:shadow-lg transition-all flex items-center justify-center gap-2 cursor-pointer';
            } else {
                btn.disabled = true;
                btn.className = 'w-full mt-6 bg-slate-200 text-slate-400 font-bold py-4 rounded-xl cursor-not-allowed transition-all flex items-center justify-center gap-2';
            }
        }

        function submitClaim() {
            const btn = document.getElementById('submitBtn');
            btn.innerHTML = '<div class="loader"></div><span>Processing...</span>';
            btn.disabled = true;

            // Simulate processing
            setTimeout(() => {
                const claim = {
                    id: 'ASU-' + Date.now().toString().slice(-6),
                    agentName: document.getElementById('agentName').value,
                    agentId: document.getElementById('agentId').value,
                    email: document.getElementById('agentEmail').value,
                    clientCount: document.getElementById('clientCount').value,
                    notes: document.getElementById('agentNotes').value,
                    fileName: currentFile.name,
                    fileSize: formatSize(currentFile.size),
                    date: new Date().toLocaleString(),
                    status: 'Pending',
                    fileData: null // In real app, this would be uploaded to server
                };

                // Read file for demo purposes
                const reader = new FileReader();
                reader.onload = function(e) {
                    claim.fileData = e.target.result;
                    claims.unshift(claim);
                    localStorage.setItem('asasu_claims', JSON.stringify(claims));

                    document.getElementById('refNumber').textContent = claim.id;
                    document.getElementById('uploadZone').classList.add('hidden');
                    document.getElementById('agentForm').classList.add('hidden');
                    btn.classList.add('hidden');
                    document.getElementById('successMessage').classList.remove('hidden');
                    updateStep(3);
                    updateStats();
                    renderSubmissions();
                    renderAdminTables();
                    showToast('Success', 'Claim submitted! Reference: ' + claim.id);
                };
                reader.readAsDataURL(currentFile);
            }, 1500);
        }

        function resetForm() {
            resetUpload();
            ['agentName', 'agentId', 'agentEmail', 'clientCount', 'agentNotes'].forEach(id => {
                document.getElementById(id).value = '';
            });
            document.getElementById('uploadZone').classList.remove('hidden');
            document.getElementById('submitBtn').classList.remove('hidden');
            document.getElementById('successMessage').classList.add('hidden');
            document.getElementById('submitBtn').innerHTML = '<span>Submit Commission Claim</span><i class="fas fa-arrow-right"></i>';
        }

        // Ticket System
        function showTicketModal() {
            document.getElementById('ticketModal').classList.add('active');
            renderMyTickets();
        }

        function closeTicketModal() {
            document.getElementById('ticketModal').classList.remove('active');
        }

        function submitTicket() {
            const type = document.getElementById('ticketType').value;
            const agentId = document.getElementById('ticketAgentId').value;
            const desc = document.getElementById('ticketDescription').value;

            if (!agentId || !desc) {
                showToast('Error', 'Please fill in all required fields', 'error');
                return;
            }

            const ticket = {
                id: 'TKT-' + Date.now().toString().slice(-6),
                type: type,
                agentId: agentId,
                description: desc,
                date: new Date().toLocaleString(),
                status: 'Open',
                responses: []
            };

            tickets.unshift(ticket);
            localStorage.setItem('asasu_tickets', JSON.stringify(tickets));

            document.getElementById('ticketAgentId').value = '';
            document.getElementById('ticketDescription').value = '';

            renderMyTickets();
            renderTickets();
            checkTicketBadge();
            updateStats();
            showToast('Success', 'Ticket created! ID: ' + ticket.id);
        }

        function renderMyTickets() {
            const list = document.getElementById('myTicketsList');
            const myId = document.getElementById('agentId').value;

            if (!tickets.length) {
                list.innerHTML = '<p class="text-slate-500 text-center py-4">No tickets yet</p>';
                return;
            }

            list.innerHTML = tickets.map(t => `
                <div class="border border-slate-200 rounded-lg p-4 ${t.status === 'Open' ? 'bg-orange-50 border-orange-200' : 'bg-slate-50'}">
                    <div class="flex justify-between items-start mb-2">
                        <span class="font-bold text-slate-900">${t.id}</span>
                        <span class="px-2 py-1 rounded text-xs font-bold ${t.status === 'Open' ? 'bg-orange-500 text-white' : 'bg-green-500 text-white'}">${t.status}</span>
                    </div>
                    <p class="text-sm text-slate-600 mb-2 capitalize"><i class="fas fa-tag mr-1"></i> ${t.type}</p>
                    <p class="text-sm text-slate-700 mb-2">${t.description}</p>
                    <p class="text-xs text-slate-400">${t.date}</p>
                </div>
            `).join('');
        }

        function renderTickets() {
            const list = document.getElementById('adminTicketsList');
            if (!tickets.length) {
                list.innerHTML = '<p class="text-slate-500 text-center py-8">No tickets yet</p>';
                return;
            }

            list.innerHTML = tickets.map(t => `
                <div class="ticket-card border border-slate-200 rounded-xl p-6 ${t.status === 'Open' ? 'bg-white border-l-4 border-l-orange-500' : 'bg-slate-50'}">
                    <div class="flex justify-between items-start mb-4">
                        <div>
                            <h4 class="font-bold text-slate-900 text-lg">${t.id}</h4>
                            <p class="text-sm text-slate-500">Agent: ${t.agentId} • ${t.date}</p>
                        </div>
                        <div class="flex gap-2">
                            <span class="px-3 py-1 rounded-full text-xs font-bold ${t.status === 'Open' ? 'bg-orange-100 text-orange-700' : 'bg-green-100 text-green-700'}">${t.status}</span>
                        </div>
                    </div>
                    <div class="mb-4">
                        <span class="inline-block px-3 py-1 bg-slate-100 rounded-full text-xs font-semibold text-slate-600 mb-2 capitalize">${t.type}</span>
                        <p class="text-slate-700">${t.description}</p>
                    </div>
                    ${t.status === 'Open' ? `
                    <div class="flex gap-2">
                        <button onclick="resolveTicket('${t.id}')" class="px-4 py-2 bg-green-600 text-white rounded-lg text-sm font-semibold hover:bg-green-700">
                            <i class="fas fa-check mr-1"></i> Mark Resolved
                        </button>
                        <button onclick="respondTicket('${t.id}')" class="px-4 py-2 bg-blue-600 text-white rounded-lg text-sm font-semibold hover:bg-blue-700">
                            <i class="fas fa-reply mr-1"></i> Respond
                        </button>
                    </div>
                    ` : '<p class="text-green-600 text-sm font-semibold"><i class="fas fa-check-circle"></i> Resolved</p>'}
                </div>
            `).join('');
        }

        function resolveTicket(id) {
            const t = tickets.find(x => x.id === id);
            if (t) {
                t.status = 'Resolved';
                localStorage.setItem('asasu_tickets', JSON.stringify(tickets));
                renderTickets();
                checkTicketBadge();
                updateStats();
                showToast('Success', 'Ticket marked as resolved');
            }
        }

        function checkTicketBadge() {
            const openCount = tickets.filter(t => t.status === 'Open').length;
            const badge = document.getElementById('ticketBadge');
            const tabBadge = document.getElementById('ticketTabBadge');

            if (openCount > 0) {
                badge.textContent = openCount;
                badge.classList.remove('hidden');
                tabBadge.textContent = openCount;
                tabBadge.classList.remove('hidden');
            } else {
                badge.classList.add('hidden');
                tabBadge.classList.add('hidden');
            }
        }

        // Admin Functions
        function toggleView() {
            isAdmin = !isAdmin;
            if (isAdmin) {
                // Simple password protection for demo
                const pwd = prompt('Enter admin password:');
                if (pwd !== 'Asasu$1@') {
                    showToast('Error', 'Wrong password', 'error');
                    isAdmin = false;
                    return;
                }
                document.getElementById('agentView').classList.add('hidden');
                document.getElementById('adminView').classList.remove('hidden');
                document.getElementById('viewToggle').innerHTML = '<i class="fas fa-user"></i><span>Agent View</span>';
                renderAdminTables();
            } else {
                document.getElementById('adminView').classList.add('hidden');
                document.getElementById('agentView').classList.remove('hidden');
                document.getElementById('viewToggle').innerHTML = '<i class="fas fa-lock"></i><span>Admin</span>';
            }
        }

        function switchTab(tab) {
            if (tab === 'claims') {
                document.getElementById('claimsPanel').classList.remove('hidden');
                document.getElementById('ticketsPanel').classList.add('hidden');
                document.getElementById('tabClaims').className = 'flex-1 px-6 py-4 text-sm font-bold text-blue-600 border-b-2 border-blue-600 bg-blue-50';
                document.getElementById('tabTickets').className = 'flex-1 px-6 py-4 text-sm font-bold text-slate-500 hover:text-slate-700';
            } else {
                document.getElementById('claimsPanel').classList.add('hidden');
                document.getElementById('ticketsPanel').classList.remove('hidden');
                document.getElementById('tabTickets').className = 'flex-1 px-6 py-4 text-sm font-bold text-blue-600 border-b-2 border-blue-600 bg-blue-50';
                document.getElementById('tabClaims').className = 'flex-1 px-6 py-4 text-sm font-bold text-slate-500 hover:text-slate-700';
                renderTickets();
            }
        }

        function renderAdminTables() {
            const tbody = document.getElementById('adminClaimsTable');
            if (!claims.length) {
                tbody.innerHTML = '<tr><td colspan="6" class="px-4 py-8 text-center text-slate-500">No claims yet</td></tr>';
                return;
            }

            tbody.innerHTML = claims.map(c => `
                <tr class="hover:bg-slate-50">
                    <td class="px-4 py-3">
                        <div class="font-semibold text-slate-900">${c.agentName}</div>
                        <div class="text-xs text-slate-500">${c.agentId}</div>
                    </td>
                    <td class="px-4 py-3">
                        <div class="flex items-center gap-2">
                            <i class="fas fa-file-excel text-green-600"></i>
                            <span class="text-sm">${c.fileName}</span>
                        </div>
                        <div class="text-xs text-slate-400">${c.fileSize}</div>
                    </td>
                    <td class="px-4 py-3 text-sm font-bold">${c.clientCount}</td>
                    <td class="px-4 py-3 text-sm text-slate-500">${c.date}</td>
                    <td class="px-4 py-3">
                        <select onchange="updateStatus('${c.id}', this.value)" class="text-xs font-bold px-2 py-1 rounded border-0 ${getStatusColor(c.status)}">
                            <option value="Pending" ${c.status === 'Pending' ? 'selected' : ''}>Pending</option>
                            <option value="Reviewed" ${c.status === 'Reviewed' ? 'selected' : ''}>Reviewed</option>
                            <option value="Paid" ${c.status === 'Paid' ? 'selected' : ''}>Paid</option>
                        </select>
                    </td>
                    <td class="px-4 py-3 text-right">
                        ${c.fileData ? `<a href="${c.fileData}" download="${c.fileName}" class="text-blue-600 hover:text-blue-700 text-sm font-semibold"><i class="fas fa-download"></i> Download</a>` : '<span class="text-slate-400 text-sm">No file</span>'}
                    </td>
                </tr>
            `).join('');
        }

        function getStatusColor(status) {
            if (status === 'Paid') return 'bg-green-100 text-green-700';
            if (status === 'Reviewed') return 'bg-blue-100 text-blue-700';
            return 'bg-yellow-100 text-yellow-700';
        }

        function updateStatus(id, status) {
            const claim = claims.find(c => c.id === id);
            if (claim) {
                claim.status = status;
                localStorage.setItem('asasu_claims', JSON.stringify(claims));
                updateStats();
                renderSubmissions();
                showToast('Updated', `Claim ${id} marked as ${status}`);
            }
        }

        function updateStats() {
            const pending = claims.filter(c => c.status === 'Pending').length;
            const processed = claims.filter(c => c.status === 'Paid').length;
            const openTickets = tickets.filter(t => t.status === 'Open').length;

            document.getElementById('pendingCount').textContent = pending;
            document.getElementById('processedCount').textContent = processed;
            document.getElementById('openTickets').textContent = openTickets;

            // Calculate estimated commission (demo calculation)
            const total = claims.reduce((sum, c) => sum + (parseInt(c.clientCount) * 5000), 0); // Assuming avg 5000 per client
            document.getElementById('totalCommission').textContent = '₦' + total.toLocaleString();
        }

        function renderSubmissions() {
            const list = document.getElementById('submissionsList');
            const recent = claims.slice(0, 3);

            if (!recent.length) {
                list.innerHTML = '<p class="text-slate-500 text-center py-4">No submissions yet</p>';
                return;
            }

            list.innerHTML = recent.map(c => `
                <div class="flex items-center justify-between p-4 bg-slate-50 rounded-xl border border-slate-100">
                    <div class="flex items-center gap-3">
                        <div class="w-10 h-10 rounded-lg ${c.status === 'Paid' ? 'bg-green-100 text-green-600' : c.status === 'Reviewed' ? 'bg-blue-100 text-blue-600' : 'bg-yellow-100 text-yellow-600'} flex items-center justify-center">
                            <i class="fas ${c.status === 'Paid' ? 'fa-check' : c.status === 'Reviewed' ? 'fa-eye' : 'fa-clock'}"></i>
                        </div>
                        <div>
                            <p class="font-semibold text-slate-900">${c.fileName}</p>
                            <p class="text-xs text-slate-500">${c.date} • ${c.clientCount} clients</p>
                        </div>
                    </div>
                    <span class="px-3 py-1 ${getStatusColor(c.status)} rounded-full text-xs font-bold">${c.status}</span>
                </div>
            `).join('');
        }

        function filterClaims() {
            const term = document.getElementById('searchClaims').value.toLowerCase();
            const rows = document.querySelectorAll('#adminClaimsTable tr');
            rows.forEach(row => {
                const text = row.textContent.toLowerCase();
                row.style.display = text.includes(term) ? '' : 'none';
            });
        }

        function exportData() {
            const data = claims.map(c => ({
                Reference: c.id,
                Agent: c.agentName,
                AgentID: c.agentId,
                Email: c.email,
                Clients: c.clientCount,
                File: c.fileName,
                Date: c.date,
                Status: c.status,
                Notes: c.notes
            }));

            const ws = XLSX.utils.json_to_sheet(data);
            const wb = XLSX.utils.book_new();
            XLSX.utils.book_append_sheet(wb, ws, "Claims");
            XLSX.writeFile(wb, `ASASU_Claims_${new Date().toISOString().split('T')[0]}.xlsx`);
            showToast('Success', 'Data exported to Excel');
        }

        function showToast(title, message, type = 'success') {
            const toast = document.getElementById('toast');
            document.getElementById('toastTitle').textContent = title;
            document.getElementById('toastMessage').textContent = message;
            document.getElementById('toastIcon').className = type === 'error' ? 'fas fa-exclamation-circle text-red-400 text-xl' : 'fas fa-check-circle text-green-400 text-xl';

            toast.classList.remove('translate-y-20', 'opacity-0');
            setTimeout(() => {
                toast.classList.add('translate-y-20', 'opacity-0');
            }, 3000);
        }

        // Close modal on outside click
        document.getElementById('ticketModal').addEventListener('click', (e) => {
            if (e.target.id === 'ticketModal') closeTicketModal();
        });
    </script>
</body>
</html>
