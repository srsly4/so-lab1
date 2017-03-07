#include "contactlib.h"
#include <string.h>
#include <stdlib.h>

size_t trimsize(size_t original, size_t limit){
    return original > limit ? limit : original;
}

contacts_unidb* cunidb_initialize(int type){
    if (type != CONTACT_UNIDB_DLL && type != CONTACT_UNIDB_BT)
        return NULL;

    contacts_unidb* db = malloc(sizeof(contacts_unidb));
    db->type = type;
    db->first = NULL;
    db->last = NULL;
    db->size = 0;
    db->primary_key_serial = 1; //let begin with 1 index
    db->current = NULL;
}

struct contact_uninode* create_node(contacts_unidb* db, char* name, char* surname,
    time_t birthdate, char* email, char* phone, char* address){
    struct contact_uninode* node = malloc(sizeof(struct contact_uninode));

    char *cname, *csurname, *cemail, *cphone, *caddress;
    size_t tmpsize;

    tmpsize = trimsize(strlen(name), CONTACT_UNIDB_STRING_SIZE);
    cname = calloc(tmpsize, sizeof(char));

    tmpsize = trimsize(strlen(surname), CONTACT_UNIDB_STRING_SIZE);
    csurname = calloc(tmpsize, sizeof(char));

    tmpsize = trimsize(strlen(email), CONTACT_UNIDB_SHORTSTRING_SIZE);
    cemail = calloc(tmpsize, sizeof(char));

    tmpsize = trimsize(strlen(phone), CONTACT_UNIDB_SHORTSTRING_SIZE);
    cphone = calloc(tmpsize, sizeof(char));

    tmpsize = trimsize(strlen(address), CONTACT_UNIDB_STRING_SIZE);
    caddress = calloc(tmpsize, sizeof(char));

    node->index = db->primary_key_serial;
    node->name = cname;
    node->surname = csurname;
    node->birthdate = birthdate;
    node->email = cemail;
    node->phone = cphone;
    node->address = caddress;
    node->left = NULL;
    node->right = NULL;
    node->parent = NULL;

    db->primary_key_serial++;
    return node;
}

void dll_insert(contacts_unidb* db, struct contact_uninode* node){
    if (db->first == NULL){
        db->first = db->last = node;
    }
    else {
        struct contact_uninode* curr = db->first;
        while (curr->right != NULL)
            curr = curr->right;
        curr->right = node;
        node->left = curr;
        db->last = node;
    }
}

void dll_free_node(struct contact_uninode* node){
    if (node == NULL) return;
    free(node->name);
    free(node->surname);
    free(node->email);
    free(node->phone);
    free(node->address);
    free(node);
}

void dll_free_all(contacts_unidb* db){
    struct contact_uninode* curr, *next;
    curr = db->first;
    if (curr == NULL) return;
    next = curr->right;
    
    while (next != NULL){
        dll_free_node(curr);
        curr = next;
        next = next->right;
    }
}

uint32_t cunidb_add(contacts_unidb* db, char* name, char* surname,
    time_t birthdate, char* email, char* phone, char* address){
    
    struct contact_uninode* node = create_node(db, name, surname, birthdate, email, phone, address);
    if (db->type == CONTACT_UNIDB_DLL){
        dll_insert(db, node);
    }

    return node->index;
}

void cunidb_free(contacts_unidb* db){
    if (db){
        //todo: iterate through items and free
        if (db->type == CONTACT_UNIDB_DLL)
            dll_free_all(db);
        free(db);
    }

}