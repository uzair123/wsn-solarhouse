/**
 * \file
 *         Fragmentation-enabled unicast
 * \author
 *         Pablo Guerrero <guerrero@dvs.tu-darmstadt.de>
 */

#include "frag_unicast.h"
#include "net/rime.h"
#include "scopes-types.h"


#define MIN(a,b) ((a)>(b)?(b):(a))

/*---------------------------------------------------------------------------*/
void drop_fragments();
/*---------------------------------------------------------------------------*/
void bitvector_set_received(uint8_t *bitvector_received_fragments,
		uint8_t fragment_nr);
/*---------------------------------------------------------------------------*/
int bitvector_get_received(uint8_t *bitvector_received_fragments,
		uint8_t fragment_nr);
/*---------------------------------------------------------------------------*/
int bitvector_all_received(uint8_t *bitvector_received_fragments,
		uint8_t num_fragments);
/*---------------------------------------------------------------------------*/
void bitvector_clear(uint8_t *bitvector_received_fragments);
/*---------------------------------------------------------------------------*/
static void recv_from_unicast(struct unicast_conn *unicast,
		const rimeaddr_t *from) {

	uint8_t data[sizeof(struct fragment_hdr) + FRAGMENT_SIZE];
	uint16_t payload_len = packetbuf_copyto(&data)
			- sizeof(struct fragment_hdr);

	struct fragment_hdr *hdr = (struct fragment_hdr *) data;
	uint8_t *payload = data + sizeof(struct fragment_hdr);

	struct frag_unicast_conn *c = (struct frag_unicast_conn *) unicast;
	//	PRINTF("[%u.%u:%lu] frag_unicast::recv_from_unicast() : From [%u.%u] \n",
	//			rimeaddr_node_addr.u8[1],rimeaddr_node_addr.u8[0], clock_time());

	uint8_t received_packet_id = hdr->packet_id;
	uint8_t fragment_nr = hdr->fragment_nr;
	uint16_t buffer_len = hdr->buffer_len;
	uint8_t num_fragments = (buffer_len + FRAGMENT_SIZE - 1) / FRAGMENT_SIZE;
	PRINTF(3,"[%u.%u:%lu] frag_unicast::recv_from_unicast() : From [%u.%u], last_received_packet_id: %u, received_packet_id: %u, fragment_nr: %u, num_fragments: %u\n",
			rimeaddr_node_addr.u8[1],rimeaddr_node_addr.u8[0], clock_time(),
			from->u8[1], from->u8[0],
			c->last_received_packet_id, received_packet_id, fragment_nr, num_fragments);

#if DEBUG>=3
	PRINTF(3,"[%u.%u:%lu] frag_unicast::recv_from_unicast() : Status is [",
			rimeaddr_node_addr.u8[1],rimeaddr_node_addr.u8[0], clock_time());
	switch (c->status) {
		case (IDLE): {
			PRINTF(3,"IDLE");
			break;
		}
		case (SENDING): {
			PRINTF(3,"SENDING");
			break;
		}
		case (RECEIVING): {
			PRINTF(3,"RECEIVING");
			break;
		}
	}
	PRINTF(3,"]\n");
#endif

	if ((c->status == IDLE) || // either this node was idle, or
			((c->status == RECEIVING) && //
					(rimeaddr_cmp(&c->from, from)) && //
					(received_packet_id == c->last_received_packet_id))) { // it was already receiving from that sender node

		c->status = RECEIVING;

		PRINTF(3,"[%u.%u:%lu] frag_unicast::recv_from_unicast() : Status is correct\n",
				rimeaddr_node_addr.u8[1],rimeaddr_node_addr.u8[0], clock_time());

		if (fragment_nr == 0) {

			rimeaddr_copy(&c->from, from);
			//			PRINTF(3,"[%u.%u:%lu] frag_unicast::recv_from_unicast() : c->from is now [%u.%u]\n",
			//					rimeaddr_node_addr.u8[1], rimeaddr_node_addr.u8[0], clock_time(), c->from.u8[1], c->from.u8[0]);
			//			PRINTF(3,"[%u.%u:%lu] frag_unicast::recv_from_unicast() : Before bitvector_clear, NULL timer is expired: %u\n",
			//					rimeaddr_node_addr.u8[1], rimeaddr_node_addr.u8[0], clock_time(), ctimer_expired(NULL));
			bitvector_clear(c->bitvector_received_fragments);
			//			PRINTF(3,"[%u.%u:%lu] frag_unicast::recv_from_unicast() : After bitvector_clear, NULL timer is expired: %u\n",
			//					rimeaddr_node_addr.u8[1], rimeaddr_node_addr.u8[0], clock_time(), ctimer_expired(NULL));
			c->last_received_packet_id = received_packet_id;
			c->buffer_len = buffer_len;
			PRINTF(2,"[%u.%u:%lu] frag_unicast::recv_from_unicast() : Setting timer\n",
					rimeaddr_node_addr.u8[1],rimeaddr_node_addr.u8[0], clock_time());
			ctimer_set(&c->reassembly_timer, REASSEMBLY_TIMEOUT,
					drop_fragments, c);
		} else {
			PRINTF(2,"[%u.%u:%lu] frag_unicast::recv_from_unicast() : Resetting timer\n",
					rimeaddr_node_addr.u8[1],rimeaddr_node_addr.u8[0], clock_time());
			ctimer_restart(&c->reassembly_timer);
		}

		//		PRINTF(3,"[%u.%u:%lu] frag_unicast::recv_from_unicast() : Before bitvector_set_received, NULL timer is expired: %u\n",
		//				rimeaddr_node_addr.u8[1], rimeaddr_node_addr.u8[0], clock_time(), ctimer_expired(NULL));
		bitvector_set_received(c->bitvector_received_fragments, fragment_nr);
		//		PRINTF(3,"[%u.%u:%lu] frag_unicast::recv_from_unicast() : After bitvector_set_received, NULL timer is expired: %u\n",
		//				rimeaddr_node_addr.u8[1], rimeaddr_node_addr.u8[0], clock_time(), ctimer_expired(NULL));

		uint8_t *buffer_ptr = c->buffer + fragment_nr * FRAGMENT_SIZE;

		//		PRINTF(3,"BEFORE memcpy (len: %u, [%s])\n", len, data);
		memcpy(buffer_ptr, payload, payload_len);
		//		PRINTF(3,"DONE memcpy.\n");

		if (bitvector_all_received(c->bitvector_received_fragments,
				num_fragments)) {
			ctimer_stop(&c->reassembly_timer);
			PRINTF(2,"[%u.%u:%lu] frag_unicast::recv_from_unicast() : Stopping timer\n",
					rimeaddr_node_addr.u8[1],rimeaddr_node_addr.u8[0], clock_time());
			//			PRINTF(3,"[%u.%u:%lu] frag_unicast::recv_from_unicast() : After bitvector_all_received and before passing control to recv, NULL timer is expired: %u\n",
			//					rimeaddr_node_addr.u8[1], rimeaddr_node_addr.u8[0], clock_time(), ctimer_expired(NULL));
			c->status = IDLE;
			if (c->u->recv != NULL)
				c->u->recv(c, from);
		}

	} else {
		if (c->status == SENDING) {
			PRINTF(2,
					"[%u.%u:%lu] frag_unicast::recv_from_unicast() node was already sending, packet discarded!\n",
					rimeaddr_node_addr.u8[1], rimeaddr_node_addr.u8[0], clock_time());
		} else if (!rimeaddr_cmp(&c->from, from)) {
			PRINTF(2,
					"[%u.%u:%lu] frag_unicast::recv_from_unicast() node was already receiving fragments from node [%u.%u], packet discarded!\n",
					rimeaddr_node_addr.u8[1], rimeaddr_node_addr.u8[0], clock_time(), c->from.u8[1], c->from.u8[0]);
			return;

		}

	}

	PRINTF(3,"[%u.%u:%lu] frag_unicast::recv_from_unicast() : END\n",
			rimeaddr_node_addr.u8[1], rimeaddr_node_addr.u8[0], clock_time());
	//	PRINTF(3,"[%u.%u:%lu] frag_unicast::recv_from_unicast() : Status: Timer is expired: %u\n",
	//			rimeaddr_node_addr.u8[1], rimeaddr_node_addr.u8[0], clock_time(), ctimer_expired(NULL));

}
/*---------------------------------------------------------------------------*/
static const struct unicast_callbacks unicast_cbs = { recv_from_unicast };
/*---------------------------------------------------------------------------*/
void frag_unicast_open(struct frag_unicast_conn *c, uint16_t channel,
		const struct frag_unicast_callbacks *u) {
	unicast_open(&c->c, channel, &unicast_cbs);
	c->last_sent_packet_id = 0;
	c->status = IDLE;
	c->u = u;

	//	PRINTF(2, "Size of bitvector_received_fragments is %u\n", sizeof(c->bitvector_received_fragments));
}
/*---------------------------------------------------------------------------*/
void frag_unicast_close(struct frag_unicast_conn *c) {
	unicast_close(&c->c);
}
/*---------------------------------------------------------------------------*/
int frag_unicast_send(struct frag_unicast_conn *c, const rimeaddr_t *receiver) {

	PRINTF(3,"[%u.%u:%lu] frag_unicast::frag_unicast_send() : Invoked with receiver [%u.%u], message: ",
			rimeaddr_node_addr.u8[1], rimeaddr_node_addr.u8[0], clock_time(), receiver->u8[1], receiver->u8[0]);
	PRINT_ARR(3, c->buffer, c->buffer_len);

	if (c->status == IDLE) {

		if (c->buffer_len < FRAGMENTATION_BUFFER_SIZE) {

			c->status = SENDING;

//			// copy data into this connection's fragment buffer:
//			memcpy(c->buffer, msg, msg_len);

			//#if DEBUG>0
			//			c_msg = (char*) c->buffer;
			//			PRINTF(3,"in buffer: [");
			//			int x = 0;
			//			for (x = 0; x < msg_len; x++)
			//				PRINTF(3,"%c",c_msg[x]);
			//			PRINTF(3,"]");
			//			PRINTF(3,"\n");
			//#endif

			uint16_t remaining_len = c->buffer_len;
			void *current_buffer_fragment = c->buffer;
			uint8_t fragment[sizeof(struct fragment_hdr) + FRAGMENT_SIZE];

			c->last_sent_packet_id++;
			int result = 1;

			/* now we calculate how many fragments are necessary.
			 * E.g. assuming FRAGMENT_SIZE = 3:
			 * buffer_len:	num_fragments:
			 * 1				1
			 * 2				1
			 * 3				1
			 * 4				2
			 * 5				2
			 * 6				2
			 * 7				3
			 * ...			...
			 * */
			uint8_t num_fragments = (c->buffer_len + FRAGMENT_SIZE - 1)
					/ FRAGMENT_SIZE;

			PRINTF(3,"[%u.%u:%lu] frag_unicast::frag_unicast_send() : Number of fragments to be sent: %u\n",
					rimeaddr_node_addr.u8[1], rimeaddr_node_addr.u8[0], clock_time(), num_fragments);

			uint8_t fragment_nr;
			for (fragment_nr = 0; fragment_nr < num_fragments; fragment_nr++) {

//				if (fragment_nr > 0)
//					printf("frag_unicast - sending fragment nr %u\n", fragment_nr);

				// prepare header:
				struct fragment_hdr *hdr = (struct fragment_hdr *) fragment;
				hdr->packet_id = c->last_sent_packet_id;
				hdr->fragment_nr = fragment_nr;
				hdr->buffer_len = c->buffer_len;

				// prepare payload:
				uint8_t *payload = ((uint8_t *) fragment)
						+ sizeof(struct fragment_hdr);
				memcpy(payload, current_buffer_fragment,
						MIN(remaining_len, FRAGMENT_SIZE));

				PRINTF(2,"[%u.%u:%lu] frag_unicast::frag_unicast_send() : Sending with last_sent_packet_id %u, fragment nr %u, num_fragments %u, to [%u.%u], len will be %u\n",
						rimeaddr_node_addr.u8[1], rimeaddr_node_addr.u8[0], clock_time(),
						c->last_sent_packet_id, fragment_nr, num_fragments, receiver->u8[1], receiver->u8[0], MIN(remaining_len, FRAGMENT_SIZE));

				packetbuf_copyfrom(fragment, sizeof(struct fragment_hdr)
						+ MIN(remaining_len, FRAGMENT_SIZE));
				//				PRINTF(1,"DONE packetbuf_copyfrom.\n");

				//				packetbuf_reference(current_buffer_fragment,
				//						MIN(remaining_len, FRAGMENT_SIZE));
				//				PRINTF(1,"DONE packetbuf_reference.\n");

				PRINTF(3,"[%u.%u:%lu] frag_unicast::frag_unicast_send() : dataptr's contents: ",
						rimeaddr_node_addr.u8[1], rimeaddr_node_addr.u8[0], clock_time());
				PRINT_ARR(3, packetbuf_dataptr(), packetbuf_datalen());

				//				PRINTF(1,"DONE packetbuf_set_attr.\n");

				result &= unicast_send(&c->c, receiver);
				//				PRINTF(1,"DONE unicast_send.\n");

				// advance pointer to data
				current_buffer_fragment += MIN(remaining_len, FRAGMENT_SIZE);
				// adjust remaining length
				remaining_len -= MIN(remaining_len, FRAGMENT_SIZE);

			}

			//			PRINTF(1,"DONE.\n");
			c->status = IDLE;
			if (result)
				return FRAG_SEND_OK;
			else
				return FRAG_SEND_ERROR;

		} else
			return FRAG_MSG_TOO_LONG;
	} else
		return FRAG_CONNECTION_BUSY;
}
/*---------------------------------------------------------------------------*/
void bitvector_set_received(uint8_t *bitvector_received_fragments,
		uint8_t fragment_nr) {
	// first, a byte is created with the correct bit set:
	uint8_t shifted_bit = 1 << (fragment_nr % 8);
	//	PRINTF(1,"Shifted bit is now %u\n", shifted_bit);

	// second, the correct byte is OR'ed with 'shifted_bit'
	//	PRINTF(1,"bitvector[%u] was %u\n", fragment_nr / 8, bitvector_received_fragments[fragment_nr/8]);
	bitvector_received_fragments[fragment_nr / 8] |= shifted_bit;
	//	PRINTF(1,"bitvector[%u] is now %u\n", fragment_nr / 8, bitvector_received_fragments[fragment_nr/8]);
}
/*---------------------------------------------------------------------------*/
int bitvector_get_received(uint8_t *bitvector_received_fragments,
		uint8_t fragment_nr) {
	// first, a byte is created with the correct bit set:
	uint8_t shifted_bit = 1 << (fragment_nr % 8);

	//	PRINTF(1,"bitvector_get_received : shifted_bit is %u\n", shifted_bit);

	// second, the correct byte is OR'ed with 'shifted_bit'
	return bitvector_received_fragments[fragment_nr / 8] | shifted_bit;
}
/*---------------------------------------------------------------------------*/
int bitvector_all_received(uint8_t *bitvector_received_fragments,
		uint8_t num_fragments) {

	//	PRINTF(1,"bitvector_all_received : num_fragments is %u\n", num_fragments);

	// we need to check if there is a 1 on each bit in the bitvector
	uint8_t one = 1;
	uint8_t result = 1;
	uint8_t fragment_nr = 0;
	while ((fragment_nr < num_fragments) && (result)) {
		//		PRINTF(1,"with fragment_nr %u, result is %u, bitvector_received_fragments[%u] is %u\n", fragment_nr, result, fragment_nr / 8, bitvector_received_fragments[fragment_nr / 8]);
		result &= (bitvector_received_fragments[fragment_nr / 8] & one) != 0;
		//		PRINTF(1,"after evaluating fragment_nr %u, result is %u\n", fragment_nr, result);
		fragment_nr++;
		one <<= 1;
		if (one == 0)
			one = 1;
		//		PRINTF(1,"after updating, one is %u\n", one);
	}

	return result;
}
/*---------------------------------------------------------------------------*/
void bitvector_clear(uint8_t *bitvector_received_fragments) {

	uint8_t fragment_nr;
	//	PRINTF(1, "clearing %u fragments\n", FRAGMENT_BITVECTOR_SIZE);
	for (fragment_nr = 0; fragment_nr < FRAGMENT_BITVECTOR_SIZE; fragment_nr++) {
		//		PRINTF(1, "clearing fragment nr %i\n", fragment_nr);
		bitvector_received_fragments[fragment_nr] = 0;
	}
}
/*---------------------------------------------------------------------------*/
void drop_fragments(void *arg) {

	PRINTF(2,"[%u.%u:%lu] frag_unicast::drop_fragments() : Dropping fragments due to expired timer!!\n",
			rimeaddr_node_addr.u8[1], rimeaddr_node_addr.u8[0], clock_time());

	struct frag_unicast_conn *c = (struct frag_unicast_conn *) arg;

	c->buffer_len = 0;
	rimeaddr_copy(&c->from, &rimeaddr_null);
	bitvector_clear(c->bitvector_received_fragments);
	c->status = IDLE;

}
/*---------------------------------------------------------------------------*/
uint8_t *frag_unicast_buffer_ptr(struct frag_unicast_conn *c) {
	PRINTF(3,"returning pointer to fragmentation buffer\n");
	return c->buffer;
}
/*---------------------------------------------------------------------------*/
uint16_t frag_unicast_buffer_getlen(struct frag_unicast_conn *c){
	return c->buffer_len;
}
/*---------------------------------------------------------------------------*/
void frag_unicast_buffer_setlen(struct frag_unicast_conn *c, uint16_t len){
	PRINTF(3,"frag_unicast_buffer_setlen() : setting length to %u\n", len);
	c->buffer_len = len;
}
/*---------------------------------------------------------------------------*/
void frag_unicast_buffer_clear(struct frag_unicast_conn *c){
	PRINTF(3,"clearing buffer\n");
	int i;
	for (i=0;i<FRAGMENTATION_BUFFER_SIZE;i++) c->buffer[i] = 0; c->buffer_len = 0;
}
/*---------------------------------------------------------------------------*/
/** @} */
