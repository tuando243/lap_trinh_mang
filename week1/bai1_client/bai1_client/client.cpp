#include <iostream>
#include <winsock2.h>

int main(int argc, char** argv) {

	// Kiem tra tham so dau vao
	if (argc != 3) {
		printf("Input error\n>e.g. 'TCPClient.exe 127.0.0.1 8888'\n");
		system("pause");
		return 0;
	}

	WSADATA wsaData;
	SOCKET newConnection;
	SOCKADDR_IN serverAddr;
	int port = atoi(argv[2]);

	// Khoi tao winsock 2.2
	if (WSAStartup(MAKEWORD(2, 2), &wsaData)) {
		printf("Version is not supported\n");
		system("pause");
		return 0;
	}

	// Tao socket client
	newConnection = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// Khoi tao cau truc SOCKADDR_IN cua server
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	serverAddr.sin_addr.s_addr = inet_addr(argv[1]);

	// Ket noi server thong qua socket newConnection
	if (connect(newConnection, (SOCKADDR*)&serverAddr, sizeof(serverAddr))) {
		printf("Cannot connect server\n");
		system("pause");
		return 0;
	}

	// Bat dau gui nhan du lieu
	char buf[256];
	int res = recv(newConnection, buf, sizeof(buf), 0);
	buf[res] = '\0';
	printf("Server: %s\n", buf);

	while (1) {
		printf("Send to server: ");
		gets_s(buf, 256);		
		res = send(newConnection, buf, strlen(buf), 0);
		if (strcmp(buf, "exit") == 0) break;
		if (res == SOCKET_ERROR) {
			printf("Error\n");
			break;
		}
		printf("Nhap 'exit' de ket thuc\n");
	}

	// Dong socket
	closesocket(newConnection);

	// Giai phong winsock
	WSACleanup();

	system("pause");
	return 0;
}
