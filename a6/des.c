#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <inttypes.h>
 /*
 * des takes two arguments on the command line:
 *    des -enc -ecb      -- encrypt in ECB mode
 *    des -enc -ctr      -- encrypt in CTR mode
 *    des -dec -ecb      -- decrypt in ECB mode
 *    des -dec -ctr      -- decrypt in CTR mode
 * des also reads some hardcoded files:
 *    message.txt            -- the ASCII text message to be encrypted,
 *                              read by "des -enc"
 *    encrypted_msg.bin      -- the encrypted message, a binary file,
 *                              written by "des -enc"
 *    decrypted_message.txt  -- the decrypted ASCII text message
 *    key.txt                -- just contains the key, on a line by itself, as an ASCII
 *                              hex number, such as: 0x34FA879B
*/

/////////////////////////////////////////////////////////////////////////////
// Type definitions
/////////////////////////////////////////////////////////////////////////////
typedef uint64_t KEYTYPE;
typedef uint32_t SUBKEYTYPE;
typedef uint64_t BLOCKTYPE;

typedef struct BLOCK {
    BLOCKTYPE block;        // the block read
    int size;               // number of "real" bytes in the block, should be 8, unless it's the last block
    struct BLOCK *next;     // pointer to the next block
} BLOCK;
typedef struct BLOCK* BLOCKLIST;

/////////////////////////////////////////////////////////////////////////////
// Initial and final permutation
/////////////////////////////////////////////////////////////////////////////
uint64_t init_perm[] = {
	58,50,42,34,26,18,10,2,
	60,52,44,36,28,20,12,4,
	62,54,46,38,30,22,14,6,
	64,56,48,40,32,24,16,8,
	57,49,41,33,25,17,9,1,
	59,51,43,35,27,19,11,3,
	61,53,45,37,29,21,13,5,
	63,55,47,39,31,23,15,7
};

int final_perm[] = {
	40,8,48,16,56,24,64,32,
	39,7,47,15,55,23,63,31,
	38,6,46,14,54,22,62,30,
	37,5,45,13,53,21,61,29,
	36,4,44,12,52,20,60,28,
	35,3,43,11,51,19,59,27,
	34,2,42,10,50,18,58,26,
	33,1,41,9, 49,17,57,25
};

/////////////////////////////////////////////////////////////////////////////
// Subkey generation
/////////////////////////////////////////////////////////////////////////////

// This function returns the i:th subkey, 48 bits long. To simplify the assignment
// you can use a trivial implementation: just take the input key and xor it with i,
uint64_t getSubKey(int i, KEYTYPE key) {
   // return the first 48 bits of the 56 bit DES key, xor:ed with i.
   uint64_t subkey = i ^ key;
   return subkey;
}

// For extra credit, write the real DES key expansion routine!
void generateSubKeys(KEYTYPE key) {
   // TODO for extra credit
}

/////////////////////////////////////////////////////////////////////////////
// P-boxes
/////////////////////////////////////////////////////////////////////////////
uint64_t expand_box[] = {
	32,1,2,3,4,5,4,5,6,7,8,9,
	8,9,10,11,12,13,12,13,14,15,16,17,
	16,17,18,19,20,21,20,21,22,23,24,25,
	24,25,26,27,28,29,28,29,30,31,32,1
};

uint32_t Pbox[] =
{
	16,7,20,21,29,12,28,17,1,15,23,26,5,18,31,10,
	2,8,24,14,32,27,3,9,19,13,30,6,22,11,4,25,
};

/////////////////////////////////////////////////////////////////////////////
// S-boxes
/////////////////////////////////////////////////////////////////////////////
uint64_t sbox_1[4][16] = {
	{14,  4, 13,  1,  2, 15, 11,  8,  3, 10 , 6, 12,  5,  9,  0,  7},
	{ 0, 15,  7,  4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5,  3,  8},
	{ 4,  1, 14,  8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10,  5,  0},
	{15, 12,  8,  2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0,  6, 13}};

uint64_t sbox_2[4][16] = {
	{15,  1,  8, 14,  6, 11,  3,  4,  9,  7,  2, 13, 12,  0,  5 ,10},
	{ 3, 13,  4,  7, 15,  2,  8, 14, 12,  0,  1, 10,  6,  9, 11,  5},
	{ 0, 14,  7, 11, 10,  4, 13,  1,  5,  8, 12,  6,  9,  3,  2, 15},
	{13,  8, 10,  1,  3, 15,  4,  2, 11,  6,  7, 12,  0,  5, 14,  9}};

