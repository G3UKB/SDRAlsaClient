/*
main.h

'C' entry point for the FCD Client application.

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

#ifndef __MAIN_H__
#define __MAIN_H__

#ifdef __cplusplus
extern "C"
{
#endif

// Defines
#define METIS_FRAME_SZ 1032
// Allow 10 writes of 1024 IQ samples
#define iq_ring_byte_sz 10*1024*4

//==================================================================
// Ring buffer to hold audio samples
ringb_t *rb_iq;

//==================================================================
// Threads for audio capture and UDP dispatch
pthread_t udp_writer_thd;
pthread_t udp_reader_thd;

// Thread data structure for UDP reader/writer
typedef struct udp_thread_data {
    int terminate;
    int socket;
    struct sockaddr_in *srv_addr;
	ringb_t *rb;
}udp_thread_data;
udp_thread_data *udp_writer_td;
udp_thread_data *udp_reader_td;


// Prototypes
int DLL_EXPORT RunClient();

#ifdef __cplusplus
}
#endif

#endif // __MAIN_H__
