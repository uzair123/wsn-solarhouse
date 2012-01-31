#ifndef TIKIDB_BUFFER_H
#define TIKIDB_BUFFER_H

#include <stdio.h>
#include <stdlib.h>

#include "table.h"
#include "resultset.h"

void tikidb_buffer_init(void);
void tikidb_buffer_close(void);
bool tikidb_buffer_add_table(table_t *db);
table_t* tikidb_buffer_get_table_id(table_id_t table_id);
table_t* tikidb_buffer_remove_table(table_id_t table_id);
table_id_t tikidb_buffer_get_free_id(void);

#endif // TIKIDB_BUFFER_H
