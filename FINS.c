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

#include "FINS.h"
#include <malloc.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>    

int FINS_ip_form_node(const char* ip)
{
  char* c = ip;
  int dot_cntr = 0;
  int char_cntr = 0;
  char* node_val = (char*)malloc(sizeof(char)*4);
  int cntr = 0;
  
  while(*c != '\0')
  {    
     if(dot_cntr == 3)
     {
       node_val[cntr++] = *c;
     }
   
    // count the dots
    if(*c == '.')
      dot_cntr++;
    
    char_cntr++;
    
     c = (ip+char_cntr);
  }
  
  return atoi(node_val);
}

int FINS_close(FINS_t* c)
{
  close(c->socket_id);
  c->socket_id = -1;
  
  return 0;
}

FINS_t* FINS_create_eth_connection(const char* iface, const char* ip, const int port)
{
  FINS_t *connection = (FINS_t*)malloc(sizeof(FINS_t));
  
  // Get The local IP
    int fd;
    struct ifreq ifr;

    fd = socket(AF_INET, SOCK_DGRAM, 0);

    // Use IPv4
    ifr.ifr_addr.sa_family = AF_INET;

    // Specify the interface we are using
    strncpy(ifr.ifr_name, iface, IFNAMSIZ-1);

    ioctl(fd, SIOCGIFADDR, &ifr);

    shutdown(fd, SHUT_RDWR);

  
    connection->local_addr = (struct sockaddr_in*)malloc(sizeof(struct sockaddr_in));
    bzero((char *)connection->local_addr, sizeof(struct sockaddr_in));
    connection->local_addr->sin_family = AF_INET;
    connection->local_addr->sin_addr.s_addr = htonl(INADDR_ANY);
    connection->local_addr->sin_port = htons(0);
    
    // Set the remote IP
    connection->remote_addr = (struct sockaddr_in*)malloc(sizeof(struct sockaddr_in));
    bzero((char *)connection->remote_addr, sizeof(struct sockaddr_in));
    connection->remote_addr->sin_family = AF_INET;
    connection->remote_addr->sin_addr.s_addr = inet_addr(ip);
    connection->remote_addr->sin_port = htons(port);
    
    connection->gp_cmd = malloc(sizeof(FINS_cmd));
    
    connection->gp_cmd[0] = 0x80;
    connection->gp_cmd[1] = 0x00;
    connection->gp_cmd[2] = 0x02;
    connection->gp_cmd[3] = 0x00;
    connection->gp_cmd[4] = (uchar)FINS_ip_form_node(ip);
    connection->gp_cmd[5] = 0x00;
    connection->gp_cmd[6] = 0x00;
    connection->gp_cmd[7] = (uchar)FINS_ip_form_node(inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
    connection->gp_cmd[8] = 0x00;
    connection->gp_cmd[9] = 0x00;
    
    return connection;
}

int FINS_connect(FINS_t* c)
{ 
    // Open UDP Socket
    c->socket_id = socket(AF_INET, SOCK_DGRAM, 0);
      
    //Bind to PORT
    if(bind(c->socket_id, (struct sockaddr *)&c->local_addr, sizeof(struct sockaddr_in)) < 0)
        return -1;
    
    return 0;
}

int FINS_set_mode_run(FINS_t * c, const int mode)
{
    FINS_cmd fins_resp;

    //Select RUN command
    c->gp_cmd[10] = 0x04;
    c->gp_cmd[11] = 0x01;

    //Set program number to 0000
    c->gp_cmd[12] = 0x00;
    c->gp_cmd[13] = 0x00;

    c->gp_cmd[14] = mode;

    static int sendlen = 15;

    if(sendto(c->socket_id, c->gp_cmd, sendlen, 0,
    (struct sockaddr *)c->remote_addr, sizeof(struct sockaddr_in)) != sendlen)
        return -1;

    // Confirm arival of package
    if(recv(c->socket_id, fins_resp, FINS_COMMAND_MAX_SIZE, 0) < 0)
        return -1;

    return 0;
}

int FINS_set_mode_stop(FINS_t * c)
{
    FINS_cmd fins_resp;

    //Select STOP command
    c->gp_cmd[10] = 0x04;
    c->gp_cmd[11] = 0x02;

    static int sendlen = 12;

    if(sendto(c->socket_id, c->gp_cmd, sendlen, 0,
    (struct sockaddr *)c->remote_addr, sizeof(struct sockaddr_in)) != sendlen)
        return -1;

    // Confirm arival of package
    if(recv(c->socket_id, fins_resp, FINS_COMMAND_MAX_SIZE, 0) < 0)
        return -1;

    return 0;

}

int FINS_read(FINS_t* c, const int type, const int from,
		const int nb, short unsigned int* oData)
{
    FINS_cmd fins_resp;
  
  // Select the read command
    c->gp_cmd[10] = 0x01;
    c->gp_cmd[11] = 0x01;
    
    // Setting memory type
    c->gp_cmd[12] = type;
    
    // Set Starting address
    c->gp_cmd[13] = (uchar)((from & 0xFF00) >> 8);
    c->gp_cmd[14] = (uchar)(from & 0x00FF);
    c->gp_cmd[15] = 0x00;
    
    // Set the number of items send though the command
    c->gp_cmd[16] = (uchar)((nb & 0xFF00) >> 8);
    c->gp_cmd[17] = (uchar)(nb & 0x00FF);

    // The length is always 18 when reading since we are not sending any
    // Data to the device
    static int sendlen = 18;
    // Send a UDP package
    if(sendto(c->socket_id, c->gp_cmd, sendlen, 0,
      (struct sockaddr *)c->remote_addr, sizeof(struct sockaddr_in))!=sendlen)
    {
        return -1;
    }

    // Receive the response from the OMRON and
    // read memory blocks received
    if(recv(c->socket_id, fins_resp, FINS_COMMAND_MAX_SIZE, 0) < 0)
    {
        return -1;
    }
    else
    {
	int i = 0;
        for(; i < nb; i++)
	{
            *oData++ = (fins_resp[14+2*i] << 8) | fins_resp[15+2*i];
        }
    }

    return 0;
}

int FINS_write(FINS_t* c, const int type, const int from,
	       const int nb, const short unsigned int* iData)
{
    FINS_cmd fins_resp;
    // Set the write command
    c->gp_cmd[10] = 0x01;
    c->gp_cmd[11] = 0x02;
    
    // Set thte memory type
    c->gp_cmd[12] = (uchar)type;
    
    // Select the starting address
    c->gp_cmd[13] = (uchar)((from & 0xFF00) >> 8);
    c->gp_cmd[14] = (uchar)(from & 0x00FF);
    c->gp_cmd[15] = 0x00;
    
    // Send the number of items that have been requested
    // to read from the device
    c->gp_cmd[16] = (uchar)((nb & 0xFF00) >> 8);
    c->gp_cmd[17] = (uchar)(nb & 0x00FF);
    
    // Populate command with the data we are sending
    int i = 0;
    for(; i < nb; i++)
    {
        c->gp_cmd[18+(2*i)] = (uchar)((*iData & 0xFF00) >> 8);
        c->gp_cmd[19+(2*i)] = (uchar)(*iData++ & 0x00FF);
    }
    

    int sendlen = 18+nb*2;
    // Send the command
    if(sendto(c->socket_id, c->gp_cmd, sendlen, 0,
	(struct sockaddr *)c->remote_addr, sizeof(struct sockaddr_in)) != sendlen)
        return -1;

    // Confirm arival of package
    if(recv(c->socket_id, fins_resp, FINS_COMMAND_MAX_SIZE, 0) < 0)
        return -1;

    return 0;
}

int FINS_is_connected(FINS_t* c)
{
  return c->is_connected;
}


void FINS_free(FINS_t* c)
{
  free(c->remote_addr); c->remote_addr = 0;
  free(c->local_addr); c->remote_addr = 0;
  free(c->gp_cmd); c->gp_cmd = 0;
}
