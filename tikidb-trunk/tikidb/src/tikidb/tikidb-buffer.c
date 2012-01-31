#include "tikidb-buffer.h"
#include "lib/list.h"

struct table_node {
	struct table_node *next;
	table_t *table;
};

/** \brief		TODO */
LIST( table_buffer);

/**
 * \brief		Initialize TikiDB's buffer
 */
void tikidb_buffer_init(void) {
	list_init(table_buffer);
}

/**
 * \brief		Searches for a free id for a table.
 *
 * \return 		free id, else 0 if no id is free
 **/
table_id_t tikidb_buffer_get_free_id(void) {

	if (list_length(table_buffer) == TIKIDB_MAX_TABLES)
	return 0;

	// search unused id
	table_id_t new_table_id = 1;
	while (new_table_id <= TIKIDB_MAX_TABLES) {
		if (new_table_id != 0 && !tikidb_buffer_get_table_id(new_table_id)) {
			return new_table_id;
		}
		new_table_id++;
	}

	return 0;
}

/**
 * \brief		Shutdowns TikiDB's buffer
 */
void tikidb_buffer_close(void) {
	while (list_length(table_buffer) > 0) {

		table_t *table = tikidb_buffer_remove_table(
				((struct table_node*) list_head(table_buffer))->table->id);
		table_free(table);
	}
}

/**
 * \brief		Adds table to buffer
 *
 * \param table	table to insert
 * \return		TRUE if addition went fine, FALSE otherwise
 */
bool tikidb_buffer_add_table(table_t *table) {

	if (list_length(table_buffer) == TIKIDB_MAX_TABLES)
		return FALSE;

	if (!table)
		return FALSE;

	if (tikidb_buffer_get_table_id(table->id))
		return FALSE;

	struct table_node *tn = malloc(sizeof(struct table_node));

	// memory full
	if (!tn)
		return FALSE;

	tn->table = table;

	list_add(table_buffer, tn);

	return TRUE;
}

/**
 * \brief		Gets table with given id from buffer
 *
 * \param table_id id of table
 * \return	table_t *pointer to table, or NULL if element wasn't found
 **/
table_t* tikidb_buffer_get_table_id(table_id_t table_id) {

	struct table_node *tn;

	for (tn = list_head(table_buffer); tn != NULL; tn = tn->next) {

		if (tn->table->id == table_id)
			return tn->table;
	}

	return NULL;
}

/**
 * \brief		Looks up table with given id and removes it from buffer
 *
 * \param table_id	id of table
 *
 * \return 			pointer to table, NULL if element wasn't found
 */
table_t* tikidb_buffer_remove_table(table_id_t table_id) {

	struct table_node *tn;

	for (tn = list_head(table_buffer); tn != NULL; tn = tn->next) {

		// table found
		if (tn->table->id == table_id) {
			table_t *result = tn->table;

			list_remove(table_buffer, tn);
			free(tn);

			return result;
		}
	}

	return NULL;
}
