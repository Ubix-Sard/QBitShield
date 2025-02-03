# Decentralized Governance Model  

## Voting Mechanism  
1. **Miner Signaling**:  
   - Miners embed a 4-byte flag `0xQBIT` in coinbase transactions to signal support.  
2. **Threshold**:  
   - Activation requires 90% of blocks over 2,016 blocks (≈2 weeks) to signal.  

## User-Activated Soft Fork (UASF)  
- If miners reject the upgrade after 12 months:  
  - Economic nodes enforce SPHINCS+ validation.  
  - Legacy transactions are deprecated.  
