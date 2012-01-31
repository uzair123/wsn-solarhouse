#include "table.h"
#include "stdlib.h"

static int get_smaller(int a, int b);
static int get_smaller_equal(int a, int b);
static int get_greater(int a, int b);
static int get_greater_equal(int a, int b);
static int get_equal(int a, int b);
static int get_not_equal(int a, int b);

static void agg_max(int *row_a, int *row_b, int *row_output, int index);
static void agg_min(int *row_a, int *row_b, int *row_output, int index);
static void agg_avg(int *row_a, int *row_b, int *row_output, int index);
static void agg_count(int *row_a, int *row_b, int *row_output, int index);
static void agg_sum(int *row_a, int *row_b, int *row_output, int index);

/**
 * \brief		Calculates max of two rows
 *
 * \param row_a			first row to aggregate
 * \param row_b			second row to aggregate
 * \param row_output	resulting row
 * \param index			index of the attribute to aggregate
 */
static void agg_max(int *row_a, int *row_b, int *row_output, int index) {

	if (row_a[index] > row_b[0])
		row_output[index] = row_a[index];
	else
		row_output[index] = row_b[index];
}

/**
 * \brief		Calculates min of two rows
 *
 * \param row_a			first row to aggregate
 * \param row_b			second row to aggregate
 * \param row_output	resulting row
 * \param index			index of the attribute to aggregate
 */
static void agg_min(int *row_a, int *row_b, int *row_output, int index) {

	if (row_a[index] < row_b[0])
		row_output[index] = row_a[index];
	else
		row_output[index] = row_b[index];
}

/**
 * \brief		Calculates average of two rows
 *
 * \param row_a			first row to aggregate
 * \param row_b			second row to aggregate
 * \param row_output	resulting row
 * \param index			index of the attribute to aggregate
 */
static void agg_avg(int *row_a, int *row_b, int *row_output, int index) {
#ifdef DEBUG_DB
	printf("DEBUG: p1: %p, p2: %p, db_avg(%u + %u = %u)\n", row_a, row_b, row_a[index], row_b[index],
			(row_a[index] + row_b[index]));
#endif //DEBUG_DB
	//row_output[index] = row_a[index] + row_b[index];
	//row_output[index + 1] = row_a[index + 1] + row_b[index + 1];

	int new_count;// put it in a var to avoid overwriting row_output to early, since it often equals row_a
	new_count = row_a[index + 1] + row_b[index + 1];
	long int tmp1;
	long int tmp2;
	tmp1 = (long int) row_a[index] * (long int) row_a[index + 1];
	tmp2 = (long int) row_b[index] * (long int) row_b[index + 1];
	//printf("DEBUG db_avg(...): t1: %d, t2: %d, t1+t2 = %d\n", tmp1, tmp2, (tmp1+tmp2));
	tmp1 = tmp1 + tmp2;
	tmp2 = tmp1 / new_count;

	row_output[index + 1] = new_count;
	row_output[index] = (int) tmp2;
	//printf("DEBUG db_avg(...): a1: %d, a2: %d, new avg = %d (long int)\n", row_a[index],row_b[index], tmp2);
}

/**
 * \brief		Calculates count of two rows
 *
 * \param row_a			first row to aggregate
 * \param row_b			second row to aggregate
 * \param row_output	resulting row
 * \param index			index of the attribute to aggregate
 */
static void agg_count(int *row_a, int *row_b, int *row_output, int index) {
	row_output[index] = row_a[index] + row_b[index];
}

/**
 * \brief		Calculates sum of two rows
 *
 * \param row_a			first row to aggregate
 * \param row_b			second row to aggregate
 * \param row_output	resulting row
 * \param index			index of the attribute to aggregate
 */
static void agg_sum(int *row_a, int *row_b, int *row_output, int index) {
	row_output[index] = row_a[index] + row_b[index];
}

/*---------------------------------------------------------------------------*/
/* All comparison operators.                                                 */
/* @param int a first argument                                               */
/* @param int b second argument                                              */
/* @return result of a compare b, should be non zero if condition is         */
/* satisfied                                                                 */
static int get_smaller(int a, int b) {
	return a < b;
}

static int get_smaller_equal(int a, int b) {
	return a <= b;
}

static int get_greater(int a, int b) {
	return a > b;
}

static int get_greater_equal(int a, int b) {
	return a >= b;
}

static int get_equal(int a, int b) {
	return a == b;
}

