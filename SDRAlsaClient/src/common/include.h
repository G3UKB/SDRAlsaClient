/*
include.h

'C' includes for the FCD Client application

Copyright (C) 2014 by G3UKB Bob Cowdery

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

#ifndef _include_h
#define _include_h

// System headers
#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>


// Library headers
#include "../../libs/portaudio/include/portaudio.h"
#include "../../libs/pthreads/include/pthread.h"

// Application headers
#include "defs.h"
#include "../ringbuffer/ringb.h"
#include "../audio/port_audio.h"

#endif
