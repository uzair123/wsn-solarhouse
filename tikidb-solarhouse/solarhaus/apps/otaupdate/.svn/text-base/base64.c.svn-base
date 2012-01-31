/*
 * base64.c
 *
 *  Created on: Jul 28, 2010
 *      Author: guerrero
 */
#include "base64.h"
#include <ctype.h>

#include <stdio.h>
struct base64_decoder_state s;

/*---------------------------------------------------------------------------*/
static int base64_decode_char(char c) {
	if (c >= 'A' && c <= 'Z') {
		return c - 'A';
	} else if (c >= 'a' && c <= 'z') {
		return c - 'a' + 26;
	} else if (c >= '0' && c <= '9') {
		return c - '0' + 52;
	} else if (c == '+') {
		return 62;
	} else if (c == '/') {
		return 63;
	} else {
		return 0;
	}
}
/*---------------------------------------------------------------------------*/
static int base64_add_char(struct base64_decoder_state *s, char c, uint8_t *output) {
	if (isspace(c)) {
		return 0;
	}

//	if (s->dataptr >= sizeof(s->data)) {
//		return 0;
//	}
	if (c == '=') {
		++s->padding;
	}

	s->tmpdata = (s->tmpdata << 6) | base64_decode_char(c);
	//printf("char: %c, sextet %u: %d\n", c, s->sextets, base64_decode_char(c));
	++s->sextets;
	if (s->sextets == 4) {
		s->sextets = 0;
		output[s->dataptr] = (uint8_t) (s->tmpdata >> 16);
		output[s->dataptr + 1] = (uint8_t) (s->tmpdata >> 8);
		output[s->dataptr + 2] = (uint8_t) (s->tmpdata);
		//printf("4 sextets decoded: [%u][%u][%u]\n", output[s->dataptr],
		//		output[s->dataptr + 1], output[s->dataptr + 2]);
		s->dataptr += 3;
		return 3-s->padding;
	}
	return 0;
}
/*---------------------------------------------------------------------------*/
uint8_t base64_decode(char *input, uint8_t *output, uint8_t output_len) {

	if (output_len >= strlen(input) * 3 / 4) {
		int i; int decoded_len = 0;

		s.sextets = s.dataptr = s.padding = 0;

		for (i = 0; i < strlen(input); ++i) {
			decoded_len += base64_add_char(&s, input[i], output);
		}

		return decoded_len;
	} else
		return 0;
}
