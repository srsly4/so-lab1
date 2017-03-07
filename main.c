#include <stdio.h>
#include "contactlib.h"

int main(){
    printf("Hello world, oh deer.\n");
    int i = 0;
    struct contacts_unidb* db = cunidb_initialize(CONTACT_UNIDB_DLL);
    printf("Initialized.\n");

    printf("Adding test items...\n");
    cunidb_add(db, "Jan", "Kowalski", 10, "test@test.pl", "123456789", "Testowo 00-000, Testowa 00");
    cunidb_add(db, "Anna", "Kowalska", 10, "test@test.pl", "123456789", "Testowo 00-000, Testowa 00");

    printf("Finished adding items!\n");

    cunidb_free(db);
    printf("Deleted from memory.\n");
    return 0;
}