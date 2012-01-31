#include "resultset.h"

/*---------------------------------------------------------------------------*/
/* Creates a new table with given scheme.                                    */
/* @param uint8_t[] scheme scheme of table, given in a sequence of integer.  */
/* @param uint8_t size of scheme definition                                  */
/* @param uint8_t[] key set of attributes choosen for primary key            */
/* @param uint8_t p_key_size numbers of attributes for primary key           */
/* @return 0 on an error, else the id of table               */
table_id_t dbms_create_table(uint8_t scheme[], data_len_t scheme_size,
		uint8_t key[], data_len_t key_size) {
	// search unused id
	table_id_t id = tikidb_buffer_get_free_id();

	if (!id) {
#ifdef DEBUG_DBMS
		printf("ERROR: dbms_create_table: max number of tables reached!\n\r");
#endif //DEBUG_DBMS
		return 0;
	}

	// create table
	table_t *new_table = table_create(id, scheme, scheme_size, key, key_size);

	// memory full
	if (!new_table) {
#ifdef DEBUG_DBMS
		printf("ERROR: dbms_create_table: memory full!\n\r");
#endif //DEBUG_DBMS
		return 0;
	}

	// add table to list
	if (!tikidb_buffer_add_table(new_table)) {
#ifdef DEBUG_DBMS
		printf("ERROR: dbms_create_table: error while adding to list!\n\r");
#endif //DEBUG_DBMS
		table_free(new_table);
		return 0;
	}

	return id;
}

/*---------------------------------------------------------------------------*/
/* Returns scheme of table with given id.                                    */
/* @param uint8_t table_id id of table                                       */
/* @param int* size size of returned char array                              */
/* @return int* pointer to scheme, if error occurs NULL                      */
uint8_t* dbms_get_table_scheme(table_id_t table_id, unsigned int *size) {
	table_t *db = tikidb_buffer_get_table_id(table_id);

	if (db) {
		*size = db->scheme_size;
		return db->scheme;
	}

	*size = 0;
	return NULL;
}

/*---------------------------------------------------------------------------*/
/* Clears a table and deletes all entries.                                */
/* @param uint8_t table_id id of table                                       */
/* @return 1 if error occurs else 0                                          */
uint8_t dbms_clear_table(uint8_t table_id) {
	if (!table_clear(tikidb_buffer_get_table_id(table_id)))
		return 1;

	return 0;
}

/*---------------------------------------------------------------------------*/
/* Clones a table and returns new id of an independet table.           */
/* Dont forget to drop table later for freeing memory space!              */
/* @param uint8_t table_id id of table                                       */
/* @return error code, else id of new table                                  */
/* error codes:                                                              */
/* -4: max number of tables reached                                          */
/* -5: memory full                                                           */
/* -6: unknown error                                                         */
int dbms_clone_table(uint8_t table_id) {
	// search unused id
	uint8_t id = tikidb_buffer_get_free_id();

	if (!id) {
#ifdef DEBUG_DBMS
		printf("ERROR: dbms_clone_table: max number of tables reached!\n\r");
#endif //DEBUG_DBMS
		return -4;
	}

	// clone table
	table_t *db = table_clone(id, tikidb_buffer_get_table_id(table_id));
	if (!db)
		return -5;

	// add table to catalog
	if (!tikidb_buffer_add_table(db)) {
#ifdef DEBUG_DBMS
		printf("ERROR: dbms_clone_table: error while adding to list!\n\r");
#endif //DEBUG_DBMS
		table_free(db);
		return -6;
	}

	return id;
}

/*---------------------------------------------------------------------------*/
/* Deletes table permantly.                                                  */
/* @param uint8_t table_id id of table                                       */
/* @return 1 if error occurs else 0                                          */
uint8_t dbms_drop_table(uint8_t table_id) {
	table_t *db = tikidb_buffer_get_table_id(table_id);

	if (db) {
		table_free(tikidb_buffer_remove_table(table_id));
		return 0;
	}

	return 1;
}

/*---------------------------------------------------------------------------*/
/* Adds entry to an existing table. If entry already with equal key          */
/* attributes already exists, it will be overwritten.                        */
/* @param uint8_t table_id id of table                                       */
/* @param uint8_t[] scheme scheme of entry, given in a sequence of integer.  */
/* @param int[] entry values                                                 */
/* @param uint8_t size numbers of attributes                                 */
/* @return 1 if error occurs else 0                                          */
uint8_t dbms_add_entry(table_id_t table_id, uint8_t scheme[], int entry[],
		uint8_t size) {
	// error occurs
	if (!table_add_entry(tikidb_buffer_get_table_id(table_id), scheme, entry,
			size)) {
#ifdef DEBUG_DBMS
		printf("ERROR: dbms_table_add_entry: error while adding entry!\n\r");
#endif //DEBUG_DBMS
		return 1;
	}

	return 0;
}

