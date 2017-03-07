#include <stdio.h>
#include "contactlib.h"

int main(){
    printf("Hello world, oh deer.\n");
    int i = 0;
    struct contacts_unidb* db = cunidb_initialize(CONTACT_UNIDB_DLL);
    printf("Initialized.\n");
    cunidb_free(db);
    printf("Deleted from memory.\n");
    return 0;
}