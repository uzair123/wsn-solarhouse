#include "dbms.h"

/*---------------------------------------------------------------------------*/
/* Initialize database management system.                                    */
/* @return error code, else 0                                                */ 
uint8_t dbms_init(void)
{
    db_last_id = 0;
    return db_list_init();
}

/*---------------------------------------------------------------------------*/
/* Shutdown database management system.                                      */
/* @return error code, else 0                                                */ 
uint8_t dbms_close(void)
{
    return db_list_close();
}

/*---------------------------------------------------------------------------*/
/* Creates a new table with given scheme.                                    */
/* @param uint8_t[] scheme scheme of table, given in a sequence of integer.  */
/* @param uint8_t size of scheme definition                                  */
/* @param uint8_t[] key set of attributes choosen for primary key            */
/* @param uint8_t p_key_size numbers of attributes for primary key           */
/* @return if negative its an error code, else the id of table               */ 
/* error codes:                                                              */
/* -4: max number of tables reached                                          */
/* -5: memory full                                                           */
/* -6: unknown error                                                         */
int dbms_create_table(uint8_t scheme[], uint8_t scheme_size, uint8_t key[], uint8_t key_size)
{
    // search unused id
    uint8_t id = dbms_get_free_id();

    if(!id) {
#ifdef DEBUG_DBMS
		printf("ERROR: dbms_create_table: max number of tables reached!\n\r");
#endif //DEBUG_DBMS
        return -4;
    }

    // create database
    database *db = db_create(id, scheme, scheme_size, key, key_size);

    // memory full
	if(!db) {
#ifdef DEBUG_DBMS
		printf("ERROR: dbms_create_table: memory full!\n\r");
#endif //DEBUG_DBMS
		return -5;
	}

    // add database to list
    if(db_list_add_table(db)) {
#ifdef DEBUG_DBMS
		printf("ERROR: dbms_create_table: error while adding to list!\n\r");
#endif //DEBUG_DBMS
        db_drop(db);
		return -6;
    }

    return id;
}

/*---------------------------------------------------------------------------*/
/* Returns table with given id.                                              */
/* @param uint8_t table_id id of table                                       */
/* @return database* pointer to table, NULL if table wasnt found             */
database* dbms_get_table(uint8_t table_id) {
    return db_list_get_table(table_id);
}

/*---------------------------------------------------------------------------*/
/* Returns scheme of table with given id.                                    */
/* @param uint8_t table_id id of table                                       */
/* @param int* size size of returned char array                              */
/* @return int* pointer to scheme, if error occurs NULL                      */
uint8_t* dbms_get_table_scheme(uint8_t table_id, unsigned int *size)
{
    database *db = db_list_get_table(table_id);

    if(db) {
        *size = db->scheme_size;
        return db->scheme;
    }

    *size = 0;
    return NULL;
}

/*---------------------------------------------------------------------------*/
/* Clears a database and deletes all entries.                                */
/* @param uint8_t table_id id of table                                       */
/* @return 1 if error occurs else 0                                          */
uint8_t dbms_clear_table(uint8_t table_id)
{
    if(!db_clear(db_list_get_table(table_id)))
        return 1;

    return 0;
}

/*---------------------------------------------------------------------------*/
/* Clones a database and returns new id of an independet database.           */
/* Dont forget to drop database later for freeing memory space!              */
/* @param uint8_t table_id id of table                                       */
/* @return error code, else id of new table                                  */
/* error codes:                                                              */
/* -4: max number of tables reached                                          */
/* -5: memory full                                                           */
/* -6: unknown error                                                         */
int dbms_clone_table(uint8_t table_id)
{
    // search unused id
    uint8_t id = dbms_get_free_id();

    if(!id) {
#ifdef DEBUG_DBMS
		printf("ERROR: dbms_clone_table: max number of tables reached!\n\r");
#endif //DEBUG_DBMS
        return -4;
    }

    // clone database
    database *db = db_clone(id, db_list_get_table(table_id));
    if(!db)
        return -5;

    // add database to list
    if(db_list_add_table(db)) {
#ifdef DEBUG_DBMS
		printf("ERROR: dbms_clone_table: error while adding to list!\n\r");
#endif //DEBUG_DBMS
        db_drop(db);
		return -6;
    }

    return id;
}

/*---------------------------------------------------------------------------*/
/* Deletes table permantly.                                                  */
/* @param uint8_t table_id id of table                                       */
/* @return 1 if error occurs else 0                                          */
uint8_t dbms_drop_table(uint8_t table_id)
{
    database *db = db_list_get_table(table_id);

    if(db) {
        db_drop(db_list_remove_table(table_id));
        return 0;
    }

    return 1;
}

/*---------------------------------------------------------------------------*/
/* Creates dump of a database in form of a char matrix. The first row        */
/* contains the scheme (table head) of the table. After this every row       */
/* contains an entry of the original database. Two pointer to integer values */
/* is needed to return the dimension of the dump. The dump itself allocs     */
/* memory, so its needed to call free over this dump, to free memory.        */
/* The 8 Bit integer pointer are optional and may be NULL.                   */
/* @param uint8_t table_id id of table                                       */
/* @param int* size size of returned char array                              */
/* @param uint8_t* row_size lenght per row (given in bytes)                  */
/* @param uint8_t* row_num numbers of rows (including head; given in bytes)  */
/* @return database dump, NULL if error occurs                               */
uint8_t* dbms_get_dump(uint8_t table_id, unsigned int *size, uint8_t *row_size, unsigned int *row_num)
{
    database *db = db_list_get_table(table_id);
    if(!db) {
#ifdef DEBUG_DBMS
		printf("ERROR: dbms_get_dump: table with given id not found!\n\r");
#endif //DEBUG_DBMS
        return NULL;
    }

    return db_get_dump(db, size, row_size, row_num);
}