static int get_not_equal(int a, int b) {
	return a != b;
}

/**
 * \brief		Creates a new table with a given scheme and id. The first row of the
 * 				array data is always the scheme definition of the table.
 *
 * \param table_id	id to use for the new table
 * \param scheme	scheme of table, given in a sequence of integer
 * \param size		size of scheme definition
 * \param key		set of attributes chosen for primary key
 * \param key_size	numbers of attributes for primary key
 *
 * \return 			new table, NULL if memory is full or other problems occurred */
table_t* table_create(table_id_t table_id, uint8_t scheme[],
		data_len_t scheme_size, uint8_t key[], data_len_t key_size) {

	table_t *table = (table_t*) malloc(sizeof(table_t));

	// memory full
	if (!table)
		return NULL;

	// init scheme
	table->scheme = (uint8_t*) malloc(scheme_size * sizeof(uint8_t));
	// memory full
	if (!table->scheme) {
		free(table);
		return NULL;
	}
	table->scheme_size = scheme_size;

	// init key
	table->key_index = (uint8_t*) malloc(key_size * sizeof(uint8_t));
	// memory full
	if (!table->key_index) {
		free(table->scheme);
		free(table);
		return NULL;
	}
	table->key_size = key_size;

	// copy scheme and save index to key attributes
	int att_index, i;
	int counter = 0;
	for (att_index = 0; att_index < scheme_size; att_index++) {
		table->scheme[att_index] = scheme[att_index];
		// check if key attribut
		for (i = 0; i < table->key_size; i++) {
			// save index of key attribute
			if (scheme[att_index] == key[i]) {
				table->key_index[i] = att_index;
				counter++;
			}
		}
	}

	// check for correct key specification
	if (key_size != counter) {
#ifdef DEBUG_DB
		PRINTF(1,"ERROR: db_create: Not all primary key attributes in entry found!\n\r");
#endif //DEBUG_DB
		free(table->key_index);
		free(table->scheme);
		free(table);
		return NULL;
	}

	// clear rows
	for (i = 0; i < MAX_ROWS; i++)
		table->rows[i] = NULL;

	// copy data
	table->id = table_id;
	table->entries = 0;

	return table;
}

/**
 * \brief		Clears a table and deletes all its entries.
 *
 * \param table	the table to clear
 *
 * \return		modified table, NULL if error occurs
 **/
table_t* table_clear(table_t *table) {
	if (!table) {
#ifdef DEBUG_DB
		PRINTF(1,"ERROR: db_clear: Got NULL pointer as table!\n\r");
#endif //DEBUG_DB
		return NULL;
	}

	int row;
	for (row = 0; row < MAX_ROWS; row++) {
		if (table->rows[row]) {
			free(table->rows[row]);
			table->rows[row] = NULL;
		}
	}

	table->entries = 0;

	return table;
}

/**
 * \brief		Clones a table and returns new pointer to an independent
 * 				table. Dont forget to drop table later for freeing
 * 				memory space!
 *
 * \param table_id	id of new table
 * \param table		table to clone
 * \return			cloned table, NULL if error occurs
 */
table_t* table_clone(table_id_t table_id, table_t *table) {
	if (!table) {
		return NULL;
	}

	// create list of key attributes
	uint8_t key[table->key_size];
	int i;
	for (i = 0; i < table->key_size; i++)
		key[i] = table->scheme[table->key_index[i]];

	// creates new table
	table_t *new_table = table_create(table_id, table->scheme,
			table->scheme_size, key, table->key_size);

	// memory full
	if (!new_table) {
		return NULL;
	}

	// copy data in new table
	for (i = 0; i < MAX_ROWS; i++) {
		if (table->rows[i]) {
			// error occurs
			if (!table_add_entry(new_table, table->scheme, table->rows[i],
					table->scheme_size)) {
				PRINTF(2,"ERROR: db_clone: error while copy data!\n\r");
				free(new_table);
				return NULL;
			}
		}
	}

	return new_table;
}

/**
 * \brief		Deletes table permanently.
 *
 * \param table	table to free
 */
void table_free(table_t *table) {
	if (!table)
		return;

	int row;
	for (row = 0; row < MAX_ROWS; row++) {
		if (table->rows[row]) {
			free(table->rows[row]);
			table->rows[row] = NULL;
		}
	}

	if (table->scheme) {
		free(table->scheme);
		table->scheme = NULL;
	}

	if (table->key_index) {
		free(table->key_index);
		table->key_index = NULL;
	}

	free(table);
}

