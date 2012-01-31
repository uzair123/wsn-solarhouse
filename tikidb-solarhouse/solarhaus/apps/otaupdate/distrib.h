#ifndef MOAP_DISTRIB_H
	#define MOAP_DISTRIB_H
	#define CHUNKSIZE 64
	#define INTERVAL_MIN 2
	#define INTERVAL_MAX 60

	#define TYPE_PUBLISH 0
	#define TYPE_DATA 1
	#define TYPE_SUBSCRIBE 2
	#define TYPE_REQUEST 3 //Request for a missing chunk
	#define MAX_MISSING 10 //Maximum number of chunks which may be missed
						   //at the same time
	#define TIMEOUT 100    //Maximum waiting time in seconds for a new packet when updating
	static struct etimer et;
	#define DELAY(X) etimer_set(&et, CLOCK_SECOND*(X)); PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

	#define DEBUG 0
	#if DEBUG
	#include <stdio.h>
	#define PRINTF(...) printf(__VA_ARGS__)
	#else
	#define PRINTF(...)
	#endif

	#if DEBUG
	#include <stdio.h>
	#define PRINTFL(x, ...)     if (x<=DEBUG) printf(__VA_ARGS__)
	#define PRINT_ARR(x, data, length) \
			 uint8_t *data2 = data;      \
			 PRINTFL(x,"[");        \
			 int XX = 0;         \
			 for (XX = 0; XX < length; XX++)    \
			  PRINTFL(x,"%u,",data2[XX]);    \
			 PRINTFL(x,"]\n")
	#else
	#define PRINTFL(x, ...)
	#define PRINT_ARR(x, data, length)
	#endif
	
	//#define COMPILE_COOJA
	#define COMPILE_REAL
	
	enum status_vars {STATUS_ANNOUNCE, STATUS_WAIT, STATUS_SEND, STATUS_BOOT, STATUS_UPDATE};
	static struct etimer moap_timer;
	static process_event_t update_finished;
	static uint8_t interval;
	static uint16_t version;
	static uint16_t recv_version;
	static uint16_t num_chunks;
	static uint16_t cur_chunk;
	static uint16_t received_chunks;
	static rimeaddr_t update_source;
	static uint32_t last_received;
	static struct unicast_conn unicast;
	
	struct publish_message {
		uint16_t version;
		uint16_t chunks;	
	};

	struct data_message {
		uint16_t offset;
		uint16_t version;
		uint16_t length;
		uint8_t data[CHUNKSIZE];
	};

	union message_payload {
		struct data_message datamsg;
		struct publish_message publishmsg;
	};

	struct broadcast_message {
		uint16_t type;
		union message_payload payload;
	};

	struct unicast_message {
		uint16_t type;
		uint16_t chunk; //Only used if type == TYPE_REQUEST
		uint8_t data[CHUNKSIZE]; //Only used if type == TYPE_DATA
	};
	
	struct list_item { struct list_item *next;
	uint16_t chunk; };
	MEMB(pool, struct list_item, 10);
	LIST(missing_chunks);


	static enum status_vars status;

	uint16_t file_size(int);
	static void addMissingChunk(uint16_t);
	static void clearMissingChunks();
	static void swapEndianness(uint16_t*);
	static struct ctimer protocol_ctimer;
	
	PROCESS(moap_process, "MOAP process");
	PROCESS(update_notifier, "Update notifier");
	PROCESS(elfloader_process, "Firmware loader");
	PROCESS(shell_loader_process, "Interactive shell");
	PROCESS(dump_process, "Firmware dump");
	PROCESS(reset_process, "Reset mote");
	AUTOSTART_PROCESSES(&moap_process, &elfloader_process, &shell_loader_process);
	//AUTOSTART_PROCESSES(&elfloader_process, &shell_loader_process);
	
	//AUTOSTART_PROCESSES(&shell_loader_process);
SHELL_COMMAND(update_command,
	      "update",
	      "update: increase version number and announce file",
	      &update_notifier);
SHELL_COMMAND(reset_command,
"reset",
"reset: delete firmware and version information",
&reset_process);

SHELL_COMMAND(dump_command,
"dump",
"dump: outputs the firmware in hex format",
&dump_process);
	
#endif
