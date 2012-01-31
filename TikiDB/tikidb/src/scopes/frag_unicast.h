/**
 * \defgroup frag_unicast Single-hop fragmentation-enabled unicast
 * @{
 *
 *
 * \section channels Channels
 *
 * The frag_unicast module uses 1 channel.
 *
 */

/**
 * \file
 *         Fragmentation-enabled unicast header file
 * \author
 *         Pablo Guerrero <guerrero@dvs.tu-darmstadt.de>
 */

#ifndef __FRAG_UNICAST_H__
#define __FRAG_UNICAST_H__

#include "net/rime.h"
#include "net/rime/unicast.h"
#include "dev/cc2420.h"

// MAX_FRAGMENTS defines the maximum number of fragments that this connection can use to fragment a large message
#define MAX_FRAGMENTS 8

// FRAGMENT_BITVECTOR_SIZE defines the size for the bitvector which checks received fragments
#define FRAGMENT_BITVECTOR_SIZE ((MAX_FRAGMENTS + 7) / 8)

// FRAGMENT_SIZE defines the size of a fragment, in bytes
#define FRAGMENT_SIZE (CC2420_MAX_PACKET_LEN - 27)
// BUFFER_SIZE defines the size of the buffer to be kept in local memory. It is the result of FRAGMENT_SIZE x MAX_FRAGMENTS
#define FRAGMENTATION_BUFFER_SIZE FRAGMENT_SIZE * MAX_FRAGMENTS

#define REASSEMBLY_TIMEOUT (5 * CLOCK_SECOND)

struct frag_unicast_conn;

/*
 * \brief Header for fragments
 */
struct fragment_hdr {
	uint8_t packet_id;
	uint8_t fragment_nr;
	uint16_t buffer_len;
};

struct frag_unicast_callbacks {
	void (* recv)(struct frag_unicast_conn *c, const rimeaddr_t *from);
};

enum frag_unicast_status {
	IDLE, SENDING, RECEIVING
};

enum {
	/**< The transmission was OK. */
	FRAG_SEND_OK,

	/**< The transmission had a problem sending one or more of the fragments. */
	FRAG_SEND_ERROR,

	/**< The specified message length was too long for this connection's buffer. */
	FRAG_MSG_TOO_LONG,

	/**< The fragmentation unicast connection was busy, either sending or receiving. */
	FRAG_CONNECTION_BUSY,
};

struct frag_unicast_conn {
	struct unicast_conn c;
	const struct frag_unicast_callbacks *u;
	uint8_t buffer[FRAGMENTATION_BUFFER_SIZE];
	uint16_t buffer_len;
	uint8_t last_received_packet_id;
	uint8_t last_sent_packet_id;
	rimeaddr_t from;
	uint8_t bitvector_received_fragments[FRAGMENT_BITVECTOR_SIZE];
	enum frag_unicast_status status;
	struct ctimer reassembly_timer;
};

void frag_unicast_open(struct frag_unicast_conn *c, uint16_t channel,
		const struct frag_unicast_callbacks *u);
void frag_unicast_close(struct frag_unicast_conn *c);

int frag_unicast_send(struct frag_unicast_conn *c, const rimeaddr_t *receiver);

uint8_t *frag_unicast_buffer_ptr(struct frag_unicast_conn *c);
uint16_t frag_unicast_buffer_getlen(struct frag_unicast_conn *c);
void frag_unicast_buffer_setlen(struct frag_unicast_conn *c, uint16_t len);
void frag_unicast_buffer_clear(struct frag_unicast_conn *c);

#endif /* __FRAG_UNICAST_H__ */
/** @} */
/** @} */
