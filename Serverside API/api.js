const express = require('express');
const bodyParser = require('body-parser');
const path = require('path');
const cors = require('cors');
const helmet = require('helmet');

// MySQL connection setup
let mysql = require('mysql')
let con = mysql.createConnection({
    host: "localhost",
    user: "ubuntu-1078853",
    password: "4G22cVPx@",
    database: "deBankDatabase"
});

const app = express();
const port = 8500;

// Middleware setup
app.use(cors({
    origin: "*", // Set CORS origin
    methods: ["POST", "GET", "OPTIONS"],
    allowedHeaders: ["Content-Type", "noob-token"],
}));

app.use(helmet()); // Adds security headers to the responses

app.use(bodyParser.urlencoded({ extended: true })); // Parse URL-encoded bodies
app.use(bodyParser.json()); // Parse JSON bodies

let uidStore = '';
let pinToSet = '';
let lastTransactionDate = null;

// Connect to MySQL database
con.connect(function(err) {
    if (err) throw err;
    console.log("Connected to MySQL database");
});

// To validate UID format
function isValidUID(uid) {
    // Define the regular expression pattern
    let uidPattern = /^[A-Z0-9]{8}$/;
    // Use the test method to check if the UID matches the pattern
    return uidPattern.test(uid);
}

// To check for NOOB-TOKEN header
const checkToken = (req, res, next) => {
    const noobToken = req.headers['noob-token'];
    const tokenRegex = /^[0-9a-f]{8}-([0-9a-f]{4}-){3}[0-9a-f]{12}$/i;
    if (!tokenRegex.test(noobToken)) {
        return res.status(403).json({ error: 'Unauthorized access' });
    }
    next();
};

app.use(checkToken);

// Health check endpoint
app.get("/api/noob/health", (req, res) => {
    res.json({
        "status": "OK"
    });
});

// Login endpoint
app.post('/login', (req, res) => {
    const { pinCode, iban, uid } = req.body;
    let pinVerify = 0;
    let resultToSend = '';

    uidStore = uid;
    pinToSet = pinCode;

    if (isValidUID(uid)) {
        // Check remaining attempts
        con.query("SELECT attempts FROM Card WHERE cardNumber = ?", [uid], function (err, result, fields) {
            if (err) {
                console.error('Error fetching attempts:', err);
                res.status(500).json({ error: 'Internal server error' });
                return;
            }

            if (result[0].attempts > 0) {
                // Validate PIN
                con.query("SELECT pinCode FROM Card WHERE cardNumber = ?", [uid], function (err, result, fields) {
                    if (err) throw err;
                    let resultPin = result[0].pinCode;

                    if (pinToSet == resultPin) {
                        pinVerify = 1;
                        // Reset attempts on succesful login
                        con.query("UPDATE Card SET attempts = 3 WHERE cardNumber = ?", [uid], function(err, result, fields) {
                            if (err) {
                                console.error('Error updating attempts:', err);
                                res.status(500).json({ error: 'Internal server error' });
                                return;
                            }
                        });
                    } else {
                        // Decrease attempts on failed login
                        con.query("UPDATE Card SET attempts = attempts - 1 WHERE cardNumber = ?", [uid], function (err, result, fields) {
                            if (err) {
                                console.error('Error updating attempts:', err);
                                res.status(500).json({ error: 'Internal server error' });
                                return;
                            }
                        });
                    }

                    if (pinVerify == 1) {
                        // Fetch account info on succesful login
                        con.query("SELECT balanceInCents, fullName, MAX(idTransaction) AS maxTransactionId FROM Account ac JOIN Card ca ON ac.idAccount = ca.idCard JOIN User us ON ac.idAccount = us.idUser JOIN Transaction tr ON ca.idCard = tr.FK_idCard WHERE cardNumber = ? GROUP BY ac.balanceInCents, us.fullName", [uidStore], function (err, result, fields) {
                            if (err) throw err;
                            let resultJSON = result[0];
                            res.status(200).json({ status: "200", data: JSON.stringify(resultJSON) });
                        });
                    } else {
                        // Handle failed login attempts
                        con.query("SELECT attempts FROM Card WHERE cardNumber = ?", [uid], function (err, result, fields) {
                            if (err) throw err;
                            let attemptsRemaining = result[0].attempts;
                            if (attemptsRemaining > 0) {
                                res.status(401).json({ status: "401", data: "Unauthorized", attempts_remaining: attemptsRemaining });
                            } else {
                                res.status(403).json({ status: "403", data: "Card is blocked" });
                            }
                        });
                    }
                });
            } else {
                res.status(403).json({ status: "403", data: "Card is blocked" });
            }
        });
    } else {
        res.status(401).json({ status: "401", data: "Invalid UID" });
    }
});

