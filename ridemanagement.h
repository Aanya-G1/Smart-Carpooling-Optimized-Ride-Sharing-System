#ifndef RIDEMANAGEMENT_H
#define RIDEMANAGEMENT_H

#include "RM.h"

#define MAX 100

struct Ride {
    int id;
    char name[MAX];
    char pickup[MAX];
    char destination[MAX];
    char time[10];
    int seats;
    struct Ride *next;
};

extern struct Ride *rideList;

void createRide();
void joinRide();
void leaveRide();
void displayRides();
void saveRidesToFile(const char *filename);
void loadRidesFromFile(const char *filename);

#endif
