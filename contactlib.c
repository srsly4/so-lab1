#include "contactlib.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

size_t trimsize(size_t original, size_t limit){
    return original+1 > limit ? limit : original+1;
}

contacts_unidb* cunidb_initialize(int type){
    if (type != CONTACT_UNIDB_DLL && type != CONTACT_UNIDB_BT)
        return NULL;

    contacts_unidb* db = malloc(sizeof(contacts_unidb));
    db->type = type;
    db->first = NULL;
    db->last = NULL;
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
    memset(cname+tmpsize-1, '\0', 1);

    tmpsize = trimsize(strlen(surname), CONTACT_UNIDB_STRING_SIZE);
    csurname = calloc(tmpsize, sizeof(char));
    memcpy(csurname, surname, tmpsize);
    memset(csurname+tmpsize-1, '\0', 1);

    tmpsize = trimsize(strlen(birthdate), CONTACT_UNIDB_STRING_SIZE);
    cbirthdate = calloc(tmpsize, sizeof(char));
    memcpy(cbirthdate, birthdate, tmpsize);
    memset(cbirthdate+tmpsize-1, '\0', 1);

    tmpsize = trimsize(strlen(email), CONTACT_UNIDB_SHORTSTRING_SIZE);
    cemail = calloc(tmpsize, sizeof(char));
    memcpy(cemail, email, tmpsize);
    memset(cemail+tmpsize-1, '\0', 1);

    tmpsize = trimsize(strlen(phone), CONTACT_UNIDB_SHORTSTRING_SIZE);
    cphone = calloc(tmpsize, sizeof(char));
    memcpy(cphone, phone, tmpsize);
    memset(cphone+tmpsize-1, '\0', 1);

    tmpsize = trimsize(strlen(address), CONTACT_UNIDB_STRING_SIZE);
    caddress = calloc(tmpsize, sizeof(char));
    memcpy(caddress, address, tmpsize);
    memset(caddress+tmpsize-1, '\0', 1);

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
    node->btdir = false;
    node->btseen = false;
    node->is_red = false;

    db->primary_key_serial++;
    return node;
}

void free_node(struct contact_uninode* node){
    free(node->name);
    free(node->surname);
    free(node->birthdate);
    free(node->email);
    free(node->phone);
    free(node->address);
    free(node);
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
    free_node(node);
}

struct contact_uninode* dll_get_by_index(contacts_unidb* db, uint32_t index){
    struct contact_uninode* ret = NULL;
    if (db->first != NULL){
        struct contact_uninode* curr = db->first;
        while (curr != NULL && curr->index != index){
            curr = curr->right;
        }
        ret = curr;
    }
    return ret;
}

void dll_free_node(struct contact_uninode* node){
    if (node == NULL) return;
    free_node(node);
}

