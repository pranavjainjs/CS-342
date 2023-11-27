// To run
// g++ q3.cpp
// ./a.out 

// Necessary header files
#include <bits/stdc++.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
using namespace std;

// For color printing in linux terminal
#define RESET "\033[0m"
#define BLACK "\033[30m"			  /* Black */
#define RED "\033[31m"				  /* Red */
#define GREEN "\033[32m"			  /* Green */
#define YELLOW "\033[33m"			  /* Yellow */
#define BLUE "\033[34m"				  /* Blue */
#define MAGENTA "\033[35m"			  /* Magenta */
#define CYAN "\033[36m"				  /* Cyan */
#define WHITE "\033[37m"			  /* White */
#define BOLDBLACK "\033[1m\033[30m"	  /* Bold Black */
#define BOLDRED "\033[1m\033[31m"	  /* Bold Red */
#define BOLDGREEN "\033[1m\033[32m"	  /* Bold Green */
#define BOLDYELLOW "\033[1m\033[33m"  /* Bold Yellow */
#define BOLDBLUE "\033[1m\033[34m"	  /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m" /* Bold Magenta */
#define BOLDCYAN "\033[1m\033[36m"	  /* Bold Cyan */
#define BOLDWHITE "\033[1m\033[37m"	  /* Bold White */
typedef long long ll;

// Function to handle error messages and exit the program
void error(string message)
{
	cout << BOLDRED << message << RESET << endl;
	exit(0);
}

// Function to take user input for the message to be sent to the server
void takeinputfromclient(char buffer[])
{
	char inp;
	cout << BOLDCYAN << "Please type the message to be sent to the server." << RESET << endl;
	bzero(buffer, 1500);
	int i = 0;
	while (1)
	{
		scanf("%c", &inp);
		if (inp == '\n')
			break;

		// Buffer can only take upto 1500 characters
		if (i == 1500)
		{
			cout << "Sorry, only the first 1500 characters of the message can be sent." << endl;
			break;
		}
		buffer[i++] = inp;
	}
	buffer[i++] = '\0';
}

// Function to close the connection with the server
void closeConnection(char buffer[], int clientsocket, string check)
{
	buffer[0] = '3';
	char temp[] = "connection_close";
	strcpy(buffer + 1, temp);
	write(clientsocket, buffer, strlen(buffer));
}

// Function to parse the input message and send it to the server
void parse_and_send(int clientsocket, char buffer[])
{
	int i = 0;
	string s;
	char temp[1500];
	int j = 0;

	cout << endl;
	// Request user input for the probability of a data packet being corrupt
	cout << BOLDCYAN << "Please define the probability of a data packet being corrupt (in the form of a/b, where a >= 0, b > 0 and a <= b) : " << RESET;
	int a, b;
	scanf("%d/%d", &a, &b);
	cout << endl;

	// Parse and send implementation
	while (buffer[i] != '\0')
	{
		s = "";
		bzero(temp, 1500);
		// Extract the message until a space character is encountered
		while (buffer[i] != ' ')
		{
			s += buffer[i];
			temp[j] = buffer[i];
			i++;
			j++;
			if (buffer[i] == '\0')
				break;
		}
		temp[j] = '\0';

		cout << BOLDMAGENTA << "Client:" << RESET << " Transmitting " << temp << "..." << endl;

		while (1)
		{
			// Generate a random number to simulate packet corruption
			int x = rand() % b;

			if (x < a)
			{
				// If the random number is within the corrupt probability, prepend '!' to the message
				char temp1[1500];
				temp1[0] = '!';
				strcpy(temp1 + 1, temp);
				write(clientsocket, temp1, strlen(temp1));
			}
			else
			{
				// Otherwise, send the message as it is
				write(clientsocket, temp, strlen(temp));
			}
			char rec[1500];
			bzero(rec, 1500);
			// Read the response from the server
			int received = read(clientsocket, rec, 1500);
			if (strcmp(rec, "ACK") == 0)
			{
				// If ACK is received, print the acknowledgment and exit the loop
				cout << BOLDMAGENTA << "Client:" << RESET << " Received " << rec << endl
					 << endl;
				break;
			}
			else
			{
				// If NAK is received, print the acknowledgment and retransmit the message
				cout << BOLDMAGENTA << "Client:" << RESET << " Received " << rec << ". Now retransmitting..." << endl
					 << endl;
			}
		}

		if (buffer[i] == '\0')
		{
			// If the end of the message is reached, break the loop
			break;
		}
		else
		{
			// Move to the next character in the input buffer
			i++;
			j = 0;
		}
	}
}

