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
    fclose(file);
    return 0;
}

void print_item(struct contact_uninode* item){
    printf("%-12s %-12s %-10s %-12s %-12s %-12s\n", item->name, item->surname,
           item->birthdate, item->email, item->phone, item->address);
}

int main(){
    db = cunidb_initialize(CONTACT_UNIDB_DLL);

    printf("Adding CSV items...\n");
    parse_data("sample100.csv");

    cunidb_sort(db, CONTACT_UNIDB_SORT_SURNAME);

    printf("Trying to delete item #74\n");
    cunidb_remove(db, cunidb_get(db, 74));

    printf("Iterating through elemets:\n");

    cunidb_iterator_reset(db);
    while (!cunidb_iterator_empty(db))
    {
        print_item(cunidb_iterator_next(db));
    }

    printf("Trying to get index #73\n");
    struct contact_uninode* item = cunidb_get(db, 73);
    if (item) print_item(item);

    printf("Trying to find something...\n");
    item = cunidb_find(db, "An", "Finch", NULL, NULL, NULL, NULL);
    if (item)
        print_item(item);

    printf("Deleting from memory...\n");
    cunidb_free(db);



    /* BinaryTree Test */
//    db = cunidb_initialize(CONTACT_UNIDB_BT);
//    printf("Initialized.\n");
//    printf("Adding CSV items...\n");
//    parse_data("sample100.csv");
//
//    printf("Freeing memory...");
//    cunidb_free(db);
//
//    printf("Finished!");
    return 0;
}