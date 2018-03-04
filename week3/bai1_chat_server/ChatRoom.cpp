
#include "stdafx.h"
#include "winsock2.h"

#define _CRT_SECURE_NO_WARNINGS

DWORD WINAPI ClientThread1(LPVOID);

SOCKET clients[64];
int isCreateThread[64];
int numClients;
fd_set fdread;
fd_set fdwrite;

int main()
{
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(8000);

	SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	bind(listener, (SOCKADDR *)&addr, sizeof(addr));
	listen(listener, 5);

	numClients = 0;
	int res;

	while (1)
	{
		FD_ZERO(&fdread);
		FD_SET(listener, &fdread);

		for (int i = 0; i < numClients; i++)
			FD_SET(clients[i], &fdread);

		res = select(0, &fdread, &fdwrite, NULL, NULL);
		if (res == SOCKET_ERROR)
			break;
		if (res > 0)
		{
			if (FD_ISSET(listener, &fdread))
			{
				// su kien nhan ket noi
				SOCKET client = accept(listener, NULL, NULL);

				const char *msg = "Please enter your id\n";
				send(client, msg, strlen(msg), 0);

				isCreateThread[numClients] = 0;

				clients[numClients] = client;
				numClients++;
			}

			for (int i = 0; i < numClients; i++) {
				if (FD_ISSET(clients[i], &fdread) && isCreateThread[i] == 0) {
					CreateThread(0, 0, &ClientThread1, &clients[i], 0, 0);
					isCreateThread[i] = 1;
				}
			}
		}
	}

	return 0;
}

void RemoveClient(SOCKET client)
{
	int i;
	for (i = 0; i < numClients; i++)
		if (clients[i] == client)
			break;
	// gan phan tu tai i bang phan tu tai numClients - 1
	if (i < numClients - 1)
		clients[i] = clients[numClients - 1];

	isCreateThread[i] = 0;
	numClients--;
}

DWORD WINAPI ClientThread1(LPVOID lpParam)
{
	SOCKET client = *(SOCKET *)lpParam;

	int res;
	char buf[128], sendbuf[128];

	char cmd[16], id[16], tmp[64];

	const char *msg = "Wrong format. Please enter your id\n";
	//send(client, msg, strlen(msg), 0);

	while (1)
	{
		res = recv(client, buf, sizeof(buf), 0);
		if (res == SOCKET_ERROR || res == 0)
		{
			closesocket(client);
			return 1;
		}

		buf[res] = 0;

		res = sscanf(buf, "%s %s %s", cmd, id, tmp);
		if (res != 2)
		{
			send(client, msg, strlen(msg), 0);
		}
		else
		{
			if (strcmp(cmd, "client_id:") != 0)
			{
				msg = "Wrong format. Please enter your id\n";
				send(client, msg, strlen(msg), 0);
			}
			break;
		}
	}

	msg = "Correct format. Please enter your message\n";
	send(client, msg, strlen(msg), 0);

	while (1)
	{
		res = recv(client, buf, sizeof(buf), 0);
		if (res == SOCKET_ERROR || res == 0)
		{
			RemoveClient(client);
			closesocket(client);
			return 2;
		}

		buf[res] = 0;
		printf("%s", buf);

		sprintf(sendbuf, "%s: %s", id, buf);

		for (int i = 0; i < numClients; i++)
			if (clients[i] != client)
				send(clients[i], sendbuf, strlen(sendbuf), 0);
	}
}