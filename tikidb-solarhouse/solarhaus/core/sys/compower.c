/**
 * \addtogroup compower
 * @{
 */

/*
 * Copyright (c) 2009, Swedish Institute of Computer Science.
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
 * $Id: compower.c,v 1.5 2010/06/14 19:19:17 adamdunkels Exp $
 */

/**
 * \file
 *         Communication power accounting module
 * \author
 *         Adam Dunkels <adam@sics.se>
 */

#include "contiki-conf.h"
#include "sys/energest.h"
#include "sys/compower.h"
#include "net/packetbuf.h"

struct compower_activity compower_idle_activity;

/*---------------------------------------------------------------------------*/
void
compower_init(void)
{
  compower_clear(&compower_idle_activity);
}
/*---------------------------------------------------------------------------*/
void
compower_accumulate(struct compower_activity *e)
{
  static uint32_t last_listen, last_transmit;
  uint32_t listen, transmit;

  listen = energest_type_time(ENERGEST_TYPE_LISTEN);
  e->listen += listen - last_listen;
  last_listen = listen;

  transmit = energest_type_time(ENERGEST_TYPE_TRANSMIT);
  e->transmit += transmit - last_transmit;
  last_transmit = transmit;
}
/*---------------------------------------------------------------------------*/
void
compower_clear(struct compower_activity *e)
{
  e->listen = e->transmit = 0;
}
/*---------------------------------------------------------------------------*/
void
compower_attrconv(struct compower_activity *e)
{
  packetbuf_set_attr(PACKETBUF_ATTR_LISTEN_TIME,
                     packetbuf_attr(PACKETBUF_ATTR_LISTEN_TIME) + e->listen);
  packetbuf_set_attr(PACKETBUF_ATTR_TRANSMIT_TIME,
                     packetbuf_attr(PACKETBUF_ATTR_TRANSMIT_TIME) + e->transmit);
}
/*---------------------------------------------------------------------------*/
void
compower_accumulate_attrs(struct compower_activity *e)
{
  e->listen += packetbuf_attr(PACKETBUF_ATTR_LISTEN_TIME);
  e->transmit += packetbuf_attr(PACKETBUF_ATTR_TRANSMIT_TIME);
}
/*---------------------------------------------------------------------------*/
/** @} */