void dll_free_all(contacts_unidb* db){
    struct contact_uninode* curr, *next;
    curr = db->first;
    if (curr == NULL) return;
    next = curr->right;
    if (next == NULL ){ dll_free_node(curr); }
    else {
        while (next != NULL){
            dll_free_node(curr);
            curr = next;
            next = next->right;
        }
        dll_free_node(db->last);
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

void dll_pintolist(struct contact_uninode** first, struct contact_uninode** last, struct contact_uninode* item){
    if ((*first) == NULL)
        (*first) = (*last) = item;
    else {
        item->left = (*last);
        (*last)->right = item;
        (*last) = item;
        item->right = NULL;
    }
}

void dll_quickersort(struct contact_uninode** first, struct contact_uninode** last,
                     int (*comparator)(struct contact_uninode*, struct contact_uninode*)){
//    if ((*first) == (*last)) return; //recurrence end condition
    struct contact_uninode* x = (*first);
    struct contact_uninode *q1_first=NULL, *q1_last=NULL, *q2_first=NULL, *q2_last=NULL,
            *q3_first=NULL, *q3_last=NULL, *curr, *next;

    q2_first = q2_last = x; // wyznacznik
    curr = x->right;
    x->left = NULL;
    x->right = NULL;
    while (curr != NULL){
        next = curr->right;

        //unpin
        curr->left = NULL;
        curr->right = NULL;
        if (next) next->left = NULL;

        int tmpres = (*comparator)(x, curr);
        if (tmpres < 0){ //curr jest "wiekszy"
            dll_pintolist(&q3_first, &q3_last, curr);
        }
        else if (tmpres > 0){
            dll_pintolist(&q1_first, &q1_last, curr);
        }
        else {
            dll_pintolist(&q2_first, &q2_last, curr);
        }
        curr = next;
    }

    //items are in separate groups, sort them
    if (q1_first != NULL && q1_first != q1_last)
        dll_quickersort(&q1_first, &q1_last, comparator);
    if (q3_first != NULL && q3_first != q3_last)
        dll_quickersort(&q3_first, &q3_last, comparator);

    //and join them to get q1_first --> q3_last
    if (q1_first == NULL) q1_first = q2_first;
    else {
        q1_last->right = q2_first;
        q2_first->left = q1_last;
    }

    if (q3_first == NULL || q3_last == NULL)
        q3_last = q2_last;
    else {
        q2_last->right = q3_first;
        q3_first->left = q2_last;
    }

    //pass results to the references
    (*last) = q3_last;
    (*first) = q1_first;
}

void dll_sort(contacts_unidb* db, int (*comparator)(struct contact_uninode*, struct contact_uninode*)){
    if (db->first == NULL) return;
    dll_quickersort(&(db->first), &(db->last), comparator);
}

struct contact_uninode* bt_get_by_index(contacts_unidb* db, uint32_t index){
    struct contact_uninode* curr = db->first;
    while (curr != NULL && curr->index != index){
        if (index < curr->index) curr = curr->left;
        else curr = curr->right;
    }

    return curr;
}

struct contact_uninode* bt_get_min_of(struct contact_uninode* item){
    if (item != NULL)
        while (item->left != NULL)
            item = item->left;
    return item;
}

struct contact_uninode* bt_get_succ_of(struct contact_uninode* item){
    struct contact_uninode* curr;
    if (item == NULL) return item;

    //if there is a right child go get its minimum
    if (item->right != NULL) return bt_get_min_of(item->right);
    else {
        //if there isn't right child we have to go up
        curr = item->parent;
        while (curr != NULL && item == curr->right){
            //go up while we are right child
            item = curr;
            curr = curr->parent;
        }
        return curr;
    }
}

void bt_rotate_left_by(contacts_unidb* db, struct contact_uninode* a){
    struct contact_uninode *b, *parent;
    b = a->right;
    if (b != NULL){
        parent = a->parent;
        a->right = b->left;
        if (a->right != NULL) a->right->parent = a;

        b->left = a;
        b->parent = parent;
        a->parent = b;

        if (parent != NULL) {
            if (parent->left == a)
                parent->left = b;
            else
                parent->right = b;
        }
        else
            db->first = b;
    }
}

void bt_rotate_right_by(contacts_unidb* db, struct contact_uninode* a){
    struct contact_uninode *b, *parent;
    b = a->left;
    if (b != NULL){
        parent = a->parent;
        a->left = b->right;
        if (a->left != NULL) a->left->parent = a;

        b->right = a;
        b->parent = parent;
        a->parent = b;

        if (parent != NULL) {
            if (parent->left == a)
                parent->left = b;
            else
                parent->right = b;
        }
        else
            db->first = b;
    }
}


void bt_insert(contacts_unidb* db, struct contact_uninode* item){
    if (db->first == NULL){ // set the root
        db->first = item;
        item->left = NULL;
        item->right = NULL;
        item->parent = NULL;
        item->is_red = false; //the root is black
    }
    else { //we need to go deeper
        struct contact_uninode* curr = db->first;
        while(true){
            if (item->index < curr->index){
                if (curr->left == NULL){
                    curr->left = item;
                    item->parent = curr;
                    break;
                }
                curr = curr->left;
            }
            else {
                if (curr->right == NULL){
                    curr->right = item;
                    item->parent = curr;
                    break;
                }
                curr = curr->right;
            }
        }

        //node has been inserted
        item->is_red = true; // we colorize newly-inserted as red
        struct contact_uninode* y;
        while (item->parent != NULL && item->parent->is_red){
            if (item->parent == item->parent->parent->left){ //if parent are left child
                y = item->parent->parent->right; //uncle
                if (y && y->is_red){ //1st case, NULL is obviously black
                    item->parent->is_red = false;
                    y->is_red = false;
                    item->parent->parent->is_red = true;
                    item = item->parent->parent;
                    continue;
                }
                if (item == item->parent->right){
                    item = item->parent;
                    bt_rotate_left_by(db, item);
                }

                item->parent->is_red = false;
                item->parent->parent->is_red = true;
                bt_rotate_right_by(db, item->parent->parent);
                break;
            }
            else { //mirrored cases
                y = item->parent->parent->left;
                if (y && y->is_red){
                    item->is_red = false;
                    y->is_red = false;
                    item->parent->parent->is_red = true;
                    item = item->parent->parent;
                    continue;
                }

                if (item == item->parent->left){
                    item = item->parent;
                    bt_rotate_right_by(db, item);
                }

                item->parent->is_red = false;
                item->parent->parent->is_red = true;
                bt_rotate_left_by(db, item->parent->parent);
                break;
            }
        }
    }
    db->first->is_red = false;
}

void bt_delete(contacts_unidb* db, struct contact_uninode* item){
    struct contact_uninode *w, *y, *z;

    if (item->left == NULL || item->right == NULL)
        y = item;
    else
        y = bt_get_succ_of(item);

    if (y->left != NULL)
        z = y->left;
    else
        z = y->right;

    if (z)
        z->parent = y->parent;

    if (y->parent == NULL)
        db->first = z;
    else if (y == y->parent->left)
        y->parent->left = z;
    else
        y->parent->right = z;

    if (y != item)
        item->index = y->index;

    //fixing R&B properties
    if (!y->is_red){ //only if successor of deleted item is black
        while (z != NULL && z != db->first && !z->is_red){ //while we are not root or red
            if (z == z->parent->left){ //if we are a left child
                w = z->parent->right; //brother of z

                if (!w) //brother is NULL (=>black)
                    continue;

                if (w && w->is_red){
                    w->is_red = false;
                    z->parent->is_red = true;
                    bt_rotate_left_by(db, z->parent);
                    w = z->parent->right;
                }

                if (w && (!w->left || !w->left->is_red) && (!w->right || !w->right->is_red)){
                    w->is_red = true;
                    z = z->parent;
                    continue;
                }

                if (w && (!w->right || !w->right->is_red)){
                    w->left->is_red = false;
                    w->is_red = true;
                    bt_rotate_right_by(db, w);
                    w = z->parent->right;
                }

                w->is_red = z->parent->is_red; //SIGSEGV
                w->right->is_red = false;
                z->parent->is_red = false;
                bt_rotate_left_by(db, z->parent);

                z = db->first;
            }
            else { //if we are a right child - mirror cases
                w = z->parent->left;

                if (!w) //brother is NULL (=>black)
                    continue;

                if (w->is_red){
                    w->is_red = false;
                    z->parent->is_red = true;
                    bt_rotate_right_by(db, z->parent);
                    w = z->parent->left;
                }

                if ((!w->left || !w->left->is_red) && (!w->right || !w->right->is_red)){
                    w->is_red = true;
                    z = z->parent;
                    continue;
                }

                if (!w->left || !w->left->is_red){
                    w->right->is_red = false;
                    w->is_red = true;
                    bt_rotate_left_by(db, w);
                    w = z->parent->left;
                }

                w->is_red = z->parent->is_red;
                z->parent->is_red = false;
                w->left->is_red = false;
                bt_rotate_right_by(db, z->parent);

                z = db->first;
            }
        }
    }
    if (z)
        z->is_red = false;
    free_node(y);
}

void bt_free_descend(struct contact_uninode* node){
    if (node->left) bt_free_descend(node->left);
    if (node->right) bt_free_descend(node->right);
    free_node(node);
}

void bt_free(contacts_unidb* db){
    if (db->first) bt_free_descend(db->first);
}


void bt_iterator_reset(contacts_unidb* db){
    if (db->first == NULL) return;
    db->current = bt_get_min_of(db->first);
}

bool bt_iterator_empty(contacts_unidb* db){
    return db->current == NULL;
}

struct contact_uninode* bt_iterator_next(contacts_unidb* db){
    struct contact_uninode* tmp = db->current;
    db->current = bt_get_succ_of(tmp);
    return tmp;
}

int comparator_surname(struct contact_uninode* first, struct contact_uninode* second){
    return strcmp(first->surname, second->surname);
}

int comparator_name(struct contact_uninode* first, struct contact_uninode* second){
    return strcmp(first->name, second->name);
}

int comparator_email(struct contact_uninode* first, struct contact_uninode* second){
    return strcmp(first->email, second->email);
}

int comparator_phone(struct contact_uninode* first, struct contact_uninode* second){
    return strcmp(first->phone, second->phone);
}

int comparator_birtdate(struct contact_uninode* first, struct contact_uninode* second){
    return strcmp(first->birthdate, second->birthdate);
}


uint32_t cunidb_add(contacts_unidb* db, char* name, char* surname,
    char* birthdate, char* email, char* phone, char* address){
    
    struct contact_uninode* node = create_node(db, name, surname, birthdate, email, phone, address);
    if (db->type == CONTACT_UNIDB_DLL){
        dll_insert(db, node);
    }
    if (db->type == CONTACT_UNIDB_BT){
        bt_insert(db, node);
    }

    return node->index;
}



void cunidb_free(contacts_unidb* db){
    if (db){
        if (db->type == CONTACT_UNIDB_DLL)
            dll_free_all(db);
        if (db->type == CONTACT_UNIDB_BT)
            bt_free(db);
        free(db);
    }

}

struct contact_uninode *cunidb_get(contacts_unidb *db, uint32_t index) {
    if (db->type == CONTACT_UNIDB_DLL)
        return dll_get_by_index(db, index);
    else if (db->type == CONTACT_UNIDB_BT)
        return bt_get_by_index(db, index);
    else return NULL;

}

void cunidb_iterator_reset(contacts_unidb *db) {
    if (db->type == CONTACT_UNIDB_DLL)
        dll_iterator_reset(db);
    if (db->type == CONTACT_UNIDB_BT)
        bt_iterator_reset(db);

}

bool cunidb_iterator_empty(contacts_unidb *db) {
    if (db->type == CONTACT_UNIDB_DLL)
        return dll_iterator_empty(db);
    if (db->type == CONTACT_UNIDB_BT)
        return bt_iterator_empty(db);
    return true;
}

struct contact_uninode *cunidb_iterator_next(contacts_unidb *db) {
    if (db->type == CONTACT_UNIDB_DLL)
        return dll_iterator_next(db);
    if (db->type == CONTACT_UNIDB_BT)
        return bt_iterator_next(db);
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
    if (db->type == CONTACT_UNIDB_BT)
        bt_delete(db, item);
}

void cunidb_sort(contacts_unidb *db, int sorttype) {
    int (*comparator)(struct contact_uninode*, struct contact_uninode*) = NULL;
    switch (sorttype){
        case CONTACT_UNIDB_SORT_BIRTDATE:
            comparator = &comparator_birtdate;
            break;
        case CONTACT_UNIDB_SORT_EMAIL:
            comparator = &comparator_email;
            break;
        case CONTACT_UNIDB_SORT_PHONE:
            comparator = &comparator_phone;
            break;
        case CONTACT_UNIDB_SORT_NAME:
            comparator = &comparator_name;
            break;
        case CONTACT_UNIDB_SORT_SURNAME:
        default:
            comparator = &comparator_surname;
            break;
    }

    if (db->type == CONTACT_UNIDB_DLL)
        dll_sort(db, comparator);
}
