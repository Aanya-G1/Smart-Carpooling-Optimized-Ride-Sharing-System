#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <stdbool.h>
#include <sys/stat.h>
#include "RM.h"
#include<ctype.h>
#define MAX_LOCATIONS 20
#define NAME_LEN 30
struct Graph graph; 
void initGraph(struct Graph* graph) {
    graph->numLocations = 0;
    for(int i = 0; i < MAX_LOCATIONS; i++) {
        graph->adjList[i] = NULL;
    }
}

int findLocationIndex(struct Graph* graph, char name[]) {
    for (int i = 0; i < graph->numLocations; i++) {
        if (strcmp(graph->locations[i], name) == 0)
            return i;
    }
    return -1;
}

int addLocation(struct Graph* graph, char name[]) {
    int index = findLocationIndex(graph, name);
    if (index != -1) return index;
    if (graph->numLocations >= MAX_LOCATIONS) {
        return -1; 
    }
    strcpy(graph->locations[graph->numLocations], name);
    graph->numLocations++;
    return graph->numLocations - 1;
}

bool addRoute(struct Graph* graph, char from[], char to[], float distance) {
    int src = addLocation(graph, from);
    int dest = addLocation(graph, to);
    if (src == -1 || dest == -1) return false;

    struct RouteNode* newnode = (struct RouteNode*)malloc(sizeof(struct RouteNode));
    if (!newnode) return false;
    newnode->destIndex = dest;
    newnode->distance = distance;
    newnode->next = graph->adjList[src];
    graph->adjList[src] = newnode;

    struct RouteNode* newNode2 = (struct RouteNode*)malloc(sizeof(struct RouteNode));
    if (!newNode2) return false;
    newNode2->destIndex = src;
    newNode2->distance = distance;
    newNode2->next = graph->adjList[dest];
    graph->adjList[dest] = newNode2;

    return true;
}

struct RouteNode* getRoutesFrom(struct Graph* graph, char from[], int* count) {
    int src = findLocationIndex(graph, from);
    if (src == -1) {
        *count = -1;
        return NULL;
    }
    struct RouteNode* temp = graph->adjList[src];
    if (!temp) {
        *count = 0;
        return NULL;
    }

   
    int c = 0;
    struct RouteNode* iter = temp;
    while (iter != NULL) {
        c++;
        iter = iter->next;
    }
    *count = c;

    return temp;
}

bool findDirectRoute(struct Graph* graph, char from[], char to[], float* distance) {
    int src = findLocationIndex(graph, from);
    int dest = findLocationIndex(graph, to);
    if (src == -1 || dest == -1) return false;

    struct RouteNode* temp = graph->adjList[src];
    while (temp != NULL) {
        if (temp->destIndex == dest) {
            *distance = temp->distance;
            return true;
        }
        temp = temp->next;
    }
    return false;
}

bool shortestPath(struct Graph* graph, char start[], char end[], float* outDistance, int* outPath, int* outPathLen) {
    int n = graph->numLocations;
    int src = findLocationIndex(graph, start);
    int dest = findLocationIndex(graph, end);
    if (src == -1 || dest == -1) return false;

    float dist[MAX_LOCATIONS];
    int prev[MAX_LOCATIONS];
    bool visited[MAX_LOCATIONS];

    for (int i = 0; i < n; i++) {
        dist[i] = FLT_MAX;
        visited[i] = false;
        prev[i] = -1;
    }
    dist[src] = 0;

    for (int count = 0; count < n - 1; count++) {
        int u = -1;
        float min = FLT_MAX;
        for (int i = 0; i < n; i++) {
            if (!visited[i] && dist[i] <= min) {
                min = dist[i];
                u = i;
            }
        }
        if (u == -1) break;
        visited[u] = true;

        struct RouteNode* temp = graph->adjList[u];
        while (temp != NULL) {
            int v = temp->destIndex;
            float weight = temp->distance;
            if (!visited[v] && dist[u] + weight < dist[v]) {
                dist[v] = dist[u] + weight;
                prev[v] = u;
            }
            temp = temp->next;
        }
    }

    if (dist[dest] == FLT_MAX) {
        return false; 
    }

    *outDistance = dist[dest];

   
    int path[MAX_LOCATIONS];
    int count = 0;
    for (int at = dest; at != -1; at = prev[at]) {
        path[count++] = at;
    }

   
    for (int i = 0; i < count; i++) {
        outPath[i] = path[count - 1 - i];
    }
    *outPathLen = count;

    return true;
}

void saveRoutesTofile(struct Graph* graph, const char* filename) {
    struct stat st;
    int fileExists = (stat(filename, &st) == 0);
    FILE* file = fopen(filename, "a");
    if (!file) {
        return;
    }
    if (!fileExists) {
        fprintf(file,"----------------------------------------------------------------------\n");
        fprintf(file,"-----------------ROUTE MANAGEMENT RECORD -----------------------------\n");
        fprintf(file,"----------------------------------------------------------------------\n");
        fprintf(file, "%-13s|%-13s|%-13s\n","FROM","TO","DISTANCE");
        fprintf(file,"----------------------------------------------------------------------\n");
    }
    for (int i = 0; i < graph->numLocations; i++) {
        struct RouteNode* temp = graph->adjList[i];
        while (temp) {
            if (i < temp->destIndex) {
                fprintf(file, "%-13s|%-13s|%-10.2f\n", graph->locations[i], graph->locations[temp->destIndex], temp->distance);
            }
            temp = temp->next;
        }
    }
    fclose(file);
}


void appendSingleRouteToFile(const char* from, const char* to, float distance, const char* filename) {
    struct stat st;
    int fileExists = (stat(filename, &st) == 0);
    FILE* file = fopen(filename, "a");
    if (!file) return;

    if (!fileExists) {
        fprintf(file,"----------------------------------------------------------------------\n");
        fprintf(file,"-----------------ROUTE MANAGEMENT RECORD -----------------------------\n");
        fprintf(file,"----------------------------------------------------------------------\n");
        fprintf(file, "%-13s|%-13s|%-13s\n","FROM","TO","DISTANCE");
        fprintf(file,"----------------------------------------------------------------------\n");
    }

    fprintf(file, "%-13s|%-13s|%-10.2f\n", from, to, distance);
    fclose(file);
}
void loadRoutesFromfile(struct Graph* graph, const char* filename) {
    initGraph(graph);  

    FILE* file = fopen(filename, "r");
    if (!file) return;

    char from[NAME_LEN], to[NAME_LEN];
    float distance;
    char line[100];

    for (int i = 0; i < 5; i++) {
        fgets(line, sizeof(line), file);
    }
    char* trim(char* str) {
    while (isspace((unsigned char)*str)) str++;
    if (*str == 0) return str;
    char* end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    end[1] = '\0';
    return str;
}

    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "%[^|]|%[^|]|%f", from, to, &distance) == 3) {
        strcpy(from, trim(from));
        strcpy(to, trim(to));
        addRoute(graph, from, to, distance);
    }
    }
    fclose(file);
}