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
 *   signed pev_cm_slac_match (struct session * session, struct channel * channel, struct message * message);
 *
 *   slac.h
 *
 *   send CM_SLAC_MATCH.REQ and receive CM_SLAC_MATCH.CNF; store NMK
 *   and NID in the session variable so that PEV-HLE can configure 
 *   PEV-PLC to form a network with EVSE-PLC;
 *
 *--------------------------------------------------------------------*/

#ifndef PEV_CM_SLAC_MATCH_SOURCE
#define PEV_CM_SLAC_MATCH_SOURCE

#include <string.h>
#include <errno.h>

#include "../ether/channel.h"
#include "../tools/memory.h"
#include "../tools/error.h"
#include "../tools/flags.h"
#include "../slac/slac.h"

signed pev_cm_slac_match (struct session * session, struct channel * channel, struct message * message) 

{
	struct cm_slac_match_request * request = (struct cm_slac_match_request *) (message);
	struct cm_slac_match_confirm * confirm = (struct cm_slac_match_confirm *) (message);
	debug (0, __func__, "--> CM_SLAC_MATCH.REQ");
	memset (message, 0, sizeof (* message));
	EthernetHeader (&request->ethernet, session->EVSE_MAC, channel->host, channel->type);
	HomePlugHeader1 (&request->homeplug, HOMEPLUG_MMV, (CM_SLAC_MATCH | MMTYPE_REQ));
	request->APPLICATION_TYPE = session->APPLICATION_TYPE;
	request->SECURITY_TYPE = session->SECURITY_TYPE;
	request->MVFLength = HTOLE16 (sizeof (request->MatchVarField));
	memcpy (request->MatchVarField.PEV_ID, session->PEV_ID, sizeof (request->MatchVarField.PEV_ID));
	memcpy (request->MatchVarField.PEV_MAC, session->PEV_MAC, sizeof (request->MatchVarField.PEV_MAC));
	memcpy (request->MatchVarField.RunID, session->RunID, sizeof (request->MatchVarField.RunID));
	if (sendmessage (channel, message, sizeof (* request)) <= 0) 
	{
		return (debug (1, __func__, CHANNEL_CANTSEND));
	}
	if (readmessage (channel, message, HOMEPLUG_MMV, (CM_SLAC_MATCH | MMTYPE_CNF)) > 0) 
	{
		if (!memcmp (session->RunID, confirm->MatchVarField.RunID, sizeof (session->RunID))) 
		{
			debug (0, __func__, "<-- CM_SLAC_MATCH.CNF");

#if SLAC_DEBUG

			if (_anyset (session->flags, SLAC_VERBOSE)) 
			{
				char string [256];
				debug (0, __func__, "CM_SLAC_MATCH.CNF.APPLICATION_TYPE %d", confirm->APPLICATION_TYPE);
				debug (0, __func__, "CM_SLAC_MATCH.CNF.SECURITY_TYPE %d", confirm->SECURITY_TYPE);
				debug (0, __func__, "CM_SLAC_MATCH.CNF.MVFLength %d", LE16TOH (confirm->MVFLength));
				debug (0, __func__, "CM_SLAC_MATCH.CNF.MatchVarField.PEV_ID %s", HEXSTRING (string, confirm->MatchVarField.PEV_ID));
				debug (0, __func__, "CM_SLAC_MATCH.CNF.MatchVarField.PEV_MAC %s", HEXSTRING (string, confirm->MatchVarField.PEV_MAC));
				debug (0, __func__, "CM_SLAC_MATCH.CNF.MatchVarField.EVSE_ID %s", HEXSTRING (string, confirm->MatchVarField.EVSE_ID));
				debug (0, __func__, "CM_SLAC_MATCH.CNF.MatchVarField.EVSE_MAC %s", HEXSTRING (string, confirm->MatchVarField.EVSE_MAC));
				debug (0, __func__, "CM_SLAC_MATCH.CNF.MatchVarField.RunID %s", HEXSTRING (string, confirm->MatchVarField.RunID));
				debug (0, __func__, "CM_SLAC_MATCH.CNF.MatchVarField.NID %s", HEXSTRING (string, confirm->MatchVarField.NID));
				debug (0, __func__, "CM_SLAC_MATCH.CNF.MatchVarField.NMK %s", HEXSTRING (string, confirm->MatchVarField.NMK));
			}

#endif

			memcpy (session->EVSE_ID, confirm->MatchVarField.EVSE_ID, sizeof (session->EVSE_ID));
			memcpy (session->EVSE_MAC, confirm->MatchVarField.EVSE_MAC, sizeof (session->EVSE_MAC));
			memcpy (session->NMK, confirm->MatchVarField.NMK, sizeof (session->NMK));
			memcpy (session->NID, confirm->MatchVarField.NID, sizeof (session->NID));
			return (0);
		}
	}
	return (debug (session->exit, __func__, "<-- CM_SLAC_MATCH.CNF ?"));
}


#endif

