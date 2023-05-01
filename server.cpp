#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#define PORT 8080
#define SA struct sockaddr
#include <unistd.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <string.h>
#include <cstring>
#include <chrono>

std::vector<char> message; // Holder for the finished message
int MAX;

// Function designed for chat between client and server.
void func(int sockfd)
{
	auto start_time = std::chrono::system_clock::now().time_since_epoch(); // Records starting time to measure total transfer time.
	
	message.clear();
	char ack[] = {'a','c','k'}; // Servers response that allows next packet to come through.
	int n = 1; // num of packets
	
	// infinite loop for chat
	for (;;)
	{
		char buff[MAX]; // Put in loop because neither memset or bzero were clearing the acknowledgement from the array
		std::memset(buff, 0, MAX);
		//bzero(buff, sizeof(buff));
		
		// read the message from client and copy it in buffer
		std::cout<<"Awaiting packet\n";
		read(sockfd, buff, sizeof(buff));
		
		if ((strncmp(buff, "exit", 4)) == 0)
		{
			printf("Recieved termination packet...\n");
			break;
		}
		
		// Copies message from buffer to holder
		std::cout<<"Recieved packet "<<n<<std::endl;
		message.insert(message.end(),buff,buff+std::strlen(buff));
		
		// Acknowledge transfer
		std::cout<<"Acknowledging...\n";
		write(sockfd, ack, sizeof(ack));
		std::memset(buff, 0, MAX);
		//bzero(buff, sizeof(buff));
		++n;
	}
	
	auto end_time = std::chrono::system_clock::now().time_since_epoch();
    auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    std::cout << "Elapsed time: " << elapsed_time.count() << " ms" << std::endl;
	std::cout << "Buffer: " << MAX << std::endl;
}

// Driver function
int main()
{
	std::ifstream infile("bufferSize.txt");
    if (infile >> MAX) 
        std::cout << "Read buffer size: " << MAX << std::endl;
	else
	{
        std::cerr << "Failed to read buffer size from file, defaulting to 1500" << std::endl;
        MAX = 1500;
    }

	
	int sockfd, connfd;
	struct sockaddr_in servaddr, cli;
        socklen_t len;
	// socket create and verification
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		printf("socket creation failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully created..\n");
	bzero(&servaddr, sizeof(servaddr));

	// assign IP, PORT
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(PORT);

	// Binding newly created socket to given IP and verification
	if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
		printf("socket bind failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully binded..\n");

	// Now server is ready to listen and verification
	if ((listen(sockfd, 5)) != 0) {
		printf("Listen failed...\n");
		exit(0);
	}
	else
		printf("Server listening..\n");
	len = sizeof(cli);

	// Accept the data packet from client and verification
	connfd = accept(sockfd, (SA*)&cli, &len);
	if (connfd < 0) {
		printf("server acccept failed...\n");
		exit(0);
	}
	else
		printf("server acccept the client...\n");

	// Function for chatting between client and server
	func(connfd);

	// saves the file again so we can make sure it correctly output.
	std::ofstream outputFile("output.txt");
    if (outputFile.is_open()) {
        for (char c : message) {
            outputFile << c;
        }
        outputFile.close();
        std::cout << "File saved successfully." << std::endl;
    }
    else {
        std::cout << "Unable to open file." << std::endl;
    }

	// After chatting close the socket
	close(sockfd);
}

