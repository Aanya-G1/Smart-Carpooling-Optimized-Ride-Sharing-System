#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"
#include "user.h"
#define USERS_FILE "users.txt"

struct User *userList = NULL;

int generateID() {
    int count = 0;
    FILE *f = fopen(USERS_FILE, "r");
    if (!f) return 100;

    char line[400];
    while (fgets(line, sizeof(line), f)) {
        count++;
    }

    fclose(f);
    return 100 + count;
}

void loadUsers() {
    FILE *f = fopen(USERS_FILE, "r");
    if (!f) return;

    char line[400];
    while (fgets(line, sizeof(line), f)) {
        struct User *u = malloc(sizeof(struct User));
        if (!u) continue;
        if (sscanf(line, "%d|%49[^|]|%49[^|]|%99[^|]|%19[^|]|%29[^|]|%29[^\n]",
                   &u->id, u->username, u->password, u->name,
                   u->role, u->phone, u->vehicle) == 7) {
            u->next = userList;
            userList = u;
        } else {
            free(u);
        }
    }
    fclose(f);
}

void saveUsers() {
    FILE *f = fopen(USERS_FILE, "w");
    if (!f) return;
    struct User *u = userList;

    while (u) {
        fprintf(f, "%d|%s|%s|%s|%s|%s|%s\n",
                u->id, u->username, u->password, u->name,
                u->role, u->phone, u->vehicle);
        u = u->next;
    }
    fclose(f);
}

int usernameExists(const char *username) {
    struct User *u = userList;
    while (u) {
        if (strcmp(u->username, username) == 0)
            return 1;
        u = u->next;
    }
    return 0;
}

void sendJSON(cJSON *json) {
    char *string = cJSON_PrintUnformatted(json);
    printf("Content-Type: application/json\n\n");
    printf("%s", string);
    fflush(stdout);
    free(string);
    cJSON_Delete(json);
}

void handle_signup(cJSON *req) {
    cJSON *uItem = cJSON_GetObjectItem(req, "username");
    cJSON *pItem = cJSON_GetObjectItem(req, "password");
    cJSON *nItem = cJSON_GetObjectItem(req, "name");
    cJSON *rItem = cJSON_GetObjectItem(req, "role");
    cJSON *phItem = cJSON_GetObjectItem(req, "phone");
    cJSON *vItem = cJSON_GetObjectItem(req, "vehicle");

    if (!cJSON_IsString(uItem) || !cJSON_IsString(pItem) ||
        !cJSON_IsString(nItem) || !cJSON_IsString(phItem) ||
        !cJSON_IsString(vItem)) {
        cJSON *res = cJSON_CreateObject();
        cJSON_AddStringToObject(res, "status", "error");
        cJSON_AddStringToObject(res, "message", "Missing signup fields");
        sendJSON(res);
        return;
    }

    const char *username = uItem->valuestring;
    const char *password = pItem->valuestring;
    const char *name = nItem->valuestring;
    const char *role = (rItem && cJSON_IsString(rItem)) ? rItem->valuestring : "user";
    const char *phone = phItem->valuestring;
    const char *vehicle = vItem->valuestring;

    if (usernameExists(username)) {
        cJSON *res = cJSON_CreateObject();
        cJSON_AddStringToObject(res, "status", "error");
        cJSON_AddStringToObject(res, "message", "Username already exists");
        sendJSON(res);
        return;
    }

    struct User *u = malloc(sizeof(struct User));
    if (!u) return;

    u->id = generateID();
    strcpy(u->username, username);
    strcpy(u->password, password);
    strcpy(u->name, name);
    strcpy(u->role, role);
    strcpy(u->phone, phone);
    strcpy(u->vehicle, vehicle);
    u->next = userList;
    userList = u;

    saveUsers();

    cJSON *res = cJSON_CreateObject();
    cJSON_AddStringToObject(res, "status", "ok");
    cJSON_AddStringToObject(res, "message", "Signup successful");
    cJSON_AddNumberToObject(res, "id", u->id);
    cJSON_AddStringToObject(res, "role", u->role);
    sendJSON(res);
}