/*---------------------------------------------------------------------------*/
/* Deletes a table entry with given scheme. The entry is searched         */
/* only with key attributes. Other attributes doesnt have any effect.        */
/* @param uint8_t table_id id of table                                       */
/* @param uint8_t[] scheme scheme of entry, given in a sequence of integer.  */
/* @param int[] entry values                                                 */
/* @param uint8_t size numbers of attributes                                 */
/* @return 1 if error occurs else 0                                          */
uint8_t dbms_delete_entry(table_id_t table_id, uint8_t scheme[], int entry[],
		uint8_t size) {
	// error occurs
	if (!table_delete_entry(tikidb_buffer_get_table_id(table_id), scheme,
			entry, size)) {
#ifdef DEBUG_DBMS
		printf("ERROR: dbms_table_add_entry: error while adding entry!\n\r");
#endif //DEBUG_DBMS
		return 1;
	}

	return 0;
}

/*---------------------------------------------------------------------------*/
/* Checks if entry already exists. The check will be only done with key      */
/* attributes. Other attributes must not match for a positive result.        */
/* @param uint8_t table_id id of table                                       */
/* @param uint8_t[] scheme scheme of entry, given in a sequence of integer.  */
/* @param int[] entry values (especialy key)                                 */
/* @param uint8_t size numbers of attributes                                 */
/* @return uint8_t -1 if entry doesn't exists, otherwise row in table     */
/* error codes:                                                              */
/* -1: entry doesnt exists                                                   */
/* -2: not all primary keys are given in entry                               */
/* -3: table is NULL pointer                                              */
/* -4: max number of tables reached                                          */
/* -5: memory full                                                           */
/* -6: unknown error                                                         */
int dbms_has_entry(table_id_t table_id, uint8_t scheme[], int entry[],
		uint8_t size) {
	return table_has_entry(tikidb_buffer_get_table_id(table_id), scheme, entry,
			size);
}

/*---------------------------------------------------------------------------*/
/* Gets specific row of table. For an specific entry the method           */
/* dbms_has_entry can be used to get the row.                                */
/* @param uint8_t table_id id of table                                       */
/* @param int row row number                                                 */
/* @param int* size size of returned char array                              */
/* @return int* pointer to data row, if error occurs NULL                    */
int* dbms_get_entry(table_id_t table_id, unsigned int row, unsigned int *size) {
	table_t *db = tikidb_buffer_get_table_id(table_id);

	if (db) {
		*size = db->scheme_size;
		return table_get_entry(db, row);
	}

	*size = 0;
	return NULL;
}

///*---------------------------------------------------------------------------*/
///* Gets specific value from table. For an specific entry the method       */
///* dbms_has_entry can be used to get the row.                                */
///* @param uint8_t table_id id of table                                       */
///* @param int row row number                                                 */
///* @param uint8_t type type of value, must be exist in scheme of table       */
///* @return int value of attribute, 0 if value wasnt found                    */
//int dbms_get_value(table_id_t table_id, unsigned int row, uint8_t type)
//{
//    table_t *db = db_list_get_table(table_id);
//
//    if(db) {
//        int *entry = db_get_entry(db, row);
//
//        if(entry) {
//            int attribute;
//            for(attribute = 0; attribute < db->scheme_size; attribute++) {
//                if(type == db->scheme[attribute])
//                    return entry[attribute];
//            }
//
//            return 0;
//        }
//    }
//
//    return 0;
//}

///* -------------------------------------------------------------------- */
//statement_id_t resultset_get_statement_id(uint8_t *resultset_arr) {
//	return resultset_arr[1];
//}

/**
 * \brief		Returns the epoch number from a dump
 *
 * \param resultset	the result set from which the epoch needs to be extracted
 * \return the epoch number
 */
uint16_t resultset_get_epoch(uint8_t *resultset_arr) {
	return resultset_arr[1 + // message type (added by Khalid, Pablo)
			1 + // statement id
			4 + // slot length (sample period / tree height)
			1 + // scheme size
			resultset_arr[6] + // the scheme itself
			1 + // key size
			resultset_arr[7 + resultset_arr[6]] + // the keys themselves
			1 + // amount of entries
			2 // node id
	] + resultset_arr[1 + // message type (added by Khalid, Pablo)
			1 + // statement id
			4 + // slot length (sample period / tree height)
			1 + // scheme size
			resultset_arr[6] + // the scheme itself
			1 + // key size
			resultset_arr[7 + resultset_arr[6]] + // the keys themselves
			1 + // amount of entries
			2 + // node id
			1 // epoch, 2nd byte
			] * 256;

}

/*--------------------------------------------------------------------------*/
/* Returns the number of entries (rows) that a dump has						*/
/* @param uint8_t* dump table dump                                   	*/
/* @return the number of entries											*/
uint8_t resultset_get_num_entries(uint8_t *resultset_arr) {
	return resultset_arr[1 + // message type (added by Khalid, Pablo)
			1 + // statement id
			4 + // slot length (sample period / tree height)
			1 + // scheme size
			resultset_arr[6] + // the scheme itself
			1 + // key size
			resultset_arr[7 + resultset_arr[6]]];

}

