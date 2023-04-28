#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <string.h>

struct string {
  char *ptr;
  size_t len;
  char *token;
  char rdaID[100];
  char login[100];
  char password[100];

  int countData;
};

void writeToFile (struct string *s) {
    FILE *file;
    char line[100];
    char *param_name;
    char *param_value;

    // попытаться открыть файл config.cfg
    file = fopen("config.cfg", "r");
    if (file == NULL) {
        printf("Ошибка открытия файла!");
        exit(1);
    }

    // читать строки из файла и записывать значение параметра в соответствующее поле структуры
    while (fgets(line, 100, file) != NULL) {
        param_name = strtok(line, "=");
        param_value = strtok(NULL, "=");
        if (strcmp(param_name, "param1") == 0) {
            strcpy(s->rdaID, param_value);
        } else if (strcmp(param_name, "param2") == 0) {
            strcpy(config.param2, param_value);
        } else if (strcmp(param_name, "param3") == 0) {
            strcpy(config.param3, param_value);
        }
    }

    // закрыть файл
    fclose(file);

    // вывести данные на экран
    printf("param1: %s\n", config.param1);
    printf("param2: %s\n", config.param2);
    printf("param3: %s\n", config.param3);

    return 0;
}


void init_string(struct string *s) {
  s->len = 0;
  s->countData = 0;
  s->ptr = malloc(s->len+1);
  s->token = malloc(s->len+1);
  s->ptr = malloc(s->len+1);
  s->token = malloc(s->len+1);
  s->data = malloc(sizeof(char*) * 3);
  if (s->ptr == NULL ||  s->token == NULL || s->data == NULL) {
    fprintf(stderr, "malloc() failed\n");
    exit(EXIT_FAILURE);
  }
  s->ptr[0] = '\0';
  s->token[0] = '\0';
}

size_t writefunc(void *ptr, size_t size, size_t nmemb, struct string *s) {
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



int gettokenfunc(struct string *s) {
 
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
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, s);
  res = curl_easy_perform(curl);
}
curl_easy_cleanup(curl);

    return 0;
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

void sendPostFunc (struct string *s) {

    CURL *curl;
    CURLcode res;
    char auth[453] = "Authorization: Bearer ";
    strcat(auth, s->token);
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

int main () {
    struct string s;
    //writeToFile();
    readFromFile(&s);
    init_string(&s);
    gettokenfunc(&s);
    parserTokenFunc(&s);
    sendPostFunc(&s);
    free(s.ptr);
    free(s.token);
    
    for (int i = 0; i < s.countData; i++) {
        printf("str %d: %s", i+1, s.data[i]);
    }
    
    
    for (int i = 0; i < s.countData; i++) {
        free(s.data[i]);
    }
    free(s.data);
    return 0;
}