# QBit Shield Signet Deployment Guide  

## 1. Prerequisites  
- Bitcoin Core v26.0+ compiled with QBit Shield patches.  
- Python 3.12+ for prototyping tools.  

## 2. Node Configuration  
```bash  
# Start Quantum Signet Node  
bitcoind -signet -daemon -quantumshield=1 -whitelist=127.0.0.1  

# Generate stealth address  
python3 prototype/stealth_address/stealth_address.py  
