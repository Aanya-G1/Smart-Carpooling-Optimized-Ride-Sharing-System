#ifndef ROUTE_H
#define ROUTE_H

#include <stdbool.h>

#define MAX_LOCATIONS 20
#define NAME_LEN 30

struct RouteNode {
    int destIndex;
    float distance;
    struct RouteNode* next;
};

struct Graph {
    char locations[MAX_LOCATIONS][NAME_LEN];
    struct RouteNode* adjList[MAX_LOCATIONS];
    int numLocations;
};
extern struct Graph graph;
void initGraph(struct Graph* graph);

int findLocationIndex(struct Graph* graph, char name[]);

int addLocation(struct Graph* graph, char name[]);

bool addRoute(struct Graph* graph, char from[], char to[], float distance);

struct RouteNode* getRoutesFrom(struct Graph* graph, char from[], int* count);

bool findDirectRoute(struct Graph* graph, char from[], char to[], float* distance);

bool shortestPath(struct Graph* graph, char start[], char end[], float* outDistance, int* outPath, int* outPathLen);

void saveRoutesTofile(struct Graph* graph, const char* filename);
void appendSingleRouteToFile(const char* from, const char* to, float distance, const char* filename);
void loadRoutesFromfile(struct Graph* graph, const char* filename);

#endif // ROUTE_H
