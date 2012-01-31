/*
 * Copyright (c) 2007, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 * $Id: example-rudolph2.c,v 1.6 2010/01/15 10:24:37 nifi Exp $
 */

/**
 * \file
 *         Testing the rudolph2 code in Rime
 * \author
 *         Adam Dunkels <adam@sics.se>
 */

#include "contiki.h"
#include "net/rime.h"
#include "net/rime/rudolph2.h"

#include "dev/button-sensor.h"
#include "dev/watchdog.h"
#include "lib/random.h"

#include "dev/leds.h"
#include "net/rime/trickle.h"
#include "loader/elfloader.h"

#include "cfs/cfs.h"
#include "math.h"
#include <stdio.h>
#include "shell.h"
#include "serial-shell.h"
#include "shell.c"
#include "serial-shell.c"
#include "distrib.h"
#include "stdlib.h"
#include "base64.h"
#include "base64.c"
#include "clock.c"
#include "node-id.h"


static void
unicast_recv(struct unicast_conn *c, const rimeaddr_t *from)
{
	
	 struct unicast_message *msg = packetbuf_dataptr();
	 #ifdef COMPILE_REAL
		swapEndianness(&msg->type);
		swapEndianness(&msg->chunk);		
	#endif
	 if(msg->type == TYPE_SUBSCRIBE) {
		 printf("subscription message received from %d.%d\n",
		from->u8[0], from->u8[1]);
		 if(status == STATUS_ANNOUNCE) {
			 static int fd;
			 fd = cfs_open("firmware", CFS_READ);
			 //num_chunks = (uint16_t) ceil(file_size(fd) / CHUNKSIZE);
			 uint16_t size = file_size(fd);
			 if(size%CHUNKSIZE == 0) 
				num_chunks = size/CHUNKSIZE;
			 else
				num_chunks = size/CHUNKSIZE + 1;
				
			 cur_chunk = 0;
			 interval = INTERVAL_MIN;
			 //etimer_set(&moap_timer, CLOCK_SECOND * interval);
			 status = STATUS_SEND;
		 }
	 }
	 else if(msg->type == TYPE_REQUEST) {
		 int fd = cfs_open("firmware", CFS_READ);
		 cfs_seek(fd, msg->chunk * CHUNKSIZE, CFS_SEEK_SET);
		 struct unicast_message answer;
		 answer.type = TYPE_DATA;
		 answer.chunk = msg->chunk;
		 cfs_read(fd, answer.data, CHUNKSIZE);
		 cfs_close(fd);
		 packetbuf_copyfrom(&answer, sizeof(struct unicast_message));
		 unicast_send(&unicast, from);
		 PRINTF("Sent missing chunk %d via unicast\n", msg->chunk);
	 }
	 else if(msg->type == TYPE_DATA && status == STATUS_UPDATE && memcmp(from, &update_source, sizeof(rimeaddr_t))==0) {
		 struct list_item* cur_item = (struct list_item*) list_head(missing_chunks);
		 while(cur_item->chunk != msg->chunk) {
			 if(cur_item->next == NULL) return;
			 cur_item = (struct list_item*) cur_item->next;
		 }
		 list_remove(missing_chunks, cur_item);
		 int fd = cfs_open("firmware", CFS_READ | CFS_WRITE);
		 cfs_seek(fd, msg->chunk * CHUNKSIZE, CFS_SEEK_SET);
		 cfs_write(fd, msg->data, CHUNKSIZE);
		 cfs_close(fd);
		 PRINTF("Received missing chunk %d via unicast\n", msg->chunk);
	 }
	
	
}
static const struct unicast_callbacks uni_call = {unicast_recv};


static void setVersion(uint16_t v) {
	version = v;
	int fd = cfs_open("version", CFS_WRITE);
	cfs_write(fd, &version, sizeof(uint16_t));
	cfs_close(fd);
}

static void swapEndianness(uint16_t *v) {
	static uint8_t offset_temp[2];
	memcpy(offset_temp, v, sizeof(uint8_t) * 2);
	*v = offset_temp[1] * 256 + offset_temp[0];
}

static void sendSubscription() {
	unicast_send(&unicast, &update_source);
	PRINTF("Subscribing to update\n"); 
	interval = INTERVAL_MIN;
	etimer_set(&moap_timer, CLOCK_SECOND * interval);
}

