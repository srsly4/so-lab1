#ifndef CONTACTLIB
#define CONTACTLIB

#include <time.h>
#include <inttypes.h>
#include <stdbool.h>

#define CONTACT_UNIDB_DLL 1 //double linked-list storage type
#define CONTACT_UNIDB_BT 2 //binary tree storage type

#define CONTACT_UNIDB_STRING_SIZE 64
#define CONTACT_UNIDB_SHORTSTRING_SIZE 24

#define CONTACT_UNIDB_SORT_NAME 1
#define CONTACT_UNIDB_SORT_SURNAME 2
#define CONTACT_UNIDB_SORT_BIRTDATE 4
#define CONTACT_UNIDB_SORT_EMAIL 8
#define CONTACT_UNIDB_SORT_PHONE 16
//#define CONTACT_UNIDB_SORT_ADDRESS 32

struct contact_uninode {
    struct contact_uninode* left; //and previous in DL-L
    struct contact_uninode* right; //and next in DL-L
    struct contact_uninode* parent; //only in BT
    bool btdir; //direction for inserting in BT
    bool btseen; //iterator flag in BT
    bool is_red; //color for BT node
    uint32_t index;
    char* name;
    char* surname;
    char* birthdate;
    char* email;
    char* phone;
    char* address;
};

typedef struct contacts_unidb {
    struct contact_uninode* first; //one and only in case of BT
    struct contact_uninode* last; //used only for DL-L
    int type;
    uint32_t primary_key_serial;
    struct contact_uninode* current;
} contacts_unidb;


contacts_unidb* cunidb_initialize(int type);

void cunidb_free(contacts_unidb* db);
uint32_t cunidb_add(contacts_unidb* db, char* name, char* surname,
    char* birthdate, char* email, char* phone, char* address); //returns an index of created element
void cunidb_remove(contacts_unidb* db, struct contact_uninode* item);
struct contact_uninode* cunidb_get(contacts_unidb* db, uint32_t index);
//iterator

void cunidb_iterator_reset(contacts_unidb* db);
bool cunidb_iterator_empty(contacts_unidb* db);
struct contact_uninode* cunidb_iterator_next(contacts_unidb* db);

//finding functions
struct contact_uninode* cunidb_find(contacts_unidb* db, char* name, char* surname,
                                    char* birthdate, char* email, char* phone, char* address);

void cunidb_sort(contacts_unidb* db, int sorttype);

#endif


