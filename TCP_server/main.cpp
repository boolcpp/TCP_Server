#include <iostream>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

void main()
{
	//initialize winsock
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);

	int wsok = WSAStartup(ver, &wsData);
	if (wsok != 0)
	{
		std::cerr << "Cant init winsock" << std::endl;
		return;
	}

	//create socket
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET)
	{
		std::cerr << "cant create a socket" << std::endl;
		return;
	}

	//bind socket to ip addr and port
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(54000);
	hint.sin_addr.S_un.S_addr = INADDR_ANY;
	int bnd = bind(listening, (sockaddr*)&hint, sizeof(hint));
	if (bnd != 0)
	{
		std::cerr << "socket bind failed with error #" << WSAGetLastError() << std::endl;
		closesocket(listening);
		WSACleanup();
		return;
	}

	//Made winsocket listenining
	int lstn = listen(listening, SOMAXCONN);
	if (lstn != 0)
	{
		std::cerr << "cant made socket listenning, error occurs #" << WSAGetLastError() << std::endl;
		closesocket(listening);
		WSACleanup();
		return;
	}
	
	//Wait for connection create client
	sockaddr_in client;
	int clientsize = sizeof(client);

	SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientsize);
	if (clientSocket == INVALID_SOCKET)
	{
		std::cerr << " cant accept clientsocket with error #" << WSAGetLastError() << std::endl;
		closesocket(listening);
		WSACleanup();
		return;
	}

	char host[NI_MAXHOST]; //client remote name
	char service[NI_MAXSERV]; //Service (port) the client connected

	ZeroMemory(host, NI_MAXHOST);
	ZeroMemory(service, NI_MAXSERV);

	if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
	{
		std::cout << host << " connected on port " << service << std::endl;
	}
	else
	{
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		std::cout << host << " connected on port " << ntohs(client.sin_port) << std::endl;
	}

	//Close socket listening
	closesocket(listening);

	//while loop: accept and echo message back to client
	char buf[4096];

	while (1)
	{
		ZeroMemory(buf, 4096);

		int bytesReceived = recv(clientSocket, buf, 4096, 0);
		if (bytesReceived == SOCKET_ERROR)
		{
			std::cerr << "Error in receiving from socket with error #" << WSAGetLastError() << std::endl;
			break;
		}
		if (bytesReceived == 0)
		{
			std::cout << "client disconnected" << std::endl;
			break;
		}

		send(clientSocket, buf, bytesReceived + 1, 0);

	}
	
	closesocket(clientSocket);
	WSACleanup();
	system("pause");
}