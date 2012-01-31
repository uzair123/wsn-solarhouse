#include "contiki.h"
#include "node-id.h"
#include "dev/leds.h"
#include "lib/random.h"

#include "tikidb.h"
#include "scopes.h"
#include "scopes-flooding.h"


#define SCOPE_ID 5
#define SCOPE_TTL 30
#define QUERY_ID 9
#define QUERY_SIZE 50
#define TIKIDB_SUBID 7
#define SAMPLE_PERIOD_VALUE1 15
#define SAMPLE_PERIOD_VALUE2 60


static uint8_t root_node = 52;
static uint8_t node_id_list[] = {50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69};

static uint8_t wrapped_query[] = {
	QUERY,
	QUERY_ID,
	0, // placeholder for tree height
	1, //number of scopes
	SCOPE_ID,
	SELECT, //
	4, // num selects
	NONE, // agg type
	CO2, // field
	NONE, // agg type
	HUMIDITY, // field
	NONE, // agg type
	TEMPERATURE, // field
	NONE,
	RSSI,
	SAMPLE_PERIOD, //
	SAMPLE_PERIOD_VALUE1, // sample time = SPV1 * SPV2
	SAMPLE_PERIOD_VALUE2, //
};

static struct scope *scope1 = NULL;
static struct subscriber scopes_tikidb;

void test_result_callback(uint8_t* resultdump, query_object *qo);

void treeforming_send_children(uint8_t *wrapped_query, int wrapped_query_size) {
	// if query has no scopes, send it through world
	scope_id_t scope = 0;
	if (wrapped_query[3] > 0) { // [SCOPES_COUNT_POS]
		// search through which scope the info is gonna be sent:
		int scope_nr = 1;

		while (scope_nr <= wrapped_query[3]) { // [SCOPES_COUNT_POS]
			scope_id_t sc = get_query_scope(wrapped_query, scope_nr);
			scopes_send(&scopes_tikidb, scopes_lookup(sc), 0, wrapped_query, wrapped_query_size);
			scope_nr++;
		}
	} else
		scopes_send(&scopes_tikidb, scopes_lookup(scope), 0, wrapped_query, wrapped_query_size);
}


uint8_t treeforming_send_parent(uint8_t *resultdump, int size, query_object *qo) {
	if (HAS_STATUS(scopes_lookup(qo->scope_id), SCOPES_STATUS_CREATOR)) {
		// root node reached, print out results:
		test_result_callback(resultdump, qo);
	} else {
		// still at intermediate node, forward:
		scopes_send(&scopes_tikidb, scopes_lookup(qo->scope_id), 1, resultdump, size);
	}
	return 1;
}


void add_scope(struct scope *scope) {}
void destroy_scope(uint16_t scope_id) {}
void remove_scope(struct scope *scope) {}


PROCESS(queryDog, "query dog");
PROCESS_THREAD(queryDog, ev, data) {
	PROCESS_BEGIN();
	static struct etimer et;
	if (node_id != root_node && scope1 != NULL) {
		if (querymanager_qidlist_contains_qid(QUERY_ID)) {
			PROCESS_YIELD();
		} else {
			while (!querymanager_qidlist_contains_qid(QUERY_ID)) {
				etimer_set(&et, CLOCK_SECOND * (random_rand() % 10));
				PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
				scopes_send(&scopes_tikidb, scope1, TRUE, "req", strlen("req"));
				PROCESS_PAUSE();
			}
		}
	}
	PROCESS_END();
}


void join_scope(struct scope *scope) {
	scope1 = scope;
	process_start(&queryDog, NULL);
}


void leave_scope(struct scope *scope) {
	if (scope1->scope_id == scope->scope_id) {
		scope1 = NULL;
		process_exit(&queryDog);
		if (querymanager_qidlist_contains_qid(QUERY_ID)) {
			querymanager_qidlist_remove(querymanager_qidlist_get_by_qid(QUERY_ID));
		}
	}
}


void recv_data(struct scope *sscopes_opencope, void *data, data_len_t data_len) {
	if (node_id == root_node && data_len == 3 && memcmp(data, "req", data_len) == 0) {
		querymanager_send_query(wrapped_query, sizeof(wrapped_query));
		printf("sending query\n");
	} else {
		querymanager_receive((uint8_t*)data, data_len, sscopes_opencope->scope_id);
	}
}


void test_result_callback(uint8_t* resultdump, query_object *qo) {
	database *db = db_list_get_table(qo->temporary_table_id);
	if (db) {
		dbms_printf_table(qo->temporary_table_id);
	}
}


void error_callback(uint8_t* msg) {
	printf("Error message received: %s\n\r", msg);
}


static struct scopes_application tikiDB_callbacks = { add_scope, remove_scope, join_scope, leave_scope, recv_data };

PROCESS(surPLUShome, "surPLUShome");
PROCESS_THREAD(surPLUShome, ev, data) {
	PROCESS_EXITHANDLER(tikidb_close());
	PROCESS_BEGIN();

	static struct etimer et;
	random_init(node_id);
	scopes_init(&scopes_flooding, &simple_membership);
	tikidb_init(test_result_callback, error_callback);
	scopes_register(&scopes_tikidb, TIKIDB_SUBID, &tikiDB_callbacks);
		
	if (node_id == root_node) {
		etimer_set(&et, CLOCK_SECOND*10 + CLOCK_SECOND*SCOPE_TTL);
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
		scopes_open(&scopes_tikidb, &world_scope, SCOPE_ID, node_id_list, sizeof(node_id_list)/sizeof(node_id_list[0]), SCOPES_FLAG_NONE, SCOPE_TTL);
	}
	PROCESS_END();
}

AUTOSTART_PROCESSES(&surPLUShome);
