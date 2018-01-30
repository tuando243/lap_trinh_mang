#include <iostream>
#include <winsock2.h>

int main(int argc, char** argv) {
	
	// Kiem tra tham so dau vao
	if (argc != 4) {
		printf("Input error\n>e.g. 'TCPServer.exe 8888 chao.txt client.txt'\n");
		system("pause");
		return 0;
	}

	WSADATA wsaData;
	SOCKET listeningSocket;
	SOCKET newConnection;
	SOCKADDR_IN serverAddr;
	SOCKADDR_IN clientAddr;
	int port = atoi(argv[1]);

	// Khoi tao winsock 2.2
	if (WSAStartup(MAKEWORD(2, 2), &wsaData)) {
		printf("Version is not supported\n");
		system("pause");
		return 0;
	}

	// Tao socket lang nghe ket noi tu client
	listeningSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// Khoi tao cau truc SOCKADDR_IN cua server
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	// Bind socket cua server
	if (bind(listeningSocket, (SOCKADDR *)&serverAddr, sizeof(serverAddr))) {
		printf("Cannot bind this address\n");
		system("pause");
		return 0;
	}

	// Chuyen sang trang thai doi ket noi
	if (listen(listeningSocket, 5)) {
		printf("Cannot listen\n");
		system("pause");
		return 0;
	}

	printf("Server started\n");

	int clientAddrLen;
	char helloFile[64];
	char clientFile[64];

	strcpy(helloFile, argv[2]);
	strcpy(clientFile, argv[3]);

	// Chap nhan ket noi moi
	clientAddrLen = sizeof(clientAddr);
	newConnection = accept(listeningSocket, (SOCKADDR *)&clientAddr, &clientAddrLen);

	// Neu ket noi thanh cong thuc hien gui, nhan du lieu tu client
	if (newConnection) {
		printf("Ket noi thanh cong\n");

		// Gui xau chao cho client tu file chi dinh
		char buf[256];
		FILE * file;
		file = fopen(helloFile, "r");
		if (file)
			while (fscanf(file, "%s", buf) != EOF)
		fclose(file);
		int res = send(newConnection, buf, strlen(buf), 0);
		if (res == SOCKET_ERROR) {
			printf("Socket error\n");
			return 0;
		}	
		
		// Luu du lieu nhan duoc tu client vao file chi dinh
		file = fopen(clientFile, "a");
		while (1) {
			res = recv(newConnection, buf, sizeof(buf), 0);
			buf[res] = '\0';
			printf("Client: %s\n", buf);

			// Neu xau nhan duoc la "exit" -> Ket thuc chuong trinh
			if (strcmp(buf, "exit") == 0) {
				printf("Server stopped\n");
				break;
			}
			fprintf(file, "%s\n", buf);		
		}
		fclose(file);
		
	}

	// Dong socket
	closesocket(newConnection);
	closesocket(listeningSocket);

	// Giai phong winsock
	WSACleanup();
	system("pause");
	return 0;
}
