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
 *   signed NetInfo2 (struct plc * plc);
 *
 *   plc.h
 *
 *   Request network membership information from the peer device using
 *   the VS_NW_INFO message; 
 *
 *   This function is similar to function NetworkInformation() but the
 *   output format is different;
 *
 *   Contributor(s):
 *      Charles Maier <cmaier@qca.qualcomm.com>
 *   Modified by Jason Harper <jharper@anl.gov>
 *
 *--------------------------------------------------------------------*/

#ifndef NETWORKINFO2_SOURCE
#define NETWORKINFO2_SOURCE

#include <stdint.h>
#include <memory.h>

#include "../tools/memory.h"
#include "../tools/number.h"
#include "../tools/error.h"
#include "../plc/plc.h"
#include "../mme/mme.h"

signed NetInfo2 (struct plc * plc) 

{
	extern char const * StationRole [];
	struct channel * channel = (struct channel *)(plc->channel);
	struct message * message = (struct message *)(plc->message);

#ifndef __GNUC__
#pragma pack (push,1)
#endif

	struct __packed vs_nw_info_request 
	{
		struct ethernet_std ethernet;
		struct qualcomm_fmi qualcomm;
	}
	* request = (struct vs_nw_info_request *)(message);
	struct __packed vs_nw_info_confirm 
	{
		struct ethernet_std ethernet;
		struct qualcomm_fmi qualcomm;
		uint8_t SUB_VERSION;
		uint8_t Reserved;
		uint16_t DATA_LEN;
		uint8_t DATA [1];
	}
	* confirm = (struct vs_nw_info_confirm *)(message);
	struct __packed station 
	{
		uint8_t MAC [ETHER_ADDR_LEN];
		uint8_t TEI;
		uint8_t Reserved [3];
		uint8_t BDA [ETHER_ADDR_LEN];
		uint16_t AVGTX;
		uint8_t COUPLING;
		uint8_t Reserved3;
		uint16_t AVGRX;
		uint16_t Reserved4;
	}
	* station;
	struct __packed network 
	{
		uint8_t NID [7];
		uint8_t Reserved1 [2];
		uint8_t SNID;
		uint8_t TEI;
		uint8_t Reserved2 [4];
		uint8_t ROLE;
		uint8_t CCO_MAC [ETHER_ADDR_LEN];
		uint8_t CCO_TEI;
		uint8_t Reserved3 [3];
		uint8_t NUMSTAS;
		uint8_t Reserved4 [5];
		struct station stations [1];
	}
	* network;
	struct __packed networks 
	{
		uint8_t Reserved;
		uint8_t NUMAVLNS;
		struct network networks [1];
	}
	* networks = (struct networks *) (confirm->DATA);

#ifndef __GNUC__
#pragma pack (pop)
#endif

	/*Request (plc, "Fetch Network Information");*/
	memset (message, 0, sizeof (* message));
	EthernetHeader (&request->ethernet, channel->peer, channel->host, channel->type);
	QualcommHeader1 (&request->qualcomm, 1, (VS_NW_INFO | MMTYPE_REQ));
	plc->packetsize = (ETHER_MIN_LEN - ETHER_CRC_LEN);
	if (SendMME (plc) <= 0) 
	{
		error ((plc->flags & PLC_BAILOUT), errno, CHANNEL_CANTSEND);
		return (-1);
	}
	while (ReadMME (plc, 1, (VS_NW_INFO | MMTYPE_CNF)) > 0) 
	{
		char string [24];
		network = (struct network *)(&networks->networks);
		if(network->NUMSTAS >= 1)
		{
			Confirm (plc, "Found %d Network(s)\n", networks->NUMAVLNS);
			while (networks->NUMAVLNS--)
			{
				printf ("\tnetwork->NID = %s\n", hexstring (string, sizeof (string), network->NID, sizeof (network->NID)));
				printf ("\tnetwork->SNID = %d\n", network->SNID);
				printf ("\tnetwork->TEI = %d\n", network->TEI);
				printf ("\tnetwork->ROLE = 0x%02X (%s)\n", network->ROLE, StationRole [network->ROLE]);
				printf ("\tnetwork->CCO_DA = %s\n", hexstring (string, sizeof (string), network->CCO_MAC, sizeof (network->CCO_MAC)));
				printf ("\tnetwork->CCO_TEI = %d\n", network->CCO_TEI);
				printf ("\tnetwork->STATIONS = %d\n", network->NUMSTAS);
				printf ("\n");
				station = (struct station *)(&network->stations);
				while (network->NUMSTAS--)
				{
					printf ("\t\tstation->MAC = %s\n", hexstring (string, sizeof (string), station->MAC, sizeof (station->MAC)));
					printf ("\t\tstation->TEI = %d\n", station->TEI);
					printf ("\t\tstation->BDA = %s\n", hexstring (string, sizeof (string), station->BDA, sizeof (station->BDA)));
					station->AVGTX = LE16TOH (station->AVGTX);
					station->AVGRX = LE16TOH (station->AVGRX);
					printf ("\t\tstation->AvgPHYDR_TX = %03d mbps %s\n", station->AVGTX, ((station->COUPLING >> 0) & 0x0F)? "Alternate": "Primary");
					printf ("\t\tstation->AvgPHYDR_RX = %03d mbps %s\n", station->AVGRX, ((station->COUPLING >> 4) & 0x0F)? "Alternate": "Primary");
					printf ("\n");
					station++;
				}
				network = (struct network *)(station);
			}
			return 0;
		}
	}
	  return 1;
}

#endif

