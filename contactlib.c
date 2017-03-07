#include "contactlib.h"
#include <string.h>
#include <stdlib.h>

struct contacts_unidb* cunidb_initialize(int type){
    if (type != CONTACT_UNIDB_DLL && type != CONTACT_UNIDB_BT)
        return NULL;

    contacts_unidb* db = malloc(sizeof(contacts_unidb));
    db->type = type;
    db->first = NULL;
    db->last = NULL;
    db->size = 0;
    db->current = NULL;
}

void cunidb_free(contacts_unidb* db){
    if (db){
        //todo: iterate through items and free
        if (db->current) free(db->current);
        free(db);
    }

}