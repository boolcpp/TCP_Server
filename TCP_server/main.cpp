#include <iostream>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#include <opencv2/opencv.hpp> 
//#include <stdlib.h>
//for JPG
#include <vector>

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
	//struct RecvImgStruct
	//{
	//	int rows;
	//	int cols;
	//	size_t total;
	//}recvImg;

	//ZeroMemory(&recvImg, sizeof(recvImg));
	size_t length = 0;

	while (true) {
		int recvStructure = recv(clientSocket, (char*)&length, sizeof(length), 0);
		if (recvStructure == SOCKET_ERROR || recvStructure == 0)
			break;
		if (recvStructure != SOCKET_ERROR)
		{
			std::cout << "Image structure received with bytes : " << recvStructure << std::endl;
			/*char* imgBuff = (char*)malloc(recvImg.total * 3);
			ZeroMemory(imgBuff, recvImg.total * 3);*/
			std::vector<uchar> Imgbuf;
			int totalReaded = 0;
			//int needToRead = recvImg.total * 3;
			size_t needToRead = length;
			//while (totalReaded < recvImg.total * 3) {
			while(totalReaded < length)
			{
				int recvData = recv(clientSocket, (char*)Imgbuf[totalReaded], needToRead, 0);
				if (recvData == SOCKET_ERROR) {
					// ToDo обработать ошибку
					//goto end;
					std::cout << "cant receive Image data with error #" << WSAGetLastError() << std::endl;
					return -1;
				}
				needToRead -= recvData;
				totalReaded += recvData;
			}
			//cv::Mat servImg = cv::Mat(recvImg.rows, recvImg.cols, CV_8UC3);
			/*cv::Mat servImg = cv::Mat(recvImg.rows, recvImg.cols, CV_8UC3);
			servImg.data = (uchar*)imgBuff;*/
			cv::Mat servImg = cv::imdecode(Imgbuf, 1);

			cv::namedWindow("image", CV_WINDOW_NORMAL);
			cv::resizeWindow("image", 800, 600);
			cv::imshow("image", servImg);
			cv::waitKey(30);
		}
	}

	closesocket(clientSocket);
	WSACleanup();
	cv::destroyWindow("image");
	//cvReleaseImage(&("image");
	system("pause");
	return 0;
}
