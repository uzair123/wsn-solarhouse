/*
 * base64.h
 *
 *  Created on: Jul 28, 2010
 *      Author: guerrero
 */

#ifndef __BASE64_H__
#define __BASE64_H__

#include "contiki.h"

#define BASE64_MAX_LINELEN 76

struct base64_decoder_state {
//  uint8_t data[3 * BASE64_MAX_LINELEN / 4];
  int dataptr;
  unsigned long tmpdata;
  int sextets;
  int padding;
};

/*---------------------------------------------------------------------------*/
uint8_t base64_decode(char *input, uint8_t *output, uint8_t output_len);
/*---------------------------------------------------------------------------*/
#endif /* __BASE64_H__ */
