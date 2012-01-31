#ifndef DATABASE_H
#define DATABASE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../tikidb-conf.h"
#include "../debug.h"

typedef struct {
    uint8_t id;             // id of database
    uint8_t *scheme;        // scheme  
    uint8_t scheme_size;    // size of scheme
    uint8_t *key_index;     // pointer to field of indexes which are key, each index points to an attribute in scheme
    uint8_t key_size;       // number of attributes in primary key
    unsigned int entries;   // num of entries
    int *rows[DB_MAX_SIZE]; // array of pointer to rows
} database;

database* db_create(uint8_t table_id, uint8_t scheme[], uint8_t scheme_size, uint8_t key[], uint8_t key_size);
database* db_clear(database *db);
database* db_clone(uint8_t table_id, database *db);
void db_drop(database *db);

int db_ctoi(char a, char b);
void db_itoc(int val, char *a, char *b);
uint16_t db_dump_get_epoch(uint8_t *dump);
uint8_t db_dump_get_num_entries(uint8_t *dump);
uint8_t* db_get_dump(database *db, unsigned int *size, uint8_t *row_size, unsigned int *row_num);
database* db_load_dump(uint8_t table_id, uint8_t *dump);

database* db_add_entry(database *db, uint8_t scheme[], int entry[], uint8_t size);
database* db_delete_entry(database *db, uint8_t scheme[], int entry[], uint8_t size);
database* db_delete_row(database *db, unsigned int row);
int db_has_entry(database *db, uint8_t scheme[], int entry[], uint8_t size); 
int* db_get_entry(database *db, unsigned int row); 
int db_has_attribute(database *db, uint8_t attribute); 

void db_printf(database *db);

#endif // DATABASE_H
