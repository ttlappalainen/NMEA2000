
/*
NMEA2000_SocketCAN.cpp

2017 Copyright (c) Al Thomason   All rights reserved

Support the socketCAN access (ala, Linux, RPi)
See: https://github.com/thomasonw/NMEA2000_socketCAN
     https://github.com/ttlappalainen/NMEA2000


Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.


THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Inherited NMEA2000 object for socketCAN
See also NMEA2000 library.
*/


#include "NMEA2000_SocketCAN.h"

#include <stdio.h>
#include <iostream>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>

//*****************************************************************************
//  Pass in pointer to character array which contains (or will contain) the
//  string of the CANsocket to use in :open().   If no paramater is passed in,
//  or NULL is passed in, the defalt socket 'can0' will be used
tNMEA2000_SocketCAN::tNMEA2000_SocketCAN(char* CANport) : tNMEA2000()
{
    static char defaultCANport[] = "can0";

    if (CANport != NULL)
        _CANport = CANport;
    else
        _CANport = defaultCANport;                                                 // NULL passed in, set to port to default: CAN0

}



//*****************************************************************************
void tNMEA2000_SocketCAN::SetCANport(char *CANport) {
    if (CANport != NULL)
       _CANport = CANport;
}



//*****************************************************************************
bool tNMEA2000_SocketCAN::CANOpen() {
    struct ifreq ifr;
    struct sockaddr_can addr;
    int flags;

    //----  Open a socket to the CAN port
    skt = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if(skt < 0) {
        cerr << "Failed open CAN socket: " << _CANport << endl;
        return (false);
        }

    strncpy(ifr.ifr_name, _CANport, (sizeof(ifr.ifr_name)-1));
    ifr.ifr_name[sizeof(ifr.ifr_name)-1] = '\0';                                          //  (And make sure to null terminate)

    if (ioctl(skt, SIOCGIFINDEX, &ifr) < 0) {
        cerr << "Failed CAN ioctl: " << ifr.ifr_name << endl;
        return (false);
        }

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    if (bind(skt, (struct sockaddr *)&addr, sizeof(addr)) < 0)  {
        cerr << "Failed CAN bind" << endl;
        return (false);
        }


    //----- Set socket for non-blocking
    flags = fcntl(skt, F_GETFL, 0);
    if (flags < 0) {
        cerr << "Failed CAN flag fetch" << endl;
        return (false);
        }

    if (fcntl(skt, F_SETFL, flags | O_NONBLOCK) < 0) {
        cerr << "Failed CAN flag set" << endl;
        return (false);
        }

    return true;
}


//*****************************************************************************
bool tNMEA2000_SocketCAN::CANSendFrame(unsigned long id, unsigned char len, const unsigned char *buf, bool wait_sent) {
   struct can_frame frame_wr;

   frame_wr.can_id  = id | CAN_EFF_FLAG;
   frame_wr.can_dlc = len;
   memcpy(frame_wr.data, buf, 8);

   return (write(skt, &frame_wr, sizeof(frame_wr)) == sizeof(frame_wr));        // Send this frame out to the socketCAN handler

             // socketCAN works to keeping all packets in-order, so
             // no need to do anything special for wait-sent
}


//*****************************************************************************
bool tNMEA2000_SocketCAN::CANGetFrame(unsigned long &id, unsigned char &len, unsigned char *buf) {
    struct can_frame frame_rd;
    struct timeval tv = {0, 0};                                                 // Non-blocking timout when checking
    fd_set fds;

    FD_ZERO(&fds);
    FD_SET(skt, &fds);

    if (select((skt + 1), &fds, NULL, NULL, &tv) < 0)                           // Is there a FD with something to read out there?
        return false;

    if (FD_ISSET(skt, &fds)) {                                                  // Was it our CAN file-descriptor that is ready?
        if (read(skt, &frame_rd, sizeof(frame_rd)) > 0) {
            memcpy(buf, frame_rd.data, 8);
            len = frame_rd.can_dlc;
            id  = frame_rd.can_id;
            return true;
            }
        }
    return false;

}


//*****************************************************************************
tSocketStream::tSocketStream(const char *_port) : port(-1) {
  if ( _port!=0 ) {
    port=open(_port, O_RDWR | O_NOCTTY | O_NDELAY);
  }

  if ( port!=-1 ) {
    cout << _port << " opened" << endl;
  } else {
    if ( _port!=0 ) cerr << "Failed to open port " << _port << ". Using stdout"<< endl;
  }
}

//*****************************************************************************
tSocketStream::~tSocketStream() {
  if ( port!=-1 ) {
    close(port);
  }
}


/********************************************************************
*	Other 'Bridge' functions and classes
*
*
*
**********************************************************************/
int tSocketStream::read() {
  if ( port!=-1 ) {
    return -1;
  } else {
    // Serial stream bridge -- Returns first byte if incoming data, or -1 on no available data.
    struct timeval tv = { 0L, 0L };
    fd_set fds;

    FD_ZERO(&fds);
    FD_SET(0, &fds);
    if (select(1, &fds, NULL, NULL, &tv) < 0)                                   // Check fd=0 (stdin) to see if anything is there (timeout=0)
        return -1;                                                              // Nothing is waiting for us.

   return (getc(stdin));                                                         // Something is there, go get one char of it.
  }
}



//*****************************************************************************
size_t tSocketStream::write(const uint8_t* data, size_t size) {                // Serial Stream bridge -- Write data to stream.
  if ( port!=-1 ) {
    return ::write(port,data,size);
  } else {
    size_t i;

    for (i=0; (i<size) && data[i];  i++)                                        // send chars to stdout for 'size' or until null is found.
        putc(data[i],stdout);

    return(i);
  }
}


// std::this_thread::sleep_for(std::chrono::milliseconds(x));
// http://stackoverflow.com/questions/4184468/sleep-for-milliseconds

//*****************************************************************************
void delay(const uint32_t ms) {
    usleep(ms*1000);
};


//*****************************************************************************
uint32_t millis(void) {
    struct timespec ticker;

    clock_gettime(CLOCK_MONOTONIC, &ticker);
    return ((uint32_t) ((ticker.tv_sec * 1000) + (ticker.tv_nsec / 1000000)));

};




