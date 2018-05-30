#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

int f=10;
int vect[10];
int p=0,u=0;
sem_t avisa;
sem_t exclusion;

void doService(int fd);

void *accept_and_do(void * v){
	while(1){
    sem_wait(&avisa);
    
		sem_wait(&exclusion);
			int connectionFD = vect[p];
			p=(p+1)%10;
			f++;
		sem_post(&exclusion);
		doService(connectionFD);
	}
}

void doService(int fd) {
int i = 0;
char buff[80];
char buff2[80];
int ret;
int socket_fd = (int) fd;
   
    ret = read(socket_fd, buff, sizeof(buff));

    while(ret > 0) {
        buff[ret]='\0';
        sprintf(buff2, "Server [%d] received: %s\n", getpid(), buff);
        write(1, buff2, strlen(buff2));
        ret = write(socket_fd, "caracola ", 8);
        if (ret < 0) {
            perror ("Error writing to socket");
            exit(1);
        }
        ret = read(socket_fd, buff, sizeof(buff));
    }
    if (ret < 0) {
            perror ("Error reading from socket");

    }
    sprintf(buff2, "Server [%d] ends service\n", getpid());
    write(1, buff2, strlen(buff2));

}


main (int argc, char *argv[])
{
  
  int socketFD;
  int connectionFD;
  char buffer[80];
  int ret;
  int port;
  pthread_t pool[10];
 
 
  if (argc != 2)
    {
      strcpy (buffer, "Usage: ServerSocket PortNumber\n");
      write (2, buffer, strlen (buffer));
      exit (1);
    }
   
   sem_init(&avisa, 0,0);
   sem_init(&exclusion, 0,1);
	
  port = atoi(argv[1]);
  socketFD = createServerSocket (port);
  if (socketFD < 0)
    {
      perror ("Error creating socket\n");
      exit (1);
    }
    
    for(int i=0;i<10;i++){
      int ret = pthread_create( &pool[i], NULL, accept_and_do, NULL);
    if (ret < 0) {
            perror ("Error creating the thread");
            exit(1);
        }
    }
    
while (1) {
	
      connectionFD = acceptNewConnections(socketFD);

      if (connectionFD < 0)
      {
          perror ("Error establishing connection \n");
          deleteSocket(socketFD);
          exit (1);
      }
      
		while(1){
			sprintf(buffer, "%d\n", f);
			write(1, buffer, strlen(buffer));
			if(f>0){
				sem_wait(&exclusion);
				vect[u]=connectionFD;
				u=(u+1)%10;
				f--;
				sem_post(&avisa);
				sem_post(&exclusion);
				break;
			}
			sprintf(buffer, "%d\n", f);
			write(1, buffer, strlen(buffer));
		}
		
				
  }

}