// Withdraw endpoint
app.post('/withdraw', (req, res) => {
    const { amount } = req.body;

    let resultBalance;
    let resultName;

    // Calculate the current time plus 2 hours
    const currentTime = new Date();
    currentTime.setHours(currentTime.getHours() + 2);
    const formattedTime = currentTime.toISOString().slice(0, 19).replace('T', ' ');

    if (lastTransactionDate && currentTime.getDate() !== lastTransactionDate.getDate()) {
        // Reset the daily limit if the date has changed
        con.query("UPDATE Card SET dailyLimit = 25000;", function (err, result, fields) {
            if (err) {
                console.error('Error resetting daily limit:', err);
                res.status(500).json({ error: 'Internal server error' });
                return;
            }
        });
    }
    // Check daily limit
    con.query("SELECT dailyLimit FROM Card WHERE cardNumber = ?", [uidStore], function (err, result, fields) {
        if (err) {
            console.error("Error fetching dailyLimit:", err);
            res.status(500).json({ error: 'Internal server error' });
            return;
        }

        if (amount <= result[0].dailyLimit) {
            // Check balance
            con.query("SELECT balanceInCents, fullName FROM Account JOIN Card ON Account.idAccount = Card.idCard JOIN User ON Account.idAccount = User.idUser WHERE cardNumber = ?", [uidStore], function (err, result, fields) {
                if (err) throw err;
                resultBalance = result[0].balanceInCents;
                resultName = result[0].fullName;

                if (resultBalance >= amount) {
                    // Create new transaction record
                    con.query("SELECT MAX(idTransaction) AS maxId FROM Transaction;", function (err, result, fields) {
                        if (err) {
                            console.error('Error fetching max idTransaction:', err);
                            res.status(500).json({ error: 'Internal server error' });
                            return;
                        }

                        const newIdTransaction = result[0].maxId + 1;

                        con.query("INSERT INTO Transaction (idTransaction, Fk_idATM, Fk_idCard, amountInPounds, time, fivePoundBills, tenPoundBills, twentyPoundBills, fiftyPoundBills) VALUES (?, 1, (SELECT idCard FROM Card WHERE cardNumber = ?), ?, ?, NULL, NULL, NULL, NULL)", [newIdTransaction, uidStore, amount / 100, formattedTime], function (err, result, fields) {
                            if (err) {
                                console.error('Error inserting transaction:', err);
                                res.status(500).json({ error: 'Internal server error' });
                                return;
                            }
                        });
                    });
                    // Update balance and daily limit
                    con.query("UPDATE Account JOIN Card ON Account.idAccount = Card.idCard SET balanceInCents = (balanceInCents - ?), dailyLimit = (dailyLimit - ?) WHERE cardNumber = ?", [amount, amount, uidStore], function (err, result, fields) {
                        if (err) throw err;
                        lastTransactionDate = currentTime;

                        res.status(200).json({ status: "200", data: "Balance updated" });
                    });

                } else {
                    res.status(412).json({ status: "412", data: "Insufficient funds" });
                }
            });
        } else {
            res.status(429).json({ status: "429", data: "Exceeding daily limit" });
        }
    });
});

