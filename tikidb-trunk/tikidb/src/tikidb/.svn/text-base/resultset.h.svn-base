#ifndef DBMS_H
#define DBMS_H

#include "dbms-operations.h"
#include "table.h"
#include "tikidb-buffer.h"

void dbms_init(void);
void dbms_close(void);

table_id_t dbms_create_table(uint8_t scheme[], data_len_t scheme_size, uint8_t key[], data_len_t key_size);
//table_t* dbms_get_table(uint8_t table_id);
uint8_t* dbms_get_table_scheme(table_id_t table_id, unsigned int *size);
uint8_t dbms_clear_table(uint8_t table_id);
int dbms_clone_table(uint8_t table_id);
uint8_t dbms_drop_table(uint8_t table_id);

uint8_t dbms_add_entry(table_id_t table_id, uint8_t scheme[], int entry[], uint8_t size);
uint8_t dbms_delete_entry(table_id_t table_id, uint8_t scheme[], int entry[], uint8_t size);
int dbms_has_entry(table_id_t table_id, uint8_t scheme[], int entry[], uint8_t size);
int* dbms_get_entry(table_id_t table_id, unsigned int row, unsigned int *size);
//int dbms_get_value(table_id_t table_id, unsigned int row, uint8_t type);

//statement_id_t resultset_get_statement_id(uint8_t *resultset_arr);
uint16_t resultset_get_epoch(uint8_t *resultset_arr);
uint8_t resultset_get_num_entries(uint8_t *resultset_arr);

#endif // DBMS_H
