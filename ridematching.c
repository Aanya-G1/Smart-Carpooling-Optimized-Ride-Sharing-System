#include "ridematching.h"

struct Request *requestList = NULL;


void addRequest() {
    struct Request *newReq = (struct Request *)malloc(sizeof(struct Request));
    if (!newReq) {
        return;
    }


}

void displayRequests() {
  
}

void matchRides() {

}

void saveRequestsToFile(const char* filename) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        return;
    }
    struct Request *r = requestList;
    while (r) {
        fprintf(file, "%d|%s|%s|%s|%s\n", r->id, r->name, r->pickup, r->destination, r->time);
        r = r->next;
    }
    fclose(file);
}

void loadRequestsFromFile(const char* filename) {
    FILE *file = fopen(filename, "r");
    if (!file) return;
    char line[1024];
    while (fgets(line, sizeof(line), file)) {
        char *p = strchr(line, '\n');
        if (p) *p = '\0';
        char *tok = strtok(line, "|");
        if (!tok) continue;
        struct Request *newReq = (struct Request *)malloc(sizeof(struct Request));
        newReq->id = atoi(tok);
        tok = strtok(NULL, "|"); if (!tok) { free(newReq); continue; }
        strcpy(newReq->name, tok);
        tok = strtok(NULL, "|"); if (!tok) { free(newReq); continue; }
        strcpy(newReq->pickup, tok);
        tok = strtok(NULL, "|"); if (!tok) { free(newReq); continue; }
        strcpy(newReq->destination, tok);
        tok = strtok(NULL, "|"); if (!tok) { free(newReq); continue; }
        strcpy(newReq->time, tok);
        newReq->next = requestList;
        requestList = newReq;
    }
    fclose(file);
}
