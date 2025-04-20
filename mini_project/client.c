#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>     
#include <netinet/in.h>  
#include <arpa/inet.h>
#include <unistd.h>

#define BUFF_SIZE 256


		
//chat function
void chat_func(int server_fd,char *IP,int PORT)
{
    int numb_write, numb_read;
    char recvbuff[BUFF_SIZE];//cap phat buffer receive
    char sendbuff[BUFF_SIZE];
    while (1) {
        memset(sendbuff, '0', BUFF_SIZE);//set sendbuff ve 0
       	memset(recvbuff, '0', BUFF_SIZE);
        printf("Please enter the message : ");
        fgets(sendbuff, BUFF_SIZE, stdin);//nhap message vao sendbuff

        //write message vao  sendbuff
        numb_write = write(server_fd, sendbuff, sizeof(sendbuff));
        if (numb_write == -1)     
            perror("write fail");

	//exit
        if (strncmp("exit", sendbuff, 4) == 0) {
            printf("Client exit ...\n");
            break;
        }

	//myip
	if (strncmp("myip", sendbuff, 4) == 0)
       	{
            printf("%s\n",IP);
        }
	//myport
	if (strncmp("myport", sendbuff, 6) == 0)
       	{
            printf("%d\n",PORT);
        }

		
        //xi li server exit
        numb_read = read(server_fd, recvbuff, sizeof(recvbuff));
        if (numb_read < 0) 
            printf("read fail");
        if (strncmp("exit", recvbuff, 4) == 0) {
            printf("Server exit ...\n");
            break;
        }

        printf("\nMessage from Server: %s\n",recvbuff);   
    }
    close(server_fd);
}

int main(int argc, char *argv[])
{
    int port_num,server_fd;
	
    //xu li stdin
    if (argc < 3)
    {
        printf("command : ./client <server address> <port number>\n");
        exit(1);
    }

    port_num = atoi(argv[2]);//dua port_num ve int
   char *IP = argv[1];

    //tao struct luu thong tin IPV4
    struct sockaddr_in serv_addr;
    {
    	memset(&serv_addr, '0',sizeof(serv_addr));
    	serv_addr.sin_family = AF_INET;//internet domain
    	serv_addr.sin_port   = htons(port_num);//luu ve big endian
    	if (inet_pton(AF_INET, argv[1], &serv_addr.sin_addr) == -1) //chuyen IP sang bin luu vao serv_addr.sin_addr
    	printf("inet_pton fail");
    }

    //socket cho listen()
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)
        perror("create socket for listen fail");
	
    //connect()
    if (connect(server_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
        perror("connect fail");

   
    chat_func(server_fd,IP,port_num);

    return 0;
}
