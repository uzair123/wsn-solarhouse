#ifndef TABLE_H
#define TABLE_H

//#include "stdlib.h"
#include "contiki.h"
#include "tikidb-types.h"

table_t* table_create(table_id_t table_id, uint8_t scheme[], data_len_t scheme_size, uint8_t key[], data_len_t key_size);
table_t* table_clear(table_t *table);
table_t* table_clone(table_id_t table_id, table_t *table);
void table_free(table_t *table);

uint8_t* table_marshall(table_t *table, data_len_t *size);
table_t* table_unmarshall(table_id_t table_id, uint8_t *resultset_arr);

table_t* table_add_entry(table_t *table, uint8_t scheme[], int entry[], uint8_t size);
table_t* table_delete_entry(table_t *table, uint8_t scheme[], int entry[], uint8_t size);
table_t* table_delete_row(table_t *table, unsigned int row);
int table_has_entry(table_t *table, uint8_t scheme[], int entry[], uint8_t size); 
int* table_get_entry(table_t *table, unsigned int row); 
int table_has_attribute(table_t *table, uint8_t attribute); 

void table_printf(table_t *table);

table_t* table_select(table_id_t table_id, table_t *table, uint8_t scheme[], data_len_t size);
table_t* table_union(table_t *table1, table_t *table2);
table_t* table_aggregate(uint8_t table_id, table_t *table, aggregation_type op, uint8_t attribute, uint8_t group[], uint8_t size);
table_t* table_where(table_t *db, comparison_operator op, uint8_t attribute, int value);



#endif // TABLE_H