// Validate withdraw endpoint
app.post('/withdrawValidity', (req, res) => {
    const { amount } = req.body;
    con.query("SELECT dailyLimit, balanceInCents FROM Card JOIN Account ON Card.idCard = Account.idAccount WHERE cardNumber = ?", [uidStore], function (err, result, fields) {
        if (err) {
            console.error("Error fetching dailyLimit:", err);
            res.status(500).json({ error: 'Internal server error' });
            return;
        }

        if (amount <= result[0].dailyLimit && amount <= result[0].balanceInCents) {
            res.status(200).json({ status: "200", data: "Withdraw validated" });
        } else if (amount > result[0].balanceInCents){
            res.status(412).json({ status: "412", data: "Insufficient funds" });
        } else {
            res.status(429).json({ status: "429", data: "Exceeding daily limit" });
        }
    });
});

// Add bills to transaction endpoint
app.post('/transactionBills', (req, res) => {
    const { fivePoundBills, tenPoundBills, twentyPoundBills, fiftyPoundBills } = req.body;
    // Fetch latest transaction
    con.query("SELECT MAX(idTransaction) AS maxId FROM Transaction;", function (err, result, fields) {
        if (err) {
            console.error('Error fetching max idTransaction:', err);
            res.status(500).json({ error: 'Internal server error' });
            return;
        }
        let idTransaction = result[0].maxId;
        // Add bills to the transaction
        con.query("UPDATE Transaction SET fivePoundBills = ?, tenPoundBills = ?, twentyPoundBills = ?, fiftyPoundBills = ? WHERE idTransaction = ?", [fivePoundBills, tenPoundBills, twentyPoundBills, fiftyPoundBills, idTransaction], function (err, result, fields) {
            if (err) {
                console.error('Error updating transaction:', err);
                res.status(500).json({ error: 'Internal server error' });
                return;
            }
            res.status(200).json({ status: '200', data: 'Bills added to transaction'});
        });
    });
});

// Validate admin account endpoint
app.post('/adminLogin', (req, res) => {
    const { username, password } = req.body;
    // Check if username and password are known in the database
    con.query("SELECT userName, password FROM Admin WHERE userName = ? AND password = ?", [username, password], function (err, result, fields) {
        if (err) {
            console.error(err);
            res.status(500).json({ status: "500", data: 'Internal Server error' });
            return;
        }

        if (result.length > 0) {
            res.status(200).json({ status: "200", data: 'Admin login successful' });
        } else {
            res.status(401).json({ status: "401", data: 'Invalid credentials' });
        }
    });
});

// Endpoint to fetch all ATM transactions
app.get('/transactions', (req, res) => {
    con.query("SELECT idTransaction, cardNumber, amountInPounds, time FROM Transaction JOIN Card ON Transaction.FK_idCard = Card.idCard;", (err, result) => {
        if (err) {
            console.error('Error fetching transactions:', err);
            res.status(500).json({ error: 'Internal server error' });
            return;
        }
        res.json(result);
    });
});

// Endpoint to fetch ATM bill inventory
app.get('/atm/:id', (req, res) => {
    const atmId = req.params.id;
    con.query("SELECT fivePoundBills, tenPoundBills, twentyPoundBills, fiftyPoundBills FROM ATM WHERE idATM = ?", [atmId], (err, result) => {
        if (err) {
            console.error('Error fetching ATM bill inventory:', err);
            res.status(500).json({ error: 'Internal server error' });
            return;
        }
        if (result.length === 0) {
            res.status(404).json({ error: 'ATM not found' });
            return;
        }
        res.json(result[0]);
    });
});

