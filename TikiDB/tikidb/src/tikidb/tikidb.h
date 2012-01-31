#ifndef TIKIDB_H
#define TIKIDB_H

#include "contiki.h"
#include "net/rime.h"

#include "tikidb-types.h"

#include "queryresolver.h"
#include "tikidb-mgr.h"
#include "resultset.h"


// --------------------------------------------------------------------
// Constructor and destructor
void tikidb_init(void(*callback_result)(uint8_t* result_dump, select_statement_t *ss));
void tikidb_close(void);

// --------------------------------------------------------------------
// Interface methods
void tikidb_process_statement(uint8_t* statement_arr, uint8_t statement_size);

//bool tikidb_open_scope(scope_id_t super_scope, scope_id_t scope_id,
//                          void *specs, data_len_t spec_len);
//
//void tikidb_close_scope(scope_id_t scope_id);

#endif // TIKIDB_H
