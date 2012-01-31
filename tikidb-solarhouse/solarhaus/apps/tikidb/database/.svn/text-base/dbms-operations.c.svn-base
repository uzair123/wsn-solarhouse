#include "dbms-operations.h"

/*---------------------------------------------------------------------------*/
/* Search free id for table.                                                 */
/* @return free id, else 0 if no id is free                                  */ 
uint8_t dbms_get_free_id(void) {
    if(db_list_size >= DBMS_MAX_TABLES) {
#ifdef DEBUG_DBMS
		printf("ERROR: dbms_get_free_id: max number of tables reached!\n\r");
#endif //DEBUG_DBMS
        return 0;
    }

    // search unused id
    uint8_t i;
    uint8_t db_new_id = db_last_id; 
    for(i = 0; i <= DBMS_MAX_TABLES; i++) {
        db_new_id++;
        if(db_new_id != 0 && !db_list_get_table(db_new_id)) {
            db_last_id = db_new_id;
            return db_last_id;
        }
    }

    return 0;
}

/*---------------------------------------------------------------------------*/
/* Do SELECT operation over a table. The given sequence of attributes for    */
/* select operation must be a subset of actual scheme of table.              */
/* @param uint8_t table_id id of table                                       */
/* @param uint8_t[] scheme scheme of entry, given in a sequence of integer.  */
/* @param uint8_t size of scheme definition                                  */
/* @return 0 if error occurs, else the id of table                           */
uint8_t dbms_select(uint8_t table_id, uint8_t scheme[], uint8_t size)
{
    // get old database
    database *db = db_list_get_table(table_id);

    // unknown id
	if(!db) {
#ifdef DEBUG_DBMS
		printf("ERROR: dbms_select: unknown table with id %d!\n\r", table_id);
#endif //DEBUG_DBMS
		return 0;
	}

    // create new database
    database *new_db = db_select(table_id, db, scheme, size);

    // memory full
	if(!new_db) {
#ifdef DEBUG_DBMS
		printf("ERROR: dbms_select: error while creating new database!\n\r");
#endif //DEBUG_DBMS
		return 0;
	}

    // replace old database with new
    db_drop(db_list_remove_table(table_id));
    db_list_add_table(new_db);

    return table_id;
}

/*---------------------------------------------------------------------------*/
/* Unions table 1 and table 2. table 2 is append on table 1.                 */
/* Already existing entries in table 1 will be overwritten. The table        */
/* must have the same scheme.                                                */
/* @param uint8_t table_id1 id of first table                                */
/* @param uint8_t table_id2 id of second table                               */
/* @return 0 if error occurs, else the id of table                           */
uint8_t dbms_union(uint8_t table_id1, uint8_t table_id2)
{
    if(!db_union(db_list_get_table(table_id1), db_list_get_table(table_id2))) {
#ifdef DEBUG_DBMS
		printf("ERROR: dbms_union: error while union!\n\r");
#endif //DEBUG_DBMS
		return 0;
    }

    return table_id1;
}
