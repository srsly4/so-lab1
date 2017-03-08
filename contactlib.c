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
    char* birthdate, char* email, char* phone, char* address){
    struct contact_uninode* node = malloc(sizeof(struct contact_uninode));

    char *cname, *csurname, *cbirthdate, *cemail, *cphone, *caddress;
    size_t tmpsize;

    tmpsize = trimsize(strlen(name), CONTACT_UNIDB_STRING_SIZE);
    cname = calloc(tmpsize, sizeof(char));
    memcpy(cname, name, tmpsize);
    memset(cname+tmpsize, '\0', 1);

    tmpsize = trimsize(strlen(surname), CONTACT_UNIDB_STRING_SIZE);
    csurname = calloc(tmpsize, sizeof(char));
    memcpy(csurname, surname, tmpsize);
    memset(csurname+tmpsize, '\0', 1);

    tmpsize = trimsize(strlen(birthdate), CONTACT_UNIDB_STRING_SIZE);
    cbirthdate = calloc(tmpsize, sizeof(char));
    memcpy(cbirthdate, birthdate, tmpsize);
    memset(cbirthdate+tmpsize, '\0', 1);

    tmpsize = trimsize(strlen(email), CONTACT_UNIDB_SHORTSTRING_SIZE);
    cemail = calloc(tmpsize, sizeof(char));
    memcpy(cemail, email, tmpsize);
    memset(cemail+tmpsize, '\0', 1);

    tmpsize = trimsize(strlen(phone), CONTACT_UNIDB_SHORTSTRING_SIZE);
    cphone = calloc(tmpsize, sizeof(char));
    memcpy(cphone, phone, tmpsize);
    memset(cphone+tmpsize, '\0', 1);

    tmpsize = trimsize(strlen(address), CONTACT_UNIDB_STRING_SIZE);
    caddress = calloc(tmpsize, sizeof(char));
    memcpy(caddress, address, tmpsize);
    memset(caddress+tmpsize, '\0', 1);

    node->index = db->primary_key_serial;
    node->name = cname;
    node->surname = csurname;
    node->birthdate = cbirthdate;
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

void dll_remove(contacts_unidb* db, struct contact_uninode* node){
    if (!node->right) {
        db->last = node->left;
    }
    else {
        node->right->left = node->left;
    }
    if (!node->left)
        db->first = node->right;
    else
        node->left->right = node->right;
}

struct contact_uninode* dll_get_by_index(contacts_unidb* db, uint32_t index){
    struct contact_uninode* ret = NULL;
    if (db->first != NULL){
        struct contact_uninode* curr = db->first;
        while (curr->index != index && curr->right != NULL){
            curr = curr->right;
        }
        ret = curr;
    }
    return ret;
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

void dll_iterator_reset(contacts_unidb *db){
    db->current = db->first;
}

bool dll_iterator_empty(contacts_unidb *db){
    return db->current == NULL;
}
struct contact_uninode* dll_iterator_next(contacts_unidb *db){
    struct contact_uninode* ret = db->current;
    if (db->current != NULL)
        db->current = db->current->right;
    return ret;
}

struct contact_uninode* dll_find(contacts_unidb* db, char* name, char* surname,
                                 char* birthdate, char* email, char* phone, char* address){
    struct contact_uninode* curr = db->first;
    while (curr != NULL){
        if (!(name && !strstr(curr->name, name))
                && !(surname && !strstr(curr->surname, surname))
                && !(birthdate && !strstr(curr->birthdate, birthdate))
                && !(email && !strstr(curr->email, email))
                && !(phone && !strstr(curr->phone, phone))
                && !(address && !strstr(curr->address, address)))
            return curr;
        curr = curr->right;
    }
    return curr;
}

uint32_t cunidb_add(contacts_unidb* db, char* name, char* surname,
    char* birthdate, char* email, char* phone, char* address){
    
    struct contact_uninode* node = create_node(db, name, surname, birthdate, email, phone, address);
    if (db->type == CONTACT_UNIDB_DLL){
        dll_insert(db, node);
    }

    return node->index;
}



void cunidb_free(contacts_unidb* db){
    if (db){
        if (db->type == CONTACT_UNIDB_DLL)
            dll_free_all(db);
        free(db);
    }

}

struct contact_uninode *cunidb_get(contacts_unidb *db, uint32_t index) {
    if (db->type == CONTACT_UNIDB_DLL)
        return dll_get_by_index(db, index);
    else return NULL;

}

void cunidb_iterator_reset(contacts_unidb *db) {
    if (db->type == CONTACT_UNIDB_DLL)
        dll_iterator_reset(db);

}

bool cunidb_iterator_empty(contacts_unidb *db) {
    if (db->type == CONTACT_UNIDB_DLL)
        return dll_iterator_empty(db);

    return true;
}

struct contact_uninode *cunidb_iterator_next(contacts_unidb *db) {
    if (db->type == CONTACT_UNIDB_DLL)
        return dll_iterator_next(db);
    return NULL;
}

struct contact_uninode *cunidb_find(contacts_unidb* db, char* name, char* surname,
                                    char* birthdate, char* email, char* phone, char* address) {
    if (db->type == CONTACT_UNIDB_DLL)
        return dll_find(db, name, surname, birthdate, email, phone, address);
    return NULL;
}

void cunidb_remove(contacts_unidb *db, struct contact_uninode* item) {
    if (!item) return;
    if (db->type == CONTACT_UNIDB_DLL)
        dll_remove(db, item);
}
