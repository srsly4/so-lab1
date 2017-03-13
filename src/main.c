#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <stdlib.h>
#include <dlfcn.h>
#include "contact.h"
#include <unistd.h>
#include <errno.h>

#ifdef DYNLIB
contacts_unidb* (*d_cunidb_initialize)(int type);
void (*d_cunidb_free)(contacts_unidb* db);
struct contact_uninode*  (*d_cunidb_add)(contacts_unidb* db, char* name, char* surname,
                                         char* birthdate, char* email, char* phone, char* address); //returns an index of created element
void (*d_cunidb_remove)(contacts_unidb* db, struct contact_uninode* item);
struct contact_uninode* (*d_cunidb_get)(contacts_unidb* db, uint32_t index);

//iterator
void (*d_cunidb_iterator_reset)(contacts_unidb* db);
bool (*d_cunidb_iterator_empty)(contacts_unidb* db);
struct contact_uninode* (*d_cunidb_iterator_next)(contacts_unidb* db);

//finding functions
struct contact_uninode* (*d_cunidb_find)(contacts_unidb* db, char* name, char* surname,
                                         char* birthdate, char* email, char* phone, char* address);

void (*d_cunidb_sort)(contacts_unidb* db, int sorttype);

void* contactlib;
void load_lib(){

    contactlib = dlopen("./../lib/libcontact-shared.so", RTLD_LAZY);
    if (!contactlib){
        fprintf(stderr, "Fatal error: could not have loaded dynamic library: %s\n", dlerror());
        exit(1);
    }

    d_cunidb_initialize = dlsym(contactlib, "cunidb_initialize");
    d_cunidb_free = dlsym(contactlib, "cunidb_free");
    d_cunidb_add = dlsym(contactlib, "cunidb_add");
    d_cunidb_remove = dlsym(contactlib, "cunidb_remove");
    d_cunidb_get = dlsym(contactlib, "cunidb_get");
    d_cunidb_iterator_reset = dlsym(contactlib, "cunidb_iterator_reset");
    d_cunidb_iterator_empty = dlsym(contactlib, "cunidb_iterator_empty");
    d_cunidb_iterator_next = dlsym(contactlib, "cunidb_iterator_next");
    d_cunidb_find = dlsym(contactlib, "cunidb_find");
    d_cunidb_sort = dlsym(contactlib, "cunidb_sort");

}

void close_lib(){
    dlclose(contactlib);
}
#endif


int parse_data(contacts_unidb* db, const char* filename){
    FILE* file = fopen(filename, "r");
    if (!file){
        fprintf(stderr, "Fatal error: Could not have opened sample data file.\n");
        exit(1);
    }
    char line[448];
    int curr = 0, field_ndx = 0;
    char* fields[6];
    fgets(line, 448, file); //skip first line
    while (fgets(line, 448, file)){
        for (int i = 0; i < 6; i++) fields[i] = NULL;
        fields[0] = line;
        field_ndx = 1;
        curr = 0;
        while (line[curr] != '\0' && line[curr] != '\n')
        {
            if (line[curr] == ';') {
                line[curr] = '\0';
                fields[field_ndx] = line + curr + 1;
                field_ndx++;
            }
            curr++;
        }
#ifndef DYNLIB
        cunidb_add(db, fields[0], fields[1], fields[2], fields[3], fields[4], fields[5]);
#else
        (*d_cunidb_add)(db, fields[0], fields[1], fields[2], fields[3], fields[4], fields[5]);
#endif

    }
    fclose(file);
    return 0;
}

void print_item(struct contact_uninode* item){
    if (item)
        printf("#%d %-12s %-12s %-10s %-12s %-12s %-12s\n", item->index, item->name, item->surname,
           item->birthdate, item->email, item->phone, item->address);
}


struct timespec *ts_start, *ts_end;
struct rusage *ru_start, *ru_end;

void start_measuring(){
    clock_gettime(CLOCK_REALTIME, ts_start);
    getrusage(RUSAGE_SELF, ru_start);
}
void print_measured(){
    clock_gettime(CLOCK_REALTIME, ts_end);
    getrusage(RUSAGE_SELF, ru_end);
    double real_delta_ms = (ts_end->tv_sec - ts_start->tv_sec)*1000.0
                            + (ts_end->tv_nsec - ts_start->tv_nsec)/1000000.0;
    double sys_delta_ms = (ru_end->ru_stime.tv_sec - ru_start->ru_stime.tv_sec)*1000
                            + (ru_end->ru_stime.tv_usec - ru_start->ru_stime.tv_usec)/1000.0;
    double usr_delta_ms = (ru_end->ru_utime.tv_sec - ru_start->ru_utime.tv_sec)*1000
                          + (ru_end->ru_utime.tv_usec - ru_start->ru_utime.tv_usec)/1000.0;
    printf(" \x1B[33m[real: %.4fms sys: %.4fms usr: %.4fms]\x1B[0m\n", real_delta_ms, sys_delta_ms, usr_delta_ms);
}


