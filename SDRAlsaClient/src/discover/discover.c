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

    // Send discovery message
    // Clear message buffer
    memset(msg,0x0,MAX_MSG);
    msg[0] = 0xEF;
    msg[1] = 0xFE;
    msg[2] = 0x02;

    // Set BC address
    memset((char *) &bcAddr, 0, sizeof(bcAddr));
    bcAddr.sin_family = AF_INET;
    bcAddr.sin_addr.S_un.S_addr = inet_addr("255.255.255.255");
    bcAddr.sin_port = htons(REMOTE_SERVER_PORT);

    // Dispatch
    if (sendto(sd, (const char*)msg, MAX_MSG, 0, (const struct sockaddr*) &bcAddr, sizeof(bcAddr)) == -1) {
        return (struct sockaddr_in *)NULL;
    }

    // Waiting for discovery response
    return udprecvcontrol(sd);
}

// Start streaming
int do_start(int sd, struct sockaddr_in *svrAddr) {
    // Send start message
    // Clear message buffer
    memset(msg,0x0,MAX_MSG);
    msg[0] = 0xEF;
    msg[1] = 0xFE;
    msg[2] = 0x04;
    msg[3] = 0x01;

    // Dispatch
    if (sendto(sd, (const char*)msg, MAX_MSG, 0, (const struct sockaddr*) svrAddr, sizeof(*svrAddr)) == -1) {
        return FALSE;
    }

    return TRUE;
}

// Stop streaming
int do_stop(int sd, struct sockaddr_in *svrAddr) {
    // Send stop message
    // Clear message buffer
    memset(msg,0x0,MAX_MSG);
    msg[0] = 0xEF;
    msg[1] = 0xFE;
    msg[2] = 0x04;

    // Dispatch
    if (sendto(sd, (const char*)msg, MAX_MSG, 0, (const struct sockaddr*) svrAddr, sizeof(*svrAddr)) == -1) {
        return FALSE;
    }

    return TRUE;
}

// Receive one packet from the client
static struct sockaddr_in *udprecvcontrol(int sd) {
    int n;
    int svrLen = sizeof(svrAddr);

    // Clear message buffer
    memset(msg,0x0,MAX_MSG);
    // receive message
    n = recvfrom(sd, (char*)msg, MAX_MSG, 0, (struct sockaddr *) &svrAddr, &svrLen);
    if(n<0)
        return (struct sockaddr_in *)NULL;
    return &svrAddr;
}

