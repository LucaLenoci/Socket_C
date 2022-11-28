/*
 ============================================================================
 Name        : Client.c
 Author      : Lenoci Luca
 =====================================================================
 */

#if defined WIN32
#include <winsock2.h>
#else
#define closesocket close
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#define PROTOPORT 609 //Default protocol port number
#define QLEN 6 //Size of request queue
#define BUFFERSIZE 512

void clearwinsock();
void errorhandler(char *);
void tokenizer(char * tokens[4], char *);
int check_if_sendable(char *token[4], int n_token);
size_t length_string(char *string);
int all_string_isdigit(char *string, size_t length);

int main(){

	#if defined WIN32
		WSADATA wsa_data;
		int start_res = WSAStartup(MAKEWORD(2, 2), &wsa_data);
		if (start_res != 0)
		{
			printf("%s", "Start up failed");
			return -1;
		}
		printf("%s", "Start up done\n");
	#endif

	//Creation of a Socket
	int c_sock;
	c_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (c_sock < 0)
	{
		errorhandler("Socket has not been created\n");
		closesocket(c_sock);
		clearwinsock();
		return -1;
	}
	printf("%s", "Socket has been created\n");

	//Struct creation for the server address
	char * ad_conn = "127.0.0.1";
	int port = PROTOPORT;
	struct sockaddr_in serv_ad;
	memset(&serv_ad, 0, sizeof(serv_ad));
	serv_ad.sin_family = AF_INET;
	serv_ad.sin_addr.s_addr = inet_addr(ad_conn);
	serv_ad.sin_port = htons(port);
	printf("%s %s %d\n", "Trying to establish connection with", ad_conn, port);

	//Establish connection with the server
	int server_connect;
	server_connect = connect(c_sock, (struct sockaddr *)&serv_ad, sizeof(serv_ad));
	if (server_connect < 0)
	{
		errorhandler("Connection failed\n");
		closesocket(c_sock);
		clearwinsock();
		return -1;
	}
	printf("%s", "Connected with server\n");

	char * s_in = malloc(BUFFERSIZE);
	memset(s_in, 0, BUFFERSIZE);

	while(1){
		memset(s_in, 0, BUFFERSIZE);
		printf("You can send operations to the server.\n");
		printf("The options are:\n");
		printf("1. + for addition\n");
		printf("2. - for subtraction\n");
		printf("3. x for multiplication\n");
		printf("4. / for division\n");
		printf("Insert the operand and two numbers.\nExample: \"+ 26 65\"\n");
		printf("To close the connection enter '=' character\n");
		printf("The server will send back the result -> ");
		fgets(s_in, BUFFERSIZE-1, stdin); //get the input string with size limit

		int s_len = strlen(s_in);

		//Control presence of new line char and remove it if is there
		if ((s_len > 0) && (s_in[s_len - 1] == '\n'))
		{
			s_in[s_len - 1] = '\0';
		}

		char * s_toks[4];
		char in_s_copy[BUFFERSIZE];
		strcpy(in_s_copy, s_in);
		tokenizer(s_toks, in_s_copy);
		int tokens_number = atoi(s_toks[3]);

		if(!strcmp(s_toks[0], "=") == 0){
			if(!(check_if_sendable(s_toks, tokens_number)==0)){

				printf("Send to server a valid operation\n");
				printf("Press Enter to continue\n");
				getchar();
				system("CLS");
				continue;
			}else{
				//Send data to the server
				int data_s;
				data_s = send(c_sock, s_in, s_len, 0);
				if (data_s != s_len)
				{
					errorhandler("Different number of bytes has been send to the server\n");
					closesocket(c_sock);
					clearwinsock();
					return -1;
				}
				printf("Data sent\n");

				//Retrieve data from server
				int bytes_r = 0;
				char server_s[BUFFERSIZE];
				memset(server_s, 0, BUFFERSIZE);
				printf("Data retrieved from server: ");
				bytes_r = recv(c_sock, server_s, BUFFERSIZE - 1, 0);
				if (bytes_r <= 0){
					errorhandler("Retrieve failed or connection closed prematurely\n");
					closesocket(c_sock);
					clearwinsock();
					return -1;
				}
				printf("%s\n", server_s);
				printf("Press Enter to continue\n");
				getchar();
				system("CLS");
				printf("%s%s\n%s%s\n", "Previous operation -> ", s_in, "Result -> ", server_s);
			}
		}else{
			send(c_sock, s_in, s_len, 0);
			printf("%s\n", "Closing connection with server\n");
			break;
		}
	}

	closesocket(c_sock);
	clearwinsock();
	printf("%s", "\n");
	system("PAUSE");

	return 0;
}


void clearwinsock(){
	#if defined WIN32
		WSACleanup();
	#endif
}

//Shows error message in output
void errorhandler(char * string){
	printf("%s", string);
}

//Do the Tokenizetion of the string
void tokenizer(char * tokens[4], char * string){
	char * operation = string;
	short temp = 0;
	char * token_string;
	token_string = strtok(operation, " ");

	if(strcmp(token_string, "=") == 0){
		tokens[0] = "=";
		tokens[1] = NULL;
		tokens[2] = NULL;
		tokens[3] = "1";
	}else{
		while (token_string != NULL){
			tokens[temp] = token_string;
			temp++;
			token_string = strtok(NULL, " ");
		}
		if(temp == 3){
			int temp_var = temp;
			char str_temp_var[BUFFERSIZE];
			memset(str_temp_var, 0, BUFFERSIZE);
			itoa(temp_var, str_temp_var, 10);
			tokens[temp] = str_temp_var;
		}
	}
}

//Check if the string that the user insert is valid to be sent to the server
int check_if_sendable(char * token[4], int n_token){
	size_t length_str_1 = length_string(token[1]);
	size_t length_str_2= length_string(token[2]);

	if(n_token == 3){
		if(strcmp(token[0],"+")==0||strcmp(token[0],"-")==0||strcmp(token[0],"*")==0||strcmp(token[0],"/")==0||strcmp(token[0],"=")==0){
			if(all_string_isdigit(token[1], length_str_1)==0){
				if(all_string_isdigit(token[2], length_str_2)==0){
					return 0;
				}
			}
		}
	}

	return 1;
}

//Return the length of the string
size_t length_string(char *string){
	return strlen(string);
}

//Check if the all string is formed by digits
int all_string_isdigit(char *string, size_t length){
	for(int i=0; i<length; i++){
		if(!isdigit(string[i])){
			return 1;
		}
	}

	return 0;
}
