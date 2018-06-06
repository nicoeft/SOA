#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/resource.h>
#include <sys/time.h>


main (int argc, char *argv[])
{
  int connectionFD;
  int ret;
  char buff[80];
  char buff2[80];
  char *server;
  int puerto;

  server = argv[1];
  puerto = atoi (argv[2]);
  sprintf (buff, "%s", argv[3]);
  connectionFD = clientConnection (server, puerto);
  if (connectionFD < 0)
    {
      perror ("Error establishing connection\n");
      exit (1);
    }
	ret = write (connectionFD, buff, strlen(buff));
  if (ret < 0)
    {
      perror ("Error writing to connection\n");
      exit (1);
    }
  while(1){
    int size;
    ret = read (connectionFD, &size, sizeof (int));
    if (ret < 0)
    {
      perror ("Error reading integer on connection\n");
      exit (1);
    }
    char message[size];
    ret = read (connectionFD, message, size);
    if (ret < 0)
    {
      perror ("Error reading on connection\n");
      exit (1);
    }
	  buff[ret] = '\0';
	  sprintf (buff2, "Client [%d] received: %s\n",getpid(), buff);
	  write(1,buff2,strlen(buff2)); 
  }


}
