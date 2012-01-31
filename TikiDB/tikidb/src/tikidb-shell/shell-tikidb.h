#ifndef _SHELL_TIKIDB_H_
#define _SHELL_TIKIDB_H_

#include "contiki.h"
#include "contiki-net.h"

#include "shell.h"

#include "tikidb.h"

#include "scopes-membership-simple.h"
#include "scopes-repository.h"

//#define MAX_QUERIES 5
#define SCOPE_ID 5

//// sample period is 1-minute==60 seconds
//#define SAMPLE_PERIOD_VALUE_L (CLOCK_SECOND*60)
//#define SAMPLE_PERIOD_VALUE_1 (SAMPLE_PERIOD_VALUE_L/(256UL*256*256))%256
//#define SAMPLE_PERIOD_VALUE_2 (SAMPLE_PERIOD_VALUE_L/(256UL*256))%256
//#define SAMPLE_PERIOD_VALUE_3 (SAMPLE_PERIOD_VALUE_L/256)%256
//#define SAMPLE_PERIOD_VALUE_4 (SAMPLE_PERIOD_VALUE_L)%256
//
//// lifetime is 5*256+160 samples == 1440 samples == 1440 * 60seconds == 1440* 1 minutes == 24*60 minutes == 24hours
//#define LIFETIME_VALUE_L ((CLOCK_SECOND*60)*60*24)				//        11,059,200
//#define LIFETIME_VALUE_1 (LIFETIME_VALUE_L/(256UL*256*256))%256	//   0 ==          0
//#define LIFETIME_VALUE_2 (LIFETIME_VALUE_L/(256*256UL))%256		// 168 == 11,010,048
//#define LIFETIME_VALUE_3 (LIFETIME_VALUE_L/256)%256				// 192 ==     49,152
//#define LIFETIME_VALUE_4 (LIFETIME_VALUE_L)%256					//   0 ==          0

// shell-tikidb functions
void shell_tikidb_init(void);

//tikidb callback
void cb_shell_tikidb_result(uint8_t* resultset, select_statement_t *ss);

#endif // _SHELL_TIKIDB_H_
