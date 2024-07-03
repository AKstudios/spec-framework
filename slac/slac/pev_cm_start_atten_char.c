/*====================================================================*"
 *   
 *   Copyright (c) 2013, Qualcomm Atheros.
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
 *   signed pev_cm_start_atten_char (struct session * session, struct channel * channel, struct message * message);
 *
 *   slac.h
 *   
 *   the PEV-HLE broadcasts a CM_START_ATTEN_CHAR.IND to initiate a 
 *   SLAC session; we broadcast three times per HPGP specification;
 *
 *--------------------------------------------------------------------*/

#ifndef PEV_CM_START_ATTEN_CHAR_SOURCE
#define PEV_CM_START_ATTEN_CHAR_SOURCE

#include <sys/time.h>
#include <memory.h>
#include <errno.h>
#include <string.h>

#include "../tools/types.h"
#include "../tools/error.h"
#include "../ether/channel.h"
#include "../slac/slac.h"

signed pev_cm_start_atten_char (struct session * session, struct channel * channel, struct message * message) 

{
	struct cm_start_atten_char_indicate * indicate = (struct cm_start_atten_char_indicate *) (message);
	debug (0, __func__, "--> CM_START_ATTEN_CHAR.IND");
	memset (message, 0, sizeof (* message));
	EthernetHeader (&indicate->ethernet, session->MSOUND_TARGET, channel->host, channel->type);
	HomePlugHeader1 (&indicate->homeplug, HOMEPLUG_MMV, (CM_START_ATTEN_CHAR | MMTYPE_IND));
	indicate->APPLICATION_TYPE = session->APPLICATION_TYPE;
	indicate->SECURITY_TYPE = session->SECURITY_TYPE;
	indicate->ACVarField.NUM_SOUNDS = session->NUM_SOUNDS;
	indicate->ACVarField.TIME_OUT = session->TIME_OUT;
	indicate->ACVarField.RESP_TYPE = session->RESP_TYPE;
	memcpy (indicate->ACVarField.FORWARDING_STA, session->FORWARDING_STA, sizeof (indicate->ACVarField.FORWARDING_STA));
	memcpy (indicate->ACVarField.RunID, session->RunID, sizeof (indicate->ACVarField.RunID));
	if (sendmessage (channel, message, (ETHER_MIN_LEN - ETHER_CRC_LEN)) <= 0) 
	{
		return (debug (1, __func__, CHANNEL_CANTSEND));
	}
	debug (0, __func__, "--> CM_START_ATTEN_CHAR.IND");
/*	
 *	the GreenPHY spec says to send CM_START_ATTEN.IND three times to ensure
 *	that is is received;
 */

	if (sendmessage (channel, message, (ETHER_MIN_LEN - ETHER_CRC_LEN)) <= 0) 
	{
		return (debug (1, __func__, CHANNEL_CANTSEND));
	}
	debug (0, __func__, "--> CM_START_ATTEN_CHAR.IND");
	if (sendmessage (channel, message, (ETHER_MIN_LEN - ETHER_CRC_LEN)) <= 0) 
	{
		return (debug (1, __func__, CHANNEL_CANTSEND));
	}

	return (0);
}


#endif

