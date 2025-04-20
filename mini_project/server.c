#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>   
#include <netinet/in.h>  
#include <arpa/inet.h>
#include <unistd.h>

#define LISTEN_BACKLOG 50
#define BUFF_SIZE 256


/* Chức năng chat */
void chat_func(int new_socket_fd,char *client_IP,int port_no)
{       
    int numb_read, numb_write;
    char sendbuff[BUFF_SIZE];
    char recvbuff[BUFF_SIZE];
	
    while (1) {        
        memset(sendbuff, '0', BUFF_SIZE);
        memset(recvbuff, '0', BUFF_SIZE);

        //doc du lieu tu new_socket_fd luu vao rcvbuff
        numb_read = read(new_socket_fd, recvbuff, BUFF_SIZE);
        if(numb_read == -1)
            perror("read()");

	//chuc nang exit
        if (strncmp("exit", recvbuff, 4) == 0) {
            system("clear");
            break;
        }
	//chuc nang myport
       if (strncmp("myport", recvbuff, 6) == 0) {
            printf("%d\n",port_no);
            //break;
        }
	//chuc nang myip
       if (strncmp("myip", recvbuff, 4) == 0) {
            printf("IP: %s\n",client_IP);
            //break;
        }


        printf("\nMessage from Client: %s\n", recvbuff);



        //nhap du lieu in ra screen
        printf("Please respond the message : ");
        fgets(sendbuff, BUFF_SIZE, stdin);

        //ghi du lieu vao 
        numb_write = write(new_socket_fd, sendbuff, sizeof(sendbuff));
        if (numb_write == -1)
            perror("write()");
        
        if (strncmp("exit", sendbuff, 4) == 0) {
            system("clear");
            break;
        }

        sleep(1);
    }
    close(new_socket_fd);
}

int main(int argc, char *argv[])
{
    int port_no, len, opt;
    int server_fd, new_socket_fd;
    struct sockaddr_in serv_addr, client_addr;



    //xoa port address already in use
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
        perror("setsockopt()");



    //kiem tra command line
    if (argc < 2) {
        printf("No port provided\ncommand: ./server <port number>\n");
        exit(EXIT_FAILURE);
    } else
        port_no = atoi(argv[1]);

    memset(&serv_addr, 0, sizeof(struct sockaddr_in));
    memset(&client_addr, 0, sizeof(struct sockaddr_in));
    
    //tao socket cho client va server
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)
        perror("socket()");
    
    //xoa port address already in use
    //if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    //    perror("setsockopt()");  

    //luu thong tin  dia chi IPV4
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port_no);
    serv_addr.sin_addr.s_addr =  INADDR_ANY; //inet_addr("192.168.5.128"); //INADDR_ANY

    //bind()
    if (bind(server_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
        perror("bind()");

    //listen() toi da 5 ket noi
    if (listen(server_fd, LISTEN_BACKLOG) == -1)
        perror("listen()");

	len = sizeof(client_addr);

    while (1) {
        printf("Server starting...\n");
		//tao socket cho ket noi moi
		new_socket_fd  = accept(server_fd, (struct sockaddr*)&client_addr, (socklen_t *)&len); 
		if (new_socket_fd == -1)
            		perror("accept()");

		system("clear");

		char client_IP[BUFF_SIZE];
		inet_ntop(client_addr.sin_family, (struct sockaddr*)&client_addr, client_IP, sizeof(client_IP));
		printf("Server : got connection \n");
		chat_func(new_socket_fd,client_IP,port_no);

		
    }
    close(server_fd);
    return 0;
}
