#ifndef TEST_H
#define TEST_H

#include <stdio.h>
#include "contiki.h"
#include "dev/button-sensor.h"

#include "../debug.h"
#include "../tikidb.h"

void test_result_callback(char* resultdump);
void test_error_callback(char* msg);

#endif // TEST_H