/*---------------------------------------------------------------------------*/
/**
 * \brief		Creates a resultset out of a table (i.e., marshalls it). The first
 * 				row contains the scheme (table head) of the table. After this, every
 * 				row contains an entry of the original table. Two pointer to integer
 * 				values is needed to return the dimension of the dump. The resultset
 * 				itself allocs memory, so it's necessary to free it eventually.
 * 				The 8 Bit integer pointers are optional and may be NULL.
 *
 * \param table		the table from which the resultset should be generated
 * \param size		size of returned resultset
 * \param row_size	length per row (given in bytes)
 * \param row_num	numbers of rows (including head; given in bytes)
 *
 * \return resulset, NULL if error occurs
 **/
uint8_t *table_marshall(table_t *table, data_len_t *size) {
	if (!table) {
		PRINTF(3,"ERROR: Got NULL pointer as table!\n\r");
		return NULL;
	}

	PRINTF(3,"function entered, table->key_size: %u, table->scheme_size: %u, table->entries: %u\n", table->key_size, table->scheme_size, table->entries);

	// write back total size of dump
	*size = 1 + // message type (RESULT vs STATEMENT)
			1 + // statement id
			4 + // slot length (sample period / tree height)
			1 + // scheme size
			table->scheme_size + // the scheme itself
			1 + // key size
			table->key_size + // the keys themselves
			1 + // amount of entries
			table->scheme_size * (table->entries) * 2; // the entries themselves

	// alloc memory (2 Integer Values + n x m for table dump)
	uint8_t *resultset_arr = (uint8_t*) malloc(*size);

	// memory full
	if (!resultset_arr) {
		PRINTF(3,"ERROR: memory full!\n\r");
		return NULL;
	}

	int i;
	for (i = 0; i < *size; i++)
		resultset_arr[i] = 0;

	uint8_t *ptr = resultset_arr + 6;

	// save scheme definition (i.e. number of scheme elements and the elements themselves)
	*ptr = table->scheme_size;
	ptr += 1;
	int att_index;
	for (att_index = 0; att_index < table->scheme_size; att_index++) {
		*ptr = table->scheme[att_index];
		ptr += 1;
	}

	// save key (i.e. number of keys and the keys themselves)
	*ptr = table->key_size;
	ptr += 1;
	for (att_index = 0; att_index < table->key_size; att_index++) {
		*ptr = table->scheme[table->key_index[att_index]];
		ptr += 1;
	}

	// save data (i.e. number of entries and the data elements themselves)
	*ptr = table->entries;
	ptr += 1;
	int row;
	for (row = 0; row < MAX_ROWS; row++)
		if (table->rows[row]) {
			for (att_index = 0; att_index < table->scheme_size; att_index++) {
				//            	printf("db::db_get_dump() : including row %u, att %u\n", row, att_index);
				uint16_t *u16 = (uint16_t*)ptr;
				*u16 = table->rows[row][att_index];
				ptr += 2;
			}
		}

	PRINTF(3," tried to allocate %u bytes, resultset's contents: ", *size);PRINT_ARR(3, resultset_arr, *size);

	return resultset_arr;
}

/**
 * \brief
 */clock_time_t *resultset_get_slot_length_ptr(uint8_t *resultset_arr) {
	return (clock_time_t*) (resultset_arr + 2);
}

/**
 * \brief		Loads dump of a table in form of a char matrix. The first row
 * 				contains the scheme (table head) of the table. After this every row
 * 				contains an entry of the original table.
 * 				Dont forget to drop table later for freeing memory space!
 *
 * \param char* dump table dump
 * \param uint8_t row_size lenght per row
 * \param uint8_t rows numbers of rows (including head)
 *
 * \return new table created from dump, NULL if error occurs
 **/
