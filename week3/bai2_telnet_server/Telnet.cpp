#include "stdafx.h"
#include "winsock2.h"

#define _CRT_SECURE_NO_WARNINGS

DWORD WINAPI ClientThread(LPVOID);

fd_set fdread;
fd_set fdwrite;
int numClients;
SOCKET clients[64];
int isCreateThread[64];

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

				const char *msg = "Please enter your username and password\n";
				send(client, msg, strlen(msg), 0);

				isCreateThread[numClients] = 0;

				clients[numClients] = client;
				numClients++;
			}

			for (int i = 0; i < numClients; i++) {
				if (FD_ISSET(clients[i], &fdread) && isCreateThread[i] == 0) {
					CreateThread(0, 0, &ClientThread, &clients[i], 0, 0);
					isCreateThread[i] = 1;
				}
			}
		}
	}

	return 0;
}

DWORD WINAPI ClientThread(LPVOID lpParam)
{
	SOCKET client = *(SOCKET *)lpParam;

	int res;
	char buf[128], filebuf[128], cmdbuf[128];

	char user[64], pass[64], tmp[64];

	const char *msg = "Wrong format. Please enter again\n";


	while (1)
	{
		res = recv(client, buf, sizeof(buf), 0);
		if (res == SOCKET_ERROR || res == 0)
		{
			closesocket(client);
			return 1;
		}

		buf[res] = 0;

		int n = sscanf(buf, "%s %s %s", user, pass, tmp);
		if (n != 2)
		{
			send(client, msg, strlen(msg), 0);
		}
		else
		{
			sprintf(buf, "%s %s", user, pass);
			FILE *f = fopen("D:\\Test\\data.txt", "r");
			int found = FALSE;
			while (fgets(filebuf, sizeof(filebuf), f) != NULL)
			{
				if (strncmp(buf, filebuf, strlen(buf)) == 0)
				{
					found = TRUE;
					break;
				}
			}
			fclose(f);

			if (found)
				break;
			else
			{
				msg = "Wrong user or pass. Please enter again\n";
				send(client, msg, strlen(msg), 0);
			}
		}
	}

	msg = "Please enter your command\n";
	send(client, msg, strlen(msg), 0);

	while (1)
	{
		res = recv(client, buf, sizeof(buf), 0);
		if (res == SOCKET_ERROR || res == 0)
		{
			closesocket(client);
			return 2;
		}

		buf[res] = 0;
		printf("%s", buf);

		// xoa ky tu ket thuc xau
		buf[res - 1] = 0;

		sprintf(cmdbuf, "%s > D:\\Test\\out.txt", buf);
		system(cmdbuf);

		FILE *f = fopen("D:\\Test\\out.txt", "r");
		while (fgets(filebuf, sizeof(filebuf), f) != NULL)
			send(client, filebuf, strlen(filebuf), 0);
		fclose(f);
	}
}

