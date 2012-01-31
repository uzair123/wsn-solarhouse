#include "db.h"

/*---------------------------------------------------------------------------*/
/* Creates a new database with given scheme. The first row of the array data */
/* is always the scheme definition of the table                              */
/* @param uint8_t table_id database id                                       */
/* @param uint8_t[] scheme scheme of table, given in a sequence of integer.  */
/* @param uint8_t size of scheme definition                                  */
/* @param uint8_t[] key set of attributes choosen for primary key            */
/* @param uint8_t p_key_size numbers of attributes for primary key           */
/* @return new database, NULL if memory is full OR not all primary key       */
/* attributes were in entry found                                            */
database* db_create(uint8_t table_id, uint8_t scheme[], uint8_t scheme_size,
		uint8_t key[], uint8_t key_size) {
	// init db structure
	database *db = (database*) malloc(sizeof(database));
	// memory full
	if (!db) {
#ifdef DEBUG_DB
		printf("ERROR: db_create: memory full!\n\r");
#endif //DEBUG_DB
		return NULL;
	}

	// init scheme
	db->scheme = (uint8_t*) malloc(scheme_size * sizeof(uint8_t));
	// memory full
	if (!db->scheme) {
#ifdef DEBUG_DB
		printf("ERROR: db_create: memory full!\n\r");
#endif //DEBUG_DB
		free(db);
		return NULL;
	}
	db->scheme_size = scheme_size;

	// init key
	db->key_index = (uint8_t*) malloc(key_size * sizeof(uint8_t));
	// memory full
	if (!db->key_index) {
#ifdef DEBUG_DB
		printf("ERROR: db_create: memory full!\n\r");
#endif //DEBUG_DB
		free(db->scheme);
		free(db);
		return NULL;
	}
	db->key_size = key_size;

	// copy scheme and save index to key attributes
	int att_index, i;
	int counter = 0;
	for (att_index = 0; att_index < scheme_size; att_index++) {
		db->scheme[att_index] = scheme[att_index];
		// check if key attribut
		for (i = 0; i < db->key_size; i++) {
			// save index of key attribute
			if (scheme[att_index] == key[i]) {
				db->key_index[i] = att_index;
				counter++;
			}
		}
	}

	// check for correct key specification
	if (key_size != counter) {
#ifdef DEBUG_DB
		printf("ERROR: db_create: Not all primary key attributes in entry found!\n\r");
#endif //DEBUG_DB
		free(db->key_index);
		free(db->scheme);
		free(db);
		return NULL;
	}

	// clear rows
	for (i = 0; i < DB_MAX_SIZE; i++)
		db->rows[i] = NULL;

	// copy data
	db->id = table_id;
	db->entries = 0;

	return db;
}

/*---------------------------------------------------------------------------*/
/* Clears a database and deletes all entries.                                */
/* @param database* db database                                              */
/* @return modified database, NULL if error occurs                           */
database* db_clear(database *db) {
	if (!db) {
#ifdef DEBUG_DB
		printf("ERROR: db_clear: Got NULL pointer as database!\n\r");
#endif //DEBUG_DB
		return NULL;
	}

	int row;
	for (row = 0; row < DB_MAX_SIZE; row++) {
		if (db->rows[row]) {
			free(db->rows[row]);
			db->rows[row] = NULL;
		}
	}

	db->entries = 0;

	return db;
}

/*---------------------------------------------------------------------------*/
/* Clones a database and returns new pointer to an independet database.      */
/* Dont forget to drop database later for freeing memory space!              */
/* @param uint8_t table_id id of new table                                   */
/* @param database* db database                                              */
/* @return clonend database, NULL if error occurs                            */
database* db_clone(uint8_t table_id, database *db) {
	if (!db) {
#ifdef DEBUG_DB
		printf("ERROR: db_clone: Got NULL pointer as database!\n\r");
#endif //DEBUG_DB
		return NULL;
	}

	// create list of key attributes
	uint8_t key[db->key_size];
	int i;
	for (i = 0; i < db->key_size; i++)
		key[i] = db->scheme[db->key_index[i]];

	// creates new database
	database *new_db = db_create(table_id, db->scheme, db->scheme_size, key,
			db->key_size);

	// memory full
	if (!new_db) {
#ifdef DEBUG_DB
		printf("ERROR: db_create: memory full!\n\r");
#endif //DEBUG_DB
		return NULL;
	}

	// copy data in new database
	for (i = 0; i < DB_MAX_SIZE; i++) {
		if (db->rows[i]) {
			// error occurs
			if (!db_add_entry(new_db, db->scheme, db->rows[i], db->scheme_size)) {
#ifdef DEBUG_DB
				printf("ERROR: db_clone: error while copy data!\n\r");
#endif //DEBUG_DB
				free(new_db);
				return NULL;
			}
		}
	}

	return new_db;
}

