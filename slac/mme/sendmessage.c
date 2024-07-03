/*====================================================================*
 *   
 *   Copyright (c) 2013 Qualcomm Atheros Inc.
 *   
 *   Permission to use, copy, modify, and/or distribute this software 
 *   for any purpose with or without fee is hereby granted, provided 
 *   that the above copyright notice and this permission notice appear 
 *   in all copies.
 *   
 *   THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL 
 *   WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED 
 *   WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL  
 *   THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR 
 *   CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM 
 *   LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, 
 *   NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 *   CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *   
 *--------------------------------------------------------------------*/

/*====================================================================*
 *
 *   ssize_t sendmessage (struct channel const * channel, struct message * message, ssize_t length);
 *
 *   plc.h
 *   
 *   send management message over Ethernet in a platform independent
 *   fashion; return the number of bytes sent on succes, 0 on timeout
 *   or -1 on error;
 *
 *   see readmessage for the vendor specific read counterpart to this 
 *   function;
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qca.qualcomm.com>
 *      Nathaniel Houghton <nhoughto@qca.qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef SENDMESSAGE_SOURCE
#define SENDMESSAGE_SOURCE

#include <stdint.h>

#include "../plc/plc.h"

ssize_t sendmessage (struct channel const * channel, struct message * message, ssize_t length) 

{
	return (sendpacket (channel, message, length));
}

#endif

