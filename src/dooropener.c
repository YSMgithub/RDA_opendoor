#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <string.h>
#include "dooropener.h"

int main(int argc, char *argv[]) {
    data_t data_s;
    initStruct(&data_s);
    int getArgs (argc, argv, &data_s);
    //dataReader(&data_s);
    //readFromFile (&data_s);
    printf("\n token = %s\n", data_s.token);
    printf("\n user = %s\n", data_s.user);
    printf("\n password = %s\n", data_s.password);
    gettokenfunc(&data_s);
    //writeToFile(&data_s);
    sendPostFunc(&data_s);

    
    return 0;
}


int getArgs (int argc, char *argv[], data_t * data_s) {
  if (argc > 0) {
      for (int i = 1; i < argc; i++) {
      if (strcmp(argv[i], "-u") == 0) {
          // Аргумент -u содержит имя пользователя
          i++;
          strcpy(data_s->user, argv[i]);
      } else if (strcmp(argv[i], "-p") == 0) {
          // Аргумент -p содержит пароль
          i++;
          strcpy(data_s->password, argv[i]);
      } else if (strcmp(argv[i], "-i") == 0) {
          // Аргумент -i содержит ID (целое число)
          i++;
          strcpy(data_s->rdaID, argv[i]);
      } else {
        printf("Not valid args");
        return 1;
      }
    }
  }
  return 0;
}

void initStruct(data_t *data_s) {
  data_s->rdaID = malloc(MAX_NAME_LENGTHS);
  data_s->user = malloc(MAX_NAME_LENGTHS);
  data_s->password = malloc(MAX_NAME_LENGTHS);
  data_s->token = malloc(MAX_TOKEN_LENGTHS);
  if (data_s->rdaID == NULL || data_s->rdaID == NULL || data_s->rdaID == NULL || data_s->rdaID == NULL) {
    fprintf(stderr, "malloc() failed\n");
    exit(EXIT_FAILURE);
  }
  data_s->rdaID[0] = '\0';
  data_s->user[0] = '\0';
  data_s->password[0] = '\0';
  data_s->token[0] = '\0';

}


void dataReader(data_t *data_s) {

  printf("Enter RDA ID: ");
  scanf("%s", data_s->rdaID);
  
  printf("Enter username: ");
  scanf("%s", data_s->user);
  
  printf("Enter password: ");
  scanf("%s", data_s->password);
}

int gettokenfunc(data_t *data_s) {

CURL *curl;
CURLcode res;
curl = curl_easy_init();
if(curl) {
  char data[512] = "grant_type=password&client_id=machine&username=";
  strcat(data, data_s->user);
  strcat(data, "&password=");
  strcat(data, data_s->password);
  curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
  curl_easy_setopt(curl, CURLOPT_URL, "https://rdba.rosdomofon.com/authserver-service/oauth/token");
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
  curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "https");
  struct curl_slist *headers = NULL;
  headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, data_s);
  
  res = curl_easy_perform(curl);
}
curl_easy_cleanup(curl);

    return 0;
}

size_t writefunc(void *ptr, size_t size, size_t nmemb, data_t *data_s) {
  char sep[10] = "\"";
   char *istr;
   istr = strtok (ptr,sep);
   for (int i = 0; i < 3; i++)
   {
      istr = strtok (NULL, sep);
   }
    int sizeISt = 0;
    for (int i = 0;istr[i] != '\0'; i++) {
        sizeISt++;
    }


  memcpy(data_s->token, istr, strlen(istr));
  return size*nmemb;
}

void sendPostFunc (data_t *data_s) {

    CURL *curl;
    CURLcode res;
    char auth[453] = "Authorization: Bearer ";
    strcat(auth, data_s->token);
    curl = curl_easy_init();
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
        curl_easy_setopt(curl, CURLOPT_URL, "https://rdba.rosdomofon.com/rdas-service/api/v1/rdas/b827eb82eb81/activate_key");
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "https");
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, auth);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        const char *data = "{\r\n    \"rele\": 3\r\n}";
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
        res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
}

int writeToFile (data_t *data_s) {
  FILE *f = fopen("config.txt", "w");
    if (f == NULL) {
        printf("Failed to open file for writing.\n");
        return 1;
    }

    fprintf(f, "%s\n", data_s->token);
    fprintf(f, "%s\n", data_s->user);
    fprintf(f, "%s\n", data_s->password);
    fprintf(f, "%s\n", data_s->rdaID);
    fclose(f);
  
  return 0;
}

void readFromFile (data_t *data_s) {
    FILE *file;
    char line[MAX_TOKEN_LENGTHS];
    file = fopen("config.txt", "r");
    if (file == NULL) {
        printf("Ошибка открытия файла!");
        exit(1);
    }

    fgets(data_s->token, MAX_TOKEN_LENGTHS, file);
    fgets(data_s->user, MAX_NAME_LENGTHS, file);
    fgets(data_s->password, MAX_NAME_LENGTHS, file);
    fgets(data_s->rdaID, MAX_NAME_LENGTHS, file);
    // закрыть файл
    fclose(file);
}