#include "shell-tikidb.h"
#include "base64.h"
/*---------------------------------------------------------------------------*/
PROCESS(shell_tikidb_statement_process, "tikidb");
SHELL_COMMAND(tikidb_statement_command,
		"tikidb",
		"tikidb <statement>: executes a TikiDB statement expressed in base 64 encoding",
		&shell_tikidb_statement_process);
/* -------------------------------------------------------------------- */
void shell_tikidb_init(void) {
	PRINTF(3,"shell_tikidb_init invoked\n");

	tikidb_init(cb_shell_tikidb_result);

	shell_register_command(&tikidb_statement_command);
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD( shell_tikidb_statement_process, ev, data) {
	PROCESS_BEGIN();

	if (data == NULL) {
		shell_output_str(&tikidb_statement_command, "parameter missing!", "");
		PROCESS_EXIT();
	}

	char *msg;
	msg = "Processing statement...\n";
	shell_output(&tikidb_statement_command, msg, (int) strlen(msg), "", 0);

	static char *input;
	input = (char *) data;
	static uint8_t input_len;
	static uint8_t decoded[127];

	input_len = strlen(data);
	uint8_t decoded_len = base64_decode(input, decoded, 127);

	//	PRINT_ARR(1, decoded, decoded_len);

	tikidb_process_statement(decoded, decoded_len);

	PROCESS_END();
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *   C A L L - B A C K   F U N C T I O N S
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**
 * \brief		 Callback for an incoming result.
 **/
void cb_shell_tikidb_result(uint8_t* resultset, select_statement_t *ss) {

	shell_output(&tikidb_statement_command, "\n", 1, "\n", 1);

	PRINTF(
			4,
			"[%u.%u:%10lu] %s::%s() : function entered, result_dump: %p, query_object: %p, temp_table_id: %u\n",
			rimeaddr_node_addr.u8[1], rimeaddr_node_addr.u8[0], clock_time(),
			__FILE__, __FUNCTION__, resultset, ss, ss->temporary_table_id);

	table_t *table = tikidb_buffer_get_table_id(ss->temporary_table_id);
	if (table) {
		uint8_t epoch = resultset_get_epoch(resultset);
		uint8_t num_entries = 0;

		if (ss->group_by == 0) {
			num_entries = table->entries;
		} else {

			int i;

			for (i = 0; i < table->scheme_size; i++) {
				uint8_t col = table->scheme[i];
				if ((col == NONE) || (col == MAX) || (col == MIN) || (col
						== AVG) || (col == COUNT) || (col == SUM))
					break;
			}

			printf("HAD aggregation in column %u\n", i);

			if (i < table->scheme_size)

				num_entries = table_get_entry(table, 0)[i];
		}

		PRINTF(2, "[%u.%u:%10lu] %s::%s() : epoch {%u} results [%u]\n",
				rimeaddr_node_addr.u8[1], rimeaddr_node_addr.u8[0],
				clock_time(), __FILE__, __FUNCTION__, epoch, num_entries);
		table_printf(tikidb_buffer_get_table_id(ss->temporary_table_id));
	} else PRINTF(
			3,
			"[%u.%u:%10lu] %s::%s() : no database found with temp_table_id\n",
			rimeaddr_node_addr.u8[1], rimeaddr_node_addr.u8[0],
			clock_time(), __FILE__, __FUNCTION__);

}
