#include <stdio.h>
#include "contactlib.h"

struct contacts_unidb* db;

int parse_data(const char* filename){
    FILE* file = fopen(filename, "r");
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

    return 0;
}

int main(){
    printf("Hello world, oh deer.\n");
    int i = 0;
    db = cunidb_initialize(CONTACT_UNIDB_DLL);
    printf("Initialized.\n");

    printf("Adding test items...\n");
    cunidb_add(db, "Jan", "Kowalski", "01-01-2001", "test@test.pl", "123456789", "Testowo 00-000, Testowa 00");
    cunidb_add(db, "Anna", "Kowalska", "01-01-2001", "test@test.pl", "123456789", "Testowo 00-000, Testowa 00");

    printf("Adding CSV items...\n");
    parse_data("sample100.csv");

    printf("Finished adding items!\n");
    printf("Iterating through elemets:\n");

    cunidb_iterator_reset(db);
    while (!cunidb_iterator_empty(db))
    {
        struct contact_uninode* item = cunidb_iterator_next(db);
        printf("#%d: %s %s\n", item->index, item->name, item->surname);
    }

    printf("Trying to get index #2\n");
    struct contact_uninode* item = cunidb_get(db, 2);
    printf("#%d: %s %s\n", item->index, item->name, item->surname);

    printf("Deleting from memory...\n");
    cunidb_free(db);
    return 0;
}