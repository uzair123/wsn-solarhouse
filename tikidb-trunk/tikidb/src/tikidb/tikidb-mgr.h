#ifndef TIKIDB_MGR_H
#define TIKIDB_MGR_H

#include "contiki.h"
#include "net/rime.h"

#include "tikidb-types.h"
#include "queryresolver.h"
#include "statement.h"
#include "resultset.h"

// Scopes Framework - related includes and defines
#include "scopes.h"
#include "scopes-selfur.h"
#include "scopes-membership-simple.h"
#include "scopes-application.h"

#define TIKIDB_SUBID	2       // TikiDB is a subscriber app for the scope framework
#define SCOPE_TTL 		SELFUR_ROUTING_REFRESH_INTERVAL/((uint16_t)3)// scope's time to live, equals tree rebuild interval (120s) / 3 == 40s

// --------------------------------------------------------------------
// Callback variable
void (*querymanager_cb_result)(uint8_t *resultset_arr,
		select_statement_t *ss);

void cb_add_scope(scope_id_t scope_id);
void cb_remove_scope(scope_id_t scope_id);
void cb_join_scope(scope_id_t scope_id);
void cb_leave_scope(scope_id_t scope_id);
void cb_receive_data(scope_id_t scope_id, void *data,
		data_len_t data_len, bool to_creator, const rimeaddr_t *source);

// --------------------------------------------------------------------
void tikidb_mgr_init(void(*callback_result)(uint8_t* resultset_arr,
		select_statement_t *ss));
void tikidb_mgr_close();
void tikidb_mgr_statement(uint8_t* statement_arr, uint8_t statement_size);


#endif // TIKIDB_MGR_H
