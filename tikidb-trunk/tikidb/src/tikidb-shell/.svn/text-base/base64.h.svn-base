#ifndef __BASE64_H__
#define __BASE64_H__

#include "contiki.h"

#define BASE64_MAX_LINELEN 76

/**
 * \file
 *         Interface to the Base64 decoding function. For more info visit: http://en.wikipedia.org/wiki/Base64
 * \author
 *         Pablo Guerrero <guerrer@dvs.tu-darmstadt.de>
 *         Adam Dunkels <adam@sics.se>         
 */

/**
 * \brief           Decodes ASCII input in Base64 format into its binary equivalent.
 * \param input     A pointer to the input to be decoded
 * \param output    A pointer to the memory space where output will be placed
 * \param ouput_len The maximum length that the output can occupy
 * \retval          The size of the decoded output, in bytes, or 0 if nothing was decoded.
 *
 *                  This function decodes ASCII input in Base64 format into its binary equivalent. 
 *                  Note that given an input of n bytes, the size of the output is around n * 4 / 3
 *                  (or 1.33333n for large n). Before the actual decoding takes place, the function
 *                  will calculate the potential output length for the string passed as parameter,
 *                  and will return if the provided output space is not sufficient.
 */
uint8_t base64_decode(char *input, uint8_t *output, uint8_t output_len);

#endif /* __BASE64_H__ */
