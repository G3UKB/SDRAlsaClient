/*
main.c

'C' main for the FCD Client application

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

#include "common/include.h"

// Local functions
static int open_bc_socket();
static int revert_sd(int sd);

// Module vars
WSADATA wsa;                    // Winsock
int sd;                         // Socket handle
struct sockaddr_in *srv_addr;   // Server address structure
char last_error[128];           // Holder for last error
char host_api[50];              // Holds the VAC audio api name
char audio_device[50];          // Holds the VAC audio device name
AudioDescriptor * audio_desc;   // Returned audio descriptor

// Run the autonomous client system
int DLL_EXPORT RunClient()
{
    int rc, success;

    // Local variables
    int pa_error_code, iq_ring_sz, i;
    DeviceEnumList* audio_output_list;

    //===========================================================================
    // Allocate a ring buffer to hold I/Q samples
    // Note this must be rounded to an acceptable size
    iq_ring_sz = pow(2, ceil(log(iq_ring_byte_sz)/log(2)));
    rb_iq = ringb_create (iq_ring_sz);

    //===========================================================================
    // Initialise the portaudio system
    if((pa_error_code = audio_init()) != paNoError ) {
        strcpy(last_error, Pa_GetErrorText(pa_error_code));
        return -1;
    }
    // Get available output devices
    audio_output_list = enum_outputs();
    success = FALSE;
    for (i=0 ; i<audio_output_list->entries ; i++) {
        // Looking for a VAC output device
        if(strstr(audio_output_list->devices[i].name, "virtual audio cable") != NULL) {
            strcpy(host_api, audio_output_list->devices[i].host_api);
            strcpy(host_api, audio_output_list->devices[i].name);
            success = TRUE;
        }
    }
    if (!success) {
        strcpy(last_error, "Failed to find a compatible VAC output device");
        return -1;
    }
    // Open audio channel
    if ((audio_desc = open_audio_channel(rb_iq, DIR_OUT, host_api, audio_device)) == (AudioDescriptor *)NULL) {
        strcpy(last_error, "Failed to open audio channel ");
        return -1;
    }
    // Start the stream
    // This will now wait on their being something in the ring buffer
    if ((pa_error_code = audio_start_stream(audio_desc->stream)) != paNoError ) {
        strcpy(last_error, Pa_GetErrorText(pa_error_code));
        return -1;
    }

    //===========================================================================
    // Initialise socket lib
    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0) {
        strcpy(last_error, WSAGetLastError());
        return -1;
    }

    //===========================================================================
    // Do discovery protocol 1 as per HPSDR
    if((sd = open_bc_socket()) == -1) {
        strcpy(last_error, "Failed to open broadcast socket for discovery protocol!");
        return -1;
    }

    srv_addr = do_discover(sd);
    if (srv_addr == (struct sockaddr_in *)NULL) {
        strcpy(last_error, "Sorry, discovery protocol failed! No device found.");
        return -1;
    }

    //===========================================================================
    // Initialise and start the UDP writer
    // Allocate thread data structure
	udp_writer_td = (udp_thread_data*)safealloc(sizeof(udp_thread_data), sizeof(char), "UDP_TD_WRITER_STRUCT");
	// Init with thread data items
	udp_writer_td->terminate = FALSE;
    udp_writer_td->rb = rb_iq;
    udp_writer_td->socket = sd;
    udp_writer_td->srv_addr = srv_addr;

	// Create the UDP writer thread
	rc = pthread_create(&udp_writer_thd, NULL, udp_writer_imp, (void *)udp_writer_td);
	if (rc){
        strcpy(last_error, "Failed to create UDP writer thread!");
        return -1;
	}

	//===========================================================================
    // Initialise and start the UDP reader
    // Allocate thread data structure
	udp_reader_td = (udp_thread_data*)safealloc(sizeof(udp_thread_data), sizeof(char), "UDP_TD_READER_STRUCT");
	// Init with thread data items
	udp_reader_td->terminate = FALSE;
    udp_reader_td->rb = rb_iq;
    udp_reader_td->socket = sd;
    udp_reader_td->srv_addr = srv_addr;

	// Create the UDP writer thread
	rc = pthread_create(&udp_reader_thd, NULL, udp_reader_imp, (void *)udp_reader_td);
	if (rc){
        strcpy(last_error, "Failed to create UDP reader thread!");
        return -1;
	}

	// Finally success
	return 0;
}

// Get the error text for the last fail return
char* DLL_EXPORT GertLastError() {
    return last_error;
}

// Open a broadcast socket
static int open_bc_socket() {
    int sd, rc;
    int  broadcast = 1;
    struct sockaddr_in serv_addr;

    // Create socket
    sd=socket(AF_INET, SOCK_DGRAM, 0);
    if (sd<0) {
        return -1;
    }

    // Set to broadcast
    if (setsockopt(sd, SOL_SOCKET, SO_BROADCAST, &broadcast,sizeof broadcast) == -1) {
        return -1;
    }

    return sd;
}

// Revert broadcast socket to a normal socket
static int revert_sd(int sd) {
    int broadcast = 0;
    int sendbuff = 32000;
    int recvbuff = 32000;
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 10;

    // Turn off broadcast
    if (setsockopt(sd, SOL_SOCKET, SO_BROADCAST, &broadcast,sizeof broadcast) == -1) {
        printf("Failed to set option SO_BROADCAST!\n");
        return FALSE;
    }
    // Set send buffer size
    if (setsockopt(sd, SOL_SOCKET, SO_SNDBUF, &sendbuff, sizeof(sendbuff)) == -1) {
         printf("Failed to set option SO_SNDBUF!\n");
        return FALSE;
    }
    // Set receive buffer size
    if (setsockopt(sd, SOL_SOCKET, SO_RCVBUF, &recvbuff, sizeof(recvbuff)) == -1) {
         printf("Failed to set option SO_RCVBUF!\n");
        return FALSE;
    }
    // Set receive timeout
    if (setsockopt(sd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv) == -1) {
         printf("Failed to set option SO_RCVTIMEO!\n");
        return FALSE;
    }
    return TRUE;
}

DLL_EXPORT BOOL APIENTRY DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH:
            // attach to process
            // return FALSE to fail DLL load
            break;

        case DLL_PROCESS_DETACH:
            // detach from process
            break;

        case DLL_THREAD_ATTACH:
            // attach to thread
            break;

        case DLL_THREAD_DETACH:
            // detach from thread
            break;
    }
    return TRUE; // succesful
}
