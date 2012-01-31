#include "db-operations.h"

/*---------------------------------------------------------------------------*/
/* Do SELECT operation over a database. The given sequence of attributes for */
/* select operation must be a subset of actual scheme of database. The       */
/* result database is a new independet database with its own memory.         */
/* So dont forget to free memory with db_drop after using it!                */
/* @param uint8_t table_id id of new table                                   */
/* @param database* db database                                              */
/* @param uint8_t[] scheme scheme of entry, given in a sequence of integer.  */
/* @param uint8_t size of scheme definition                                  */
/* @return new database, NULL if error occurs                                */
database* db_select(uint8_t table_id, database *db, uint8_t scheme[],
		uint8_t size) {
	if (!db) {
#ifdef DEBUG_DB
		printf("ERROR: db_select: Got NULL pointer as database!\n\r");
#endif //DEBUG_DB
		return NULL;
	}

	// remaining key
	uint8_t key[size];
	uint8_t key_size = 0;

	// index in old scheme of remaining attributes
	uint8_t att[size];

	// check scheme
	int att_index1, att_index2;
	uint8_t counter = 0;
	for (att_index1 = 0; att_index1 < size; att_index1++) {
		// check matching of schemes
		for (att_index2 = 0; att_index2 < db->scheme_size; att_index2++) {
			if (scheme[att_index1] == db->scheme[att_index2]) {
				att[counter++] = att_index2;
				break;
			}
		}

		// find reminding keys
		for (att_index2 = 0; att_index2 < db->key_size; att_index2++) {
			if (scheme[att_index1] == db->scheme[db->key_index[att_index2]]) {
				key[key_size++] = scheme[att_index1];
				break;
			}
		}
	}

	if (counter != size) {
#ifdef DEBUG_DB
		printf("ERROR: db_select: Given attributes are not subset of database scheme!\n\r");
#endif //DEBUG_DB
		return NULL;
	}

	// run select operation
	database *new_db = db_create(table_id, scheme, size, key, key_size);

	// memory full
	if (!new_db) {
#ifdef DEBUG_DB
		printf("ERROR: db_select: memory full!\n\r");
#endif //DEBUG_DB
		return NULL;
	}

	// copy data
	int entry[size];
	int row;
	for (row = 0; row < DB_MAX_SIZE; row++) {
		if (db->rows[row]) {
#ifdef DEBUG_DBMS
			int x;
			for (x = 0; x < db->scheme_size; x++)
				printf("%u,", db->rows[row][x]);
			printf("\n");
#endif DEBUG_DBMS
			// copy entry
			for (att_index1 = 0; att_index1 < size; att_index1++)
				entry[att_index1] = db->rows[row][att[att_index1]];

			// error occurs
			if (!db_add_entry(new_db, scheme, entry, size)) {
#ifdef DEBUG_DB
				printf("ERROR: db_select: error while copy data!\n\r");
#endif //DEBUG_DB
				free(new_db);
				return NULL;
			}
		}
	}

#ifdef DEBUG_DB
	printf("db-operations::db_select() : created a new_db which has scheme_size of %u, key_size of %u\n", new_db->scheme_size, new_db->key_size);
#endif //DEBUG_DB
	return new_db;
}

/*---------------------------------------------------------------------------*/
/* Unions database 1 and database 2. Database 2 is append on database 1.     */
/* Already existing entries in database 1 will be overwritten. The databases */
/* must have the same scheme.                                                */
/* @param database* db database 1                                            */
/* @param database* db database 2                                            */
/* @return union of databases, NULL if error occurs                          */
database* db_union(database *db1, database *db2) {
	if (!db1 || !db2) {
#ifdef DEBUG_DB
		printf("ERROR: db_union: Got NULL pointer as database!\n\r");
#endif //DEBUG_DB
		return NULL;
	}

	if (db1 == db2) {
#ifdef DEBUG_DB
		printf("ERROR: db_union: Can not union the database with itsself!\n\r");
#endif //DEBUG_DB
		return NULL;
	}

	// check schemes
	if (db1->scheme_size != db2->scheme_size) {
#ifdef DEBUG_DB
		printf("ERROR: db_union: Scheme of databases are not equal!\n\r");
#endif //DEBUG_DB
		return NULL;
	}

	int i;
	for (i = 0; i < db1->scheme_size; i++) {
		if (db_has_attribute(db2, db1->scheme[i]) < 0) {
#ifdef DEBUG_DB
			printf("ERROR: db_union: Scheme of databases are not equal!\n\r");
#endif //DEBUG_DB
			return NULL;
		}
	}

	// copy data
	for (i = 0; i < DB_MAX_SIZE; i++) {
		if (db2->rows[i]) {
			if (!db_add_entry(db1, db2->scheme, db2->rows[i], db2->scheme_size)) {
#ifdef DEBUG_DB
				printf("ERROR: db_union: error occurs while copy data!\n\r");
#endif //DEBUG_DB
				return NULL;
			}
		}
	}

	return db1;
}