/*---------------------------------------------------------------------------*/
/* Deletes database permantly.                                               */
/* @param database* db database to delete                                    */
/* @return error code, else 0                                                */
void db_drop(database *db) {
	if (db) {
		int row;
		for (row = 0; row < DB_MAX_SIZE; row++) {
			if (db->rows[row]) {
				free(db->rows[row]);
				db->rows[row] = NULL;
			}
		}

		if (db->scheme) {
			free(db->scheme);
			db->scheme = NULL;
		}

		if (db->key_index) {
			free(db->key_index);
			db->key_index = NULL;
		}

		free(db);
	}
}

/*---------------------------------------------------------------------------*/
/* Converts two char values into integer.                                    */
/* @param char* a MSB of integer                                             */
/* @param char* b LSB of integer                                             */
/* @return int value of bitwise combined chars                               */
int db_ctoi(char a, char b) {
	return (a << 8) | (b & 0xff);
}

/*---------------------------------------------------------------------------*/
/* Converts integer into two char. The integer value will be split into      */
/* two 8 bit char values.                                                    */
/* @param int val 16 Bit integer value                                       */
/* @param char* a target for MSB of integer                                  */
/* @param char* b target for LSB of integer                                  */
void db_itoc(int val, char *a, char *b) {
	*a = (val >> 8) & 0xff;
	*b = val & 0xff;
}

/*---------------------------------------------------------------------------*/
/* Creates dump of a database in form of a char matrix. The first row        */
/* contains the scheme (table head) of the table. After this every row       */
/* contains an entry of the original database. Two pointer to integer values */
/* is needed to return the dimension of the dump. The dump itself allocs     */
/* memory, so its needed to call free over this dump, to free memory.        */
/* The 8 Bit integer pointer are optional and may be NULL.                   */
/* @param database* db database                                              */
/* @param int* size size of returned char array                              */
/* @param uint8_t* row_size lenght per row (given in bytes)                  */
/* @param uint8_t* row_num numbers of rows (including head; given in bytes)  */
/* @return database dump, NULL if error occurs                               */
uint8_t *db_get_dump(database *db, unsigned int *size, uint8_t *row_size,
		unsigned int *row_num) {
	if (!db) {
#ifdef DEBUG_DB
		printf("ERROR: db_get_dump: Got NULL pointer as database!\n\r");
#endif //DEBUG_DB
		return NULL;
	}

#ifdef DEBUG_DB
	printf("db::db_get_dump() : function entered, db->key_size: %u, db->scheme_size: %u, db->entries: %u\n", db->key_size, db->scheme_size, db->entries);
#endif //DEBUG_DB
	// write back total size of dump
	*size = 1 + // message type (added by Khalid, Pablo)
			1 + // query id
			1 + // slot length (sample period / tree height)
			1 + // scheme size
			db->scheme_size + // the scheme itself
			1 + // key size
			db->key_size + // the keys themselves
			1 + // amount of entries
			db->scheme_size * (db->entries) * 2; // the entries themselves

	// alloc memory (2 Integer Values + n x m for database dump)
	char *dump = (char*) malloc(*size), *dump2 = dump + 3;

	// memory full
	if (!dump) {
#ifdef DEBUG_DB
		printf("ERROR: db_get_dump: memory full!\n\r");
#endif //DEBUG_DB
		return NULL;
	}

	int i;
	for (i = 0; i < *size; i++)
		dump[i] = 0;

	char *ptr = dump2;

	// save scheme defintion
	*ptr = db->scheme_size;
	ptr += 1;
	int att_index;
	for (att_index = 0; att_index < db->scheme_size; att_index++) {
		*ptr = db->scheme[att_index];
		ptr += 1;
	}

	// save key
	*ptr = db->key_size;
	ptr += 1;
	for (att_index = 0; att_index < db->key_size; att_index++) {
		*ptr = db->scheme[db->key_index[att_index]];
		ptr += 1;
	}

	//save data
	*ptr = db->entries;
	ptr += 1;
	int row;
	for (row = 0; row < DB_MAX_SIZE; row++)
		if (db->rows[row]) {
			for (att_index = 0; att_index < db->scheme_size; att_index++) {
				//            	printf("db::db_get_dump() : including row %u, att %u\n", row, att_index);
				db_itoc(db->rows[row][att_index], ptr, ptr + 1);
				ptr += 2;
			}
		}

	// writes back to optional parameters
	if (row_size)
		*row_size = db->scheme_size;
	if (row_num)
		*row_num = db->entries + 1;

#ifdef DEBUG_DB
	printf("db::db_get_dump() : tried to allocate %u bytes, dump's contents:[", *size);
	for (i=0; i<(*size); i++) printf("%u,", dump[i]);
	printf("]\n");
#endif //DEBUG_DB
	return dump;
}

