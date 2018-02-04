#include <iostream>
#include <winsock2.h>
#include <Windows.h>

int main(int argc, char** argv) {

	// Kiem tra tham so dau vao
	if (argc != 3) {
		printf("Input error\n>e.g. 'TCPClient.exe 127.0.0.1 1234'\n");
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
	char buf[1024];
	DWORD bufCount = 1024;
	//Gui thong tin ve may
	if (GetComputerNameA(buf, &bufCount)) {
		printf("Computer name: %s\n", buf);
		send(newConnection, buf, strlen(buf) + 1, 0);
	}

	//Lay thong tin ve o dia trong may
	FILE *fd = _popen("wmic logicaldisk get size, freespace ,caption", "r");
	if (fd == NULL) {
		fprintf(stderr, "Could not open pipe\n");
		return 1;
	}

	while (fgets(buf, sizeof(buf), fd) != NULL) {		
		send(newConnection, buf, strlen(buf) + 1, 0);
		printf("%s", buf);
	}

	// Dong socket
	closesocket(newConnection);

	// Giai phong winsock
	WSACleanup();

	

	system("pause");
	return 0;
}