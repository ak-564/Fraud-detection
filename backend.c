#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_ACCOUNTS 10
#define MAX_TRANSACTIONS 100
#define HIGH_VALUE_THRESHOLD 50000
#define RAPID_TIME_WINDOW 60
#define TRAVEL_TIME_WINDOW 600

typedef struct {
    int accNo;
    char name[50];
    double balance;
    char lastLocation[30];
    time_t lastTxnTime;
} Account;

typedef struct {
    int txnId;
    int accNo;
    double amount;
    char location[30];
    time_t timestamp;
} Transaction;

Account accounts[MAX_ACCOUNTS];
Transaction transactions[MAX_TRANSACTIONS];
int txnCount = 0;

/**
 * Initialize sample accounts
 */
void setupAccounts() {
    printf("Initializing accounts...\n");
    for (int i = 0; i < MAX_ACCOUNTS; i++) {
        accounts[i].accNo = 100 + i;
        sprintf(accounts[i].name, "Customer%d", i + 1);
        accounts[i].balance = 100000.0;
        strcpy(accounts[i].lastLocation, "Unknown");
        accounts[i].lastTxnTime = 0;
    }
}

/**
 * Detect fraudulent patterns in transaction
 */
void checkFraud(Transaction txn, char alerts[1024]) {
    alerts[0] = '\0';
    Account *acc = NULL;
    
    // Find account
    for (int i = 0; i < MAX_ACCOUNTS; i++) {
        if (accounts[i].accNo == txn.accNo) { 
            acc = &accounts[i]; 
            break; 
        }
    }
    
    if (!acc) {
        strcpy(alerts, "\"Account not found\"");
        return;
    }

    int alertCount = 0;

    // Rule 1: High-value transaction
    if (txn.amount > HIGH_VALUE_THRESHOLD) {
        strcat(alerts, "\"High-value transaction detected\"");
        alertCount++;
    }

    // Rule 2: Rapid multiple transactions
    int recentCount = 0;
    for (int i = txnCount - 1; i >= 0; i--) {
        if (transactions[i].accNo == txn.accNo) {
            double diff = difftime(txn.timestamp, transactions[i].timestamp);
            if (diff <= RAPID_TIME_WINDOW) recentCount++;
            if (recentCount >= 2) break; // Already found enough
        }
    }
    
    if (recentCount >= 2) { // Current + 2 previous = 3 total
        if (alertCount > 0) strcat(alerts, ",");
        strcat(alerts, "\"Rapid multiple transactions detected\"");
        alertCount++;
    }

    // Rule 3: Impossible travel
    if (strlen(acc->lastLocation) > 0 && strcmp(acc->lastLocation, "Unknown") != 0) {
        double diff = difftime(txn.timestamp, acc->lastTxnTime);
        if (strcmp(acc->lastLocation, txn.location) != 0 && diff < TRAVEL_TIME_WINDOW) {
            if (alertCount > 0) strcat(alerts, ",");
            strcat(alerts, "\"Impossible travel detected\"");
            alertCount++;
        }
    }

    // Update account's last transaction info
    strcpy(acc->lastLocation, txn.location);
    acc->lastTxnTime = txn.timestamp;
}

/**
 * Process a transaction and check for fraud
 */
void performTransaction(int accNo, double amount, const char *location) {
    if (txnCount >= MAX_TRANSACTIONS) { 
        printf("{\"alerts\":[\"Transaction limit reached\"],\"success\":false}");
        return; 
    }

    Transaction txn;
    txn.txnId = txnCount + 1;
    txn.accNo = accNo;
    txn.amount = amount;
    strcpy(txn.location, location);
    txn.timestamp = time(NULL);

    // Check account existence and balance
    int accountFound = 0;
    for (int i = 0; i < MAX_ACCOUNTS; i++) {
        if (accounts[i].accNo == accNo) {
            accountFound = 1;
            if (accounts[i].balance >= amount) {
                accounts[i].balance -= amount;
                transactions[txnCount++] = txn;
                
                char alerts[1024] = "";
                checkFraud(txn, alerts);
                
                if (strlen(alerts) == 0) {
                    printf("{\"alerts\":[],\"success\":true,\"balance\":%.2f}", accounts[i].balance);
                } else {
                    printf("{\"alerts\":[%s],\"success\":true,\"balance\":%.2f}", alerts, accounts[i].balance);
                }
            } else {
                printf("{\"alerts\":[\"Insufficient balance\"],\"success\":false}");
            }
            break;
        }
    }
    
    if (!accountFound) {
        printf("{\"alerts\":[\"Account not found\"],\"success\":false}");
    }
}

/**
 * Main function - handles command line arguments
 */
int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("{\"alerts\":[\"Usage: ./fraudBackend <accNo> <amount> <location>\"],\"success\":false}");
        return 1;
    }

    setupAccounts();
    
    int accNo = atoi(argv[1]);
    double amount = atof(argv[2]);
    char *location = argv[3];

    // Input validation
    if (accNo < 100 || accNo > 109) {
        printf("{\"alerts\":[\"Invalid account number. Use 100-109\"],\"success\":false}");
        return 1;
    }
    
    if (amount <= 0) {
        printf("{\"alerts\":[\"Invalid amount\"],\"success\":false}");
        return 1;
    }

    performTransaction(accNo, amount, location);
    return 0;
}
