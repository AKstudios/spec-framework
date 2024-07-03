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
 *   signed pev_cm_slac_param (struct session * session, struct channel * channel, struct message * message);
 *
 *   slac.h
 *
 *   PEV-HLE broadcasts cm_slac_param requests until at least one
 *   matching cm_slac_param confirm is received; matching confirm 
 *   has the same run identifier; EVSE-HLE returns information to 
 *   the PEV-HLE;
 *
 *   return 0 on success, -1 on error and 1 on timeout;
 *
 *   this interaction effectivey synchronizes the PEV-HLE and one
 *   or more EVSE-HLEs by initiating the SLAC protocol sequence;
 *
 *   MSOUND-TARGET will be FF:FF:FF:FF:FF:FF for SLAC;
 *   NUM_SOUNDS will be 8 for SLAC;
 *   TIME_OUT will be 10 for SLAC;
 *   RESP_TYPE will be 00 for SLAC (???);
 *   FORWARDING_STA will be 00:00:00:00:00:00 for SLAC when RESP_TYPE=0;
 *   APPLICATION_TYPE will be 0 for SLAC;
 *   SECUTITY_TYPE will be 0 for this application;
 *   RunID will be that defined by PEV-HLE;
 *
 *   send cm_slac_param request then wait up to channel.timeout 
 *   milliseconds for a cm_slac_param confirm having the same 
 *   RunID as the request; the application type and security 
 *   type should be the same as in the request; 
 *
 *   save the msound target, forwarding station, number of sounds, 
 *   sounding timeout and response type for use later in the session; 
 *
 *   the forwarding stations is a vague concept; the specification 
 *   authors say it should be FF:FF:FF:FF:FF:FF but here, the EVSE-HLE
 *   will set it to the PEV MAC;
 *   
 *--------------------------------------------------------------------*/

#ifndef PEV_CM_SLAC_PARAM_SOURCE
#define PEV_CM_SLAC_PARAM_SOURCE

#include <string.h>
#include <errno.h>

#include "../ether/channel.h"
#include "../tools/memory.h"
#include "../tools/error.h"
#include "../tools/flags.h"
#include "../slac/slac.h"

signed pev_cm_slac_param (struct session * session, struct channel * channel, struct message * message) 

{
	extern byte const broadcast [ETHER_ADDR_LEN];
	struct cm_slac_param_request * request = (struct cm_slac_param_request *) (message);
	struct cm_slac_param_confirm * confirm = (struct cm_slac_param_confirm *) (message);
	debug (0, __func__, "--> CM_SLAC_PARAM.REQ");
	memset (message, 0, sizeof (* message));
	EthernetHeader (&request->ethernet, broadcast, channel->host, channel->type);
	HomePlugHeader1 (&request->homeplug, HOMEPLUG_MMV, (CM_SLAC_PARAM | MMTYPE_REQ));
	request->APPLICATION_TYPE = session->APPLICATION_TYPE;
	request->SECURITY_TYPE = session->SECURITY_TYPE;
	memcpy (request->RunID, session->RunID, sizeof (request->RunID));
	request->CipherSuite [0] = HTOLE16 ((uint16_t)(session->counter));
	if (sendmessage (channel, message, (ETHER_MIN_LEN - ETHER_CRC_LEN)) <= 0) 
	{
		return (debug (1, __func__, CHANNEL_CANTSEND));
	}
	while (readmessage (channel, message, HOMEPLUG_MMV, (CM_SLAC_PARAM | MMTYPE_CNF)) > 0) 
	{
		if (!memcmp (session->RunID, confirm->RunID, sizeof (session->RunID))) 
		{
			debug (0, __func__, "<-- CM_SLAC_PARAM.CNF");
			if (confirm->APPLICATION_TYPE != session->APPLICATION_TYPE) 
			{
				debug (session->exit, __func__, "Unexpected APPLICATION_TYPE");
			}
			if (confirm->SECURITY_TYPE != session->SECURITY_TYPE) 
			{
				debug (session->exit, __func__, "Unexpected SECURITY_TYPE");
			}
			if (_anyset (session->flags, SLAC_COMPARE)) 
			{
				if (LE16TOH (confirm->CipherSuite) != (uint16_t)(session->counter)) 
				{
					debug (session->exit, __func__, "session->counter mismatch! PEV=(%d) EVSE=(%d)", LE16TOH (confirm->CipherSuite), session->counter);
				}
			}

#if SLAC_DEBUG

			if (_anyset (session->flags, SLAC_VERBOSE)) 
			{
				char string [256];
				debug (0, __func__, "CM_SLAC_PARAM.CNF.MSOUND_TARGET %s", HEXSTRING (string, confirm->MSOUND_TARGET));
				debug (0, __func__, "CM_SLAC_PARAM.CNF.NUM_SOUNDS %d", confirm->NUM_SOUNDS);
				debug (0, __func__, "CM_SLAC_PARAM.CNF.TIME_OUT %d", confirm->TIME_OUT);
				debug (0, __func__, "CM_SLAC_PARAM.CNF.RESP_TYPE %d", confirm->RESP_TYPE);
				debug (0, __func__, "CM_SLAC_PARAM.CNF.FORWARDING_STA %s", HEXSTRING (string, confirm->FORWARDING_STA));
				debug (0, __func__, "CM_SLAC_PARAM.CNF.APPLICATION_TYPE %d", confirm->APPLICATION_TYPE);
				debug (0, __func__, "CM_SLAC_PARAM.CNF.SECURITY_TYPE %d", confirm->SECURITY_TYPE);
				debug (0, __func__, "CM_SLAC_PARAM.CNF.RunID %s", HEXSTRING (string, confirm->RunID));
				debug (0, __func__, "CM_SLAC_PARAM.CNF.CipherSuite %d", confirm->CipherSuite);
			}

#endif

			memcpy (session->FORWARDING_STA, confirm->FORWARDING_STA, sizeof (session->FORWARDING_STA));
			memcpy (session->MSOUND_TARGET, confirm->MSOUND_TARGET, sizeof (session->MSOUND_TARGET));
			session->NUM_SOUNDS = confirm->NUM_SOUNDS;
			session->TIME_OUT = confirm->TIME_OUT;
			session->RESP_TYPE = confirm->RESP_TYPE;
			return (0);
		}
	}
	return (debug (0, __func__, "<-- CM_SLAC_PARAM.CNF ?"));
}


#endif