table_t* table_unmarshall(table_id_t table_id, uint8_t *resultset_arr) {

	if (!table_id) {
		PRINTF(3,"ERROR: max number of tables reached!\n\r");
		return NULL;
	}

	uint8_t *ptr = resultset_arr + 6;

	// load scheme definition from dump
	data_len_t scheme_size = *ptr;
	ptr += 1;
	uint8_t scheme[scheme_size];
	int att_index;
	for (att_index = 0; att_index < scheme_size; att_index++) {
		scheme[att_index] = *ptr;
		ptr += 1;
	}

	// load key
	data_len_t key_size = *ptr;
	ptr += 1;
	uint8_t key[key_size];
	for (att_index = 0; att_index < key_size; att_index++) {
		key[att_index] = *ptr;
		ptr += 1;
	}

	table_t *new_table = table_create(table_id, scheme, scheme_size, key,
			key_size);

	// memory full
	if (!new_table) {
		PRINTF(3,"ERROR: memory full or not all primary key attributes in entry were found!\n\r");
		return NULL;
	}

	new_table->statement_id = resultset_arr[1];

	// load data
	int entries = *ptr;
	ptr += 1;
	int entry[scheme_size];
	int row;
	for (row = 0; row < entries; row++) {
		for (att_index = 0; att_index < scheme_size; att_index++) {
			uint16_t *u16 = (uint16_t*)ptr;
			entry[att_index] = *u16;
			ptr += 2;
		}

		// add loaded entry to table
		if (!table_add_entry(new_table, scheme, entry, scheme_size)) {
			PRINTF(3,"ERROR: db_load_dump: error while adding entry!\n\r");
			return NULL;
		}
	}

	return new_table;
}

/*---------------------------------------------------------------------------*/
/* Creates a new table entry with given scheme. The first row of the      */
/* array data is always the scheme definition of the table. If entry already */
/* with equal key attributes already exists, it will be overwritten.         */
/* @param table* db table                                              */
/* @param uint8_t[] scheme scheme of entry, given in a sequence of integer.  */
/* @param int[] entry values                                                 */
/* @param uint8_t size numbers of attributes                                 */
/* @return modified table, NULL if error occurs                           */
table_t* table_add_entry(table_t *table, uint8_t scheme[], int entry[],
		uint8_t size) {
	if (!table) {
#ifdef DEBUG_DB
		PRINTF(1,"ERROR: db_add_entry: Got NULL pointer as table!\n\r");
#endif //DEBUG_DB
		return NULL;
	}

	if (table->entries == MAX_ROWS) {
#ifdef DEBUG_DB
		PRINTF(1,"WARNING: db_add_entry: Table is full!\n\r");
#endif //DEBUG_DB
		return NULL;
	}

	int row = 0;

	// check for duplicate entry
	if ((row = table_has_entry(table, scheme, entry, size)) >= 0) {
#ifdef DEBUG_DB
		PRINTF(1,"WARNING: db_add_entry: Entry in row %d already exists, will overwrite!\n\r", row);
#endif //DEBUG_DB
		table->entries--;
	}
	// allocate more memory
	else {
		// find free row
		for (row = 0; row < MAX_ROWS; row++) {
			if (!table->rows[row])
				break;
		}

		table->rows[row] = (int*) malloc(table->scheme_size * sizeof(int));
		// memory full
		if (!table->rows[row]) {
#ifdef DEBUG_DB
			PRINTF(1,"ERROR: db_add_entry: memory full!\n\r");
#endif //DEBUG_DB
			return NULL;
		}
	}

	// pointer to row
	int *ptr = table->rows[row];

	// clear row
	memset(ptr, 0, table->scheme_size * sizeof(int));

	// insert data, find correct coloumn for each attribute
	int i, col;
	for (i = 0; i < size; i++) {
		for (col = 0; col < table->scheme_size; col++)
			if (table->scheme[col] == scheme[i])
				break;

		// check if attribute was really found
		if (table->scheme[col] != scheme[i]) {
#ifdef DEBUG_DB
			PRINTF(1,"ERROR: db_add_entry: attribute not found!\n\r");
#endif //DEBUG_DB
			free(table->rows[row]);
			table->rows[row] = NULL;
			return NULL;
		}

		ptr[col] = entry[i];
	}

	table->entries++;

	return table;
}

/**
 * \brief		Deletes a table entry with given scheme. The entry is searched
 * 				only with key attributes. Other attributes doesn't have any effect.
 *
 * \param table		table to inspect
 * \param scheme	scheme of entry, given in a sequence of integer.
 * \param entry		values
 * \param size		numbers of attributes
 *
 * \return 			modified table, NULL if error occurs
 */
table_t* table_delete_entry(table_t *table, uint8_t scheme[], int entry[],
		uint8_t size) {
	int row = table_has_entry(table, scheme, entry, size);

	// entry not found
	if (row == -1) {
#ifdef DEBUG_DB
		PRINTF(1,"WARNING: db_delete_entry: entry not found!\n\r");
#endif //DEBUG_DB
		return table;
	}
	// error occurs
	else if (row < 0) {
#ifdef DEBUG_DB
		PRINTF(1,"ERROR: db_delete_entry: error occurs while searching entry: %d!\n\r", row);
#endif //DEBUG_DB
		return NULL;
	}
	// row found, delete it
	else
		return table_delete_row(table, row);
}

