#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <string.h>
#include "dooropener.h"

int main() {
    data_t data_s;
    initStruct(&data_s);
    dataReader(&data_s);

    printf("\n%s\n", data_s.user);
    return 0;
}


void initStruct(data_t *data_s) {
  data_s->rdaID = malloc(MAX_NAME_LENGTHS);
  data_s->user = malloc(MAX_NAME_LENGTHS);
  data_s->password = malloc(MAX_NAME_LENGTHS);
  data_s->token = malloc(MAX_TOKEN_LENGTHS);
  if (data->rdaID == NULL || data->rdaID == NULL || data->rdaID == NULL || data->rdaID == NULL) {
    fprintf(stderr, "malloc() failed\n");
    exit(EXIT_FAILURE);
  }
  data->rdaID[0] = '\0';
  data->user[0] = '\0';
  data->password[0] = '\0';
  data->token[0] = '\0';

}


void dataReader(data_t *data) {

  printf("Enter RDA ID: ");
  scanf("%s", data->rdaID);
  
  printf("Enter username: ");
  scanf("%s", data->user);
  
  printf("Enter password: ");
  scanf("%s", data->password);
}

int gettokenfunc(data_t *data) {
 
CURL *curl;
CURLcode res;
curl = curl_easy_init();
if(curl) {
  curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
  curl_easy_setopt(curl, CURLOPT_URL, "https://rdba.rosdomofon.com/authserver-service/oauth/token");
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
  curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "https");
  struct curl_slist *headers = NULL;
  headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  const char *data = "grant_type=password&client_id=machine&username=p.kuharenok&password=YswE571$";
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, data);
  res = curl_easy_perform(curl);
}
curl_easy_cleanup(curl);

    return 0;
}

size_t writefunc(void *ptr, size_t size, size_t nmemb, data_t *data) {
  size_t new_len = s->len + size*nmemb;
  s->ptr = realloc(s->ptr, new_len+1);
  if (s->ptr == NULL) {
    fprintf(stderr, "realloc() failed\n");
    exit(EXIT_FAILURE);
  }
  memcpy(s->ptr+s->len, ptr, size*nmemb);
  s->ptr[new_len] = '\0';
  s->len = new_len;

  return size*nmemb;
}

void parserTokenFunc (struct string *s) {

    char sep[10] = "\"";
   char *istr;
   istr = strtok (s->ptr,sep);
   for (int i = 0; i < 3; i++)
   {
      istr = strtok (NULL, sep);
   }
    int sizeISt = 0;
    for (int i = 0;istr[i] != '\0'; i++) {
        sizeISt++;
    }
    s->token = realloc(s->token, sizeISt);
    if (s->token == NULL) {
        fprintf(stderr, "realloc() failed\n");
        exit(EXIT_FAILURE);
    }
    memcpy(s->token, istr, sizeISt);
    s->token[sizeISt] = '\0';
}