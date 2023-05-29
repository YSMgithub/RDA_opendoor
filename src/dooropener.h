
#ifndef SRC_DOOROPENER_H_
#define SRC_DOOROPENER_H_

#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME_LENGTHS 160
#define MAX_TOKEN_LENGTHS 800

typedef struct flags_s {
    int dataChgFlg;
    int doorOpen;
    int getOrPost;
    int whileCounter;
} flags_t;

typedef struct data_s {
    char *rdaID;
    char *user;
    char *password;
    char *token;
    char *chunk;
    flags_t flags;
} data_t;

void initStruct(data_t *data_s);

void dataReader(data_t *data_s);

int gettokenfunc(data_t *data_s);

size_t writefunc(void *ptr, size_t size, size_t nmemb, data_t *data_s);

int sendPostFunc(data_t *data_s);

int writeToFile(data_t *data_s);

void readFromFile(data_t *data_s);

int getArgs(int argc, char *argv[], data_t *data_s);

void memFree(data_t *data_s);

#endif  // SRC_DOOROPENER_H_