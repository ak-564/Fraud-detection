const express = require('express');
const { execFile } = require('child_process');
const path = require('path');

const app = express();
const PORT = process.env.PORT || 3000;

// Middleware
app.use(express.json());
app.use(express.static('public'));

// CORS middleware
app.use((req, res, next) => {
    res.header('Access-Control-Allow-Origin', '*');
    res.header('Access-Control-Allow-Headers', 'Content-Type');
    next();
});

/**
 * Transaction endpoint
 */
app.post('/transaction', (req, res) => {
    const { accNo, amount, location } = req.body;
    
    // Input validation
    if (!accNo || !amount || !location) {
        return res.status(400).json({ 
            alerts: ["Missing required fields"], 
            success: false 
        });
    }

    if (isNaN(accNo) || accNo < 100 || accNo > 109) {
        return res.status(400).json({ 
            alerts: ["Invalid account number. Use 100-109"], 
            success: false 
        });
    }

    console.log(`Processing transaction: Account ${accNo}, Amount ${amount}, Location ${location}`);

    execFile('./fraudBackend', [accNo.toString(), amount.toString(), location], (err, stdout, stderr) => {
        if (err) {
            console.error('Backend error:', stderr);
            return res.status(500).json({ 
                alerts: ["Backend service error"], 
                success: false 
            });
        }
        
        try {
            const result = JSON.parse(stdout);
            console.log('Transaction result:', result);
            res.json(result);
        } catch (parseError) {
            console.error('Parse error:', stdout);
            res.status(500).json({ 
                alerts: ["Invalid response from backend"], 
                success: false 
            });
        }
    });
});

/**
 * Health check endpoint
 */
app.get('/health', (req, res) => {
    res.json({ 
        status: 'OK', 
        service: 'Fraud Detection System',
        timestamp: new Date().toISOString()
    });
});

/**
 * Serve frontend
 */
app.get('/', (req, res) => {
    res.sendFile(path.join(__dirname, '../public/index.html'));
});

/**
 * Start server
 */
app.listen(PORT, () => {
    console.log(`🚀 Fraud Detection System running on http://localhost:${PORT}`);
    console.log(`📊 Health check: http://localhost:${PORT}/health`);
});