static void broadcast_recv(struct broadcast_conn *c, const rimeaddr_t *from) {
	struct broadcast_message *msg = packetbuf_dataptr();
	PRINT_ARR(3, packetbuf_dataptr(), packetbuf_datalen());
	PRINTF("Message type: %d\n", msg->type);
	#ifdef COMPILE_REAL
		swapEndianness(&msg->type);
	#endif
	
	if(msg->type == TYPE_PUBLISH) {
		#ifdef COMPILE_REAL
			swapEndianness(&msg->payload.publishmsg.version);
			swapEndianness(&msg->payload.publishmsg.chunks);
		#endif
		
		if(msg->payload.publishmsg.version > version && status != STATUS_UPDATE) {
			num_chunks = msg->payload.publishmsg.chunks;
			received_chunks = 0;
			clearMissingChunks();
			last_received = clock_time();
			recv_version = msg->payload.publishmsg.version;
			status = STATUS_UPDATE;
			cfs_remove("firmware");
			update_source = *from;
			struct unicast_message answer;
			answer.type = TYPE_SUBSCRIBE;
			packetbuf_copyfrom(&answer, sizeof(struct unicast_message));
			//clock_wait(node_id * 5);
			ctimer_set(&protocol_ctimer, (CLOCK_SECOND / 32) * node_id, &sendSubscription, NULL);
		}
	}
	else if(msg->type == TYPE_DATA && status == STATUS_UPDATE && memcmp(from, &update_source, sizeof(rimeaddr_t))==0) {
		//TODO: Sliding window to get missing chunks	
		uint16_t offset;
		#ifdef COMPILE_REAL
			swapEndianness(&msg->payload.datamsg.length);
			swapEndianness(&msg->payload.datamsg.offset);
			swapEndianness(&msg->payload.datamsg.version);
		#endif
			
		last_received = clock_time();
		PRINTF("Current time: %lu\n", (last_received/CLOCK_SECOND));	
		int fd = cfs_open("firmware", CFS_READ | CFS_WRITE);
		
		offset = msg->payload.datamsg.offset;
		PRINTF("Received chunk %d offset %d\n", (offset/CHUNKSIZE), offset);
		while(offset >= (received_chunks + 1) * CHUNKSIZE) {
			PRINTF("Missing chunk %d\n", received_chunks);
			if(list_length(missing_chunks) == MAX_MISSING) {
				//Abort update
				PRINTF("Update failed due to bad connection (too many packets missed)\n");
				status = STATUS_BOOT;
				break;
			}
			else {
				addMissingChunk(received_chunks);
				received_chunks++;
			}
		}
		cfs_seek(fd, offset, CFS_SEEK_SET);
		cfs_write(fd, msg->payload.datamsg.data, msg->payload.datamsg.length);
		cfs_close(fd);
		received_chunks++;
		if(num_chunks == received_chunks) {
			setVersion(recv_version);
			printf("Received new firmware - rebooting\n");
			watchdog_reboot();
		}
	}
}

static void dumpFirmware() {
	static int fd, count_print;
	static uint8_t buffer[32];
	
	printf("Dump of first 32 bytes: \n");
	
	fd = cfs_open("firmware", CFS_READ);
	cfs_read(fd, buffer, 32);
	
	for(count_print = 0; count_print<32; count_print++) {
		  if((*(buffer + count_print))==0)
		  printf("00");
		  else if ((*(buffer + count_print))<10)
		  printf("0%hX", (*(buffer + count_print)));
		  else
		  printf("%0hX", *(buffer + count_print));
	  }
	cfs_close(fd);
	printf("\n");  
}

static struct broadcast_callbacks br_call = {broadcast_recv};
static struct broadcast_conn broadcast;

static void addMissingChunk(uint16_t chunk_nr) {
	struct list_item *item = memb_alloc(&pool);
	item->chunk = chunk_nr;
	list_add(missing_chunks, item); 
}

static void clearMissingChunks() {
	while(list_length(missing_chunks) > 0) {
		list_remove(missing_chunks, list_head(missing_chunks));
	}
}



uint16_t file_size(int fd) {
	return cfs_seek(fd, 0, CFS_SEEK_END);
}



static void increase_interval() {
	uint16_t random_nr = random_rand();
	random_nr = random_nr % 10;
	interval = interval + random_nr;
	if(interval > INTERVAL_MAX) interval = INTERVAL_MAX;
	if(interval < INTERVAL_MIN) interval = INTERVAL_MIN;
}

