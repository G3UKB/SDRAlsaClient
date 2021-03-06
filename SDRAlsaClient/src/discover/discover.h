/*
discover.h

Header file for discover

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

/* Constants */
#define REMOTE_SERVER_PORT 1024
#define MAX_MSG 63
#define MAX_RESP 60
#define DISCOVERY_MSG 0
#define START_MSG 1
#define STOP_MSG 2
#define UNKNOWN_MSG -1
#define READ_FAILURE -2
#define DISCOVERY_RESP 3

// Prototypes
struct sockaddr_in *do_discover();
int do_start(int sd, struct sockaddr_in *svrAddr);
int do_stop(int sd, struct sockaddr_in *svrAddr);