// Update ATM bill inventory endpoint
app.post('/billAmount', (req, res) => {
    const { fivePoundBills, tenPoundBills, twentyPoundBills, fiftyPoundBills, isAdmin } = req.body;

    if (isAdmin) {
        // Allow admin to change the bill inventory in the database
        con.query("UPDATE ATM SET fivePoundBills = ?, tenPoundBills = ?, twentyPoundBills = ?, fiftyPoundBills = ?", [fivePoundBills, tenPoundBills, twentyPoundBills, fiftyPoundBills], function (err, result, fields) {
            if (err) {
                console.error('Error updating ATM bill inventory:', err);
                res.status(500).json({ error: 'Internal server error' });
                return;
            }
        });
    } else {
        con.querty("SELECT fivePoundBills, tenPoundBills, twentyPoundBills, fiftyPoundBills FROM ATM", function (err, result, fields) {
            if (err) {
                console.error('Error getting ATM bill inventory:', err);
                res.status(500).json({ error: 'Internal server error' });
                return;
            }

            if (fivePoundBills <= result[0].fivePoundBills && tenPoundBills <= result[0].tenPoundBills && twentyPoundBills <= result[0].twentyPoundBills && fiftyPoundBills <= result[0].fiftyPoundBills) {
                // Update bill inventory according to the last transaction
                con.query("UPDATE ATM SET fivePoundBills = (fivePoundBills - ?), tenPoundBills = (tenPoundBills - ?), twentyPoundBills = (twentyPoundBills - ?), fiftyPoundBills = (fiftyPoundBills - ?)", [fivePoundBills, tenPoundBills, twentyPoundBills, fiftyPoundBills], function (err, result, fields) {
                    if (err) {
                        console.error('Error updating ATM bill inventory:', err);
                        res.status(500).json({ error: 'Internal server error' });
                        return;
                    }
                    res.status(200).json({ status: '200', data: 'Bill amount updated' });
                });
            } else {
                res.status(418).json({ status: '418', data: 'Too many bills' });
            }
        });
    }
});

// Regular expression to match the target pattern IMxxDMKBxxxxxxxxxx
const targetPattern = /^IM\d{2}DMKB\d{10}$/;

// Endpoint to get account info for other banks
app.post('/api/accountinfo', (req, res) => {
    const { target } = req.query; // Get the target from query parameters
    const { uid, pincode } = req.body;

    if (isValidUID(uid)) {
        // Validate the target parameter against the pattern
        if (targetPattern.test(target)) {
            con.query("SELECT attempts FROM Card WHERE cardNumber = ?", [uid], function (err, result, fields) {
                if (err) {
                    console.error('Error fetching attempts:', err);
                    res.status(500).json({ error: 'Internal server error' });
                    return;
                }

                if (result[0].attempts > 0) {
                    // Query to verify pincode
                    con.query("SELECT pinCode FROM Card WHERE cardNumber = ?", [uid], function (err, result, fields) {
                        if (err) {
                            console.error('Error fetching pincode:', err);
                            return res.status(500).json({ error: 'Internal server error' });
                        }
                        if (result.length > 0 && result[0].pinCode === pincode) {
                            con.query("UPDATE Card SET attempts = 3 WHERE cardNumber = ?", [uid], function(err, result, fields) {
                                if (err) {
                                    console.error('Error updating attempts:', err);
                                    res.status(500).json({ error: 'Internal server error' });
                                    return;
                                }

                                // Query to get account info
                                con.query("SELECT balanceInCents, fullName FROM Account JOIN Card ON Account.idAccount = Card.idCard JOIN User ON Account.idAccount = User.idUser WHERE cardNumber = ?", [uid], function (err, result, fields) {
                                    console.log(result);
                                    if (err) {
                                        console.error('Error fetching account info:', err);
                                        return res.status(500).json({ error: 'Internal server error' });
                                    }
                                    if (result.length > 0) {
                                        // Send data according to NOOB documentation
                                        const accountInfo = result[0];
                                        const fullName = accountInfo.fullName.split(' ');
                                        const firstName = fullName[0];
                                        const lastName = fullName.slice(1).join(' ');
                                        res.status(200).json({
                                            balance: accountInfo.balanceInCents,
                                            firstname: firstName,
                                            lastname: lastName
                                        });
                                    } else {
                                        res.status(404).json({ error: 'Account not found' });
                                    }
                                });
                            });
                        } else {
                            con.query("UPDATE Card SET attempts = attempts - 1 WHERE cardNumber = ?", [uid], function (err, result, fields) {
                                if (err) {
                                    console.error('Error updating attempts:', err);
                                    res.status(500).json({ error: 'Internal server error' });
                                    return;
                                }
                            });
                            con.query("SELECT attempts FROM Card WHERE cardNumber = ?", [uid], function (err, result, fields) {
                                if (err) throw err;
                                let attemptsRemaining = result[0].attempts;
                                if (attemptsRemaining > 0) {
                                    res.status(401).json({ status: "401", data: "Unauthorized", attempts_remaining: attemptsRemaining });
                                } else {
                                    res.status(403).json({ status: "403", data: "Card is blocked" });
                                }
                            });
                        }
                    });
                } else {
                    res.status(403).json({ status: "403", data: "Card is blocked" });
                }
            });
        }
    } else {
        res.status(401).json({ status: "401", data: "Invalid UID" });
    }
});

