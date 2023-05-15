#ifndef SRC_DOOROPENER_H_
#define SRC_DOOROPENER_H_

#define MAX_NAME_LENGTHS 160
#define MAX_TOKEN_LENGTHS 800

typedef struct data_s {
    char * rdaID;
    char * user;
    char * password;
    char * token;
    char * ptr;
} data_t;


void initStruct(data_t * data_s);

void dataReader(data_t * data_s);

int gettokenfunc(data_t * data_s);

size_t writefunc(void *ptr, size_t size, size_t nmemb, data_t *data_s);

void sendPostFunc (data_t *data_s);

#endif  // SRC_DOOROPENER_H_"