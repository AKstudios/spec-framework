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
 *   signed pev_cm_set_key (struct session * session, struct channel * channel, struct message * message);
 *
 *   slac.h
 *
 *   PEV-HLE sets the NMK and NID on PEV-PLC using CM_SET_KEY.REQ; 
 *   the NMK and NID must match those provided by EVSE-HLE using 
 *   CM_SLAC_MATCH.CNF;
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qca.qualcomm.com>
 *
 *--------------------------------------------------------------------*/

#ifndef PEV_CM_SET_KEY_SOURCE
#define PEV_CM_SET_KEY_SOURCE

#include <string.h>

#include "../ether/channel.h"
#include "../tools/memory.h"
#include "../tools/error.h"
#include "../tools/flags.h"
#include "../mme/qualcomm.h"
#include "../mme/homeplug.h"
#include "../slac/slac.h"

signed pev_cm_set_key (struct session * session, struct channel * channel, struct message * message) 

{

#ifndef __GNUC__
#pragma pack(push,1)
#endif

	struct __packed cm_set_key_request 
	{
		struct ethernet_std ethernet;
		struct homeplug_fmi homeplug;
		uint8_t KEYTYPE;
		uint32_t MYNOUNCE;
		uint32_t YOURNOUNCE;
		uint8_t PID;
		uint16_t PRN;
		uint8_t PMN;
		uint8_t CCOCAP;
		uint8_t NID [SLAC_NID_LEN];
		uint8_t NEWEKS;
		uint8_t NEWKEY [SLAC_NMK_LEN];
		uint8_t RSVD [3];
	}
	* request = (struct cm_set_key_request *)(message);
	struct __packed cm_set_key_confirm 
	{
		struct ethernet_std ethernet;
		struct homeplug_fmi homeplug;
		uint8_t RESULT;
		uint32_t MYNOUNCE;
		uint32_t YOURNOUNCE;
		uint8_t PID;
		uint16_t PRN;
		uint8_t PMN;
		uint8_t CCOCAP;
		uint8_t RSVD [27];
	}
	* confirm = (struct cm_set_key_confirm *)(message);

#ifndef __GNUC__
#pragma pack (pop)
#endif

	memset (message, 0, sizeof (* message));
	debug (0, __func__, "--> CM_SET_KEY.REQ");
	EthernetHeader (&request->ethernet, channel->peer, channel->host, channel->type);
	HomePlugHeader1 (&request->homeplug, HOMEPLUG_MMV, (CM_SET_KEY | MMTYPE_REQ));
	request->KEYTYPE = SLAC_CM_SETKEY_KEYTYPE;
	memset (&request->MYNOUNCE, 0xAA, sizeof (request->MYNOUNCE));
	memset (&request->YOURNOUNCE, 0x00, sizeof (request->YOURNOUNCE));
	request->PID = SLAC_CM_SETKEY_PID;
	request->PRN = HTOLE16 (SLAC_CM_SETKEY_PRN);
	request->PMN = SLAC_CM_SETKEY_PMN;
	request->CCOCAP = SLAC_CM_SETKEY_CCO;
	memcpy (request->NID, session->NID, sizeof (request->NID));
	request->NEWEKS = SLAC_CM_SETKEY_EKS;
	memcpy (request->NEWKEY, session->NMK, sizeof (request->NEWKEY));

#if SLAC_DEBUG

	if (_anyset (session->flags, SLAC_VERBOSE)) 
	{
		char string [1024];
		debug (0, __func__, "CM_SET_KEY.KEYTYPE %d", request->KEYTYPE);
		debug (0, __func__, "CM_SET_KEY.MYNOUNCE %s", hexstring (string, sizeof (string), &request->MYNOUNCE, sizeof (request->MYNOUNCE)));
		debug (0, __func__, "CM_SET_KEY.YOURNOUNCE %s", hexstring (string, sizeof (string), &request->YOURNOUNCE, sizeof (request->MYNOUNCE)));
		debug (0, __func__, "CM_SET_KEY.PID %d", request->PID);
		debug (0, __func__, "CM_SET_KEY.PRN %d", LE32TOH (request->PRN));
		debug (0, __func__, "CM_SET_KEY.PMN %d", request->PMN);
		debug (0, __func__, "CM_SET_KEY.CCoCAP %d", request->CCOCAP);
		debug (0, __func__, "CM_SET_KEY.NID %s", HEXSTRING (string, request->NID));
		debug (0, __func__, "CM_SET_KEY.NEWEKS %d", request->NEWEKS);
		debug (0, __func__, "CM_SET_KEY.NEWKEY %s", HEXSTRING (string, request->NEWKEY));
	}

#endif

	if (sendpacket (channel, request, sizeof (* request)) <= 0) 
	{
		return (debug (1, __func__, CHANNEL_CANTSEND));
	}
	while (readpacket (channel, confirm, sizeof (* confirm)) > 0) 
	{
		if (htons (confirm->ethernet.MTYPE) != HOMEPLUG_MTYPE) 
		{
			debug (session->exit, __func__, "Ignore MTYPE 0x%04X", htons (confirm->ethernet.MTYPE));
			continue;
		}
		if (confirm->homeplug.MMV != HOMEPLUG_MMV) 
		{
			debug (session->exit, __func__, "Ignore MMV 0x%02X", confirm->homeplug.MMV);
			continue;
		}
		if (LE32TOH (confirm->homeplug.MMTYPE) != (CM_SET_KEY | MMTYPE_CNF)) 
		{
			debug (session->exit, __func__, "Ignore MMTYPE 0x%04X", LE32TOH (confirm->homeplug.MMTYPE));
			continue;
		}
		debug (0, __func__, "<-- CM_SET_KEY.CNF");
		if (!confirm->RESULT) 
		{
			return (debug (session->exit, __func__, "Can't set keys"));
		}

#if SLAC_DEBUG

		if (_anyset (session->flags, SLAC_VERBOSE)) 
		{
			char string [1024];
			debug (0, __func__, "CM_SET_KEY.RESULT %d", confirm->RESULT);
			debug (0, __func__, "CM_SET_KEY.MYNOUNCE %s", hexstring (string, sizeof (string), &confirm->MYNOUNCE, sizeof (confirm->MYNOUNCE)));
			debug (0, __func__, "CM_SET_KEY.YOURNOUNCE %s", hexstring (string, sizeof (string), &confirm->YOURNOUNCE, sizeof (confirm->MYNOUNCE)));
			debug (0, __func__, "CM_SET_KEY.PID %d", confirm->PID);
			debug (0, __func__, "CM_SET_KEY.PRN %d", LE32TOH (confirm->PRN));
			debug (0, __func__, "CM_SET_KEY.PMN %d", confirm->PMN);
			debug (0, __func__, "CM_SET_KEY.CCoCAP %d", confirm->CCOCAP);
		}

#endif

		return (0);
	}
	return (debug (session->exit, __func__, "<-- CM_SET_KEY.CNF ?"));
}


#endif

