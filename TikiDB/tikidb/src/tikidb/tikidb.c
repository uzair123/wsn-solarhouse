#include "tikidb.h"

/**
 * \brief		Initializes TikiDB with the necessary components
 */
void tikidb_init(void(*callback_result)(uint8_t* result_dump,
		select_statement_t *ss)) {

	// Initialize the query manager datebase.
	tikidb_mgr_init(callback_result);
}

/**
 * \brief		Shuts TikiDB down.
 **/
void tikidb_close(void) {
	tikidb_mgr_close();
	return;
}

/**
 * \brief Interface for a new query
 */
void tikidb_process_statement(uint8_t* statement_arr, uint8_t statement_size) {
	tikidb_mgr_statement(statement_arr, statement_size);
}

///**
// * \brief
// */
//bool tikidb_open_scope(scope_id_t super_scope_id, scope_id_t scope_id,
//		void *specs, data_len_t spec_len) {
//	return querymanager_open_scope(super_scope_id, scope_id, specs, spec_len);
//}
//
///**
// * \brief
// */
//void tikidb_close_scope(scope_id_t scope_id) {
//	querymanager_close_scope(scope_id);
//}
