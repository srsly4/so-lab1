#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <stdlib.h>
#include "contactlib.h"


int parse_data(contacts_unidb* db, const char* filename){
    FILE* file = fopen(filename, "r");
    if (!file){
        perror("Could not have opened sample data file.");
        return 1;
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
        cunidb_add(db, fields[0], fields[1], fields[2], fields[3], fields[4], fields[5]);

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
    double cpu_delta_ms = (ru_end->ru_stime.tv_sec - ru_start->ru_stime.tv_sec)*1000
                            + (ru_end->ru_stime.tv_usec - ru_start->ru_stime.tv_usec)/1000000.0;
    double usr_delta_ms = (ru_end->ru_utime.tv_sec - ru_start->ru_utime.tv_sec)*1000
                          + (ru_end->ru_utime.tv_usec - ru_start->ru_utime.tv_usec)/1000000.0;
    printf(" \x1B[33m[real: %.4fms cpu: %.4fms usr: %.4fms]\x1B[0m\n", real_delta_ms, cpu_delta_ms, usr_delta_ms);
}


void test_cunidb(contacts_unidb* db){
    ts_start = malloc(sizeof(struct timespec));
    ts_end = malloc(sizeof(struct timespec));
    ru_start = malloc(sizeof(struct rusage));
    ru_end = malloc(sizeof(struct rusage));

    printf("---------\nTESTING CUNIDB library\n-------\n");
    printf("Type: %s\n", db->type == CONTACT_UNIDB_BT ? "R&B Binary tree" : "Double-linked list");

    start_measuring();
    printf("=>Importing data...");
    parse_data(db, "sample1000.csv");
    printf(" [+]");
    print_measured();

    start_measuring();
    printf("=>Listing data...");
    cunidb_iterator_reset(db);
    int counter = 0;
    while (!cunidb_iterator_empty(db)){
        struct contact_uninode* tt = cunidb_iterator_next(db);
        if (tt) counter++;
    }
    printf(" [%d items][+]", counter);
    print_measured();

    start_measuring();
    printf("=>Adding item (to the end)...");
    struct contact_uninode* added =
            cunidb_add(db, "Test", "Added", "01-01-2001",
                       "test@test.pl", "123456789", "TestCity, Test 15");
    printf(" [#%d][+]", added->index);
    print_measured();

    start_measuring();
    struct contact_uninode* searched;
    printf("=>Searching for first item from file... (optimistic)");
    searched = cunidb_find(db, "H", "Bell", NULL, "hbell0@ezinearticles.com", NULL, NULL);
    if (searched) {
        printf(" [#%d][+]", searched->index);
    }
    else printf(" [not found]");
    print_measured();

    start_measuring();
    printf("=>Searching for the last item from file... (pessimistic)");
    searched = cunidb_find(db, "C", "Perez", NULL, NULL, NULL, NULL);
    if (searched) {
        printf(" [#%d][+]", searched->index);
    }
    else printf(" [not found]");
    print_measured();

    start_measuring();
    printf("=>Deleting random item #[2, n-1]...");
    uint32_t ndx = (rand() % (added->index-1)) + 1;
    cunidb_remove(db, cunidb_get(db, ndx));
    printf(" [#%d][+]", ndx);
    print_measured();

    start_measuring();
    printf("=>Deleting first item (or root)... (optimistic)");
    cunidb_remove(db, db->first);
    printf(" [+]");
    print_measured();

    start_measuring();
    printf("=>Deleting last item (by index)... (pessimistic)");
    cunidb_remove(db, cunidb_get(db, added->index-1));
    printf(" [+]");
    print_measured();

    start_measuring();
    printf("=>Rebuilding database with surname sorting...");
    cunidb_sort(db, CONTACT_UNIDB_SORT_SURNAME);
    printf(" [+]");
    print_measured();

    start_measuring();
    printf("=>Listing data again (checking db consistency)...");
    cunidb_iterator_reset(db);
    counter = 0;
    while (!cunidb_iterator_empty(db)){
        struct contact_uninode* tt = cunidb_iterator_next(db);
        if (tt) counter++;
    }
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

    contacts_unidb* db = cunidb_initialize(CONTACT_UNIDB_DLL);
    test_cunidb(db);
    cunidb_free(db);

    db = cunidb_initialize(CONTACT_UNIDB_BT);
    test_cunidb(db);
    cunidb_free(db);

    return 0;
}