/*--------------------------------------------------------------------------*/
/* Returns the epoch number from a dump										*/
/* @param uint8_t* dump database dump                                   	*/
/* @return the epoch number													*/
uint16_t db_dump_get_epoch(uint8_t *dump) {
	return dump[1 + // message type (added by Khalid, Pablo)
			1 + // query id
			1 + // slot length (sample period / tree height)
			1 + // scheme size
			dump[3] + // the scheme itself
			1 + // key size
			dump[4 + dump[3]] + // the keys themselves
			1 + // amount of entries
			2 + // node id
			2 // query id
	] * 256 + dump[1 + // message type (added by Khalid, Pablo)
			1 + // query id
			1 + // slot length (sample period / tree height)
			1 + // scheme size
			dump[3] + // the scheme itself
			1 + // key size
			dump[4 + dump[3]] + // the keys themselves
			1 + // amount of entries
			2 + // node id
			2 + // query id
			1 // epoch, 2nd byte
			];

}

/*--------------------------------------------------------------------------*/
/* Returns the number of entries (rows) that a dump has						*/
/* @param uint8_t* dump database dump                                   	*/
/* @return the number of entries											*/
uint8_t db_dump_get_num_entries(uint8_t *dump) {
	return dump[1 + // message type (added by Khalid, Pablo)
			1 + // query id
			1 + // slot length (sample period / tree height)
			1 + // scheme size
			dump[3] + // the scheme itself
			1 + // key size
			dump[4 + dump[3]]];

}

/*---------------------------------------------------------------------------*/
/* Loads dump of a database in form of a char matrix. The first row          */
/* contains the scheme (table head) of the table. After this every row       */
/* contains an entry of the original database.                               */
/* Dont forget to drop database later for freeing memory space!              */
/* @param char* dump database dump                                           */
/* @param uint8_t row_size lenght per row                                    */
/* @param uint8_t rows numbers of rows (including head)                      */
/* @return new database created from dump, NULL if error occurs              */
database* db_load_dump(uint8_t table_id, uint8_t *dump) {
	uint8_t *ptr = dump + 3;

	// load scheme definition from dump
	uint8_t scheme_size = *ptr;
	ptr += 1;
	uint8_t scheme[scheme_size];
	int att_index;
	for (att_index = 0; att_index < scheme_size; att_index++) {
		scheme[att_index] = *ptr;
		ptr += 1;
	}

	// load key
	uint8_t key_size = *ptr;
	ptr += 1;
	uint8_t key[key_size];
	for (att_index = 0; att_index < key_size; att_index++) {
		key[att_index] = *ptr;
		ptr += 1;
	}

	database *db = db_create(table_id, scheme, scheme_size, key, key_size);

	// memory full
	if (!db) {
#ifdef DEBUG_DB
		printf("ERROR: db_load_dump: memory full or not all primary key attributes in entry were found!\n\r");
#endif //DEBUG_DB
		return NULL;
	}

	// load data
	int entries = *ptr;
	ptr += 1;
	int entry[scheme_size];
	int row;
	for (row = 0; row < entries; row++) {
		for (att_index = 0; att_index < scheme_size; att_index++) {
			entry[att_index] = db_ctoi(*ptr, *(ptr + 1));
			ptr += 2;
		}

		// add loaded entry to database
		if (!db_add_entry(db, scheme, entry, scheme_size)) {
#ifdef DEBUG_DB
			printf("ERROR: db_load_dump: error while adding entry!\n\r");
#endif //DEBUG_DB
			return NULL;
		}
	}

	return db;
}

