#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "RM.h"
#include <ctype.h>

struct Graph graph;

void printJSONHeader() {
    printf("Content-Type: application/json\n\n");
}


char* trim(char* str) {
    while (isspace((unsigned char)*str)) str++;
    if (*str == 0) return str;
    char* end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    end[1] = '\0';
    return str;
}


bool extractJSONValue(const char* json, const char* key, char* outValue) {
    char pattern[64];
    snprintf(pattern, sizeof(pattern), "\"%s\"", key);
    const char* pos = strstr(json, pattern);
    if (!pos) return false;

    pos = strchr(pos + strlen(pattern), ':');
    if (!pos) return false;
    pos++;

    while (isspace((unsigned char)*pos)) pos++;

    if (*pos == '"') {
        pos++;
        const char* end = strchr(pos, '"');
        if (!end) return false;
        strncpy(outValue, pos, end - pos);
        outValue[end - pos] = '\0';
    } else {
        const char* end = pos;
        while (*end && *end != ',' && *end != '}') end++;
        strncpy(outValue, pos, end - pos);
        outValue[end - pos] = '\0';
    }

    return true;
}

int main() {
    printJSONHeader();
    loadRoutesFromfile(&graph, "rfile");

    char buffer[2048];
    if (!fgets(buffer, sizeof(buffer), stdin)) {
        printf("{\"error\":\"No input received\"}");
        return 0;
    }

    char command[50], from[NAME_LEN], to[NAME_LEN], distStr[50];
    float distance = 0.0f;
    strcpy(from, ""); strcpy(to, ""); strcpy(distStr, "");

    extractJSONValue(buffer, "command", command);
    extractJSONValue(buffer, "from", from);
    extractJSONValue(buffer, "to", to);
    extractJSONValue(buffer, "distance", distStr);

    if (strlen(distStr) > 0) distance = atof(distStr);


    if (strcmp(command, "add_route") == 0) {
        if (strlen(from) == 0 || strlen(to) == 0 || distance <= 0) {
            printf("{\"error\":\"Invalid input data\"}");
            return 0;
        }

        if (addRoute(&graph, from, to, distance)) {
            appendSingleRouteToFile(from, to, distance, "rfile");
            printf("{\"message\":\"Route added successfully\",\"from\":\"%s\",\"to\":\"%s\",\"distance\":%.2f}", from, to, distance);
        } else {
            printf("{\"error\":\"Failed to add route\"}");
        }
    }


    else if (strcmp(command, "display_routes") == 0) {
        printf("{\"message\":\"");
        for (int i = 0; i < graph.numLocations; i++) {
            struct RouteNode* temp = graph.adjList[i];
            while (temp) {
                if (i < temp->destIndex)
                    printf("%s -> %s : %.2f km\\n", graph.locations[i], graph.locations[temp->destIndex], temp->distance);
                temp = temp->next;
            }
        }
        printf("\"}");
    }


    else if (strcmp(command, "show_from") == 0) {
        int count;
        struct RouteNode* routes = getRoutesFrom(&graph, from, &count);

        if (count == -1) {
            printf("{\"error\":\"Location not found\"}");
        } else if (count == 0) {
            printf("{\"error\":\"No routes found from %s\"}", from);
        } else {
            printf("{\"from\":\"%s\",\"routes\":[", from);
            struct RouteNode* temp = routes;
            while (temp) {
                printf("{\"to\":\"%s\",\"distance\":%.2f}", graph.locations[temp->destIndex], temp->distance);
                if (temp->next) printf(",");
                temp = temp->next;
            }
            printf("]}");
        }
    }


    else if (strcmp(command, "find_direct") == 0) {
        float dist;
        if (findDirectRoute(&graph, from, to, &dist)) {
            printf("{\"status\":\"Direct route found\",\"from\":\"%s\",\"to\":\"%s\",\"distance\":%.2f}", from, to, dist);
        } else {
            printf("{\"error\":\"No direct route between %s and %s\"}", from, to);
        }
    }

else if (strcmp(command, "add_route_from_osm") == 0) {
    char intermediates[10][NAME_LEN];
    int count = 0;

    const char* ptr = strstr(buffer, "\"intermediates\"");
    if (ptr) {
        const char* arrStart = strchr(ptr, '[');
        const char* arrEnd = strchr(ptr, ']');
        if (arrStart && arrEnd && arrEnd > arrStart) {
            char temp[512];
            strncpy(temp, arrStart + 1, arrEnd - arrStart - 1);
            temp[arrEnd - arrStart - 1] = '\0';

            char* token = strtok(temp, ",");
            while (token && count < 10) {
                token = trim(token);
                if (*token == '"') token++;
                char* end = strchr(token, '"');
                if (end) *end = '\0';
                strncpy(intermediates[count++], token, NAME_LEN);
                token = strtok(NULL, ",");
            }
        }
    }

    if (strlen(from) == 0 || strlen(to) == 0) {
        printf("{\"error\":\"Missing from or to\"}");
        return 0;
    }

    char prev[NAME_LEN];
    strcpy(prev, from);
    for (int i = 0; i < count; i++) {
        addRoute(&graph, prev, intermediates[i], 1.0); 
        appendSingleRouteToFile(prev, intermediates[i], 1.0, "rfile");
        strcpy(prev, intermediates[i]);
    }
    addRoute(&graph, prev, to, 1.0);
    appendSingleRouteToFile(prev, to, 1.0, "rfile");

    printf("{\"status\":\"success\",\"message\":\"Route with intermediates added\"}");
    }

    else if (strcmp(command, "shortest_path") == 0) {
        float totalDist;
        int path[MAX_LOCATIONS];
        int len = 0;

        if (shortestPath(&graph, from, to, &totalDist, path, &len)) {
            printf("{\"status\":\"Shortest path found\",\"distance\":%.2f,\"path\":[", totalDist);
            for (int i = 0; i < len; i++) {
                printf("\"%s\"", graph.locations[path[i]]);
                if (i < len - 1) printf(",");
            }
            printf("]}");
        } else {
            printf("{\"error\":\"No path found between %s and %s\"}", from, to);
        }
    }


    else {
        printf("{\"error\":\"Invalid command or missing parameters\"}");
    }

    return 0;
}
