#ifndef DBMS_OPERATIONS_H
#define DBMS_OPERATIONS_H

#include <stdio.h>
#include <stdlib.h>

#include "../tikidb-conf.h"
#include "../debug.h"
#include "db.h"
#include "db-operations.h"
#include "db-list.h"

uint8_t db_last_id; // last given id to a new database

uint8_t dbms_get_free_id(void);

uint8_t dbms_select(uint8_t table_id, uint8_t scheme[], uint8_t size);
uint8_t dbms_union(uint8_t table_id1, uint8_t table_id2);

#endif // DBMS_OPERATIONS_H