/*---------------------------------------------------------------------------*/
/* Creates a new database entry with given scheme. The first row of the      */
/* array data is always the scheme definition of the table. If entry already */
/* with equal key attributes already exists, it will be overwritten.         */
/* @param database* db database                                              */
/* @param uint8_t[] scheme scheme of entry, given in a sequence of integer.  */
/* @param int[] entry values                                                 */
/* @param uint8_t size numbers of attributes                                 */
/* @return modified database, NULL if error occurs                           */
database* db_add_entry(database *db, uint8_t scheme[], int entry[],
		uint8_t size) {
	if (!db) {
#ifdef DEBUG_DB
		printf("ERROR: db_add_entry: Got NULL pointer as database!\n\r");
#endif //DEBUG_DB
		return NULL;
	}

	if (db->entries == DB_MAX_SIZE) {
#ifdef DEBUG_DB
		printf("WARNING: db_add_entry: Table is full!\n\r");
#endif //DEBUG_DB
		return NULL;
	}

	int row = 0;

	// check for duplicate entry
	if ((row = db_has_entry(db, scheme, entry, size)) >= 0) {
#ifdef DEBUG_DB
		printf("WARNING: db_add_entry: Entry in row %d already exists, will overwrite!\n\r", row);
#endif //DEBUG_DB
		db->entries--;
	}
	// allocate more memory
	else {
		// find free row
		for (row = 0; row < DB_MAX_SIZE; row++) {
			if (!db->rows[row])
				break;
		}

		db->rows[row] = (int*) malloc(db->scheme_size * sizeof(int));
		// memory full
		if (!db->rows[row]) {
#ifdef DEBUG_DB
			printf("ERROR: db_add_entry: memory full!\n\r");
#endif //DEBUG_DB
			return NULL;
		}
	}

	// pointer to row
	int *ptr = db->rows[row];

	// clear row
	memset(ptr, 0, db->scheme_size * sizeof(int));

	// insert data, find correct coloumn for each attribute
	int i, col;
	for (i = 0; i < size; i++) {
		for (col = 0; col < db->scheme_size; col++)
			if (db->scheme[col] == scheme[i])
				break;

		// check if attribute was really found
		if (db->scheme[col] != scheme[i]) {
#ifdef DEBUG_DB
			printf("ERROR: db_add_entry: attribute not found!\n\r");
#endif //DEBUG_DB
			free(db->rows[row]);
			db->rows[row] = NULL;
			return NULL;
		}

		ptr[col] = entry[i];
	}

	db->entries++;

	return db;
}

/*---------------------------------------------------------------------------*/
/* Deletes a database entry with given scheme. The entry is searched         */
/* only with key attributes. Other attributes doesnt have any effect.        */
/* @param database* db database                                              */
/* @param uint8_t[] scheme scheme of entry, given in a sequence of integer.  */
/* @param int[] entry values                                                 */
/* @param uint8_t size numbers of attributes                                 */
/* @return modified database, NULL if error occurs                           */
database* db_delete_entry(database *db, uint8_t scheme[], int entry[],
		uint8_t size) {
	int row = db_has_entry(db, scheme, entry, size);

	// entry not found
	if (row == -1) {
#ifdef DEBUG_DB
		printf("WARNING: db_delete_entry: entry not found!\n\r");
#endif //DEBUG_DB
		return db;
	}
	// error occurs
	else if (row < 0) {
#ifdef DEBUG_DB
		printf("ERROR: db_delete_entry: error occurs while searching entry: %d!\n\r", row);
#endif //DEBUG_DB
		return NULL;
	}
	// row found, delete it
	else
		return db_delete_row(db, row);
}

/*---------------------------------------------------------------------------*/
/* Deletes specific row of database. For an specific entry the method        */
/* db_has_entry can be used to get the row.                                  */
/* @param database* db database                                              */
/* @param int row row number                                                 */
/* @return int* pointer to data row, if error occurs NULL                    */
database* db_delete_row(database *db, unsigned int row) {
	if (db->rows[row]) {
		free(db->rows[row]);
		db->rows[row] = NULL;
		db->entries--;
	}
	return db;
}

