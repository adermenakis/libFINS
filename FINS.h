/*
    libFINS is a library that allows communicating with Omron PLC devices
    through ethernet.
    
    Copyright (C) 2012  Alexandros Dermenakis
    email: alexandros.dermenakis@gmail.com

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
    MA  02110-1301  USA
*/

#ifndef __FINS_H_
#define __FINS_H_

#ifdef __cplusplus
extern "C" {
#endif 

struct sockaddr_in;
typedef unsigned char uchar;

#define FINS_COMMAND_MAX_SIZE 2010
typedef uchar FINS_cmd[FINS_COMMAND_MAX_SIZE];
typedef uchar* FINS_cmd_ptr;

typedef struct FINS_t
{
#ifdef __linux__
  struct sockaddr_in* local_addr;
  struct sockaddr_in* remote_addr;
  int socket_id;
#endif
  int is_connected;
  FINS_cmd_ptr gp_cmd;
} FINS_t;

FINS_t* FINS_create_eth_connection(const char* iface, const char* ip, const int port);
int FINS_connect(FINS_t * c);
int FINS_close(FINS_t * c);
int FINS_set_mode_run(FINS_t * c, const int mode);
int FINS_set_mode_stop(FINS_t * c);
int FINS_read(FINS_t * c, const int type, const int from, const int nb, unsigned short *oData);
int FINS_write(FINS_t * c, const int type, const int from, const int nb, const unsigned short *iData);
int FINS_is_connected(FINS_t * c);

void FINS_free(FINS_t *c);

#ifdef __cplusplus
};
#endif

#endif // __FINS_H_
