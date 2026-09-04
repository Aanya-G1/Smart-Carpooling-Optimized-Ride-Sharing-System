#ifndef USER_H
#define USER_H

struct User {
    int id;
    char username[50];
    char password[50];
    char name[100];
    char role[20];
    char phone[30];      
    char vehicle[30];   
    struct User *next;
};


extern struct User *userList;   


void loadUsers();       
void saveUsers();      
int usernameExists(const char *username);

struct User* findUserByName(const char *name);

#endif