// Function to set up server and handle incoming connections
void server(char *argv[])
{
	// Declare structures for server and client addresses
	struct sockaddr_in server, client;

	// Create a socket for the server
	int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

	// Check if socket creation was successful
	if (serverSocket == -1)
	{
		error("Server: Oops! Sorry, socket creation failed. Please try again :(");
	}

	// Configure server details
	server.sin_family = AF_INET;				   
	server.sin_addr.s_addr = INADDR_ANY;		   
	server.sin_port = htons(atoi(argv[1]));		   
	memset(&server.sin_zero, 8, 0);				   
	socklen_t length = sizeof(struct sockaddr_in); 

	// Bind
	if (bind(serverSocket, (struct sockaddr *)&server, length) < 0)
	{
		error("Server: Oops! Sorry, binding has failed. Please try again :(");
	}

	// Listen
	if (listen(serverSocket, 20) == -1)
	{
		error("Server: Oops! Sorry, listening has failed. Please try again :(");
	}

	cout << endl;
	cout << BOLDGREEN << "Server is up and running!" << RESET << endl;
	fflush(stdout);

	// Accept incoming client connection
	int clientSocket = accept(serverSocket, (struct sockaddr *)&client, &length);

	// Check if the connection with the client is established
	if (clientSocket < 0)
	{
		error("Server: Oops! Sorry, server - client connection couldn't be established. Please try again :(");
	}

	char buffer[1500];

	// Receive and process messages from the client
	while (1)
	{
		// Clear the buffer before receiving a new message
		bzero(buffer, 1500);

		// Read the message from the client
		int st = read(clientSocket, buffer, 1500);

		cout << BOLDYELLOW << "Server:" << RESET << " Message received from client - " << buffer << ". ";

		// If the message indicates connection close, break the loop
		if (buffer[0] == '3')
		{
			cout << endl;
			break;
		}

		if (buffer[0] == '!')
		{
			// If the message is a corrupt packet, send NAK to client
			cout << BOLDRED << "CORRUPT PACKET RECEIVED!" << RESET << endl;
			strcpy(buffer, "NAK");
		}
		else
		{
			// If the message is a good packet, send ACK to client
			cout << BOLDGREEN << "GOOD PACKET RECEIVED!" << RESET << endl;
			strcpy(buffer, "ACK");
		}
		// Send the response (ACK / NAK) back to the client
		write(clientSocket, buffer, strlen(buffer));
	}
	// Close the client socket and print a closing message
	close(clientSocket);
	cout << BOLDYELLOW << "Server:" << RESET << " Connection with the client has been closed" << endl;
	cout << endl;
}

void client(char *argv[])
{
	// Create a socket for the client
	int clientsocket = socket(AF_INET, SOCK_STREAM, 0);
	char buffer[1500];

	// Check for socket creation errors
	if (clientsocket == -1)
	{
		error("Client: Socket creation failed.");
	}

	// Configure server address structure
	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(atoi(argv[1]));
	socklen_t length = sizeof(struct sockaddr_in);

	// Convert and set server IP address
	if (inet_aton(argv[2], &server.sin_addr) == 0)
	{
		error("Client: Invalid IP address.");
	}

	// Connect to the server
	if (connect(clientsocket, (struct sockaddr *)&server, length) == -1)
	{
		error("Client: Couldn't connect to the server.");
	}

	cout << endl
		 << BOLDMAGENTA << "Client:" << RESET << " Connected to the server at port " << atoi(argv[1]) << "." << endl;

	string check;

	// Client input loop
	while (1)
	{
		cout << endl
			 << BOLDYELLOW << "Client:" << RESET << " Type 'y' to send a message, '-1' to exit." << RESET << endl;
		cin >> check;

		if (check != "y")
		{
			if (check == "-1")
			{
				// Handle client exit
				cout << endl
					 << BOLDCYAN << "Client:" << RESET << " Thanks for the conversation! Closing the connection." << endl
					 << endl;
			}
			else
			{
				// Handle invalid input
				cout << BOLDRED << "Client: Invalid input. Disconnecting from the server." << RESET << endl
					 << endl;
			}
			// Close the connection
			closeConnection(buffer, clientsocket, check);
			break;
		}
		getchar();
		takeinputfromclient(buffer);
		parse_and_send(clientsocket, buffer);
	}
	// Close the client socket
	close(clientsocket);
}

int main(int argc, char *argv[])
{
	// 0 is executable
	// 1 is port
	// 2 is client's address
	if (argc < 3)
	{
		error("Syntax : ./a.out [port] [127.0.0.1]");
	}

	thread t1(server, argv);
	sleep(2);
	thread t2(client, argv);

	t1.join();
	t2.join();

	return 0;
}