/*---------------------------------------------------------------------------*/
/* Loads dump of a database in form of a char matrix. The first row          */
/* contains the scheme (table head) of the table. After this every row       */
/* contains an entry of the original database.                               */
/* Dont forget to drop database later for freeing memory space!              */
/* @param char* dump database dump                                           */
/* @param uint8_t row_size lenght per row                                    */
/* @param uint8_t rows numbers of rows (including head)                      */
/* @return error code, else id of new table created from dump                */
/* error codes:                                                              */
/* -4: max number of tables reached                                          */
/* -5: memory full                                                           */
/* -6: unknown error                                                         */
int dbms_load_dump(uint8_t *dump)
{
    // search unused id
    uint8_t id = dbms_get_free_id();

    if(!id) {
#ifdef DEBUG_DBMS
		printf("ERROR: dbms_load_dump: max number of tables reached!\n\r");
#endif //DEBUG_DBMS
        return -4;
    }

    // load dump
    database *db = db_load_dump(id, dump);

    if(!db) {
#ifdef DEBUG_DBMS
		printf("ERROR: dbms_load_dump: error while reading dump!\n\r");
#endif //DEBUG_DBMS
        return -6;
    }

    // add database to list
    if(db_list_add_table(db)) {
#ifdef DEBUG_DBMS
		printf("ERROR: dbms_load_dump: error while adding to list!\n\r");
#endif //DEBUG_DBMS
        db_drop(db);
		return -6;
    }

    return id;
}

/*---------------------------------------------------------------------------*/
/* Adds entry to an existing table. If entry already with equal key          */
/* attributes already exists, it will be overwritten.                        */              
/* @param uint8_t table_id id of table                                       */
/* @param uint8_t[] scheme scheme of entry, given in a sequence of integer.  */
/* @param int[] entry values                                                 */
/* @param uint8_t size numbers of attributes                                 */
/* @return 1 if error occurs else 0                                          */
uint8_t dbms_add_entry(uint8_t table_id, uint8_t scheme[], int entry[], uint8_t size)
{
    // error occurs
    if(!db_add_entry(db_list_get_table(table_id), scheme, entry, size)) {
#ifdef DEBUG_DBMS
		printf("ERROR: dbms_table_add_entry: error while adding entry!\n\r");
#endif //DEBUG_DBMS
		return 1;
    }

    return 0;
}

/*---------------------------------------------------------------------------*/
/* Deletes a database entry with given scheme. The entry is searched         */
/* only with key attributes. Other attributes doesnt have any effect.        */
/* @param uint8_t table_id id of table                                       */
/* @param uint8_t[] scheme scheme of entry, given in a sequence of integer.  */
/* @param int[] entry values                                                 */
/* @param uint8_t size numbers of attributes                                 */
/* @return 1 if error occurs else 0                                          */
uint8_t dbms_delete_entry(uint8_t table_id, uint8_t scheme[], int entry[], uint8_t size)
{
    // error occurs
    if(!db_delete_entry(db_list_get_table(table_id), scheme, entry, size)) {
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
/* @return uint8_t -1 if entry doesn't exists, otherwise row in database     */
/* error codes:                                                              */
/* -1: entry doesnt exists                                                   */
/* -2: not all primary keys are given in entry                               */
/* -3: database is NULL pointer                                              */
/* -4: max number of tables reached                                          */
/* -5: memory full                                                           */
/* -6: unknown error                                                         */
int dbms_has_entry(uint8_t table_id, uint8_t scheme[], int entry[], uint8_t size)
{
    return db_has_entry(db_list_get_table(table_id), scheme, entry, size);
}

/*---------------------------------------------------------------------------*/
/* Gets specific row of database. For an specific entry the method           */
/* dbms_has_entry can be used to get the row.                                */
/* @param uint8_t table_id id of table                                       */
/* @param int row row number                                                 */
/* @param int* size size of returned char array                              */
/* @return int* pointer to data row, if error occurs NULL                    */
int* dbms_get_entry(uint8_t table_id, unsigned int row, unsigned int *size)
{
    database *db = db_list_get_table(table_id);

    if(db) {
        *size = db->scheme_size;
        return db_get_entry(db, row);
    }

    *size = 0;
    return NULL;
}

/*---------------------------------------------------------------------------*/
/* Gets specific value from database. For an specific entry the method       */
/* dbms_has_entry can be used to get the row.                                */
/* @param uint8_t table_id id of table                                       */
/* @param int row row number                                                 */
/* @param uint8_t type type of value, must be exist in scheme of table       */
/* @return int value of attribute, 0 if value wasnt found                    */
int dbms_get_value(uint8_t table_id, unsigned int row, uint8_t type)
{
    database *db = db_list_get_table(table_id);

    if(db) {
        int *entry = db_get_entry(db, row);

        if(entry) {
            int attribute;
            for(attribute = 0; attribute < db->scheme_size; attribute++) {
                if(type == db->scheme[attribute])
                    return entry[attribute];
            }

            return 0;
        }
    }

    return 0;
}

/*---------------------------------------------------------------------------*/
/* Prints table to console.                                                  */
/* @param uint8_t table_id id of table                                       */
void dbms_printf_table(uint8_t table_id)
{
    db_printf(db_list_get_table(table_id));
}
