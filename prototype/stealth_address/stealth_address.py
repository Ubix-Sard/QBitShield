import hashlib  
from ecdsa import SECP256k1, SigningKey  

class StealthAddressGenerator:  
    def __init__(self):  
        self.curve = SECP256k1  

    def generate_shared_secret(self, sender_privkey: bytes, receiver_pubkey: bytes) -> bytes:  
        """Derive ECDH shared secret using secp256k1."""  
        shared_point = sender_privkey * receiver_pubkey  
        return hashlib.sha3_256(shared_point.to_bytes(32, "big")).digest()  

    def generate_stealth_address(self, shared_secret: bytes, receiver_pubkey: bytes) -> str:  
        """Generate one-time stealth address using SHA3-256."""  
        return hashlib.sha3_256(shared_secret + receiver_pubkey).hexdigest()  

# Example Usage  
if __name__ == "__main__":  
    # Sender's private key (testnet only)  
    sender_privkey = SigningKey.generate(curve=SECP256k1).to_string()  
    # Receiver's public key (hex)  
    receiver_pubkey = bytes.fromhex("02abcdef123456...")  

    generator = StealthAddressGenerator()  
    shared_secret = generator.generate_shared_secret(sender_privkey, receiver_pubkey)  
    stealth_address = generator.generate_stealth_address(shared_secret, receiver_pubkey)  
    print(f"Stealth Address: {stealth_address}")  
