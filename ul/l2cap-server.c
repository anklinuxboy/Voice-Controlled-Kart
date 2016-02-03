/* Prepared by Team BlueGum
 Juan Jhong Chung, Laura Kamfonik, & Ankit Sharma
 EC535 Fall 2014 Final Project
 Original Source code by Albert Huang
 from the BlueZ programming tutorial */

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/l2cap.h>

int main(int argc, char **argv)
{
/* Bluetooth Setup:
   Create a socket and assign the Gumstix
   Bluetooth Address to it, using port 0x1001, and
   allow any address to connect to it. */

    struct sockaddr_l2 loc_addr = { 0 }, rem_addr = { 0 }, addr = {0};

    char buf[1024] = { 0 };
    int s, client, bytes_read;
    socklen_t opt = sizeof(rem_addr);
    int fd, res, status;


    s = socket(AF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);
      
    loc_addr.l2_family = AF_BLUETOOTH;
    loc_addr.l2_bdaddr = *BDADDR_ANY;
    loc_addr.l2_psm = htobs(0x1001);

    str2ba(dest, &addr.l2_bdaddr);

    bind(s, (struct sockaddr *)&loc_addr, sizeof(loc_addr));

    // put socket into listening mode
    listen(s, 1);

    // accept one connection
    client = accept(s, (struct sockaddr *)&rem_addr, &opt);
    
    ba2str( &rem_addr.l2_bdaddr, buf );
    fprintf(stderr, "accepted connection from %s\n", buf);

    /* */
    while (1)
    {
	fd = open("/proc/robokart", O_RDWR);
    	memset(buf, 0, sizeof(buf));
    	/* read data from the client */
    	bytes_read = read(client, buf, sizeof(buf));
    	if( bytes_read > 0 ) {
            printf("received [%s]\n", buf);
	    res = write(fd, buf, sizeof(buf));
			
	     if (strstr(buf,"bye") != NULL)
	         break;
	}
    }

    //close connection
    close(fd);
    close(client);
    close(s);
}
