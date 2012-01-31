#include "query-parser.h"

/* --------------------------------------------------------------------
 * Take an data stream from the network and copy his data into the
 * query object.
 *
 * @param query* Pointer to the new query object.
 * @param uint8_t*  Pointer to the packet's data.
 * @param int    The actual index from which the data have to
 *               parse further.
 * @param int	 The length of the packet's data in byte.
 */
enum query_type query_parser_start(query* q, uint8_t* wrapped_query, int index, int wrapped_query_size) {

#ifdef DEBUG_QUERYMANAGER
	printf("query_parser_start()\n");
#endif // DEBUG_QUERYMANAGER
	enum query_type result = STOP;
	// Read the first byte. It could be:
	// * 1 SELECT
	// * 6 STOP
	// * 7 LIST QUERIES

	switch (wrapped_query[index]) {

	case 1: {
		result = SELECT;
		query_parser_select(q, wrapped_query, index + 1, wrapped_query_size);
	}
		break;

	case 6: {
		result = STOP;

#ifdef DEBUG_QUERYMANAGER
		printf("this could be wrong:\n"); // @TODO
#endif // DEBUG_QUERYMANAGER
		// stop_id is a 32 bit value. NOW: 16
		q->qid = wrapped_query[1];

	}
		break;

	case 7: {
		result = LIST_QUERIES;
	}
		break;

	default:
#ifdef DEBUG_QUERYMANAGER
		printf("query_parser_start(): Type of the packet isn't correct. Set it to STOP (data[index] = %d / index %d)\n", wrapped_query[index],index);
#endif // DEBUG_QUERYMANAGER
		break;

	}

	return result;

}

/* --------------------------------------------------------------------
 * Parse the packet's data after the select statement.
 *
 * @param query* Pointer to the new query object.
 * @param uint8_t*  Pointer to the packet's data.
 * @param int    The actual index from which the data have to
 *               parse further.
 * @param int	 The length of the packet's data in byte.
 */
void query_parser_select(query* q, uint8_t* wrapped_query, int index,
		int wrapped_query_size) {

#ifdef DEBUG_QUERYMANAGER
	printf("query_parser_select()\n");
#endif // DEBUG_QUERYMANAGER
	q->qid = wrapped_query[1];
	//	q->qid = q->qid << 8;
	//	q->qid = q->qid + wrapped_query[2];

	// wrapped_query[index] contains the number n of the following arguments.
#ifdef DEBUG_QUERYMANAGER
	printf("query_parser::query_parser_select(): there are %u columns in the query\n", wrapped_query[index]);
#endif // DEBUG_QUERYMANAGER
	uint8_t argcount = wrapped_query[index];

#ifdef DEBUG_QUERYMANAGER
	if (argcount < 1)
	printf("query_parser_select(): (1) The form of the query isn't correct.\n");
#endif // DEBUG_QUERYMANAGER
	// Link the first structure to the main structure.
	q->select = malloc(sizeof(struct select_col) * argcount);

	if (q->select == NULL) {
#ifdef DEBUG_QUERYMANAGER
		printf("query_parser_select(): Could not allocate memory for q->select! Aborting\n");
#endif // DEBUG_QUERYMANAGER
		return;
	}

	q->num_selects = argcount;

	// Buffer for the new structures.
	struct select_col *s = q->select;

	// Create 'argcount' structures for the select statement
	int i;
	for (i = 0; i < argcount; i++) {

		// wrapped_query[index+n+1] contains the aggragation type
		s->aggregation = wrapped_query[index + 1];

		// data[index+n+2] contains the id of the col
		s->value = wrapped_query[index + 2];

		s++;
		index += 2;
	}

	//	printf("Debugging select_cols: [");
	//	uint8_t *x = (uint8_t*)q->select;
	//	for (i=0;i<(sizeof(struct select_col) * argcount); i++) printf("%u,", x[i]);
	//	printf("] (each struct select_col is %u bytes long, argcount was %u)\n", sizeof(struct select_col), argcount);

	// Go on with the next part(s), if exists.
	if (index + 1 < wrapped_query_size) {

		//		printf("query_parser_select(): wrapped_query's next byte is %u\n", wrapped_query[index+1]);
		// At least one further point exists.
		switch (wrapped_query[index + 1]) {

		case SAMPLE_PERIOD: { // SAMPLE PERIOD

			query_parser_sample_period(q, wrapped_query, index + 1, wrapped_query_size);
		}
			break;

		default: {
#ifdef DEBUG_QUERYMANAGER
			printf("query_parser_select(): (2) The form of the query isn't correct. Abort\n");
#endif // DEBUG_QUERYMANAGER
		}
			break;

		} // switch

	}

}


/* --------------------------------------------------------------------
 * Parse the packet's data begining at the sample period statement.
 *
 * @param query* Pointer to the new query object.
 * @param uint8_t*  Pointer to the packet's data.
 * @param int    The actual index from which the data have to
 *               parse further.
 * @param int	 The length of the packet's data in byte.
 */
void query_parser_sample_period(query* q, uint8_t* wrapped_query, int index, int wrapped_query_size) {
#ifdef DEBUG_QUERYMANAGER
	printf("query_parser_sample_period()\n");
#endif // DEBUG_QUERYMANAGER
	// Copy the time for the SAMPLE PERIOD in the structure.
	q->sample_period = wrapped_query[index + 1] * wrapped_query[index + 2];
}
