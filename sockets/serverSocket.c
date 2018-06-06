#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int broadcast[256] = {0};

doService(int fd) {
int i = 0;
char buff[80];
char buff2[80];
int ret;

	ret = read(fd, buff, sizeof(buff));
	if (ret < 0) {
			perror ("Error reading from socket");

	}
	while(i<256 && broadcast[i]>0){
		int size=13;
		ret = write(broadcast[i],&size, sizeof(size));
		if (ret < 0) {
			perror ("Error writing to number");
			exit(1);
		}
		ret = write(broadcast[i], buff, sizeof(buff));
		if (ret < 0) {
			perror ("Error writing to socket");
			exit(1);
		}
		i++;
	}
	
}


main (int argc, char *argv[])
{
  int socketFD;
  int connectionFD;
  char buffer[80];
  int ret;
  int port;


  if (argc != 2)
    {
      strcpy (buffer, "Usage: ServerSocket PortNumber\n");
      write (2, buffer, strlen (buffer));
      exit (1);
    }

  port = atoi(argv[1]);
  socketFD = createServerSocket (port);
  if (socketFD < 0)
    {
      perror ("Error creating socket\n");
      exit (1);
    }
	int i=0;
  while (1) {
	  connectionFD = acceptNewConnections (socketFD);
	  if (connectionFD < 0)
	  {
		  perror ("Error establishing connection \n");
		  close(socketFD);
		  exit (1);
	  }
		broadcast[i]= connectionFD;
		i++;
	  doService(connectionFD);
  }

}
