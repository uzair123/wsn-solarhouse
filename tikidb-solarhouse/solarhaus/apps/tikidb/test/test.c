#include "test.h"

/*---------------------------------------------------------------------------*/
/* Callback for an incoming result.                                          */
void test_result_callback(char* resultdump) {
    printf("Antwort empfangen\n\r");
    uint8_t id = dbms_load_dump(resultdump);
	dbms_printf_table(id);
	dbms_drop_table(id);
}

/*---------------------------------------------------------------------------*/
/* Callback for an error message.                                            */
void test_error_callback(char* msg) {
    printf("Fehlermeldung empfangen: %s\n\r", msg);
}


/*---------------------------------------------------------------------------*/
/* MAIN PROCESS                                                              */
PROCESS(main_process, "main process");
AUTOSTART_PROCESSES(&main_process);

PROCESS_THREAD(main_process, ev, data)
{
	PROCESS_EXITHANDLER(tikidb_close());

	PROCESS_BEGIN();

	static struct etimer et;
	static struct etimer et2;

    SENSORS_ACTIVATE(button_sensor);
	tikidb_init(test_result_callback, test_error_callback);

    //cc2420_set_txpower(2);

	printf("Hallo TikiDB!\n");

	// ----------------------------------------------------------------
	// Test 1
	static char test1[] = {SELECT, 2, NONE, ROOM, NONE, TEMPERATURE};
	static int test1_size = 6;

	// ----------------------------------------------------------------
	// Stop
	static char stop[] = {STOP, 5, 0, 0, 0};
	static int stop_size = 5;

	// ----------------------------------------------------------------
	// List queries
	static char lq[] = {LIST_QUERIES};
	static int lq_size = 1;

	// ----------------------------------------------------------------
	// Test 2
	static char test2[] = {SELECT, 2, NONE, ROOM, NONE, TEMPERATURE,
		SAMPLE_PERIOD, 0, 5, LIFETIME, 0, 25};
	static int test2_size = 12;

	// ----------------------------------------------------------------
	// Test Aggregation: MAX
	static char test3[] = {SELECT, 2, NONE, ROOM, MAX, TEMPERATURE,
		GROUP_BY, ROOM, SAMPLE_PERIOD, 0, 5, LIFETIME, 0, 25};
	static int test3_size = 14;

	// ----------------------------------------------------------------
	// Test Aggregation: MIN
	static char test4[] = {SELECT, 2, NONE, ROOM, MIN, TEMPERATURE,
		GROUP_BY, ROOM, SAMPLE_PERIOD, 0, 5, LIFETIME, 0, 25};
	static int test4_size = 14;

	// ----------------------------------------------------------------
	// Test Aggregation: COUNT
	static char test5[] = {SELECT, 2, NONE, ROOM, COUNT, TEMPERATURE,
		GROUP_BY, ROOM, SAMPLE_PERIOD, 0, 5, LIFETIME, 0, 25};
	static int test5_size = 14;

	// ----------------------------------------------------------------
	// Test Aggregation: SUM
	static char test6[] = {SELECT, 2, NONE, ROOM, SUM, TEMPERATURE,
		GROUP_BY, ROOM, SAMPLE_PERIOD, 0, 5, LIFETIME, 0, 25};
	static int test6_size = 14;

	// ----------------------------------------------------------------
	// Test Aggregation: AVG
	static char test7[] = {SELECT, 2, NONE, ROOM, AVG, TEMPERATURE,
		GROUP_BY, ROOM, SAMPLE_PERIOD, 0, 5, LIFETIME, 0, 25};
	static int test7_size = 14;


	// ----------------------------------------------------------------
	// Official test:
	//	SELECT NODE_ID_TIKIDB, ambient_light, PAR_light, humidity,
	//	       temperature, internal_temperature,
	//	       room, floor, building
	//	  FROM SENSORS
	//	  SAMPLE PERIOD 15s
	static char oftest1[] = {SELECT, 5, NONE, AMBIENT_LIGHT,
		NONE, PAR_LIGHT, NONE, HUMIDITY, NONE, TEMPERATURE,
		NONE, INTERNAL_TEMPERATURE,
		SAMPLE_PERIOD, 0, 15};
	static int oftest1_size = 14;


	// ----------------------------------------------------------------
	// Official test:
	//	SELECT NODE_ID_TIKIDB
	//	  FROM SENSORS
	//	  WHERE NODE_ID_TIKIDB > 10 AND NODE_ID_TIKIDB <= 15
	//	  SAMPLE PERIOD 10s
	//	  LIFETIME 100s
	static char oftest2[] = {SELECT, 1, NONE, NODE_ID_TIKIDB,
		WHERE, GREATER, NODE_ID_TIKIDB, 0, 3, AND, SMALLER_EQUAL, NODE_ID_TIKIDB, 15,
		SAMPLE_PERIOD, 0, 10, LIFETIME, 0, 100};
	static int oftest2_size = 19;


	// ----------------------------------------------------------------
	// Official test:
	//	SELECT COUNT(*)
	//	  FROM SENSORS
	//	  SAMPLE PERIOD 15s
	static char oftest3[] = {SELECT, 2, NONE, TEMPERATURE, COUNT, ROOM,
		GROUP_BY, TEMPERATURE,
		SAMPLE_PERIOD, 0, 15};
	static int oftest3_size = 11;


	// ----------------------------------------------------------------
	// Official test:
	//	SELECT AVG(temp)
	//	  FROM SENSORS
	//	  WHERE NODE_ID_TIKIDB > 6
	//	  SAMPLE PERIOD 15 seconds
	static char oftest4[] = {SELECT, 2, NONE, ROOM, AVG, TEMPERATURE,
		WHERE, GREATER, NODE_ID_TIKIDB, 0, 6,
		GROUP_BY, ROOM,
		SAMPLE_PERIOD, 0, 15};
	static int oftest4_size = 16;


	// ----------------------------------------------------------------
	// Official test:
	//	SELECT AVG(light), room
	//	  FROM SENSORS
	//	  WHERE floor = 6
	//	  GROUP BY room
	//	  SAMPLE PERIOD 15s
	static char oftest5[] = {SELECT, 2, NONE, ROOM, AVG, TEMPERATURE,
		//WHERE, EQUAL, ROOM, 0, 12,
		GROUP_BY, ROOM,
		SAMPLE_PERIOD, 0, 15};
	static int oftest5_size = 11;





    if(rimeaddr_node_addr.u8[0] == TREEFORMING_BASE_NODE && rimeaddr_node_addr.u8[1] == 0) {
//        tikidb_set_location(1, 0, 2, 3, 4, 5, 6, 7);
//        //tikidb_set_location(2, 0, 3, 4, 5);
//        tikidb_set_location(3, 0, 4, 5, 6, -7, 8, 9);
//        tikidb_set_location(4, 0, 5, 6, 7, -8, 9, 10);
//        tikidb_set_location(5, 0, 6, 7, 8, -9, 10, 11);
//        tikidb_set_location(6, 0, 7, 8, 9, -10, 11, 12);
//        tikidb_set_location(7, 0, 8, 9, 10, -11, 12, 13);
//        tikidb_set_location(8, 0, 9, 10, 11, -12, 13, 14);
//        tikidb_set_location(9, 0, 10, 11, 12, -13, 14, 15);

		// 3. Raum
        tikidb_set_location(15, 0, 0, 3, 4, 5, 6, 7);
        tikidb_set_location(17, 0, 1, 10, 11, -12, 13, 14);
        tikidb_set_location(18, 0, 1, 5, 6, -7, 8, 9);
        tikidb_set_location(19, 0, 2, 6, 7, -8, 9, 10);
        tikidb_set_location(20, 0, 2, 7, 8, -9, 10, 11);

      //  PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);

    	while(1) {
    		// wait for start signal
            PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);


            printf("Sende Anfrage\n\r");
            //tikidb_send_query(stop, stop_size);
    		tikidb_send_query(oftest4, oftest4_size);


    		PROCESS_WAIT_EVENT_UNTIL(0);
    	}
	}
    else
        PROCESS_WAIT_EVENT_UNTIL(0);


	PROCESS_END();

}