void handle_login(cJSON *req) {
    cJSON *uItem = cJSON_GetObjectItem(req, "username");
    cJSON *pItem = cJSON_GetObjectItem(req, "password");

    if (!cJSON_IsString(uItem) || !cJSON_IsString(pItem)) {
        cJSON *res = cJSON_CreateObject();
        cJSON_AddStringToObject(res, "status", "error");
        cJSON_AddStringToObject(res, "message", "Missing login fields");
        sendJSON(res);
        return;
    }

    const char *username = uItem->valuestring;
    const char *password = pItem->valuestring;

    struct User *u = userList;
    while (u) {
        if (strcmp(u->username, username) == 0 &&
            strcmp(u->password, password) == 0) {
            cJSON *res = cJSON_CreateObject();
            cJSON_AddStringToObject(res, "status", "ok");
            cJSON_AddStringToObject(res, "message", "Login successful");
            cJSON_AddStringToObject(res, "name", u->name);
            cJSON_AddStringToObject(res, "role", u->role);
            cJSON_AddStringToObject(res, "phone", u->phone);
            cJSON_AddStringToObject(res, "vehicle", u->vehicle);
            sendJSON(res);
            return;
        }
        u = u->next;
    }

    cJSON *res = cJSON_CreateObject();
    cJSON_AddStringToObject(res, "status", "error");
    cJSON_AddStringToObject(res, "message", "Invalid credentials");
    sendJSON(res);
}

void handle_get_profile(cJSON *req) {
    cJSON *uItem = cJSON_GetObjectItem(req, "username");
    if (!cJSON_IsString(uItem)) {
        cJSON *res = cJSON_CreateObject();
        cJSON_AddStringToObject(res, "status", "error");
        cJSON_AddStringToObject(res, "message", "Missing username");
        sendJSON(res);
        return;
    }

    const char *username = uItem->valuestring;
    struct User *u = userList;

    while (u) {
        if (strcmp(u->username, username) == 0) {
            cJSON *res = cJSON_CreateObject();
            cJSON_AddStringToObject(res, "status", "ok");
            cJSON_AddStringToObject(res, "name", u->name);
            cJSON_AddStringToObject(res, "phone", u->phone);
            cJSON_AddStringToObject(res, "vehicle", u->vehicle);
            cJSON_AddStringToObject(res, "role", u->role);
            sendJSON(res);
            return;
        }
        u = u->next;
    }

    cJSON *res = cJSON_CreateObject();
    cJSON_AddStringToObject(res, "status", "error");
    cJSON_AddStringToObject(res, "message", "User not found");
    sendJSON(res);
}

int main() {
    loadUsers();

    char *lenStr = getenv("CONTENT_LENGTH");
    if (!lenStr) {
        printf("Content-Type: application/json\n\n");
        printf("{\"status\":\"error\",\"message\":\"Missing CONTENT_LENGTH\"}");
        return 0;
    }

    int content_length = atoi(lenStr);
    char *body = malloc(content_length + 1);
    if (!body) return 1;
    fread(body, 1, content_length, stdin);
    body[content_length] = '\0';

    cJSON *json = cJSON_Parse(body);
    free(body);

    if (!json) {
        printf("Content-Type: application/json\n\n");
        printf("{\"status\":\"error\",\"message\":\"Invalid JSON\"}");
        return 0;
    }

    cJSON *actionItem = cJSON_GetObjectItem(json, "action");
    if (!cJSON_IsString(actionItem)) {
        printf("Content-Type: application/json\n\n");
        printf("{\"status\":\"error\",\"message\":\"Missing action\"}");
        cJSON_Delete(json);
        return 0;
    }

    const char *action = actionItem->valuestring;

    if (strcmp(action, "signup") == 0) {
        handle_signup(json);
    } else if (strcmp(action, "login") == 0) {
        handle_login(json);
    } else if (strcmp(action, "get_profile") == 0) {
        handle_get_profile(json);
    } else {
        printf("Content-Type: application/json\n\n");
        printf("{\"status\":\"error\",\"message\":\"Unknown action\"}");
    }

    cJSON_Delete(json);
    return 0;
}