#include "dbms-operations.h"

/**
 * \brief		Do SELECT operation over a table. The given sequence of attributes for
 * 				select operation must be a subset of actual scheme of table.
 *
 * \param table_id	id of table
 * \param scheme	scheme of entry, given in a sequence of integer.
 * \param size		size of scheme definition
 *
 * \return 			0 if error occurs, else the id of table*/
table_id_t dbms_select(table_id_t table_id, uint8_t scheme[], uint8_t size) {

	// get pointer to old table
	table_t *table = tikidb_buffer_get_table_id(table_id);

	// unknown id
	if (!table) {
#ifdef DEBUG_DBMS
		printf("ERROR: dbms_select: unknown table with id %d!\n\r", table_id);
#endif //DEBUG_DBMS
		return 0;
	}

	// create new table
	table_t *new_table = table_select(table_id, table, scheme, size);

	// memory full
	if (!new_table) {
#ifdef DEBUG_DBMS
		printf("ERROR: dbms_select: error while creating new table!\n\r");
#endif //DEBUG_DBMS
		return 0;
	}

	// replace old table with new
	table_free(tikidb_buffer_remove_table(table_id));
	tikidb_buffer_add_table(new_table);

	return table_id;
}

/*---------------------------------------------------------------------------*/
/* Performs an aggregate operation over a table. Its possible to give     */
/* a list of attributes for an implict group by operation. An explicit call  */
/* of groupby is not necassary. The result table is a new independet      */
/* table with its own memory.                                             */
/* @param uint8_t table_id id of table                                       */
/* @param aggregation_type op type of aggregation                       */
/* @param uint8_t attribute attribute to aggregate over                      */
/* @param uint8_t group[] attribute to group by                              */
/* @param uint8_t size size of group by attributes                           */
/* @return 0 if error occurs, else the id of table                           */
table_id_t dbms_aggregate(table_id_t table_id, aggregation_type op,
		uint8_t attribute, uint8_t group[], uint8_t size) {
	// get old table
	table_t *db = tikidb_buffer_get_table_id(table_id);

	// create new table
	table_t *new_table = table_aggregate(table_id, db, op, attribute, group, size);

	printf("dbms_aggregate(...): after db_aggregate, new_db is %p\n", new_table);

	if (!new_table) {
#ifdef DEBUG_DBMS
		printf("ERROR: dbms_aggregate: error while aggregate operation!\n\r");
#endif //DEBUG_DBMS
		return 0;
	}

	// replace old table with new
	table_free(tikidb_buffer_remove_table(table_id));
	tikidb_buffer_add_table(new_table);

	return table_id;
}

/*---------------------------------------------------------------------------*/
/* Performs an WHERE operation over a table.                              */
/* @param uint8_t table_id id of table                                       */
/* @param comparison_type op type of comparison                         */
/* @param uint8_t attribute attribute to compare                             */
/* @param int value value to compare again                                   */
/* @return 0 if error occurs, else the id of table                           */
table_id_t dbms_where(table_id_t table_id, comparison_operator op,
		uint8_t attribute, int value) {
	if (!table_where(tikidb_buffer_get_table_id(table_id), op, attribute, value)) {
#ifdef DEBUG_DBMS
		printf("ERROR: dbms_where: error while where operation!\n\r");
#endif //DEBUG_DBMS
		return 0;
	}

	return table_id;
}