// Endpoint to make a withdrawal at other banks
app.post('/api/withdraw', (req, res) => {
    const { target } = req.query;
    const { uid, pincode, amount } = req.body;

    let currentBalance;

    if (targetPattern.test(target)) {
        // Calculate the current time plus 2 hours
        const currentTime = new Date();
        currentTime.setHours(currentTime.getHours() + 2);
        const formattedTime = currentTime.toISOString().slice(0, 19).replace('T', ' ');

        if (lastTransactionDate && currentTime.getDate() !== lastTransactionDate.getDate()) {
            // Reset the daily limit if the date has changed
            con.query("UPDATE Card SET dailyLimit = 1000;", function (err, result, fields) {
                if (err) {
                    console.error('Error resetting daily limit:', err);
                    res.status(500).json({ error: 'Internal server error' });
                    return;
                }
            });
        }

        con.query("SELECT dailyLimit FROM Card WHERE cardNumber = ?", [uid], function (err, result, fields) {
            if (err) {
                console.error("Error fetching dailyLimit:", err);
                res.status(500).json({ error: 'Internal server error' });
                return;
            }

            if (amount <= result[0].dailyLimit) {
                con.query("SELECT balanceInCents FROM Account JOIN Card ON Account.idAccount = Card.idCard JOIN User ON Account.idAccount = User.idUser WHERE cardNumber = ?", [uid], function (err, result, fields) {
                    if (err) {
                        console.error('Error fetching pincode:', err);
                        return res.status(500).json({ error: 'Internal server error' });
                    }

                    currentBalance = result[0].balanceInCents;

                    if (currentBalance >= amount) {
                        con.query("SELECT MAX(idTransaction) AS maxId FROM Transaction;", function (err, result, fields) {
                            if (err) {
                                console.error('Error fetching max idTransaction:', err);
                                res.status(500).json({ error: 'Internal server error' });
                                return;
                            }

                            const newIdTransaction = result[0].maxId + 1;


                            con.query("INSERT INTO Transaction (idTransaction, Fk_idATM, Fk_idCard, amountInPounds, time) VALUES (?, 1, (SELECT idCard FROM Card WHERE cardNumber = ?), ?, ?)", [newIdTransaction, uid, amount / 100, formattedTime], function (err, result, fields) {
                                if (err) {
                                    console.error('Error inserting transaction:', err);
                                    res.status(500).json({ error: 'Internal server error' });
                                    return;
                                }
                            });
                        });

                        con.query("UPDATE Account JOIN Card ON Account.idAccount = Card.idCard SET balanceInCents = (balanceInCents - ?), dailyLimit = (dailyLimit - ?) WHERE cardNumber = ?", [amount, amount, uid], function (err, result, fields) {
                            if (err) {
                                console.error('Error updating balance:', err);
                                return res.status(500).json({ error: 'Internal server error' });
                            }
                            lastTransactionDate = currentTime;
                            res.status(200).json({ status: "200", data: "Balance updated" });
                        });
                    } else {
                        res.status(412).json({ status: "412", data: "Insufficient funds" });
                    }
                });
            } else {
                res.status(429).json({ status: "429", data: "Exceeding daily limit" });
            }
        });
    }
});

// Start server on specified port
app.listen(port, () => {
    console.log(`Server is listening at http://localhost:${port}`);
});