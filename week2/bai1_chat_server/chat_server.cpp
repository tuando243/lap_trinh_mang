#include <stdio.h>
#include "winsock2.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS

DWORD WINAPI ClientThread(LPVOID);
SOCKET connectedSocket[30];
int numberConnect = 0;

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
		const char *msg = "Xin chao! Hay thiet lap nick chat theo cu phap client_id: xxxxxxxx. \n";
		send(client, msg, strlen(msg), 0);

		// Them luong khi client ket noi toi
		CreateThread(0, 0, ClientThread, &client, 0, 0);
	}

	closesocket(listener);

	WSACleanup();

	return 0;
}

// remove socket
void removeSocket(SOCKET socketRemove) {
	int i;
	for (i = 0; i < numberConnect; i++)
		if (connectedSocket[i] == socketRemove)
			break;
	if (i < numberConnect) {
		for (int j = i; j < numberConnect; j++)
			connectedSocket[j] = connectedSocket[j + 1];
		numberConnect--;
	}
}

DWORD WINAPI ClientThread(LPVOID lpParams)
{
	SOCKET client = *(SOCKET *)lpParams;
	char buf[1024], temp[50], Hellomsg[100];
	int res;

	while (1)
	{
		res = recv(client, buf, sizeof(buf), 0);
		if (res == 0 || res == SOCKET_ERROR) {
			printf("Client disconnected.");
			closesocket(client);
			return 1;
		}
		if (res < 1024)
			buf[res] = 0;
		const char *regex = "client_id:";
		memcpy(&temp, &buf, strlen(regex));
		if (stricmp(temp, regex) == 0) {
			memcpy(&temp, &buf[strlen(regex)], strlen(buf) - strlen(regex) - 1);
			temp[strlen(buf) - strlen(regex) - 1] = 0;
			if (strlen(temp) == 0)
				continue;
			sprintf(Hellomsg, "Hi %s!. Send your message.\n", temp);
			send(client, Hellomsg, strlen(Hellomsg), 0);
			connectedSocket[numberConnect] = client;
			numberConnect++;
			break;

		}
		const char * errormsg = "Nhap ten nick dung cu phap: client_id:xxxxxx\n";
		send(client, errormsg, strlen(errormsg), 0);

	}

	//Chuyen tin nhan den cac client khac
	while (1) {
		res = recv(client, buf, sizeof(buf), 0);
		if (res == 0 || res == SOCKET_ERROR) {
			break;
		}
		if (res < 1024)
			buf[res] = 0;
		for (int i = 0; i < numberConnect; i++) {
			if (connectedSocket[i] != client) {
				char broadcastMSG[1024];
				sprintf(broadcastMSG, "%s : %s", temp, buf);
				send(connectedSocket[i], broadcastMSG, strlen(broadcastMSG), 0);
			}
		}

	}
	removeSocket(client);
	return 0;
}
