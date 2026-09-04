#ifndef RIDEMATCHING_H
#define RIDEMATCHING_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX 100

struct Request {
    int id;
    char name[MAX];
    char pickup[MAX];
    char destination[MAX];
    char time[10];
    struct Request *next;
};

extern struct Request *requestList;



void addRequest();
void displayRequests();
void matchRides();
void saveRequestsToFile(const char* filename);
void loadRequestsFromFile(const char* filename);
void loadRidesFromFile(const char* filename);
#endif
