
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "cJSON.h"
#include "ridematching.h"
#include "ridemanagement.h"
#include "RM.h"
#include "user.h"

extern struct Graph graph;   
struct Graph *g = &graph;
int nextRideId = 100;

void printJSONHeader() {
    printf("Content-Type: application/json\n\n");
}


struct User* findUserByName(const char* name) {
    struct User* u = userList;
    while (u) {
        if (strcmp(u->name, name) == 0) return u;
        u = u->next;
    }
    return NULL;
}


static const char* json_get_str_or_empty(cJSON* root, const char* key) {
    cJSON* it = cJSON_GetObjectItem(root, key);
    if (cJSON_IsString(it) && it->valuestring) return it->valuestring;
    return "";
}


static int json_get_int_def(cJSON* root, const char* key, int def) {
    cJSON* it = cJSON_GetObjectItem(root, key);
    if (cJSON_IsNumber(it)) return it->valueint;
    return def;
}

int main(void) {
    printJSONHeader();

    loadUsers();
    loadRoutesFromfile(g, "rfile");   
    loadRequestsFromFile("ridefile");
    loadRidesFromFile("rides.txt");


    char *clen = getenv("CONTENT_LENGTH");
    int len = clen ? atoi(clen) : 0;
    if (len <= 0 || len >= 16384) {
        printf("{\"status\":\"error\",\"message\":\"Invalid CONTENT_LENGTH\"}");
        return 0;
    }

    char *body = (char*)malloc(len + 1);
    if (!body) {
        printf("{\"status\":\"error\",\"message\":\"Memory allocation failed\"}");
        return 0;
    }
    if (fread(body, 1, len, stdin) != (size_t)len) {
        printf("{\"status\":\"error\",\"message\":\"Failed to read body\"}");
        free(body);
        return 0;
    }
    body[len] = '\0';

    cJSON *root = cJSON_Parse(body);
    free(body);
    if (!root) {
        printf("{\"status\":\"error\",\"message\":\"Invalid JSON\"}");
        return 0;
    }

    cJSON *cmdItem = cJSON_GetObjectItem(root, "command");
    if (!cJSON_IsString(cmdItem) || !cmdItem->valuestring) {
        printf("{\"status\":\"error\",\"message\":\"Missing command\"}");
        cJSON_Delete(root);
        return 0;
    }
    const char *command = cmdItem->valuestring;


    if (strcmp(command, "publish_ride") == 0) {
        const char *pickup = json_get_str_or_empty(root, "from");
        const char *dest   = json_get_str_or_empty(root, "to");
        const char *time   = json_get_str_or_empty(root, "time");
        const char *name   = json_get_str_or_empty(root, "name");
        int seats = json_get_int_def(root, "seats", 1);
        if (seats <= 0) seats = 1;

        if (!pickup[0] || !dest[0]) {
            printf("{\"status\":\"error\",\"message\":\"Missing from/to\"}");
            cJSON_Delete(root);
            return 0;
        }


        int pickIdx = findLocationIndex(g, (char*)pickup);
        int destIdx = findLocationIndex(g, (char*)dest);
        if (pickIdx == -1 || destIdx == -1) {
        }

        struct Ride *newRide = malloc(sizeof(struct Ride));
        if (!newRide) {
            printf("{\"status\":\"error\",\"message\":\"Memory allocation failed\"}");
            cJSON_Delete(root);
            return 0;
        }
        newRide->id = nextRideId++;
        strncpy(newRide->name, name && name[0] ? name : "DriverX", sizeof(newRide->name)-1);
        newRide->name[sizeof(newRide->name)-1] = '\0';
        strncpy(newRide->pickup, pickup, sizeof(newRide->pickup)-1);
        newRide->pickup[sizeof(newRide->pickup)-1] = '\0';
        strncpy(newRide->destination, dest, sizeof(newRide->destination)-1);
        newRide->destination[sizeof(newRide->destination)-1] = '\0';
        strncpy(newRide->time, time, sizeof(newRide->time)-1);
        newRide->time[sizeof(newRide->time)-1] = '\0';
        newRide->seats = seats;
        newRide->next = rideList;
        rideList = newRide;
        saveRidesToFile("rides.txt");


        float existingDist = 0.0f;
        if (!findDirectRoute(g, (char*)pickup, (char*)dest, &existingDist)) {

            if (addRoute(g, (char*)pickup, (char*)dest, 1.0f)) {
                appendSingleRouteToFile(pickup, dest, 1.0f, "rfile");
            }
        }

        printf("{\"status\":\"success\",\"message\":\"Ride published successfully\",\"ride_id\":%d}", newRide->id);
    }


    else if (strcmp(command, "find_rides") == 0) {
        const char *pickup = json_get_str_or_empty(root, "from");
        const char *dest   = json_get_str_or_empty(root, "to");

        if (!pickup[0] || !dest[0]) {
            printf("{\"status\":\"error\",\"message\":\"Missing from/to\"}");
            cJSON_Delete(root);
            return 0;
        }

        printf("{\"status\":\"success\",\"rides\":[");
        struct Ride *r = rideList;
        int first = 1;
        while (r) {
            if (strcmp(r->pickup, pickup) == 0 && strcmp(r->destination, dest) == 0) {
               
                float dist = 0.0f;
                int pathIndices[MAX_LOCATIONS];
                int pathLen = 0;
                bool hasPath = false;
                if (shortestPath(g, (char*)r->pickup, (char*)r->destination, &dist, pathIndices, &pathLen)) {
                    hasPath = true;
                }

                if (!first) printf(",");
                first = 0;

                struct User* u = findUserByName(r->name);


                int eta_min = -1;
                if (hasPath) {
                    float avgSpeedKmph = 40.0f;
                    eta_min = (int)((dist / avgSpeedKmph) * 60.0f + 0.5f);
                    if (eta_min < 1) eta_min = 1;
                }

                printf("{\"id\":%d,\"driver\":\"%s\",\"from\":\"%s\",\"to\":\"%s\","
                       "\"time\":\"%s\",\"seats\":%d,"
                       "\"phone\":\"%s\",\"vehicle\":\"%s\",",
                       r->id, r->name, r->pickup, r->destination, r->time, r->seats,
                       u ? u->phone : "", u ? u->vehicle : "");

                if (hasPath) {
                    printf("\"distance\":%.2f,\"eta_minutes\":%d", dist, eta_min);
                } else {

                    printf("\"distance\":null,\"eta_minutes\":null");
                }

                printf("}");
            }
            r = r->next;
        }
        printf("]}");
    }


    else if (strcmp(command, "join_ride") == 0) {
        int ride_id = json_get_int_def(root, "ride_id", -1);


        if (ride_id < 0) {
            printf("{\"status\":\"error\",\"message\":\"Missing ride_id\"}");
            cJSON_Delete(root);
            return 0;
        }

        struct Ride *r = rideList;
        while (r) {
            if (r->id == ride_id) {
                if (r->seats > 0) {
                    r->seats--;
                    saveRidesToFile("rides.txt");
                    struct User* u = findUserByName(r->name);
                    printf("{\"status\":\"success\",\"driver\":\"%s\",\"phone\":\"%s\","
                           "\"vehicle\":\"%s\",\"eta\":\"10 min\",\"from\":\"%s\",\"to\":\"%s\"}",
                           r->name,
                           u ? u->phone : "",
                           u ? u->vehicle : "",
                           r->pickup, r->destination);
                } else {
                    printf("{\"status\":\"error\",\"message\":\"No seats available\"}");
                }
                cJSON_Delete(root);
                return 0;
            }
            r = r->next;
        }
        printf("{\"status\":\"error\",\"message\":\"Ride not found\"}");
    }


    else {
        printf("{\"status\":\"error\",\"message\":\"Unknown command\"}");
    }

    cJSON_Delete(root);
    return 0;
}
