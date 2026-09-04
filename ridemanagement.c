#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "RM.h"
#include "ridemanagement.h"

struct Ride *rideList = NULL;

void createRide() {
    struct Ride *newRide = malloc(sizeof(struct Ride));
    if (!newRide) {
        printf("Memory allocation failed.\n");
        return;
    }

    printf("\nEnter Ride ID: ");
    scanf("%d", &newRide->id);
    printf("Enter Driver Name: ");
    scanf("%s", newRide->name);
    printf("Enter Pickup Location: ");
    scanf("%s", newRide->pickup);
    printf("Enter Destination: ");
    scanf("%s", newRide->destination);
    printf("Enter Time (HH:MM): ");
    scanf("%s", newRide->time);
    printf("Enter Available Seats: ");
    scanf("%d", &newRide->seats);

    newRide->next = NULL;

    if (rideList == NULL) {
        rideList = newRide;
    } else {
        struct Ride *temp = rideList;
        while (temp->next != NULL)
            temp = temp->next;
        temp->next = newRide;
    }

    printf("\n✅ Ride created successfully!\n");
}

void joinRide() {
    int id;
    char passenger[MAX];
    printf("\nEnter Ride ID to join: ");
    scanf("%d", &id);
    printf("Enter Passenger Name: ");
    scanf("%s", passenger);

    struct Ride *temp = rideList;
    while (temp != NULL && temp->id != id)
        temp = temp->next;

    if (temp == NULL) {
        printf("❌ Ride not found!\n");
        return;
    }

    if (temp->seats <= 0) {
        printf("❌ No seats available!\n");
        return;
    }

    temp->seats--;
    printf("✅ %s joined Ride ID %d successfully!\n", passenger, id);
}

void leaveRide() {
    int id;
    char passenger[MAX];
    printf("\nEnter Ride ID to leave: ");
    scanf("%d", &id);
    printf("Enter Passenger Name: ");
    scanf("%s", passenger);

    struct Ride *temp = rideList;
    while (temp != NULL && temp->id != id)
        temp = temp->next;

    if (temp == NULL) {
        printf("❌ Ride not found!\n");
        return;
    }

    temp->seats++;
    printf("✅ %s left Ride ID %d successfully!\n", passenger, id);
}

void displayRides() {
    if (rideList == NULL) {
        printf("\nNo rides available!\n");
        return;
    }

    printf("\n------ Available Rides ------\n");
    struct Ride *temp = rideList;
    while (temp != NULL) {
        printf("\nRide ID: %d", temp->id);
        printf("\nDriver Name: %s", temp->name);
        printf("\nFrom: %s ➡ To: %s", temp->pickup, temp->destination);
        printf("\nTime: %s", temp->time);
        printf("\nAvailable Seats: %d", temp->seats);
        printf("\n-----------------------------\n");
        temp = temp->next;
    }
}

void saveRidesToFile(const char* filename) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        printf("Couldn't open rides file for writing\n");
        return;
    }
    struct Ride *r = rideList;
    while (r) {
        fprintf(file, "%d|%s|%s|%s|%s|%d\n",
                r->id, r->name, r->pickup, r->destination, r->time, r->seats);
        r = r->next;
    }
    fclose(file);
}

void loadRidesFromFile(const char* filename) {
    FILE *file = fopen(filename, "r");
    if (!file) return;
    char line[1024];
    while (fgets(line, sizeof(line), file)) {
        char *p = strchr(line, '\n');
        if (p) *p = '\0';
        struct Ride *newRide = malloc(sizeof(struct Ride));
        if (!newRide) continue;

        char *tok = strtok(line, "|");
        if (!tok) { free(newRide); continue; }
        newRide->id = atoi(tok);
        tok = strtok(NULL, "|"); if (!tok) { free(newRide); continue; }
        strcpy(newRide->name, tok);
        tok = strtok(NULL, "|"); if (!tok) { free(newRide); continue; }
        strcpy(newRide->pickup, tok);
        tok = strtok(NULL, "|"); if (!tok) { free(newRide); continue; }
        strcpy(newRide->destination, tok);
        tok = strtok(NULL, "|"); if (!tok) { free(newRide); continue; }
        strcpy(newRide->time, tok);
        tok = strtok(NULL, "|"); if (!tok) { free(newRide); continue; }
        newRide->seats = atoi(tok);
        newRide->next = NULL;

        if (rideList == NULL) rideList = newRide;
        else {
            struct Ride *t = rideList;
            while (t->next) t = t->next;
            t->next = newRide;
        }
    }
    fclose(file);
}
