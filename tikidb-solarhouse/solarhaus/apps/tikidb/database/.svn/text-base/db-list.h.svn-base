#ifndef DB_LIST_H
#define DB_LIST_H

#include <stdio.h>
#include <stdlib.h>

#include "../tikidb-conf.h"
#include "../debug.h"
#include "db.h"

struct db_list_element {
    struct db_list_element *next;
    database *db;
} *db_list;

uint8_t db_list_size;

uint8_t db_list_init(void);
uint8_t db_list_close(void);
uint8_t db_list_add_table(database *db);
database* db_list_get_table(uint8_t table_id);
database* db_list_remove_table(uint8_t table_id);

#endif // DB_LIST_H