/*---------------------------------------------------------------------------*/
/* Deletes specific row of table. For an specific entry the method        */
/* db_has_entry can be used to get the row.                                  */
/* @param table* db table                                              */
/* @param int row row number                                                 */
/* @return int* pointer to data row, if error occurs NULL                    */
table_t* table_delete_row(table_t *table, unsigned int row) {
	if (table->rows[row]) {
		free(table->rows[row]);
		table->rows[row] = NULL;
		table->entries--;
	}
	return table;
}

/*---------------------------------------------------------------------------*/
/* Checks if entry already exists. The check will be only done with key      */
/* attributes. Other attributes must not match for a positive result.        */
/* @param table* db table                                              */
/* @param uint8_t[] scheme scheme of entry, given in a sequence of integer.  */
/* @param int[] entry values (especialy key)                                 */
/* @param uint8_t size numbers of attributes                                 */
/* @return uint8_t -1 if entry doesn't exists, otherwise row in table     */
/* error codes:                                                              */
/* -1: entry doesnt exists                                                   */
/* -2: not all primary keys are given in entry                               */
/* -3: table is NULL pointer                                              */
int table_has_entry(table_t *table, uint8_t scheme[], int entry[], uint8_t size) {
	if (!table) {
#ifdef DEBUG_DB
		PRINTF(1,"ERROR: db_has_entry: Got NULL pointer as table!\n\r");
#endif //DEBUG_DB
		return -3;
	}

	if (table->entries) {
		uint8_t key_index[table->key_size];

		// search key attributes in input
		int i, j;
		int counter = 0;
		for (i = 0; i < table->key_size; i++) {
			for (j = 0; j < size; j++) {
				if (table->scheme[table->key_index[i]] == scheme[j]) {
					key_index[i] = j;
					counter++;
					break;
				}
			}
		}

		if (counter != table->key_size) {
#ifdef DEBUG_DB
			PRINTF(1,"WARNING: db_has_entry: Not all key attributes in entry found!\n\r");
#endif //DEBUG_DB
			return -2;
		}

		// search entry
		for (i = 0; i < MAX_ROWS; i++) {
			if (table->rows[i]) {
				for (j = 0; j < table->key_size; j++) {
					if (table->rows[i][table->key_index[j]]
							!= entry[key_index[j]])
						break;
				}

				// entry found
				if (j == table->key_size)
					return i;
			}
		}
	}

	return -1;
}

/*---------------------------------------------------------------------------*/
/* Gets specific row of table. For an specific entry the method           */
/* db_has_entry can be used to get the row.                                  */
/* @param table* db table                                              */
/* @param int row row number                                                 */
/* @return int* pointer to data row, if error occurs NULL                    */
int* table_get_entry(table_t *table, unsigned int row) {
	if (!table) {
#ifdef DEBUG_DB
		PRINTF(1,"ERROR: db_get_entry: Got NULL pointer as table!\n\r");
#endif //DEBUG_DB
		return NULL;
	}

	if (row > MAX_ROWS) {
#ifdef DEBUG_DB
		PRINTF(1,"ERROR: db_get_entry: row doesnt exists!\n\r");
#endif //DEBUG_DB
		return NULL;
	}

	return table->rows[row];
}

/*---------------------------------------------------------------------------*/
/* Checks if table contains a specific attribute.                            */
/* @param table* db table                                              */
/* @param uint8_t attribute attribute type                                   */
/* @return int index in scheme if attribute was found, otherwise -1          */
int table_has_attribute(table_t *table, uint8_t attribute) {
	int i;
	for (i = 0; i < table->scheme_size; i++) {
		if (table->scheme[i] == attribute)
			break;
	}

	if (table->scheme[i] == attribute)
		return i;

	return -1;
}

