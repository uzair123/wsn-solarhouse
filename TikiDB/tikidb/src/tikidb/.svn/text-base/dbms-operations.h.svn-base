#ifndef DBMS_OPERATIONS_H
#define DBMS_OPERATIONS_H

#include "table.h"
#include "tikidb-buffer.h"

//table_id_t last_table_id; // last given id to a new table

table_id_t dbms_select(table_id_t table_id, uint8_t scheme[], uint8_t size);

table_id_t dbms_aggregate(table_id_t table_id, aggregation_type op, uint8_t attribute, uint8_t group[], uint8_t size);
table_id_t dbms_where(table_id_t table_id, comparison_operator op, uint8_t attribute, int value);

#endif // DBMS_OPERATIONS_H