uint64_t sbox_3[4][16] = {
	{10,  0,  9, 14,  6,  3, 15,  5,  1, 13, 12,  7, 11,  4,  2,  8},
	{13,  7,  0,  9,  3,  4,  6, 10,  2,  8,  5, 14, 12, 11, 15,  1},
	{13,  6,  4,  9,  8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7},
	{ 1, 10, 13,  0,  6,  9,  8,  7,  4, 15, 14,  3, 11,  5,  2, 12}};


uint64_t sbox_4[4][16] = {
	{ 7, 13, 14,  3,  0 , 6,  9, 10,  1 , 2 , 8,  5, 11, 12,  4 ,15},
	{13,  8, 11,  5,  6, 15,  0,  3,  4 , 7 , 2, 12,  1, 10, 14,  9},
	{10,  6,  9 , 0, 12, 11,  7, 13 ,15 , 1 , 3, 14 , 5 , 2,  8,  4},
	{ 3, 15,  0,  6, 10,  1, 13,  8,  9 , 4 , 5, 11 ,12 , 7,  2, 14}};


uint64_t sbox_5[4][16] = {
	{ 2, 12,  4,  1 , 7 ,10, 11,  6 , 8 , 5 , 3, 15, 13,  0, 14,  9},
	{14, 11 , 2 ,12 , 4,  7, 13 , 1 , 5 , 0, 15, 10,  3,  9,  8,  6},
	{ 4,  2 , 1, 11, 10, 13,  7 , 8 ,15 , 9, 12,  5,  6 , 3,  0, 14},
	{11,  8 ,12 , 7 , 1, 14 , 2 ,13 , 6 ,15,  0,  9, 10 , 4,  5,  3}};


uint64_t sbox_6[4][16] = {
	{12,  1, 10, 15 , 9 , 2 , 6 , 8 , 0, 13 , 3 , 4 ,14 , 7  ,5 ,11},
	{10, 15,  4,  2,  7, 12 , 9 , 5 , 6,  1 ,13 ,14 , 0 ,11 , 3 , 8},
	{ 9, 14 ,15,  5,  2,  8 ,12 , 3 , 7 , 0,  4 ,10  ,1 ,13 ,11 , 6},
	{ 4,  3,  2, 12 , 9,  5 ,15 ,10, 11 ,14,  1 , 7  ,6 , 0 , 8 ,13}};


uint64_t sbox_7[4][16] = {
	{ 4, 11,  2, 14, 15,  0 , 8, 13, 3,  12 , 9 , 7,  6 ,10 , 6 , 1},
	{13,  0, 11,  7,  4 , 9,  1, 10, 14 , 3 , 5, 12,  2, 15 , 8 , 6},
	{ 1 , 4, 11, 13, 12,  3,  7, 14, 10, 15 , 6,  8,  0,  5 , 9 , 2},
	{ 6, 11, 13 , 8,  1 , 4, 10,  7,  9 , 5 , 0, 15, 14,  2 , 3 ,12}};

uint64_t sbox_8[4][16] = {
	{13,  2,  8,  4,  6 ,15 ,11,  1, 10,  9 , 3, 14,  5,  0, 12,  7},
	{ 1, 15, 13,  8 ,10 , 3  ,7 , 4, 12 , 5,  6 ,11,  0 ,14 , 9 , 2},
	{ 7, 11,  4,  1,  9, 12, 14 , 2,  0  ,6, 10 ,13 ,15 , 3  ,5  ,8},
	{ 2,  1, 14 , 7 , 4, 10,  8, 13, 15, 12,  9,  0 , 3,  5 , 6 ,11}};

/////////////////////////////////////////////////////////////////////////////
// I/O
/////////////////////////////////////////////////////////////////////////////


BLOCK *push(uint64_t block, int size){
  BLOCK *new_node = malloc(sizeof(BLOCK));
  new_node->block = block;
  new_node->size = size;
  new_node->next = NULL;
  return new_node;
}

void print_blocks(BLOCKLIST head){
  BLOCK *cur_node = head;
  int count = 1;
  while(cur_node != NULL){
    printf("Size: %d\n", cur_node->size);
    printf("Block: %" PRIu64"\n", cur_node->block);
    printf("number: %d\n", count);
    printf("\n");
    count++;
    cur_node = cur_node->next;
  }
}