/*---------------------------------------------------------------------------*/
/* Checks if entry already exists. The check will be only done with key      */
/* attributes. Other attributes must not match for a positive result.        */
/* @param database* db database                                              */
/* @param uint8_t[] scheme scheme of entry, given in a sequence of integer.  */
/* @param int[] entry values (especialy key)                                 */
/* @param uint8_t size numbers of attributes                                 */
/* @return uint8_t -1 if entry doesn't exists, otherwise row in database     */
/* error codes:                                                              */
/* -1: entry doesnt exists                                                   */
/* -2: not all primary keys are given in entry                               */
/* -3: database is NULL pointer                                              */
int db_has_entry(database *db, uint8_t scheme[], int entry[], uint8_t size) {
	if (!db) {
#ifdef DEBUG_DB
		printf("ERROR: db_has_entry: Got NULL pointer as database!\n\r");
#endif //DEBUG_DB
		return -3;
	}

	if (db->entries) {
		uint8_t key_index[db->key_size];

		// search key attributes in input
		int i, j;
		int counter = 0;
		for (i = 0; i < db->key_size; i++) {
			for (j = 0; j < size; j++) {
				if (db->scheme[db->key_index[i]] == scheme[j]) {
					key_index[i] = j;
					counter++;
					break;
				}
			}
		}

		if (counter != db->key_size) {
#ifdef DEBUG_DB
			printf("WARNING: db_has_entry: Not all key attributes in entry found!\n\r");
#endif //DEBUG_DB
			return -2;
		}

		// search entry
		for (i = 0; i < DB_MAX_SIZE; i++) {
			if (db->rows[i]) {
				for (j = 0; j < db->key_size; j++) {
					if (db->rows[i][db->key_index[j]] != entry[key_index[j]])
						break;
				}

				// entry found
				if (j == db->key_size)
					return i;
			}
		}
	}

	return -1;
}

/*---------------------------------------------------------------------------*/
/* Gets specific row of database. For an specific entry the method           */
/* db_has_entry can be used to get the row.                                  */
/* @param database* db database                                              */
/* @param int row row number                                                 */
/* @return int* pointer to data row, if error occurs NULL                    */
int* db_get_entry(database *db, unsigned int row) {
	if (!db) {
#ifdef DEBUG_DB
		printf("ERROR: db_get_entry: Got NULL pointer as database!\n\r");
#endif //DEBUG_DB
		return NULL;
	}

	if (row > DB_MAX_SIZE) {
#ifdef DEBUG_DB
		printf("ERROR: db_get_entry: row doesnt exists!\n\r");
#endif //DEBUG_DB
		return NULL;
	}

	return db->rows[row];
}

/*---------------------------------------------------------------------------*/
/* Checks if table contains a specific attribute.                            */
/* @param database* db database                                              */
/* @param uint8_t attribute attribute type                                   */
/* @return int index in scheme if attribute was found, otherwise -1          */
int db_has_attribute(database *db, uint8_t attribute) {
	int i;
	for (i = 0; i < db->scheme_size; i++) {
		if (db->scheme[i] == attribute)
			break;
	}

	if (db->scheme[i] == attribute)
		return i;

	return -1;
}

/*---------------------------------------------------------------------------*/
/* Prints table to console.                                                  */
/* @param database* db database to print                                     */
void db_printf(database *db) {
	if (!db) {
#ifdef DEBUG_DB
		printf("ERROR: db_printf: Got NULL pointer as database!\n\r");
#endif //DEBUG_DB
		printf("db_printf: NULL\n");
		return;
	}

	// print header
	printf("DB Infos: ID: %d; private key: {", db->id);
	int i;
	for (i = 0; i < db->key_size; i++)
		printf(" %d", db->scheme[db->key_index[i]]);
	printf(" }; columns: %d; rows: %d\n", db->scheme_size, db->entries);

	// print scheme
	for (i = 0; i < db->scheme_size; i++)
		printf("------");
	printf("-\n");

	for (i = 0; i < db->scheme_size; i++) {
		if (db->scheme[i] < 10)
			printf("|   %d ", db->scheme[i]);
		else if (db->scheme[i] < 100)
			printf("|  %d ", db->scheme[i]);
		else
			printf("| %d ", db->scheme[i]);
	}
	printf("|\n");

	for (i = 0; i < db->scheme_size; i++)
		printf("======");
	printf("=\n");

	// print entries
	int entry;
	for (entry = 0; entry < DB_MAX_SIZE; entry++) {
		if (db->rows[entry]) {
			for (i = 0; i < db->scheme_size; i++) {
				if (db->rows[entry][i] < 10)
					printf("|   %d ", db->rows[entry][i]);
				else if (db->rows[entry][i] < 100)
					printf("|  %d ", db->rows[entry][i]);
				else
					printf("| %d ", db->rows[entry][i]);
			}
			printf("|\n");

			for (i = 0; i < db->scheme_size; i++)
				printf("------");
			printf("-\n");
		}
	}
}
