#include <stdio.h>
#include "winsock2.h"
#include "conio.h"
#include "stdlib.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS
DWORD WINAPI ClientThread(LPVOID);
CRITICAL_SECTION cs;

int main()
{
	WSADATA wsa;
	int res;
	res = WSAStartup(MAKEWORD(2, 2), &wsa);
	if (res == SOCKET_ERROR)
	{
		res = WSAGetLastError();
		printf("Error Code: %d", res);
		return 1;
	}

	SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(8000);

	bind(listener, (SOCKADDR *)&addr, sizeof(addr));

	listen(listener, 5);

	while (1)
	{
		printf("Waiting for client...\n");
		int clientAddrLen = sizeof(addr);
		SOCKET client = accept(listener, (SOCKADDR *)&addr, &clientAddrLen);

		printf("Da ket noi voi client %s\n", inet_ntoa(addr.sin_addr));
		const char *msg = "Telnet Client. Pleases login\n";
		send(client, msg, strlen(msg), 0);

		// Tao nhieu luong khi client ket noi den
		CreateThread(0, 0, ClientThread, &client, 0, 0);
	}

	closesocket(listener);

	WSACleanup();

	return 0;
}

DWORD WINAPI ClientThread(LPVOID lpParams)
{
	bool isLoggedIn = false;
	SOCKET client = *(SOCKET *)lpParams;
	char userName[50], passwd[50];
	int res;

	//Xac thuc dang nhap kiem tra username va password
	while (1) {
		send(client, "UserName: ", 10, 0);
		res = recv(client, userName, sizeof(userName), 0);
		if (res == 0 || res == SOCKET_ERROR) {
			printf("Client disconnected.");
			closesocket(client);
			return 1;
		}
		if (res < 50)
			userName[res - 1] = 0;
		send(client, "Password: ", 11, 0);
		res = recv(client, passwd, sizeof(passwd), 0);
		if (res == 0 || res == SOCKET_ERROR) {
			printf("Client disconnected.");
			closesocket(client);
			return 1;
		}
		if (res < 50)
			passwd[res - 1] = 0;
		char formatLogin[50];
		sprintf(formatLogin, "%s %s",userName, passwd);
		EnterCriticalSection(&cs);
		FILE *f;
		if ((f = fopen("login.txt", "r")) == NULL)
		{ 
			perror("fopen source-file");
			return 1;
		}
		
		char line[50];
		while (fgets(line, 50, f) != NULL) {
			line[strlen(line) - 1] = '\0';
			if (strncmp(line, formatLogin, strlen(line)) == 0) {
				isLoggedIn = true;
				break;
			}
		}
		fclose(f);
		LeaveCriticalSection(&cs);

		if (isLoggedIn){
			printf("Dang nhap thanh cong");
			break;
		}
			
		else {
			printf("Sai user hoac mat khau");
			const char *msg = "Wrong User Name or password!\n";
			send(client, msg, strlen(msg), 0);
		}
	}
	while (1) {
		const char *Hellomsg = "cmd> ";
		send(client, Hellomsg, strlen(Hellomsg), 0);
		char cmd[100];
		res = recv(client, cmd, sizeof(cmd), 0);
		if (res == 0 || res == SOCKET_ERROR) {
			printf("Client disconnected.");
			break;
		}
		if (res < 100)
			cmd[res] = 0;
		FILE *fd = _popen(cmd, "r");
		if (fd == NULL) {
			fprintf(stderr, "Could not open pipe.\n");
			return 1;
		}
		char buff[1024];
		bool check = true;
		while (fgets(buff, sizeof(buff), fd) != NULL) {
			check = false;
			printf("%s", buff);
			send(client, buff, strlen(buff) + 1, 0);
		}
	}
	closesocket(client);
	return 0;
}