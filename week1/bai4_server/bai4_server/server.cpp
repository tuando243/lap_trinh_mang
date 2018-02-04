#include <iostream>
#include <winsock2.h>
#include <Windows.h>

int main(int argc, char** argv) {

	// Kiem tra tham so dau vao
	if (argc != 2) {
		printf("Input error\n>e.g. 'TCPServer.exe 1234'\n");
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

	// Chap nhan ket noi moi
	clientAddrLen = sizeof(clientAddr);
	newConnection = accept(listeningSocket, (SOCKADDR *)&clientAddr, &clientAddrLen);

	// Neu ket noi thanh cong thuc hien gui, nhan du lieu tu client
	if (newConnection) {
		char buf[1024];
		int res = recv(newConnection, buf, 1024, 0);
		printf("Computer name client: %s\n", buf);
		printf("Drive information:\n");
		while (true) {
			res = recv(newConnection, buf, 1024, 0);
			printf("%s", buf);
			if (res <= 0) break;
		}
	}

	// Dong socket
	closesocket(newConnection);
	closesocket(listeningSocket);

	// Giai phong winsock
	WSACleanup();
	system("pause");
	return 0;
}
