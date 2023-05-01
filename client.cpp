#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#define PORT 8080
#define SA struct sockaddr
#include <unistd.h>
#include <arpa/inet.h>
#include <fstream>
#include <iostream>
#include <ostream>
#include <cstring>
#include <chrono>

int MAX;

void func(int sockfd, int dataSize, char dataSet[])
{
	auto start_time = std::chrono::system_clock::now().time_since_epoch(); // Records starting time to measure total transfer time.
	
	char term[] = {'e','x','i','t'}; // Termination packet
	int index = 0;
	int n = 1; // num of packets
	
	while(index < dataSize)
	{
		char buff[MAX];
		//std::cout << index << " index | total " << dataSize << std::endl;
		std::memset(buff, 0, sizeof(buff)); // resets buffer optimally
		//bzero(buff, sizeof(buff));
		for (int i = 0; i < MAX; i++) {
            if (index < dataSize) {
                buff[i] = dataSet[index];
                index++;
            }
            else {
                break;
            }
        } 
		
		std::cout<<"Sending packet number " <<n<<std::endl;
		write(sockfd, buff, sizeof(buff));
		
		printf("Awaiting Ack.\n");
		std::memset(buff, 0, sizeof(buff));
		//bzero(buff, sizeof(buff));
		read(sockfd, buff, sizeof(buff));
		
		if ((strncmp(buff, "exit", 4)) == 0) {
			printf("Client Exit...\n");
			break;
		}
		else if ((strncmp(buff, "ack", 3)) == 0) {
			printf("Acknowledged, continuing\n");
		}
		++n;
	}
	printf("File has been transfered.\n");
	write(sockfd, term, sizeof(term)); // terminates server
	
	auto end_time = std::chrono::system_clock::now().time_since_epoch();
    auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    std::cout << "Elapsed time: " << elapsed_time.count() << " ms" << std::endl;
    std::cout << "Buffer: " << MAX << std::endl;
	
	std::ofstream file("result.txt", std::ios_base::app);
    if (!file) {
        std::cerr << "Failed to open file\n";
        return;
    }

    file << "Total time required with a buffer size of "<<MAX<<": "<<elapsed_time.count()<<" ms\n";
    file.close();
}

int main()
{
	// Dynamic updating of the buffer size
	std::ifstream infile("bufferSize.txt");
    if (infile >> MAX)
        std::cout << "Read buffer size: " << MAX << std::endl;
    else 
	{
        std::cerr << "Failed to read buffer size from file, defaulting to 1500" << std::endl;
        MAX = 1500;
    }

    infile.close();
	
	int sockfd, connfd;
	struct sockaddr_in servaddr, cli;

	// socket create and varification
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
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(PORT);

	// connect the client socket to server socket
	if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
		printf("connection with the server failed...\n");
		exit(0);
	}
	else
		printf("connected to the server..\n");
	
	// After connection is established, lets read the file
	std::ifstream file("file.txt"); 
	if (!file)
		{ 
	std::cerr << "Error opening file" << std::endl;
    return 1;
    }

    // get file length
    file.seekg(0, std::ios::end);
   int length = file.tellg();
    file.seekg(0, std::ios::beg);

    // allocate memory for char array
    char* buffer = new char[length];

    // read file into buffer
    file.read(buffer, length);
	printf("Data has been read into buffer");

	// function for chat
	func(sockfd,length,buffer);

	// close the socket
	close(sockfd);
}

