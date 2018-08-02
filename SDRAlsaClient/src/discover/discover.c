/*
discover.c

Emulate HPSDR discovery protocol 1

Copyright (C) 2018 by G3UKB Bob Cowdery

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

The authors can be reached by email at:

	bob@bobcowdery.plus.com
*/

// Includes
#include "../common/include.h"

// Module vars
// Client and server addresses
struct sockaddr_in bcAddr, svrAddr;
unsigned char msg[MAX_MSG];
unsigned char resp[MAX_RESP];
unsigned char pcdata[METIS_FRAME_SZ];

// Forward refs
static struct sockaddr_in * udprecvcontrol(int sd);

// Discover protocol
struct sockaddr_in *do_discover(int sd) {
    int rc, udpmsg;

    // Send discovery message
    // Clear message buffer
    memset(msg,0x0,MAX_MSG);
    msg[0] = 0xEF;
    msg[1] = 0xFE;
    msg[2] = 0x02;
    // Set BC address
    bcAddr.sin_addr.s_addr = inet_addr("255.255.255.255");
    bcAddr.sin_port = REMOTE_SERVER_PORT;
    // Dispatch
    if (sendto(sd, resp, MAX_RESP, 0, (struct sockaddr*) &bcAddr, sizeof(bcAddr)) == -1)
    {
        return (struct sockaddr_in *)NULL;
    }

    // Waiting for discovery response
    return udprecvcontrol(sd);
}

// Receive one packet from the client
static struct sockaddr_in * udprecvcontrol(int sd) {
    int n;
    unsigned int svrLen = sizeof(svrAddr);

    // Clear message buffer
    memset(msg,0x0,MAX_MSG);
    // receive message
    n = recvfrom(sd, msg, MAX_MSG, 0, (struct sockaddr *) &svrAddr, &svrLen);

    if(n<0)
        return (struct sockaddr_in *)NULL;
    return &svrAddr;
}

