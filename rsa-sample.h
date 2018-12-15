#define CHUNK_SIZE 50
typedef struct rsa_variables{
    int start_index ;
    char message[CHUNK_SIZE+CHUNK_SIZE+1 ];
    int temp_message[CHUNK_SIZE+CHUNK_SIZE+1];
    int decrypted[CHUNK_SIZE+CHUNK_SIZE+1];
    int en_message[CHUNK_SIZE+CHUNK_SIZE+1];
}rsa_variable;

void encrypt(rsa_variable *variable );
void decrypt(rsa_variable * variable);
int rsa_init();