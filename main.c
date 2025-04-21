#include <stdio.h>
#include <stdint.h>

int IP[] = {2, 6, 3, 1, 4, 8, 5, 7};

int IP_inv[] = {4, 1, 3, 5, 7, 2, 8, 6};

int EP[] = {4, 1, 2, 3, 2, 3, 4, 1};

int S_BOX[2][4][4] = {
    { {1, 0, 3, 2}, {3, 2, 1, 0}, {0, 2, 1, 3}, {3, 1, 0, 2} },
    { {0, 1, 2, 3}, {2, 0, 1, 3}, {3, 0, 1, 2}, {2, 1, 0, 3} }
};

uint8_t permute(uint8_t input, int *table, int size) {
    uint8_t output = 0;
    for (int i = 0; i < size; i++) {
        output |= ((input >> (8 - table[i])) & 1) << (size - 1 - i);
    }
    return output;
}

uint8_t s_box_substitution(uint8_t input, int sbox[4][4]) {
    int row = ((input & 0x08) >> 2) | (input & 0x01); 
    int col = (input & 0x06) >> 1;                   
    return sbox[row][col];
}

uint8_t feistel(uint8_t right, uint8_t key) {
    uint8_t expanded = permute(right, EP, 8);
    uint8_t xored = expanded ^ key;

    uint8_t left_half = (xored & 0xF0) >> 4;
    uint8_t right_half = xored & 0x0F;

    uint8_t s_output = (s_box_substitution(left_half, S_BOX[0]) << 2) |
                       s_box_substitution(right_half, S_BOX[1]);
    return s_output;
}

uint8_t des_round(uint8_t input, uint8_t key) {
    uint8_t left = (input & 0xF0) >> 4;
    uint8_t right = input & 0x0F;

    uint8_t new_right = left ^ feistel(right, key);

    return (right << 4) | new_right;
}


uint8_t encrypt(uint8_t plaintext, uint8_t key1, uint8_t key2) {
    uint8_t permuted = permute(plaintext, IP, 8);
    uint8_t round1 = des_round(permuted, key1);
    uint8_t swapped = (round1 << 4) | (round1 >> 4); 
    uint8_t round2 = des_round(swapped, key2);
    return permute(round2, IP_inv, 8);
}

uint8_t decrypt(uint8_t ciphertext, uint8_t key1, uint8_t key2) {
    uint8_t permuted = permute(ciphertext, IP, 8);
    uint8_t round1 = des_round(permuted, key2);
    uint8_t swapped = (round1 << 4) | (round1 >> 4);
    uint8_t round2 = des_round(swapped, key1);
    return permute(round2, IP_inv, 8);
}

int main() {
    uint8_t plaintext = 0b11010101;  
    uint8_t key1 = 0b10101010;       
    uint8_t key2 = 0b01010101;       

    uint8_t ciphertext = encrypt(plaintext, key1, key2);
    printf("Encrypted: %02X\n", ciphertext);

    uint8_t decrypted = decrypt(ciphertext, key1, key2);
    printf("Decrypted: %02X\n", decrypted);

    return 0;
}

