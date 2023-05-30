
#include "dooropener.h"

int main(int argc, char *argv[]) {
    data_t data_s;
    initStruct(&data_s);
    getArgs(argc, argv, &data_s);
    readFromFile(&data_s);
    dataReader(&data_s);
    while (sendPostFunc(&data_s)) {
        if (gettokenfunc(&data_s)) {
            printf("\nAuth failed\n");
            data_s.user[0] = '\0';
            data_s.password[0] = '\0';
            dataReader(&data_s);
        }
        if (data_s.flags.whileCounter == 3) {
            break;
        }
        data_s.flags.whileCounter++;
    }
    if (!data_s.flags.doorOpen) writeToFile(&data_s);
    memFree(&data_s);
    return 0;
}

int getArgs(int argc, char *argv[], data_t *data_s) {
    if (argc > 0) {
        data_s->flags.dataChgFlg = 1;
        for (int i = 1; i < argc; i++) {
            if (strcmp(argv[i], "-u") == 0) {
                i++;
                strcpy(data_s->user, argv[i]);
            } else if (strcmp(argv[i], "-p") == 0) {
                i++;
                strcpy(data_s->password, argv[i]);
            } else if (strcmp(argv[i], "-i") == 0) {
                i++;
                strcpy(data_s->rdaID, argv[i]);
            } else if (strcmp(argv[i], "--reset") == 0) {
                i++;
                data_s->rdaID[0] = '\0';
                data_s->user[0] = '\0';
                data_s->password[0] = '\0';
                data_s->token[0] = '\0';
                writeToFile(data_s);
                break;
            } else {
                printf("Not valid args");
                data_s->flags.dataChgFlg = 0;
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
    data_s->chunk = malloc(MAX_TOKEN_LENGTHS);
    if (data_s->rdaID == NULL || data_s->rdaID == NULL ||
        data_s->rdaID == NULL || data_s->rdaID == NULL) {
        fprintf(stderr, "malloc() failed\n");
        exit(EXIT_FAILURE);
    }
    data_s->rdaID[0] = '\0';
    data_s->user[0] = '\0';
    data_s->password[0] = '\0';
    data_s->token[0] = '\0';
    data_s->chunk[0] = '\0';
    data_s->flags.dataChgFlg = 0;
    data_s->flags.doorOpen = 1;
    data_s->flags.getOrPost = 0;
    data_s->flags.whileCounter = 0;
}

void dataReader(data_t *data_s) {
    if (data_s->rdaID[0] == '\0') {
        printf("Enter RDA ID: ");
        scanf("%s", data_s->rdaID);
    }

    if (data_s->user[0] == '\0') {
        printf("Enter username: ");
        scanf("%s", data_s->user);
    }

    if (data_s->password[0] == '\0') {
        printf("Enter password: ");
        scanf("%s", data_s->password);
    }
    data_s->flags.dataChgFlg = 1;
}

int gettokenfunc(data_t *data_s) {
    data_s->flags.getOrPost = 1;
    int flg = 0;
    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();
    if (curl) {
        char data[512] = "grant_type=password&client_id=machine&username=";
        strcat(data, data_s->user);
        strcat(data, "&password=");
        strcat(data, data_s->password);
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
        curl_easy_setopt(
            curl, CURLOPT_URL,
            "https://rdba.rosdomofon.com/authserver-service/oauth/token");
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "https");
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(
            headers, "Content-Type: application/x-www-form-urlencoded");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, data_s);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK || !strncmp(data_s->token, "unauth", 6)) {
            flg = 1;
        }

        if (flg == 0) {
            printf("\n<< GET TOKEN SUCCESS >>\n");
        }
        curl_easy_cleanup(curl);
    }

    return flg;
}

