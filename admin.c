#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"

#define USERS_FILE "users.txt"

int main() {
    FILE *f = fopen(USERS_FILE, "r");
    printf("Content-Type: application/json\n\n");

    if (!f) {
        printf("{\"status\":\"error\",\"message\":\"Could not open users file\"}");
        return 0;
    }

    cJSON *arr = cJSON_CreateArray();
    char line[300];

    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = '\0';

        int id;
        char username[50], password[50], name[100], role[20], phone[30], vehicle[30];

        if (sscanf(line, "%d|%49[^|]|%49[^|]|%99[^|]|%19[^|]|%29[^|]|%29[^\n]",
                   &id, username, password, name, role, phone, vehicle) == 7)
        {
            cJSON *u = cJSON_CreateObject();
            cJSON_AddNumberToObject(u, "id", id);
            cJSON_AddStringToObject(u, "username", username);
            cJSON_AddStringToObject(u, "name", name);
            cJSON_AddStringToObject(u, "role", role);
            cJSON_AddStringToObject(u, "phone", phone);
            cJSON_AddStringToObject(u, "vehicle", vehicle);

            cJSON_AddItemToArray(arr, u);
        }
    }

    fclose(f);

    char *out = cJSON_PrintUnformatted(arr);
    printf("%s", out);

    free(out);
    cJSON_Delete(arr);

    return 0;
}