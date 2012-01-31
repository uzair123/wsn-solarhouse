#ifndef QM_QIDLIST_H
#define QM_QIDLIST_H

#include <stdio.h>
#include <stdlib.h>

#include "../tikidb-conf.h"
#include "../debug.h"

#include "query-types.h"

struct qid_list_element {
    struct qid_list_element *next;
    query_object *qo;
} *qid_list;

uint8_t qid_list_size;

uint8_t querymanager_qidlist_init(void);
uint8_t querymanager_qidlist_add(query_object *qo);
query_object* querymanager_qidlist_get_by_qid(uint8_t qid);
int querymanager_qidlist_contains_qid(uint8_t qid);
void querymanager_qidlist_remove(query_object *qo);
struct qid_list_element* querymanager_qidlist_get_list(void);

#endif // querymanager_QIDLIST_H