// Pad the list of blocks, so that every block is 64 bits, even if the
// file isn't a perfect multiple of 8 bytes long. In the input list of blocks,
// the last block may have "size" < 8. In this case, it needs to be padded. See
// the slides for how to do this (the last byte of the last block
// should contain the number if real bytes in the block, add an extra block if
// the file is an exact multiple of 8 bytes long.) The returned
// list of blocks will always have the "size"-field=8.
// Example:
//    1) The last block is 5 bytes long: [10,20,30,40,50]. We pad it with 2 bytes,
//       and set the length to 5: [10,20,30,40,50,0,0,5]. This means that the
//       first 5 bytes of the block are "real", the last 3 should be discarded.
//    2) The last block is 8 bytes long: [10,20,30,40,50,60,70,80]. We keep this
//       block as is, and add a new final block: [0,0,0,0,0,0,0,0]. When we decrypt,
//       the entire last block will be discarded since the last byte is 0
BLOCKLIST pad_last_block(BLOCKLIST blocks) {
  BLOCK *cur_node = blocks;
  while(cur_node->next != NULL){
    cur_node = cur_node->next;
  }
  if(cur_node->size < 8){
    cur_node->block = cur_node->block << (8 - cur_node->size) * 8;
    cur_node->block = cur_node->block | cur_node->size;
  } else {
    cur_node->next = push(0, 0);
  }
  return blocks;
}

// Reads the message to be encrypted, an ASCII text file, and returns a linked list
// of BLOCKs, each representing a 64 bit block. In other words, read the first 8 characters
// from the input file, and convert them (just a C cast) to 64 bits; this is your first block.
// Continue to the end of the file.
BLOCKLIST read_cleartext_message(FILE *msg_fp) {
  // call pad_last_block() here to pad the last block!
  char str[8];
  BLOCKLIST head = NULL;
  BLOCK *cur_node;
  int count = 1;
  uint64_t *block;
  char new_str[8];
  while(fread(str, sizeof(char), 8, msg_fp) > 0){
    block = (uint64_t) str;
    if(head == NULL){
      head = (BLOCKLIST)push(*block, strlen(str) - 1);
      cur_node = head;
    } else {
      cur_node->next = push(*block, strlen(str) - 1);
      cur_node = cur_node->next;
    }
    count++;
    memset(str, 0, sizeof(str));
  }
  head = pad_last_block(head);
  //print_blocks(head);
  return head;
}


// Reads the encrypted message, and returns a linked list of blocks, each 64 bits.
// Note that, because of the padding that was done by the encryption, the length of
// this file should always be a multiople of 8 bytes. The output is a linked list of
// 64-bit blocks.
BLOCKLIST read_encrypted_file(FILE *msg_fp) {
    // TODO
   return NULL;
}

// Reads 56-bit key into a 64 bit unsigned int. We will ignore the most significant byte,
// i.e. we'll assume that the top 8 bits are all 0. In real DES, these are used to check
// that the key hasn't been corrupted in transit. The key file is ASCII, consisting of
// exactly one line. That line has a single hex number on it, the key, such as 0x08AB674D9.
KEYTYPE read_key(FILE *key_fp) {
  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  KEYTYPE *key;
  read = getline(&line, &len, key_fp);
  key = (uint64_t) line;
  //printf("key: %s\n", line);
  //printf("numeric key: %" PRIu64 "\n", key);
  return key;
}

// Write the encrypted blocks to file. The encrypted file is in binary, i.e., you can
// just write each 64-bit block directly to the file, without any conversion.
void write_encrypted_message(FILE *msg_fp, BLOCKLIST msg) {
    // TODO
}

// Write the encrypted blocks to file. This is called by the decryption routine.
// The output file is a plain ASCII file, containing the decrypted text message.
void write_decrypted_message(FILE *msg_fp, BLOCKLIST msg) {
    // TODO
}

/////////////////////////////////////////////////////////////////////////////
// Encryption
/////////////////////////////////////////////////////////////////////////////

BLOCKTYPE first_permutation(BLOCKTYPE v){
  BLOCKTYPE permutated_input = 0;
  BLOCKTYPE aux;
  for (int i = 0; i < 56; i++){
    aux = 1;
    aux = aux << (init_perm[i] - 1); //create mask
    aux = aux & v;
    if(init_perm[i] > i){ //shift to the ith bit
      aux = aux >> (init_perm[i] - i - 1);
    } else {
      aux = aux << (i - init_perm[i] + 1);
    }
    permutated_input = permutated_input | aux; //assign the bit
  }
  return permutated_input;
}