/*---------------------------------------------------------------------------*/
/* Prints table to console.                                                  */
/* @param table* db table to print                                     */
void table_printf(table_t *table) {
	if (!table) {
#ifdef DEBUG_DB
		PRINTF(1,"ERROR: db_printf: Got NULL pointer as table!\n\r");
#endif //DEBUG_DB
		printf("table_printf: NULL\n");
		return;
	}

	// print header
	PRINTF(1,"Table id: %d; private key: {", table->id);
	int i;
	for (i = 0; i < table->key_size; i++)
		printf(" %d", table->scheme[table->key_index[i]]);
	PRINTF(1," }; columns: %d; rows: %d\n", table->scheme_size, table->entries);

	// print scheme
	for (i = 0; i < table->scheme_size; i++)
		PRINTF(1,"----------");PRINTF(1,"-\n");

	for (i = 0; i < table->scheme_size; i++) {
		PRINTF(1,"|   %5d ", table->scheme[i]);
	}PRINTF(1,"|\n");

	for (i = 0; i < table->scheme_size; i++)
		PRINTF(1,"==========");PRINTF(1,"=\n");

	// print entries
	int entry;
	for (entry = 0; entry < MAX_ROWS; entry++) {
		if (table->rows[entry]) {
			for (i = 0; i < table->scheme_size; i++) {
				PRINTF(1,"|   %5d ", table->rows[entry][i]);
			}PRINTF(1,"|\n");

			for (i = 0; i < table->scheme_size; i++)
				PRINTF(1,"----------");PRINTF(1,"-\n");
		}
	}
}

/**
 * \brief		Do SELECT operation over a table. The given sequence of
 * 				attributes for select operation must be a subset of actual
 * 				scheme of table. The result table is a new independent
 * 				table with its own memory. So dont forget to free memory
 * 				with table_drop after using it!
 *
 * \param uint8_t table_id id of new table
 * \param table* db table
 * \param uint8_t[] scheme scheme of entry, given in a sequence of integer
 * \param uint8_t size of scheme definition
 * \return new table, NULL if error occurs
 */
