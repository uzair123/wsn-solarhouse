#ifndef DBMS_H
#define DBMS_H

#include <stdio.h>
#include <stdlib.h>

#include "../tikidb-conf.h"
#include "../debug.h"
#include "dbms-operations.h"
#include "db.h"
#include "db-operations.h"
#include "db-list.h"

uint8_t dbms_init(void);
uint8_t dbms_close(void);

int dbms_create_table(uint8_t scheme[], uint8_t scheme_size, uint8_t key[], uint8_t key_size);
database* dbms_get_table(uint8_t table_id);
uint8_t* dbms_get_table_scheme(uint8_t table_id, unsigned int *size);
uint8_t dbms_clear_table(uint8_t table_id);
int dbms_clone_table(uint8_t table_id);
uint8_t dbms_drop_table(uint8_t table_id);

uint8_t* dbms_get_dump(uint8_t table_id, unsigned int *size, uint8_t *row_size, unsigned int *row_num);
int dbms_load_dump(uint8_t *dump);

uint8_t dbms_add_entry(uint8_t table_id, uint8_t scheme[], int entry[], uint8_t size);
uint8_t dbms_delete_entry(uint8_t table_id, uint8_t scheme[], int entry[], uint8_t size);
int dbms_has_entry(uint8_t table_id, uint8_t scheme[], int entry[], uint8_t size); 
int* dbms_get_entry(uint8_t table_id, unsigned int row, unsigned int *size);
int dbms_get_value(uint8_t table_id, unsigned int row, uint8_t type);

void dbms_printf_table(uint8_t table_id);

#endif // DBMS_H