BLOCKTYPE get_half(BLOCKTYPE v, int which){
  BLOCKTYPE retVal = 0;
  BLOCKTYPE aux = 1;
  for(int i = 0; i < 31){
    retVal = retVal | aux;
    retVal = retVal << 1;
  }
  if(which == 1){
    retVal = retVal << 32;
  }
  retVal = retVal & v;
  return retVal;
}
// Encrypt one block. This is where the main computation takes place. It takes
// one 64-bit block as input, and returns the encrypted 64-bit block. The
// subkeys needed by the Feistel Network is given by the function getSubKey(i).
BLOCKTYPE des_enc(BLOCKTYPE v, KEYTYPE key){
  // TODO
  BLOCKTYPE p = first_permutation(v);
  KEYTYPE subkey;
  BLOCKTYPE first_half, second_half;
  for(int i = 1; i < 17; i++){
    subkey = getSubKey(i, key);
    first_half = get_half(p, 0);
    second_half = get_half(p, 1);

  }
  return p;
}

// Encrypt the blocks in ECB mode. The blocks have already been padded
// by the input routine. The output is an encrypted list of blocks.
BLOCKLIST des_enc_ECB(BLOCKLIST msg, KEYTYPE key) {
  // TODO
  // Should call des_enc in here repeatedly
  BLOCKTYPE p = des_enc(msg->block, key);
  return NULL;
}

// Same as des_enc_ECB, but encrypt the blocks in Counter mode.
// SEE: https://en.wikipedia.org/wiki/Block_cipher_mode_of_operation#Counter_(CTR)
// Start the counter at 0.
BLOCKLIST des_enc_CTR(BLOCKLIST msg) {
    // TODO
    // Should call des_enc in here repeatedly
   return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// Decryption
/////////////////////////////////////////////////////////////////////////////
// Decrypt one block.
BLOCKTYPE des_dec(BLOCKTYPE v){
    // TODO
   return 0;
}

// Decrypt the blocks in ECB mode. The input is a list of encrypted blocks,
// the output a list of plaintext blocks.
BLOCKLIST des_dec_ECB(BLOCKLIST msg) {
    // TODO
    // Should call des_dec in here repeatedly
   return NULL;
}

// Decrypt the blocks in Counter mode
BLOCKLIST des_dec_CTR(BLOCKLIST msg) {
    // TODO
    // Should call des_enc in here repeatedly
   return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// Main routine
/////////////////////////////////////////////////////////////////////////////

void encrypt (int argc, char **argv, KEYTYPE key) {
      FILE *msg_fp = fopen("message.txt", "r");
      if(msg_fp == NULL){
        printf("Could not open message.txt.\n");
        exit(1);
      }
      BLOCKLIST msg = read_cleartext_message(msg_fp);
      fclose(msg_fp);

      BLOCKLIST encrypted_message;
      if (strcmp(argv[2], "-ecb") == 0) {
         encrypted_message = des_enc_ECB(msg, key);
      } else if (strcmp(argv[2], "-ctr") == 0) {
         encrypted_message = des_enc_CTR(msg);
      } else {
         printf("No such mode.\n");
      };
      FILE *encrypted_msg_fp = fopen("encrypted_msg.bin", "wb");
      write_encrypted_message(encrypted_msg_fp, encrypted_message);
      fclose(encrypted_msg_fp);
}

void decrypt (int argc, char **argv, KEYTYPE key) {
      FILE *encrypted_msg_fp = fopen("encrypted_msg.bin", "wb");
      BLOCKLIST encrypted_message = read_encrypted_file(encrypted_msg_fp);
      fclose(encrypted_msg_fp);

      BLOCKLIST decrypted_message;
      if (strcmp(argv[2], "-ecb") == 0) {
         decrypted_message = des_dec_ECB(encrypted_message);
      } else if (strcmp(argv[2], "-ctr") == 0) {
         decrypted_message = des_dec_CTR(encrypted_message);
      } else {
         printf("No such mode.\n");
      };

      FILE *decrypted_msg_fp = fopen("decrypted_message.txt", "r");
      write_decrypted_message(decrypted_msg_fp, decrypted_message);
      fclose(decrypted_msg_fp);
}

int main(int argc, char **argv){

    if(argc < 3){
      printf("Not enough args \n");
      return 1;
    }

    FILE *key_fp = fopen("key.txt","r");
    KEYTYPE key = read_key(key_fp);
    generateSubKeys(key);              // This does nothing right now.
    fclose(key_fp);

    if (strcmp(argv[1], "-enc") == 0) {
      encrypt(argc, argv, key);
    } else if (strcmp(argv[1], "-dec") == 0) {
      decrypt(argc, argv, key);
    } else {
     printf("First argument should be -enc or -dec\n");
    }
    return 0;
}
