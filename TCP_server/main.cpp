#include <iostream>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#include <opencv2/opencv.hpp> 
//#include <stdlib.h>

int main()
{
	
	//initialize winsock
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);

	int wsok = WSAStartup(ver, &wsData);
	if (wsok != 0)
	{
		std::cerr << "Cant init winsock" << std::endl;
		return -1;
	}

	//create socket
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET)
	{
		std::cerr << "cant create a socket" << std::endl;
		return -1;
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
		return -1;
	}

	//Made winsocket listenining
	int lstn = listen(listening, SOMAXCONN);
	if (lstn != 0)
	{
		std::cerr << "cant made socket listenning, error occurs #" << WSAGetLastError() << std::endl;
		closesocket(listening);
		WSACleanup();
		return -1;
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
		return -1;
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
	char buf[1000000];

	/*while (1)
	{*/
		ZeroMemory(buf, 1000000);
		char rows[5];
		ZeroMemory(rows, 5);
		char cols[5];
		ZeroMemory(cols, 5);
		char lengthBuf[16];
		ZeroMemory(lengthBuf, 16);

		int bytesReceived = recv(clientSocket, buf, 1000000, 0);
		if (bytesReceived == SOCKET_ERROR)
		{
			std::cerr << "Error in receiving from socket with error #" << WSAGetLastError() << std::endl;
			return -1;
		}
		if (bytesReceived == 0)
		{
			std::cout << "client disconnected" << std::endl;
			return -1;
		}
		if (bytesReceived > 0)
		{
			
			for (size_t i = 0; i < 6; i++)
			{
				rows[i] = buf[i];
				cols[i] = buf[i + 6];
			}
			int rowsCount = atoi(rows);
			int colsCount = atoi(cols);

			for (size_t i = 0; i < 17; i++)
			{
				lengthBuf[i] = buf[i + 17];
			}
			int length = atoi(lengthBuf);
			cv::Mat image = cv::Mat(rowsCount, colsCount, CV_8UC3);
			for (size_t i = 0; i < length - 26; i++)
			{
				image.data[i] = buf[i + 27];
			}
			cv::namedWindow("frame", CV_WINDOW_AUTOSIZE);
			cv::imshow("frame", image);
			cv::waitKey(0);
			//cv::Mat imageRecv
		}
		//send(clientSocket, buf, bytesReceived + 1, 0);

	//}
	
	closesocket(clientSocket);
	WSACleanup();
	system("pause");
	return 0;
}