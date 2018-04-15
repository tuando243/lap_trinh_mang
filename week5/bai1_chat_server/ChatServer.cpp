// Server2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "winsock2.h"

#define _CRT_SECURE_NO_WARNINGS

int main()
{
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	struct idUser{
		char cmd[16], id[16], tmp[64];
	};

	SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(9000);

	bind(listener, (SOCKADDR *)&addr, sizeof(addr));
	listen(listener, 5);

	WSAEVENT serverEvent = WSACreateEvent();
	WSAEventSelect(listener, serverEvent, FD_ACCEPT);

	WSAEVENT events[64];
	SOCKET sockets[64];
	boolean isSignIn[64];
	int numEvents = 0;

	struct idUser user[64];

	SOCKET clients[64];
	int numClients = 0;

	events[numEvents] = serverEvent;
	sockets[numEvents] = listener;
	numEvents++;

	int ret;
	WSANETWORKEVENTS networkEvents;

	char buf[1024], sendbuf[1024];

	char *msg;

	while (1)
	{
		ret = WSAWaitForMultipleEvents(numEvents, events, FALSE, WSA_INFINITE, FALSE);
		if (ret == WSA_WAIT_FAILED)
			continue;

		int index = ret - WSA_WAIT_EVENT_0;
		for (int i = index; i < numEvents; i++)
		{
			ret = WSAWaitForMultipleEvents(1, &events[i], TRUE, 100, FALSE);

			if (ret == WSA_WAIT_FAILED)
				continue;

			if (ret == WSA_WAIT_TIMEOUT)
			{
				printf("Timed out\n");
				continue;
			}

			WSAEnumNetworkEvents(sockets[i], events[i], &networkEvents);
			WSAResetEvent(events[i]);

			if (networkEvents.lNetworkEvents & FD_ACCEPT)
			{
				if (networkEvents.iErrorCode[FD_ACCEPT_BIT] != 0)
					continue;

				SOCKET client = accept(listener, NULL, NULL);

				if (numEvents > WSA_MAXIMUM_WAIT_EVENTS)
				{
					printf("Too many connections\n");
					continue;
				}

				WSAEVENT clientEvent = WSACreateEvent();
				WSAEventSelect(client, clientEvent, FD_READ|FD_CLOSE);

				events[numEvents] = clientEvent;
				sockets[numEvents] = client;
				numEvents++;

				msg = "Please enter your id\n";

				send(client, msg, strlen(msg), 0);
			}
			else if (networkEvents.lNetworkEvents & FD_READ)
			{
				if (networkEvents.iErrorCode[FD_READ_BIT] != 0)
					continue;

				ret = recv(sockets[i], buf, sizeof(buf), 0);
				buf[ret] = 0;

					if (isSignIn[i] == true) {
						for (int j = 0; j < numEvents; j++) {
							if (j != i) {
								sprintf(sendbuf, "%s: %s", user[i].id, buf);
								send(sockets[j], sendbuf, strlen(sendbuf), 0);
							}				
						}
					}
					else {
						ret = sscanf(buf, "%s %s %s", user[i].cmd, user[i].id, user[i].tmp);
						if (ret != 2)
						{
							msg = "Wrong format. Please enter your id\n";
							send(sockets[i], msg, strlen(msg), 0);
						}
						else
						{
							if (strcmp(user[i].cmd, "client_id:") != 0)
							{
								msg = "Wrong format. Please enter your id\n";
								send(sockets[i], msg, strlen(msg), 0);
							}
							else
							{
								isSignIn[i] = true;				
								break;
							}
						}
					}
				}
			}
		}

	return 0;
}

