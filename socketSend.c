/*
    C ECHO client example using sockets
*/
#include<stdio.h> //printf
#include<string.h>    //strlen
#include<sys/socket.h>    //socket
#include<arpa/inet.h> //inet_addr
#include <unistd.h>
//opencv includes
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
int sock;
using namespace cv; 
void socket_prepare(int argc , const char *argv[])
{
	/**
	$program_name <junk> IP port
	**/
    struct sockaddr_in server;
    char message[1000] , server_reply[2000];
     
    //Create socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");
     
    server.sin_addr.s_addr = inet_addr(argv[2]);
    server.sin_family = AF_INET;
    server.sin_port = htons( atoi(argv[3]) );
 
    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        exit(602);
    }
     
    puts("Connected\n");
    return;
}
void sendMat(Mat& img){ 
	int count;
	char c;
        int imgSize = img.total()* img.elemSize(); 
       	count = send(sock, &img.rows, sizeof(int), 0);
	printf("rows: %d\n",img.rows); 
	printf("cols: %d\n",img.cols);
	 
	printf("Data length %d\n", count);

       	count = send(sock, &img.cols, sizeof(int), 0); 
	printf("Data length %d\n", count);

	count = send(sock, img.data, imgSize, 0);
	printf("Data length %d\n", count);

//	recv(sock,&c, 1 ,0);
//	printf("welcome %c\n", c);
    return; 
}
void get_letter_20(char* letter){
	int c = recv(sock, letter, 20, 0);
	letter[c] = '\0';
	return;
}
void socket_stop(){

    close(sock);
	return;
}
