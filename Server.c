#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <pthread.h>

#define PORT 6969 
#define THREADS_MAX 4

void *slave(void *param){
	int new_socket = *((int *)(param));
	int valread;
	char buffer[1024] = {0}; 
    	char *hello = "Hello from server"; 

	valread = read( new_socket , buffer, 1024); 
        printf("%s\n",buffer ); 
        send(new_socket , hello , strlen(hello) , 0 ); 
        printf("Hello message sent\n"); 
	close(new_socket);		
}

void *masterThread(void *param)
{
    int server_fd, new_socket; 
    struct sockaddr_in address; 
    int opt = 1; 
    int addrlen = sizeof(address); 
    
       
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    { 
        perror("socket failed"); 
        exit(EXIT_FAILURE); 
    } 
       
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) 
    { 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
    } 
    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons( PORT ); 
       
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 
    if (listen(server_fd, 3) < 0) 
    { 
        perror("listen"); 
        exit(EXIT_FAILURE); 
    } 
	
       pthread_t threads[THREADS_MAX];
       int thread_args[THREADS_MAX];
       
    
    while(1){        
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) 
        { 
            perror("accept"); 
            exit(EXIT_FAILURE); 
        } 

	int i;
       for (i = 0; i < THREADS_MAX; i++)
       {
               pthread_create(&threads[i], NULL, slave, (void *) 								&new_socket);
	}
    }
       pthread_exit(NULL);
}


int main(int argc, char const *argv[]) 
{ 
    pthread_t master;
    
    pthread_create(&master, NULL, masterThread, (void *) NULL);
    pthread_join(master, NULL);

    return 0; 
} 
