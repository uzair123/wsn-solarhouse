#include "tikidb.h"

/*---------------------------------------------------------------------------*/
/* Initialize tikiDB system with all necessary components.                   */
/* @return int zero if no error occurs, otherwise non-zero                   */
int tikidb_init(void(*callback_result)(uint8_t* resultdump, query_object *qo),
		void(*callback_error)(uint8_t* msg)) {
	// Initialize the layers and the datebase.
	queryresolver_init(tikidb_result_callback, tikidb_error_callback);

	// Save the adress of the callback method.
	callback_handle_result_function = callback_result;
	callback_handle_error_function = callback_error;

	return -1;
}

/*---------------------------------------------------------------------------*/
/* Shutdown tikiDB system.                                                   */
void tikidb_close(void) {
	queryresolver_close();
	return;
}

/*---------------------------------------------------------------------------*/
/* Callback for an incoming result.                                          */
void tikidb_result_callback(uint8_t* resultdump) {
	callback_handle_result_function(resultdump);
}

/*---------------------------------------------------------------------------*/
/* Callback for an error message.                                            */
void tikidb_error_callback(uint8_t* msg) {
	callback_handle_error_function(msg);
}

/*---------------------------------------------------------------------------*/
/* Interface for a new query                                                 */
void tikidb_send_query(uint8_t* querystring, int size) {
	//	queryresolver_send_query(querystring, size);
}

///*---------------------------------------------------------------------------*/
///* Sets location of a node.                                                  */
///* @param int NODE_ID_TIKIDB_1 first digit of node id                               */
///* @param int NODE_ID_TIKIDB_2 second digit of node id                              */
///* @param int room room id of the node                                       */
///* @param int floor floor id of the node                                     */
///* @param int building building id of the node                               */
///* @return int zero if no error occures, otherwise non-zero                  */
//int tikidb_set_location(int NODE_ID_TIKIDB_1, int NODE_ID_TIKIDB_2, int room, int floor,
//		int building, int x, int y, int z)
//{
//    rimeaddr_t addr;
//    addr.u8[0] = NODE_ID_TIKIDB_1;
//    addr.u8[1] = NODE_ID_TIKIDB_2;
//
//    return settings_set_values(&addr, room, floor, building, x, y, z);
//}

void tikidb_traffic_statistics_reset() {
#ifdef DATALINK_RUN_STATISTICS
	datalink_statistics_reset();
#endif //DATALINK_RUN_STATISTICS
}

void tikidb_traffic_statistics_printf() {
#ifdef DATALINK_RUN_STATISTICS
	datalink_statistics_printf();
#endif //DATALINK_RUN_STATISTICS
}
