/*
 ============================================================================
 Name        : Server.c
 Author      : Lenoci Luca
 =====================================================================
 */

#if defined WIN32
	#include <winsock.h>
#else
	#define closesocket close
	#include <sys/socket.h>
	#include <arpa/inet.h>
	#include <unistd.h>
#endif


#include "../../headers/headers.h"

#define NO_ERROR 0
#define BUFFERSIZE 512
#define PROTOPORT 609
#define SERVER_ADDRESS "127.0.0.1"
#define QLEN 5

int operation_switcher(char *, char *, char *, char *);
void tokenizer(char * [4], char *);

int main(int argc, char *argv[])
{

	int port;
	char * server_ad_start = "";
	if (argc > 1){
		server_ad_start = argv[1];
		port = atoi(argv[2]);
	}else{
		port = PROTOPORT;
		server_ad_start = SERVER_ADDRESS;
		if (port < 0){
			printf("%d is a bad port number\n", port);
			return 0;
		}
		printf("Port available\n");
	}
	printf("%s %s:%d\n", "Starting up server with", server_ad_start, port);

	#if defined WIN32
		WSADATA wsa_data;
		int result;
		result = WSAStartup(MAKEWORD(2, 2), &wsa_data);
		if (result != NO_ERROR)
		{
			errorhandler("Start up failed\n");
			return 0;
		}
		printf("Start up done\n");
	#endif

	// Creation of a Socket
	int s_sock;
	s_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (s_sock < 0)
	{
		errorhandler("Socket creation failed\n");
		clearwinsock();
		return -1;
	}
	printf("Socket created\n");

	//Struct creation for the server address
	struct sockaddr_in s_ad;
	memset(&s_ad, 0, sizeof(s_ad));
	s_ad.sin_family = AF_INET;
	s_ad.sin_addr.s_addr = inet_addr(server_ad_start);
	s_ad.sin_port = htons(port);
	int server_bind;
	server_bind = bind(s_sock, (struct sockaddr *)&s_ad, sizeof(s_ad));
	if (server_bind < NO_ERROR)
	{
		errorhandler("Binding failed\n");
		closesocket(s_sock);
		clearwinsock();
		return -1;
	}
	printf("Binding done\n");

	//Making the server listen
	int listen_res;
	listen_res = listen(s_sock, QLEN-1);
	if (listen_res < NO_ERROR)
	{
		errorhandler("Listening failed\n");
		closesocket(s_sock);
		clearwinsock();
		return -1;
	}
	printf("Server is listening\n");

	printf("Server waiting for a client connection\n");

	//Accept the connection
	struct sockaddr_in c_ad;
	int c_sock, c_len;
	int continue_loop = 1;
	while (1)
	{
		c_len = sizeof(c_ad);
		c_sock = accept(s_sock, (struct sockaddr *)&c_ad, &c_len);
		if (c_sock < NO_ERROR)
		{
			errorhandler("Client not accepted\n");
			closesocket(c_sock);
			clearwinsock();
			return 0;
		}
		printf("%s%s%s%d\n", "Successful connection with ", inet_ntoa(c_ad.sin_addr), ":", ntohs(c_ad.sin_port));
		char c_data[BUFFERSIZE];
		continue_loop = 1;
		do{
			memset(c_data, 0, BUFFERSIZE);
			recv(c_sock, c_data, BUFFERSIZE - 1, 0);
			//Control presence of new line char and remove it if is there
			int string_len = strlen(c_data);
			if ((string_len > 0) && (c_data[string_len - 1] == '\n'))
			{
				c_data[string_len - 1] = '\0';
			}
			printf("\n%s ", "Data received from client: ");
			printf("%s\n", c_data);

			char * str_toks[4];
			tokenizer(str_toks, c_data);
			if(strcmp(str_toks[0], "=") == 0){
				continue_loop = 0;
				printf("%s%s\n", "Connection closed with client ", inet_ntoa(c_ad.sin_addr));
			}else{
				printf("Server is performing operation...\n");
				char op_res[BUFFERSIZE];
				memset(op_res, 0, BUFFERSIZE);
				int switcher = operation_switcher(str_toks[0], str_toks[1], str_toks[2], str_toks[3]);
				itoa(switcher, op_res, 10);
				string_len = strlen(op_res);
				Sleep(1000);
				printf("%s %s \n", "Operation result -> ", op_res);
				send(c_sock, op_res, string_len, 0);
			}
		}while(continue_loop);
	}

	system("PAUSE");

	return 0;
}

int operation_switcher(char * op, char * number_1, char * number_2, char * num_tok){

	int token_number = atoi(num_tok);
	if(token_number < 3){
		printf("%s", "Parsing error");
		return -999999999;
	}
	int first_n = atoi(number_1);
	int second_n = atoi(number_2);
	int op_r;

	switch(op[0]){
		case '+':
			op_r = add(first_n, second_n);
			break;
		case '-':
			op_r = sub(first_n, second_n);
			break;
		case 'x':
			op_r = mult(first_n, second_n);
			break;
		case '/':
			if(second_n == 0) {
				printf("%s", "Cannot divide by zero\n");
				return -999999999;
			}
			op_r = division(first_n, second_n);
			break;
	}

	return op_r;
}

void tokenizer(char * toks[4], char * string){
	char * operation = string;
	short temp = 0;
	char * token_string;
	token_string = strtok(operation, " ");

	if(strcmp(token_string, "=") == 0){
		toks[0] = "=";
		toks[1] = NULL;
		toks[2] = NULL;
		toks[3] = "3";
	}else{
		while (token_string != NULL){
			toks[temp] = token_string;
			temp++;
			token_string = strtok(NULL, " ");
		}
		if(temp == 3){
			int temp_var = temp;
			char temp_str[BUFFERSIZE];
			memset(temp_str, 0, BUFFERSIZE);
			itoa(temp_var, temp_str, 10);
			toks[temp] = temp_str;
		}
	}
}