table_t* table_select(table_id_t table_id, table_t *table, uint8_t scheme[],
		data_len_t size) {
	if (!table) {
		return NULL;
	}

	// remaining key
	uint8_t key[size];
	data_len_t key_size = 0;

	// index in old scheme of remaining attributes
	uint8_t att[size];

	// check scheme
	int att_index1, att_index2;
	data_len_t counter = 0;
	for (att_index1 = 0; att_index1 < size; att_index1++) {
		// check matching of schemes
		for (att_index2 = 0; att_index2 < table->scheme_size; att_index2++) {
			if (scheme[att_index1] == table->scheme[att_index2]) {
				att[counter++] = att_index2;
				break;
			}
		}

		// find reminding keys
		for (att_index2 = 0; att_index2 < table->key_size; att_index2++) {
			if (scheme[att_index1]
					== table->scheme[table->key_index[att_index2]]) {
				key[key_size++] = scheme[att_index1];
				break;
			}
		}
	}

	if (counter != size) {
#ifdef DEBUG_DB
		printf("ERROR: db_select: Given attributes are not subset of table scheme!\n\r");
#endif //DEBUG_DB
		return NULL;
	}

	// run select operation
	table_t *new_db = table_create(table_id, scheme, size, key, key_size);

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
	for (row = 0; row < MAX_ROWS; row++) {
		if (table->rows[row]) {

			//			int x;
			//			for (x = 0; x < db->scheme_size; x++)
			//				printf("%u,", db->rows[row][x]);
			//			printf("\n");

			// copy entry
			for (att_index1 = 0; att_index1 < size; att_index1++)
				entry[att_index1] = table->rows[row][att[att_index1]];

			// error occurs
			if (!table_add_entry(new_db, scheme, entry, size)) {
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

/**
 * \brief		Unions table 1 and table 2 by appending Table 2 is append to
 * 				table 1. Already existing entries in table 1 will be overwritten
 * 				The tables must have the same scheme.
 *
 * \param table1 	First table
 * \param table2	Second table
 *
 * \return 			union of tables, NULL if error occurs
 **/
table_t* table_union(table_t *table1, table_t *table2) {
	if (!table1 || !table2) {
		return NULL;
	}

	if (table1 == table2) {
		return NULL;
	}

	// check schemes
	if (table1->scheme_size != table2->scheme_size) {
		return NULL;
	}

	int i;
	for (i = 0; i < table1->scheme_size; i++) {
		if (table_has_attribute(table2, table1->scheme[i]) < 0) {
			return NULL;
		}
	}

	// copy data
	for (i = 0; i < MAX_ROWS; i++) {
		if (table2->rows[i]) {
			if (!table_add_entry(table1, table2->scheme, table2->rows[i],
					table2->scheme_size)) {
				return NULL;
			}
		}
	}

	return table1;
}

/**
 * \brief		Performs an aggregate operation over a table. It's possible to
 * 				give a list of attributes for an implicit group by operation.
 * 				An explicit call of group by is not necessary. The result
 * 				table is a new independent table with its own memory, hence
 * 				it must be freed eventually
 *
 * \param table_id		id of new table
 * \param table			table to perform aggregation on
 * \param op			type of aggregation operation
 * \param attribute		attribute to aggregate over
 * \param group			the attribute to group by
 * \param size			size of group by attributes
 *
 * \return 				updated table, NULL if error occurs
 */
table_t* table_aggregate(uint8_t table_id, table_t *table, aggregation_type op,
		uint8_t attribute, uint8_t group[], uint8_t size) {

	printf(
			"db_aggregate(table_id %u, table %p, aggr_type %u, attribute %u, group[] %u, size %u\n",
			table_id, table, op, attribute, group[0], size);
	if (!table) {
#ifdef DEBUG_DB
		printf("ERROR: db_aggregate: Got NULL pointer as table!\n\r");
#endif //DEBUG_DB
		return NULL;
	}

	if (table_has_attribute(table, attribute) < 0) {
#ifdef DEBUG_DB
		printf("ERROR: db_aggregate: attribute type %d doesnt exists in table!\n\r", attribute);
#endif //DEBUG_DB
		return NULL;
	}

	//	printf("db had attribute %u at pos %u\n", attribute, db_has_attribute(db,
	//			attribute));

	// search index of attributes in scheme
	int i, j;
	for (i = 0; i < size; i++) {
		//        printf("db_aggregate: group[i]: %u, attribute: %u\n",group[i], attribute);
		if (group[i] == attribute) {
#ifdef DEBUG_DB
			printf("ERROR: db_aggregate: cant group by aggregate attribute!\n\r");
#endif //DEBUG_DB
			return NULL;
		}
	}

	int grp_index = table_has_attribute(table, group[size - 1]); // index of attribute in scheme with lowest priority
	int agg_index = table_has_attribute(table, attribute); // index of attribute in scheme with should aggregate

	if (!agg_index) {
#ifdef DEBUG_DB
		printf("ERROR: db_aggregate: aggregate attribute not in table!\n\r");
#endif //DEBUG_DB
		return NULL;
	}

	//	printf("aggregation attributes were fine\n");
	// sort table
	//	db = db_groupby(db, group, size);

	//	printf("db_aggregate: made it here -2 after db group by db is %p, grp_index is %u, aggr_index is %u\n", db, grp_index, agg_index);


	//	if (!db) {
	//#ifdef DEBUG_DB
	//		printf("ERROR: db_aggregate: couldnt group table elements!\n\r");
	//#endif //DEBUG_DB
	//		return NULL;
	//	}

	//	printf("db_aggregate: made it here -1 after group\n");

	//	// dispatch operation
	//	int ((*op_call)(int[], int, int*)) = NULL;
	//	int group_mode = 0;

	//	printf("db_aggregate: made it here -0.5, op is %p\n", op_call);

	//	// create scheme and key of new table
	//	uint8_t scheme_size = db->scheme_size + 1;
	//	uint8_t key_size = db->key_size;

	//	// group mode handles reduced tables
	//	if (group_mode) {
	//		scheme_size = size + 1;
	//		key_size = size;
	//	}

	//	printf("db_aggregate: made it here 0\n");

	// create new table
	//	printf(
	//			"about to create a table with db->scheme %p, db->scheme_size %u, db->key_index %p, db->key_size %u\n",
	//			db->scheme, db->scheme_size, db->key_index, db->key_size);

	// create key of new table
	uint8_t *key = (uint8_t*) malloc(table->key_size * sizeof(uint8_t));
	for (i = 0; i < table->key_size; i++) {
		key[i] = table->scheme[table->key_index[i]];
	}

	table_t *new_table = table_create(table_id, table->scheme,
			table->scheme_size, key, table->key_size);

	// memory full
	if (!new_table) {
#ifdef DEBUG_DB
		printf("ERROR: db_aggregate: memory full!\n\r");
#endif //DEBUG_DB
		//		free(att_index);
		//		free(scheme);
		//		free(key);
		return NULL;
	}

	//	printf("aggregate: creation of table went through\n");

	// ------------ the aggregation itself: ------------
	// First: search for unique groups and insert them into new table
	for (i = 0; i < table->entries; i++) {
		int *row = table->rows[i];

		int found = 0;
		for (j = 0; j < new_table->entries && !found; j++) {
			int *row_new = new_table->rows[j];
			if (row_new[grp_index] == row[grp_index])
				found = 1;
		}
		if (!found) {
			int *row_new = malloc(table->scheme_size * sizeof(int));

			// copy first row to result table
			for (j = 0; j < table->scheme_size; j++)
				row_new[j] = row[j];

			table_add_entry(new_table, new_table->scheme, row_new,
					table->scheme_size);

		}

	}

	//	printf("aggregate: first loop went through, num entries: %u\n",
	//			new_table->entries);

	// Second: iterate over each row of these unique groups and for each of them
	// perform the aggregation function
	for (i = 0; i < new_table->entries; i++) {
		int *row = new_table->rows[i];
		int *row_a = 0, *row_b = 0;

		//		int x;
		//
		//		printf("the initial row is as follows: i=%u [", i);
		//		for (x = 0; x < new_table->scheme_size; x++)
		//			printf("%u,", row[x]);
		//		printf("]\n");

		// search for the first row that matches
		for (j = 0; j < table->entries; j++) {
			if (table->rows[j][grp_index] == row[grp_index]) {
				row_a = table->rows[j];
				break;
			}
		}

		//		printf("first row to be aggregated: j=%u [", j);
		//		for (x = 0; x < new_table->scheme_size; x++)
		//			printf("%u,", row_a[x]);
		//		printf("]\n");

		// now search for the remaining entries that match
		for (++j; j < table->entries; j++) {
			if (table->rows[j][grp_index] == row[grp_index]) {

				row_b = table->rows[j];

				//				printf("found another row in j=%u [", j);
				//				for (x = 0; x < new_table->scheme_size; x++)
				//					printf("%u,", row_b[x]);
				//				printf("]\n");

				switch (op) {
				case MAX:
					agg_max(row_a, row_b, row, agg_index);
					break;
				case MIN:
					agg_min(row_a, row_b, row, agg_index);
					break;
				case AVG:
					agg_avg(row_a, row_b, row, agg_index);
					break;
				case COUNT:
					agg_count(row_a, row_b, row, agg_index);
					break;
				case SUM:
					agg_sum(row_a, row_b, row, agg_index);
					break;
				default:
#ifdef DEBUG_DB
					printf("ERROR: db_aggregate: Unknown operation!\n\r");
#endif //DEBUG_DB
					return NULL;
				}

				//				printf("DEBUG: db_aggregate(op: %u): resulting row [", op);
				//				for (x = 0; x < table->scheme_size; x++)
				//					printf("%u,", row[x]);
				//				printf("]\n");

			}

			row_a = row;
		}
	}

	return new_table;
}

/**
 * \brief		Performs an WHERE operation over a table.
 *
 * \param table		table on which to check the conditions
 * \param op		operation to perform
 * \param attribute	attribute to compare
 * \param value		value to compare again
 *
 * \return 			updated tables, NULL if error occurs
 */
table_t* table_where(table_t *db, comparison_operator op, uint8_t attribute,
		int value) {
	if (!db) {
#ifdef DEBUG_DB
		printf("ERROR: db_where: Got NULL pointer as table!\n\r");
#endif //DEBUG_DB
		return NULL;
	}

	int ((*op_call)(int, int)) = NULL;

	// select operation
	switch (op) {
	case SMALLER:
		op_call = get_smaller;
		break;
	case SMALLER_EQUAL:
		op_call = get_smaller_equal;
		break;
	case GREATER:
		op_call = get_greater;
		break;
	case GREATER_EQUAL:
		op_call = get_greater_equal;
		break;
	case EQUAL:
		op_call = get_equal;
		break;
	case NOT_EQUAL:
		op_call = get_not_equal;
		break;
	default:
		printf("ERROR: dbms_where: Unknown operation!\n\r");
		return 0;
	}

	int index;
	if ((index = table_has_attribute(db, attribute)) < 0) {
#ifdef DEBUG_DB
		printf("ERROR: db_where: attribute type %d doesn't exists in table!\n\r", attribute);
#endif //DEBUG_DB
		return NULL;
	}

	// remove all entries which not satisfy the condition
	int row;
	for (row = 0; row < MAX_ROWS && db->entries; row++) {
		int *data = db->rows[row];
		if (data) {
			if (!op_call(data[index], value))
				table_delete_row(db, row);
		}
	}

	return db;
}
