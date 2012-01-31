/**
 * \file
 *         Interface to the Base64 decoding function.
 * \author
 *         Pablo Guerrero <guerrer@dvs.tu-darmstadt.de>
 *         Adam Dunkels <adam@sics.se>         
 */


#include "base64.h"
#include <ctype.h> // for isspace()

#define DEBUG 0
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

/*---------------------------------------------------------------------------*/
struct base64_decoder_state {
  int dataptr;
  unsigned long tmpdata;
  int sextets;
  int padding;
} s;

/*-----------------------------------------------------------------------------------*/
/** 
 * \internal
 * Helper function to decode a single character using the Base64 index table.
 */
/*-----------------------------------------------------------------------------------*/
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
/*-----------------------------------------------------------------------------------*/
/** 
 * \internal
 * Helper function that accumulates sextets
 */
/*-----------------------------------------------------------------------------------*/
static int base64_add_char(struct base64_decoder_state *s, char c, uint8_t *output) {
  if (isspace(c)) {
    return 0;
  }

  if (c == '=') {
    ++s->padding;
  }

  s->tmpdata = (s->tmpdata << 6) | base64_decode_char(c);
  PRINTF("char: %c, sextet %u: %d\n", c, s->sextets, base64_decode_char(c));
  ++s->sextets;
  if (s->sextets == 4) {
    s->sextets = 0;
    output[s->dataptr]     = (uint8_t) (s->tmpdata >> 16);
    output[s->dataptr + 1] = (uint8_t) (s->tmpdata >> 8);
    output[s->dataptr + 2] = (uint8_t) (s->tmpdata);
    PRINTF("4 sextets decoded: [%u][%u][%u]\n", 
      output[s->dataptr], 
      output[s->dataptr + 1], 
      output[s->dataptr + 2]);
    s->dataptr += 3;
    return 3-(s->padding);
  }
  return 0;
}
/*---------------------------------------------------------------------------*/
/**
 * \brief           Decodes ASCII input in Base64 format into its binary equivalent.
 *
 * \param input     A pointer to the input to be decoded
 * \param output    A pointer to the memory space where output will be placed
 * \param max_output_len
 * 					The maximum length that the output can occupy
 * \return          The size of the decoded output, in bytes, or 0 if nothing was decoded.
 *
 *                  This function decodes ASCII input in Base64 format into its binary equivalent. 
 *                  Note that given an input of n bytes, the size of the output is around n * 4 / 3
 *                  (or 1.33333n for large n). Before the actual decoding takes place, the function
 *                  will calculate the potential output length for the string passed as parameter,
 *                  and will return if the provided output space is not sufficient.
 */
uint8_t base64_decode(char *input, uint8_t *output, uint8_t max_output_len) {

  // Check if the specified output length suffices:
  if (max_output_len >= strlen(input) * 3 / 4) {
    int i; int decoded_len = 0;

    // Initialize state
    s.sextets = s.dataptr = s.padding = 0;

    for (i = 0; i < strlen(input); ++i) {
      decoded_len += base64_add_char(&s, input[i], output);
    }

    return decoded_len;
  } else
    return 0;
}
/*---------------------------------------------------------------------------*/
