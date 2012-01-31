#include "db-list.h"

/*---------------------------------------------------------------------------*/
/* Initialize database list.                                                 */
/* @return error code, else 0                                                */
uint8_t db_list_init(void) {
	db_list = NULL;
	db_list_size = 0;
	return 0;
}

/*---------------------------------------------------------------------------*/
/* Shutdowns database list.                                                  */
/* @return error code, else 0                                                */
uint8_t db_list_close(void) {
	while (db_list) {
		database *db = db_list_remove_table(db_list->db->id);
		db_drop(db);
	}

	db_list_size = 0;

	return 0;
}

/*---------------------------------------------------------------------------*/
/* Adds database to list.                                                    */
/* @param database* db database                                              */
/* @return error code, else 0                                                */
/* error codes:                                                              */
/* 1: memory full                                                            */
/* 2: given db is NULL pointer                                               */
/* 3: database already in list                                               */
/* 4: max number of tables reached                                           */
uint8_t db_list_add_table(database *db) {
	if (db_list_size >= DBMS_MAX_TABLES) {
#ifdef DEBUG_DBMS
		printf("ERROR: db_list_add_table: max number of tables reached!\n\r");
#endif //DEBUG_DBMS
		return 4;
	}

	if (!db) {
#ifdef DEBUG_DBMS
		printf("ERROR: db_list_add_table: Got NULL pointer as database!\n\r");
#endif //DEBUG_DBMS
		return 2;
	}

	if (db_list_get_table(db->id)) {
#ifdef DEBUG_DBMS
		printf("ERROR: db_list_add_table: database already in list!\n\r");
#endif //DEBUG_DBMS
		return 3;
	}

	struct db_list_element *e = malloc(sizeof(struct db_list_element));

	// memory full
	if (!e) {
#ifdef DEBUG_DBMS
		printf("ERROR: db_list_add_table: memory full!\n\r");
#endif //DEBUG_DBMS
		return 1;
	}

	e->db = db;
	e->next = db_list;
	db_list = e;
	db_list_size++;

	return 0;
}

/*---------------------------------------------------------------------------*/
/* Gets database with given id from list.                                    */
/* @param uint8_t table_id id of database                                    */
/* @return database* pointer to database, NULL if element wasnt found        */
database* db_list_get_table(uint8_t table_id) {
	database *db = NULL;

	struct db_list_element *e = db_list;
#ifdef DEBUG_DBMS
	printf("db_list:%p\n", e);
#endif //DEBUG_DBMS
	// search database
	while (e) {
		// database found

		if (e->db->id == table_id) {
			db = e->db;
			break;
		}
		e = (struct db_list_element*) e->next;
	}

	return db;
}

/*---------------------------------------------------------------------------*/
/* Gets database with given id and remove it from list.                      */
/* @param uint8_t table_id id of database                                    */
/* @return database* pointer to database, NULL if element wasnt found        */
database* db_list_remove_table(uint8_t table_id) {
	database *db = NULL;

	struct db_list_element *e = db_list;
	struct db_list_element *prev = NULL;

	// search database
	while (e) {
		// database found
		if (e->db->id == table_id) {
			db = e->db;

			// remove database from list
			// element is not head
			if (prev)
				prev->next = e->next;
			// element is head of list
			else
				db_list = e->next;
			free(e);

			db_list_size--;

			break;
		}
		prev = e;
		e = e->next;
	}

	return db;
}