static void send_chunk() {
	PRINTF("Sending chunk %d offset %d\n", cur_chunk, (cur_chunk * CHUNKSIZE));
	static struct broadcast_message msg;
	msg.type = TYPE_DATA;
	msg.payload.datamsg.version = version;
	uint16_t pointer = cur_chunk * CHUNKSIZE;
	msg.payload.datamsg.offset = pointer;
	static int fd;
	fd = cfs_open("firmware", CFS_READ);
	cfs_seek(fd, cur_chunk * CHUNKSIZE, CFS_SEEK_SET);
	int len = cfs_read(fd, msg.payload.datamsg.data, CHUNKSIZE);
	msg.payload.datamsg.length = len;
	cfs_close(fd);
	packetbuf_copyfrom(&msg, sizeof(struct broadcast_message)); 
	PRINT_ARR(3, packetbuf_dataptr(), packetbuf_datalen());
	broadcast_send(&broadcast);		
	cur_chunk++;
}
PROCESS(owndec64, "decode");
SHELL_COMMAND(dec64_command,
	      "decode",
	      "decode: decode base64 input",
	      &owndec64);
	      
PROCESS_THREAD(owndec64, ev, data) {
	PROCESS_BEGIN();
	int fd, fo;
	uint32_t size, pointer, out_pointer=0, decode_len;
	uint8_t buffer[5];
	uint8_t output_buffer[5];
	buffer[4] = 0;
	fd = cfs_open("encoded", CFS_READ);
	fo = cfs_open("firmware", CFS_WRITE);
	size = cfs_seek(fd, 0, CFS_SEEK_END);
	//printf("Encoded file size: %d\n", size);
	cfs_seek(fd, 0, CFS_SEEK_SET);
	
	for(pointer = 0; pointer < size; pointer = pointer + 4) {
		cfs_seek(fd, pointer, CFS_SEEK_SET);
		cfs_seek(fo, out_pointer, CFS_SEEK_SET);
		cfs_read(fd, buffer, 4);
		decode_len = base64_decode((char*) buffer, output_buffer, 3);
		cfs_write(fo, output_buffer, decode_len);		
		out_pointer += 3;
		//printf("File size: %d\n", file_size(fo));
	}
	//printf("Pointer: %d, out_pointer: %d\n", pointer, out_pointer);
	cfs_close(fd);
	cfs_close(fo);
	
	PROCESS_END();	
}

PROCESS_THREAD(moap_process, ev, data) {
	PROCESS_BEGIN();
	broadcast_open(&broadcast, 142, &br_call);
	unicast_open(&unicast, 143, &uni_call);
	update_finished = process_alloc_event();
	version = 1; //TODO: Correct version number
	interval = INTERVAL_MIN;
	memb_init(&pool);
	list_init(missing_chunks);

	while(1) {
		etimer_set(&moap_timer, CLOCK_SECOND * interval);
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&moap_timer));
		
		if(status == STATUS_ANNOUNCE) {
			struct broadcast_message msg;
			msg.type = TYPE_PUBLISH;
			msg.payload.publishmsg.version = version;
			int fd = cfs_open("firmware", CFS_READ);
			uint16_t size = file_size(fd);
			cfs_close(fd);
			 if(size%CHUNKSIZE == 0) 
				num_chunks = size/CHUNKSIZE;
			 else
				num_chunks = size/CHUNKSIZE + 1;
			msg.payload.publishmsg.chunks = num_chunks;
			packetbuf_copyfrom(&msg, sizeof(struct broadcast_message));
			increase_interval();
			PRINT_ARR(3, packetbuf_dataptr(), packetbuf_datalen());
			broadcast_send(&broadcast);
			PRINTF("Announced version %d chunks %d\n", version, num_chunks);
		}
		else if(status == STATUS_SEND) {
			send_chunk();
			if(cur_chunk >= num_chunks) {
				status = STATUS_ANNOUNCE;
				cur_chunk = 0;
				num_chunks = 0;
			}
		}
		else if(status == STATUS_UPDATE) { 
			//Get missing chunks and prevent timeout
			if(clock_time() - last_received > (CLOCK_SECOND * TIMEOUT)) {
				PRINTF("Update failed due to bad connection (timeout)\n");
				status = STATUS_BOOT;
				setVersion(0);
			}
			else if(list_length(missing_chunks) > 0) {
				//If we have missing chunks, request them
				struct unicast_message msg;
				struct list_item *cur_item = (struct list_item*) list_head(missing_chunks);
				msg.type = TYPE_REQUEST;
				msg.chunk = cur_item->chunk;
				packetbuf_copyfrom(&msg, sizeof(struct unicast_message));
				unicast_send(&unicast, &update_source);
				PRINTF("Requested missing chunk %d\n", msg.chunk);
			}
		}
	}
	
	PROCESS_END();
}


