#ifndef CONTACTLIB
#define CONTACTLIB

#include <time.h>
#include <inttypes.h>
#include <stdbool.h>

#define CONTACT_UNIDB_DLL 1 //double linked-list storage type
#define CONTACT_UNIDB_BT 2 //binary tree storage type

typedef struct contact_uninode {
    struct contact_uninode* left; //and previous in DL-L
    struct contact_uninode* right; //and next in DL-L
    struct contact_uninode* parent; //only in BT
    uint32_t index;
    char* name;
    char* surname;
    time_t birthdate;
    char* email;
    char* phone;
    char* address;
} contact_uninode;

typedef struct contacts_unidb {
    struct contact_uninode* first; //one and only in case of BT
    struct contact_uninode* last; //used only for DL-L
    int type;
    uint32_t size;
    struct contact_uninode* current;
} contacts_unidb;

#endif

contacts_unidb* cunidb_initialize(int type);
void cunidb_free(contacts_unidb* db);
uint32_t cunidb_add(contacts_unidb* db, char* name, char* surname,
    time_t birthdate, char* email, char* phone, char* address); //returns an index of created element
int cunidb_remove(contacts_unidb* db, uint32_t type);

//iterator
bool cunidb_iterator_empty(contacts_unidb* db);
contact_uninode* cunidb_iterator_next(contacts_unidb* db);