void test_cunidb(contacts_unidb* db){
    printf("---------\nTESTING CUNIDB library\n-------\n");
    if (!db){
        printf("Fatal error: db has not been initialized!\n");
        exit(1);
    }
    printf("Type: %s\n", db->type == CONTACT_UNIDB_BT ? "R&B Binary tree" : "Double-linked list");

    ts_start = malloc(sizeof(struct timespec));
    ts_end = malloc(sizeof(struct timespec));

    ru_start = malloc(sizeof(struct rusage));
    ru_end = malloc(sizeof(struct rusage));

    start_measuring();
    printf("=>Importing data...");
    parse_data(db, "../sample1000.csv");
    printf(" [+]");
    print_measured();

    start_measuring();
    printf("=>Listing data...");
#ifndef DYNLIB
    cunidb_iterator_reset(db);
    int counter = 0;
    while (!cunidb_iterator_empty(db)){
        struct contact_uninode* tt = cunidb_iterator_next(db);
        if (tt) counter++;
    }
#else
    (*d_cunidb_iterator_reset)(db);
    int counter = 0;
    while (!(*d_cunidb_iterator_empty)(db)){
        struct contact_uninode* tt = (*d_cunidb_iterator_next)(db);
        if (tt) counter++;
    }
#endif
    printf(" [%d items][+]", counter);
    print_measured();

    start_measuring();
    printf("=>Adding item (to the end)...");
#ifndef DYNLIB
#ifndef BUGGED
    struct contact_uninode* added =
            cunidb_add(db, "Test", "Added", "01-01-2001",
                       "test@test.pl", "123456789", "TestCity, Test 15");
#else
    struct contact_uninode* added = cunidb_get(db, 0);
    cunidb_add(db, "Test", "Added", "01-01-2001",
               "test@test.pl", "123456789", "TestCity, Test 15");
#endif
#else
    struct contact_uninode* added =
            (*d_cunidb_add)(db, "Test", "Added", "01-01-2001",
                       "test@test.pl", "123456789", "TestCity, Test 15");
#endif
    printf(" [#%d][+]", added->index);
    print_measured();

    start_measuring();
    struct contact_uninode* searched;
    printf("=>Searching for first item from file... (optimistic)");
#ifndef DYNLIB
    searched = cunidb_find(db, "H", "Bell", NULL, "hbell0@ezinearticles.com", NULL, NULL);
#else
    searched = (*d_cunidb_find)(db, "H", "Bell", NULL, "hbell0@ezinearticles.com", NULL, NULL);
#endif
    if (searched) {
        printf(" [#%d][+]", searched->index);
    }
    else printf(" [not found]");
    print_measured();

    start_measuring();
    printf("=>Searching for the last item from file... (pessimistic)");
#ifndef DYNLIB
    searched = cunidb_find(db, "C", "Perez", NULL, NULL, NULL, NULL);
#else
    searched = (*d_cunidb_find)(db, "C", "Perez", NULL, NULL, NULL, NULL);
#endif
    if (searched) {
        printf(" [#%d][+]", searched->index);
    }
    else printf(" [not found]");
    print_measured();

    start_measuring();
    printf("=>Deleting random item #[2, n-1]...");
    uint32_t ndx = (rand() % (added->index-1)) + 1;
#ifndef DYNLIB
    cunidb_remove(db, cunidb_get(db, ndx));
#else
    (*d_cunidb_remove)(db, (*d_cunidb_get)(db, ndx));

#endif
    printf(" [#%d][+]", ndx);
    print_measured();

    start_measuring();
    printf("=>Deleting first item (or root)... (optimistic)");
#ifndef DYNLIB
    cunidb_remove(db, db->first);
#else
    (*d_cunidb_remove)(db, db->first);
#endif
    printf(" [+]");
    print_measured();

    start_measuring();
    printf("=>Deleting last item (by index)... (pessimistic)");
#ifndef DYNLIB
    cunidb_remove(db, cunidb_get(db, added->index-1));
#else
    (*d_cunidb_remove)(db, (*d_cunidb_get)(db, added->index-1));
#endif
    printf(" [+]");
    print_measured();

    start_measuring();
    printf("=>Rebuilding database with surname sorting...");
#ifndef DYNLIB
    cunidb_sort(db, CONTACT_UNIDB_SORT_SURNAME);
#else
    (*d_cunidb_sort)(db, CONTACT_UNIDB_SORT_SURNAME);
#endif
    printf(" [+]");
    print_measured();

    start_measuring();
    printf("=>Listing data again (checking db consistency)...");
#ifndef DYNLIB
    cunidb_iterator_reset(db);
    counter = 0;
    while (!cunidb_iterator_empty(db)){
        struct contact_uninode* tt = cunidb_iterator_next(db);
        if (tt) counter++;
    }
#else
    (*d_cunidb_iterator_reset)(db);
    counter = 0;
    while (!(*d_cunidb_iterator_empty)(db)){
        struct contact_uninode* tt = (*d_cunidb_iterator_next)(db);
        if (tt) counter++;
    }
#endif
    printf(" [%d items][+]", counter);
    print_measured();

    free(ts_start);
    free(ts_end);
    free(ru_end);
    free(ru_start);

    printf("-------\nTEST FINISHED\n--------\n\n");

}

int main(void){
    time_t t;
    srand((unsigned int)time(&t));

#ifdef DYNLIB
    load_lib();
#endif

#ifndef DYNLIB
    contacts_unidb* db = cunidb_initialize(CONTACT_UNIDB_DLL);
    test_cunidb(db);
    cunidb_free(db);

    db = cunidb_initialize(CONTACT_UNIDB_BT);
    test_cunidb(db);
    cunidb_free(db);
#else
    contacts_unidb* db = (*d_cunidb_initialize)(CONTACT_UNIDB_DLL);
    test_cunidb(db);
    (*d_cunidb_free)(db);

    db = (*d_cunidb_initialize)(CONTACT_UNIDB_BT);
    test_cunidb(db);
    (*d_cunidb_free)(db);
#endif

#ifdef DYNLIB
    close_lib();
#endif
    return 0;
}