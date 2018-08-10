/*
udp_writer.c

Write data to UDP.

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

short *smpl_buffer;
unsigned char *packet_buffer;

// Local functions
static int fcd_get_freq();

// Module vars
pthread_mutex_t fcd_mutex = PTHREAD_MUTEX_INITIALIZER;
int freq_hz;
int last_freq = -1;

// Safe set frequency
void fcd_set_freq(int f) {
    pthread_mutex_lock(&fcd_mutex);
    freq_hz = f;
    pthread_mutex_unlock(&fcd_mutex);
}

// Thread entry point for ALSA processing
void udp_writer_imp(void* data){
    unsigned int new_freq;

    // Get our thread parameters
    udp_thread_data* td = (udp_thread_data*)data;
    ringb_t *rb = td->rb;
    int sd = td->socket;
    struct sockaddr_in *srv_addr = td->srv_addr;

    printf("Started UDP writer thread\n");

    while (td->terminate == FALSE) {
        new_freq = fcd_get_freq();
        if (new_freq != last_freq){
            // We have a frequency change
            // Format the message
            memset(msg,0x0,MAX_MSG);
            msg[0] = 0xEF;
            msg[1] = 0xFE;
            msg[2] = 0x01;
            msg[3] = 0x02;
            msg[8] = 0x7f;
            msg[9] = 0x7f;
            msg[10] = 0x7f;
            msg[11] = 0x02;
            msg[12] = (new_freq >> 24) & 0xff;
            msg[13] = (new_freq >> 16) & 0xff;
            msg[14] = (new_freq >> 8) & 0xff;
            msg[15] = new_freq & 0xff;
            msg[8+512] = 0x7f;
            msg[9+512] = 0x7f;
            msg[10+512] = 0x7f;
            msg[11+512] = 0x02;
            msg[12+512] = (new_freq >> 24) & 0xff;
            msg[13+512] = (new_freq >> 16) & 0xff;
            msg[14+512] = (new_freq >> 8) & 0xff;
            msg[15+512] = new_freq & 0xff;

            // Dispatch
            if (sendto(sd, msg, MAX_MSG, 0, (struct sockaddr_in*) svrAddr, sizeof(*svrAddr)) == -1) {
                return FALSE;
            }
        }
    }
    printf("UDP Writer thread exiting...\n");
}

// Safe get frequency
static int fcd_get_freq() {
    int f;
    pthread_mutex_lock(&fcd_mutex);
    f = freq_hz;
    pthread_mutex_unlock(&fcd_mutex);
    return f;
}
