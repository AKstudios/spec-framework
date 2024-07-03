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
 *   signed evse_cm_slac_match (struct session * session, struct channel * channel, struct message * message);
 *
 *   slac.h
 *  
 *   receive CM_SLAC_MATCH.REQ and send CM_SLAC_MATCH.CNF containing
 *   random NMK/NID that EVSE-HLE will use to configure EVSE-PLC for
 *   charging; 
 *
 *   users must develop their own means of generating random NMK/NID
 *   pairs because it is not part of the standard; this example does
 *   not attempt to randomize NMK/NID;
 *  
 *--------------------------------------------------------------------*/

#ifndef EVSE_CM_SLAC_MATCH_SOURCE
#define EVSE_CM_SLAC_MATCH_SOURCE

#include <time.h>
#include <stdio.h>
#include <string.h>

#include "../ether/channel.h"
#include "../tools/memory.h"
#include "../tools/error.h"
#include "../tools/flags.h"
#include "../slac/slac.h"
// #include "sockets.h"
// #include "sdpserver.h"

signed evse_cm_slac_match (struct session * session, struct channel * channel, struct message * message) 

{
	 __time_t CurrentTime, DeltaTime, InitTime;
	struct cm_slac_match_request * request = (struct cm_slac_match_request *) (message);
	struct cm_slac_match_confirm * confirm = (struct cm_slac_match_confirm *) (message);
	InitTime = time(0);
	while (readmessage (channel, message, HOMEPLUG_MMV, (CM_SLAC_MATCH | MMTYPE_REQ)) > 0) 
	{
		if (!memcmp (session->RunID, request->MatchVarField.RunID, sizeof (session->RunID))) 
		{
			debug (0, __func__, "<-- CM_SLAC_MATCH.REQ");
			memcpy (session->PEV_ID, request->MatchVarField.PEV_ID, sizeof (session->PEV_ID));
			memcpy (session->PEV_MAC, request->MatchVarField.PEV_MAC, sizeof (session->PEV_MAC));
			memcpy (session->RunID, request->MatchVarField.RunID, sizeof (session->RunID));

#if SLAC_DEBUG

			if (_anyset (session->flags, SLAC_VERBOSE)) 
			{
				char string [256];
				debug (0, __func__, "CM_SLAC_MATCH.REQ.APPLICATION_TYPE %d", request->APPLICATION_TYPE);
				debug (0, __func__, "CM_SLAC_MATCH.REQ.SECURITY_TYPE %d", request->SECURITY_TYPE);
				debug (0, __func__, "CM_SLAC_MATCH.REQ.MVFLength %d", LE16TOH (request->MVFLength));
				debug (0, __func__, "CM_SLAC_MATCH.REQ.PEV_ID %s", HEXSTRING (string, request->MatchVarField.PEV_ID));
				debug (0, __func__, "CM_SLAC_MATCH.REQ.PEV_MAC %s", HEXSTRING (string, request->MatchVarField.PEV_MAC));
				debug (0, __func__, "CM_SLAC_MATCH.REQ.EVSE_ID %s", HEXSTRING (string, request->MatchVarField.EVSE_ID));
				debug (0, __func__, "CM_SLAC_MATCH.REQ.EVSE_MAC %s", HEXSTRING (string, request->MatchVarField.EVSE_MAC));
				debug (0, __func__, "CM_SLAC_MATCH.REQ.RunID %s", HEXSTRING (string, request->MatchVarField.RunID));
			}

#endif

			debug (0, __func__, "--> CM_SLAC_MATCH.CNF");
			memset (message, 0, sizeof (* message));
			EthernetHeader (&confirm->ethernet, session->PEV_MAC, channel->host, channel->type);
			HomePlugHeader1 (&confirm->homeplug, HOMEPLUG_MMV, (CM_SLAC_MATCH | MMTYPE_CNF));
			confirm->APPLICATION_TYPE = session->APPLICATION_TYPE;
			confirm->SECURITY_TYPE = session->SECURITY_TYPE;
			confirm->MVFLength = HTOLE16 (sizeof (confirm->MatchVarField));
			memcpy (confirm->MatchVarField.PEV_ID, session->PEV_ID, sizeof (confirm->MatchVarField.PEV_ID));
			memcpy (confirm->MatchVarField.PEV_MAC, session->PEV_MAC, sizeof (confirm->MatchVarField.PEV_MAC));
			memcpy (confirm->MatchVarField.EVSE_ID, session->EVSE_ID, sizeof (confirm->MatchVarField.EVSE_ID));
			memcpy (confirm->MatchVarField.EVSE_MAC, session->EVSE_MAC, sizeof (confirm->MatchVarField.EVSE_MAC));
			memcpy (confirm->MatchVarField.RunID, session->RunID, sizeof (confirm->MatchVarField.RunID));
			memcpy (confirm->MatchVarField.NID, session->NID, sizeof (confirm->MatchVarField.NID));
			memcpy (confirm->MatchVarField.NMK, session->NMK, sizeof (confirm->MatchVarField.NMK));
			if (sendmessage (channel, message, sizeof (* confirm)) <= 0) 
			{
				return (debug (1, __func__, CHANNEL_CANTSEND));
			}
			return (0);
		}
		// if (PollServer() == -1)
		//   {
		// 	break;  /*close application*/
		//   }
		/*check for TT_EVSE_match_session Timeout (10 s)*/
		CurrentTime = time(0);
		DeltaTime = CurrentTime - InitTime;
		if(DeltaTime >= 10) /*allow PEV 10 Seconds to send CM_SLAC_MATCH.REQ*/
		{
			debug (0, __func__, "Timeout: PEV did not send CM_SLAC_MATCH.REQ within 10 sec!");
			return 1;
		}
	}
	return (debug (session->exit, __func__, "<-- CM_SLAC_MATCH.REQ ?"));
}


#endif

