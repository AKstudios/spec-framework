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
 *   void slac_connect (struct session * session);
 *   
 *   slac.h
 *
 *   compute the arithmetic mean of session attenuation values and
 *   compare to the session limit; average attenuation greater than
 *   the session limit is considered a bad connection;
 *
 *--------------------------------------------------------------------*/

#ifndef SLAC_AVERAGE_SOURCE
#define SLAC_AVERAGE_SOURCE

#include "../tools/error.h"
#include "../tools/flags.h"
#include "../tools/memory.h"
#include "../slac/slac.h"

signed slac_connect (struct session * session) 

{
	unsigned group = 0;
	unsigned total = 0;
	if (session->NumGroups > SIZEOF (session->AAG)) 
	{
		return (debug (session->exit, __func__, "Too much data to analyse!"));
	}
	if (session->NumGroups > 0) 
	{
		char string [512];
		while (group < session->NumGroups) 
		{
			total += session->AAG [group];
			group++;
		}
		total /= group;
		if (total > session->limit) 
		{
			char string [512];
			debug (0, __func__, "Average attenuation (%u) more than limit (%u) frow %d groups", total, session->limit, group);
			debug (0, __func__, "%s", HEXSTRING (string, session->AAG));
			return (-1);
		}
		if (total > 0) 
		{
			debug (0, __func__, "Average attenuation (%u) less than limit (%u) from %d groups", total, session->limit, group);
			debug (0, __func__, "%s", HEXSTRING (string, session->AAG));
			return (0);
		}
	}
	return (debug (session->exit, __func__, "Nothing to analyse"));
}


#endif