size_t writefunc(void *ptr, size_t size, size_t nmemb, data_t *data_s) {
    char sep[10] = "\"";
    char *istr;
    istr = strtok(ptr, sep);
    for (int i = 0; i < 3; i++) {
        istr = strtok(NULL, sep);
    }
    if (data_s->flags.getOrPost == 1) {
        memcpy(data_s->token, istr, strlen(istr));
    } else if (data_s->flags.getOrPost == 2) {
        memcpy(data_s->chunk, istr, strlen(istr));
    }
    return size * nmemb;
}

int sendPostFunc(data_t *data_s) {
    data_s->flags.getOrPost = 2;
    int flg = 0;
    CURL *curl;
    CURLcode res;
    char auth[453] = "Authorization: Bearer ";
    strcat(auth, data_s->token);
    curl = curl_easy_init();
    char url[456] = "https://rdba.rosdomofon.com/rdas-service/api/v1/rdas/";
    strcat(url, data_s->rdaID);
    strcat(url, "/activate_key");
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "https");
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, auth);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    const char *data = "{\r\n    \"rele\": 3\r\n}";
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, data_s);
    res = curl_easy_perform(curl);

    if (res != CURLE_OK || !strncmp(data_s->chunk, "invalid", 7)) {
        flg = 1;
    }

    if (!strncmp(data_s->chunk, "RDA", 3)) {
        printf("\n<< Wrong RDA ID >>\n");
        data_s->rdaID[0] = '\0';
        dataReader(data_s);
        flg = 1;
    }

    data_s->flags.doorOpen = flg;
    if (!flg) {
        printf("\n<< DOOR OPEN >>\n");
    }
    curl_easy_cleanup(curl);

    return flg;
}

int writeToFile(data_t *data_s) {
    FILE *file = fopen("config.txt", "w");
    if (file == NULL) {
        printf("Failed to open file for writing.\n");
        return 1;
    }
    fprintf(file, "%s\n", data_s->token);
    fprintf(file, "%s\n", data_s->user);
    fprintf(file, "%s\n", data_s->password);
    fprintf(file, "%s\n", data_s->rdaID);

    fclose(file);

    return 0;
}

void readFromFile(data_t *data_s) {
    FILE *file;
    char line[MAX_TOKEN_LENGTHS];
    file = fopen("config.txt", "r");
    if (file == NULL) {
        printf("Failed to open file");
        exit(1);
    }
    char buffer[MAX_TOKEN_LENGTHS];

    fgets(buffer, MAX_TOKEN_LENGTHS, file);
    if (data_s->token[0] == '\0') {
        strcpy(data_s->token, buffer);
        data_s->token[strlen(data_s->token) - 1] = '\0';
    }

    fgets(buffer, MAX_TOKEN_LENGTHS, file);
    if (data_s->user[0] == '\0') {
        strcpy(data_s->user, buffer);
        data_s->user[strlen(data_s->user) - 1] = '\0';
    }

    fgets(buffer, MAX_TOKEN_LENGTHS, file);
    if (data_s->password[0] == '\0') {
        strcpy(data_s->password, buffer);
        data_s->password[strlen(data_s->password) - 1] = '\0';
    }

    fgets(buffer, MAX_TOKEN_LENGTHS, file);
    if (data_s->rdaID[0] == '\0') {
        strcpy(data_s->rdaID, buffer);
        data_s->rdaID[strlen(data_s->rdaID) - 1] = '\0';
    }

    fclose(file);
}

void memFree(data_t *data_s) {
    if (data_s->token != NULL) {
        free(data_s->token);
        data_s->token = NULL;
    }
    if (data_s->user != NULL) {
        free(data_s->user);
        data_s->user = NULL;
    }
    if (data_s->password != NULL) {
        free(data_s->password);
        data_s->password = NULL;
    }
    if (data_s->rdaID != NULL) {
        free(data_s->rdaID);
        data_s->rdaID = NULL;
    }
    if (data_s->chunk != NULL) {
        free(data_s->chunk);
        data_s->chunk = NULL;
    }
}