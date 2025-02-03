#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <openssl/ec.h>  
#include <openssl/ecdsa.h>  
#include <openssl/obj_mac.h>  
#include <sphincs/sphincs.h>  

// Define hybrid signature structure  
typedef struct {  
    unsigned char* ecdsa_der;  
    size_t ecdsa_len;  
    unsigned char* sphincs_sig;  
    size_t sphincs_len;  
} HybridSignature;  

// Hybrid signing function  
HybridSignature* hybrid_sign(const unsigned char* tx_data, size_t tx_len, EC_KEY* ecdsa_key, const sphincs_secret_key* sphincs_sk) {  
    HybridSignature* hybrid_sig = malloc(sizeof(HybridSignature));  
    if (!hybrid_sig) {  
        perror("Failed to allocate memory for hybrid signature");  
        return NULL;  
    }  

    // --- ECDSA Signing ---  
    ECDSA_SIG* ecdsa_sig = ECDSA_do_sign(tx_data, tx_len, ecdsa_key);  
    if (!ecdsa_sig) {  
        fprintf(stderr, "ECDSA signing failed\n");  
        free(hybrid_sig);  
        return NULL;  
    }  

    // Serialize ECDSA signature to DER format  
    hybrid_sig->ecdsa_len = i2d_ECDSA_SIG(ecdsa_sig, &hybrid_sig->ecdsa_der);  
    if (hybrid_sig->ecdsa_len <= 0) {  
        fprintf(stderr, "ECDSA DER serialization failed\n");  
        ECDSA_SIG_free(ecdsa_sig);  
        free(hybrid_sig);  
        return NULL;  
    }  

    // --- SPHINCS+ Signing ---  
    hybrid_sig->sphincs_len = SPHINCS_SIG_LENGTH;  
    hybrid_sig->sphincs_sig = malloc(hybrid_sig->sphincs_len);  
    if (!hybrid_sig->sphincs_sig) {  
        perror("Failed to allocate memory for SPHINCS+ signature");  
        free(hybrid_sig->ecdsa_der);  
        ECDSA_SIG_free(ecdsa_sig);  
        free(hybrid_sig);  
        return NULL;  
    }  

    // Sign the ECDSA DER bytes with SPHINCS+  
    if (sphincs_sign(hybrid_sig->sphincs_sig, hybrid_sig->ecdsa_der, hybrid_sig->ecdsa_len, sphincs_sk) != 0) {  
        fprintf(stderr, "SPHINCS+ signing failed\n");  
        free(hybrid_sig->sphincs_sig);  
        free(hybrid_sig->ecdsa_der);  
        ECDSA_SIG_free(ecdsa_sig);  
        free(hybrid_sig);  
        return NULL;  
    }  

    // Cleanup  
    ECDSA_SIG_free(ecdsa_sig);  
    return hybrid_sig;  
}  

// Serialize hybrid signature to byte stream  
unsigned char* serialize_hybrid_sig(const HybridSignature* sig, size_t* total_len) {  
    *total_len = sizeof(size_t) + sig->ecdsa_len + sizeof(size_t) + sig->sphincs_len;  
    unsigned char* buffer = malloc(*total_len);  
    if (!buffer) return NULL;  

    unsigned char* ptr = buffer;  

    // Write ECDSA length & data  
    memcpy(ptr, &sig->ecdsa_len, sizeof(size_t));  
    ptr += sizeof(size_t);  
    memcpy(ptr, sig->ecdsa_der, sig->ecdsa_len);  
    ptr += sig->ecdsa_len;  

    // Write SPHINCS+ length & data  
    memcpy(ptr, &sig->sphincs_len, sizeof(size_t));  
    ptr += sizeof(size_t);  
    memcpy(ptr, sig->sphincs_sig, sig->sphincs_len);  

    return buffer;  
}  

// Free hybrid signature  
void hybrid_sig_free(HybridSignature* sig) {  
    if (sig) {  
        free(sig->ecdsa_der);  
        free(sig->sphincs_sig);  
        free(sig);  
    }  
}  

// Example usage  
int main() {  
    // Initialize ECDSA key (secp256k1)  
    EC_KEY* ecdsa_key = EC_KEY_new_by_curve_name(NID_secp256k1);  
    EC_KEY_generate_key(ecdsa_key);  

    // Initialize SPHINCS+ key (example)  
    sphincs_secret_key sphincs_sk;  
    // ... (keygen logic here)  

    // Sign transaction data  
    const char* tx_data = "Quantum-resistant transaction";  
    HybridSignature* sig = hybrid_sign((const unsigned char*)tx_data, strlen(tx_data), ecdsa_key, &sphincs_sk);  

    if (sig) {  
        size_t total_len;  
        unsigned char* serialized = serialize_hybrid_sig(sig, &total_len);  
        printf("Hybrid signature serialized (%zu bytes)\n", total_len);  
        free(serialized);  
        hybrid_sig_free(sig);  
    }  

    EC_KEY_free(ecdsa_key);  
    return 0;  
}  
