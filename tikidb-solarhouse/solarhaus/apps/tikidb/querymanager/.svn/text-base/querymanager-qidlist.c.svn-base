#include "querymanager-qidlist.h"

/*---------------------------------------------------------------------------*/
/* Initialize database list.                                                 */
/* @return error code, else 0                                                */
uint8_t querymanager_qidlist_init(void) {
#ifdef DEBUG_QUERYMANAGER
	printf("querymanager_qidlist_init()\n\r");
#endif //DEBUG_QUERYMANAGER
	qid_list = NULL;
	qid_list_size = 0;
	return 0;
}

/*---------------------------------------------------------------------------*/
/* Adds database to list.                                                    */
/* @param query_object* Pointer to the query object                          */
/* @return error code, else 0                                                */
/* error codes:                                                              */
/* 1: memory full                                                            */
uint8_t querymanager_qidlist_add(query_object *qo) {
#ifdef DEBUG_QUERYMANAGER
	printf("querymanager_qidlist_add()\n\r");
#endif //DEBUG_QUERYMANAGER
	struct qid_list_element *e = malloc(sizeof(struct qid_list_element));

	// memory full
	if (!e) {
#ifdef DEBUG_QUERYMANAGER
		printf("ERROR: querymanager_qidlist_add(): memory full!\n\r");
#endif //DEBUG_QUERYMANAGER
		return 1;
	}

	e->qo = qo;
	e->next = qid_list;
	qid_list = e;
	qid_list_size++;

	return 0;
}

/*---------------------------------------------------------------------------*/
/* Returs the query object with the qid, if exists in the list.              */
/* @param uint16_t First part of the QID.                                    */
/* @param uint16_t Second part of the QID.                                   */
/* @return pointer to the query object with the QID, else NULL               */
query_object* querymanager_qidlist_get_by_qid(uint8_t qid) {
#ifdef DEBUG_QUERYMANAGER
	printf("querymanager_qidlist_get_by_qid()\n\r");
#endif //DEBUG_QUERYMANAGER
	struct qid_list_element *e = qid_list;

	while (e) {
		if (e->qo->qid == qid) {
			return e->qo;
		}
		e = e->next;
	}

	return NULL;
}

/*---------------------------------------------------------------------------*/
/* Is 1, if the parameter qid exists in the list.                            */
/* @param uint8_t table_id id of database                                    */
/* @return database* pointer to database, NULL if element wasnt found        */
int querymanager_qidlist_contains_qid(uint8_t qid) {
#ifdef DEBUG_QUERYMANAGER
	printf("querymanager_qidlist_contains_qid()\n\r");
#endif //DEBUG_QUERYMANAGER
	struct qid_list_element *e = qid_list;

	while (e) {
		if (e->qo->qid == qid) {
			return 1;
		}
		e = (struct qid_list_element*) e->next;
	}

	return 0;
}

/*---------------------------------------------------------------------------*/
/* Remove the pointer to the query object from list, if exists.              */
/* @param query_object* Pointer to the query object                          */
void querymanager_qidlist_remove(query_object *qo) {
#ifdef DEBUG_QUERYMANAGER
	printf("querymanager_qidlist_remove()\n\r");
#endif //DEBUG_QUERYMANAGER
	struct qid_list_element *e = qid_list;
	struct qid_list_element *prev = NULL;

	// search
	while (e) {
		// found
		if (e->qo == qo) {

			// remove from list
			// element is not head
			if (prev)
				prev->next = e->next;
			// element is head of list
			else
				qid_list = e->next;
			free(e);

			qid_list_size--;

			break;
		}
		prev = e;
		e = e->next;
	}

}

/*---------------------------------------------------------------------------*/
/* Returns the adress of the list.                                           */
/* @return see above.                                                        */
struct qid_list_element* querymanager_qidlist_get_list(void) {
#ifdef DEBUG_QUERYMANAGER
	printf("querymanager_qidlist_get_list()\n\r");
#endif //DEBUG_QUERYMANAGER
	return qid_list;
}

