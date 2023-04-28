#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <string.h>
#include "dooropener.h"

int main() {
    data_t data;
    initStruct(&data);
    return 0;
}


void initStruct(data_t *data) {
  data->rdaID = malloc(MAX_NAME_LENGTHS);
  data->user = malloc(MAX_NAME_LENGTHS);
  data->password = malloc(MAX_NAME_LENGTHS);
  data->token = malloc(MAX_TOKEN_LENGTHS);
  if (data->rdaID == NULL || data->rdaID == NULL || data->rdaID == NULL || data->rdaID == NULL) {
    fprintf(stderr, "malloc() failed\n");
    exit(EXIT_FAILURE);
  }
  data->rdaID[0] = '\0';
  data->user[0] = '\0';
  data->password[0] = '\0';
  data->token[0] = '\0';

}

