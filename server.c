// NAME,PORT
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <unistd.h>
#include<string.h>
#include<pthread.h>
#include<semaphore.h>

char *file_path="output";

char buffer[BUFSIZ],buffer2[BUFSIZ];

int client_sockfd,server_sockfd,*new_sock;

int filefd,file_fd,i,read_return,rd_rt;
socklen_t client_len;

sem_t s1;

void *thread_handler(void *agr);

int main(int argc, char **argv) 
{
	struct sockaddr_in client_address, server_address;
	unsigned short server_port = 12345;
    
	if (argc > 1) 
	{
	        file_path = argv[1];
		if(argc>2)
		{	
			server_port=strtol(argv[2],NULL,10);        
		}
	}


	//socket
	server_sockfd = socket(AF_INET,SOCK_STREAM,0);
	if (server_sockfd == -1) 
	{
		perror("socket");
		exit(EXIT_FAILURE);
	}
	
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	server_address.sin_port = htons(server_port);
	
	if (bind(server_sockfd,(struct sockaddr*)&server_address,sizeof(server_address)) == -1) 
	{
		perror("bind");
		exit(EXIT_FAILURE);
	}
	
	//listen
	if (listen(server_sockfd, 5) == -1) 
	{
	        perror("listen");
	        exit(EXIT_FAILURE);
	}
	
	fprintf(stderr, "listening on port %d\n", server_port);
	
	//take client and recieve code
	while (1) 
	{
		puts("waiting for client");
		client_sockfd = accept(server_sockfd,(struct sockaddr*)NULL,NULL);
		puts("connection accepted");
		
		pthread_t t1;
	
		sem_init(&s1,0,1);
		if(pthread_create(&t1,NULL,&thread_handler,NULL))
		{
			perror("could not create thread");
			return 1;
		}       
	}

	pthread_exit(NULL);
	return 1; 
}

void *thread_handler(void *agr)
{
	int count=0;
	char c[10];
	int i, LEN = 5;
	srand(time(NULL));
	unsigned char text[LEN];
	for (i = 0; i < LEN; i++) 
	{
		text[i] = (unsigned char) rand()%25+97;
	}
	strcpy(file_path,text);	
	strcat(file_path,".cpp");

	filefd = open(file_path,O_WRONLY | O_CREAT | O_TRUNC,S_IRUSR | S_IWUSR);

	if (filefd == -1) 
	{
		perror("open");
		exit(EXIT_FAILURE);
	}
	
	sem_wait(&s1);        
	do 
	{
		read_return = read(client_sockfd, buffer, BUFSIZ);
		if (read_return == -1) 
		{
			perror("read");
			exit(EXIT_FAILURE);
		}
		if (write(filefd, buffer, read_return) == -1) 
		{
			perror("write");
			exit(EXIT_FAILURE);
		}
        } while (read_return > 0);
        close(filefd);
	memset(buffer,'\0',BUFSIZ);
	
	//here compile
	char *cmd;
	cmd=(char *)malloc(20*sizeof(char*));
	strcpy(cmd,"g++ ");
	strcat(cmd,file_path);
	strcat(cmd," -o op");
	system(cmd);
	//here end
	//here 2 send back
	file_fd = open("op", O_RDONLY);
    	if (file_fd == -1) 
	{	
		perror("open");
		exit(EXIT_FAILURE);
	}

	while (1) 
	{
		rd_rt = read(file_fd, buffer2, 255);
		if (rd_rt == 0)
			break;
		if (rd_rt == -1) 
		{
			perror("read");
			exit(EXIT_FAILURE);
		}
		if (write(client_sockfd, buffer2, rd_rt) == -1) 
		{
			perror("write");
			exit(EXIT_FAILURE);
		}
	}
	//here 2 end
        close(client_sockfd);
	memset(buffer2,'\0',BUFSIZ);
	sem_post(&s1);
}
