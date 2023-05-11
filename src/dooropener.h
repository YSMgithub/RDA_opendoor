#ifndef SRC_DOOROPENER_H_
#define SRC_DOOROPENER_H_

#define MAX_NAME_LENGTHS 160
#define MAX_TOKEN_LENGTHS 512

typedef struct data_s {
    char * rdaID;
    char * user;
    char * password;
    int * token;
} data_t;


void initStruct(data_t *data);

void dataReader(data_t *data);

#endif  // SRC_DOOROPENER_H_"