PROCESS_THREAD(update_notifier, ev, data) {
	PROCESS_BEGIN();
	//SENSORS_ACTIVATE(button_sensor);
	
	//while(1) {
	//	PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event &&
	//		     data == &button_sensor);
		setVersion(version + 1);
		PRINTF("Version set to %d\n", version);
		printf("Version set - rebooting\n");
		watchdog_reboot();
	//}
	
	
	PROCESS_END();
}


PROCESS_THREAD(shell_loader_process, ev, data)
{
  PROCESS_BEGIN();

  serial_shell_init();
  /*shell_blink_init();*/
  shell_file_init();
  shell_coffee_init();
  /*shell_ps_init();*/
  /*shell_reboot_init();*/
  /*shell_rime_init();*/
  /*shell_rime_netcmd_init();*/
  /*shell_rime_ping_init();*/
  /*shell_rime_debug_init();*/
  /*shell_rime_sniff_init();*/
  /*shell_sky_init();*/
  shell_text_init();
  /*shell_time_init();*/
  /*  shell_checkpoint_init();*/
  //shell_exec_init();
  //shell_base64_init();

  shell_register_command(&update_command);
  shell_register_command(&reset_command);
  shell_register_command(&dump_command);
  shell_register_command(&dec64_command);

  PROCESS_END();
}

PROCESS_THREAD(reset_process, ev, data) 
{
	PROCESS_BEGIN();
	cfs_remove("firmware");
	cfs_remove("version");
	printf("Reset - rebooting\n");
	watchdog_reboot();
	
	PROCESS_END();
}

PROCESS_THREAD(dump_process, ev, data)
{
	PROCESS_BEGIN();
	static int fd, count_print, size, pointer;
	static uint8_t buffer[32];
	printf("Dump of complete firmware: \n");
	
	fd = cfs_open("firmware", CFS_READ);
	size = file_size(fd);
	for(pointer = 0; pointer <size; pointer += 32) {
		cfs_seek(fd, pointer, CFS_SEEK_SET);
		cfs_read(fd, buffer, 32);
		for(count_print = 0; count_print<32; count_print++) {
			  if((*(buffer + count_print))==0)
			  printf("00");
			  else if ((*(buffer + count_print))<16)
			  printf("0%hX", (*(buffer + count_print)));
			  else
			  printf("%0hX", *(buffer + count_print));
		}
		printf("\n"); 
	}
	cfs_close(fd);
	printf("\n"); 
	
	PROCESS_END();	
}
	
	

PROCESS_THREAD(elfloader_process, ev, data)
{
	static int fd, errNo;
	PROCESS_BEGIN();
	status = STATUS_BOOT;
	
	void *bla = malloc(4);
	free(bla);
	
	//PRINTF("Hello\n");
	fd = cfs_open("version", CFS_READ);
	 if(fd!=-1) {
	  cfs_read(fd, &version, sizeof(uint16_t));
	  }
	  else {
		  version = 0; 
	  }
	  cfs_close(fd);
	  printf("Current version: %u\n", version);

	//rudi_finished = process_alloc_event();
	//update_available = process_alloc_event();
	
	//dumpFirmware();
	fd = cfs_open("firmware", CFS_READ | CFS_WRITE);
	//PRINTF("File handle %d, file size %d\n", fd, file_size(fd));
	cfs_seek(fd, 0, CFS_SEEK_SET);
	PRINTF("Loading ELF-file\n");
	errNo = elfloader_load(fd);
	if(errNo == 0) {
		PRINTF("Found firmware\n");
		autostart_start(elfloader_autostart_processes);
		printf("Firmware loaded\n");
		cfs_close(fd);
		status = STATUS_ANNOUNCE;
	}
	else {
		printf("Couldn't load firmware or no firmware found!\n");
		printf("Error code: %d\n", errNo);
		PRINTF("Missing symbols: %s\n", elfloader_unknown);
		cfs_close(fd);
		cfs_remove("firmware");
		version = 0;
		//PROCESS_WAIT_EVENT_UNTIL(ev == rudi_finished);
		//printf("Firmware is updated!\n");
		dumpFirmware();
				
		
	}
		
	//PROCESS_WAIT_EVENT_UNTIL(ev == update_available);
	//PROCESS_WAIT_EVENT_UNTIL(ev == rudi_finished);
	//watchdog_reboot();
	
		
	

PROCESS_END();
}





