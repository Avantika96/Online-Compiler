// CODE NAME,PORT
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <unistd.h>
#include<string.h>

int main(int argc, char **argv)
{
	char *file_path = "input.tmp";
	char *file_path2="out";
	file_path2=(char *)malloc(10*sizeof(char));
	char *server_hostname = "127.0.0.1";
	char buffer[BUFSIZ];
	char buffer2[10];
	in_addr_t in_addr;
	in_addr_t server_addr;
	int filefd;
	int sockfd;
	int i;
	int read_return;
	int readreturn;
	struct sockaddr_in sockaddr_in;
	unsigned short server_port = 12345;

	if (argc > 1) 
	{
		file_path = argv[1];
	       	if (argc > 2) 
		{
			server_port = strtol(argv[2], NULL, 10);
		}
	}

	filefd = open(file_path, O_RDONLY);
	if (filefd == -1) 
	{
		perror("open");
		exit(EXIT_FAILURE);
	}

	//socket
	sockfd = socket(AF_INET, SOCK_STREAM,0);
	if (sockfd == -1) 
	{
		perror("socket");
		exit(EXIT_FAILURE);
	}

	//sockaddr_in.sin_addr.s_addr = in_addr;
	sockaddr_in.sin_family = AF_INET;
	sockaddr_in.sin_port = htons(server_port);
	//if(inet_pton(AF_INET,argv[2], &sockaddr_in.sin_addr)<=0)
	if(inet_pton(AF_INET,server_hostname, &sockaddr_in.sin_addr)<=0)
	{
		printf("\n inet_pton error occured\n");
		return 1;
	}
	
	//connection
	if (connect(sockfd, (struct sockaddr*)&sockaddr_in, sizeof(sockaddr_in)) == -1) 
	{
		perror("connect");
		return EXIT_FAILURE;
	}

	//send
	while (1) 
	{
		read_return = read(filefd, buffer, BUFSIZ);
		if (read_return == 0)
			break;
		if (read_return == -1)
		{
			perror("read");
			exit(EXIT_FAILURE);
		}
		if (write(sockfd, buffer, read_return) == -1) 
		{
			perror("write");
			exit(EXIT_FAILURE);
		}
	}
	close(filefd);
	shutdown(sockfd,SHUT_WR);

	//recieve
	int ii, LEN = 5;
	srand(time(NULL));
	unsigned char text[LEN];
	for (ii = 0; ii < LEN; ii++) 
	{
		text[i] = (unsigned char) rand()%25+97;
	}

	strcpy(file_path2,text);	
	printf("%s",file_path);

	filefd = open(file_path2,O_WRONLY | O_CREAT | O_TRUNC,S_IRUSR | S_IWUSR);
        if (filefd == -1)
	{
		perror("open");
		exit(EXIT_FAILURE);
	}
	do 
	{
		read_return = read(sockfd, buffer, 255);
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
        }
	while (read_return > 0);
        	close(filefd);
	
	//printf("\n success"); 

	//grant exec per   
	char *grant;
	grant=(char *)malloc(200*sizeof(char));
	strcpy(grant,"sudo chmod +x ");
	strcat(grant,file_path2);
	system(grant);

	//run
	char *run;
	run=(char *)malloc(200*sizeof(char));
	strcpy(run,"./");
	strcat(run,file_path2);
	system(run);

exit(EXIT_SUCCESS);
}

