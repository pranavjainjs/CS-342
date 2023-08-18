// Necessary header files
#include <bits/stdc++.h>
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <string.h>
using namespace std;
// For color printing in linux terminal
#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */

// Error function
void error(string message)
{
	cout << BOLDRED << message << RESET << endl;
	exit(0);
}

// Function for taking input from the client
void takeinputfromclient(char buffer[])
{
	char inp;
	cout << BOLDCYAN << "You: " << RESET;
	bzero(buffer, 1500); 
	int i = 1;
	while(1)
	{
		scanf("%c", &inp);   
		if(inp == '\n') 
			break;
		// Buffer can only take upto 1500 characters
		if (i == 1000)
		{
			cout << "Sorry, only the first 1500 characters of the message can be sent." << endl;
			break;
		}
		buffer[i++] = inp; 
	}
}

// Function to close connection
void closeConnection(char buffer[], int clientSocket, string check)
{
	buffer[0] =  '3'; // to send to the server, 3 means to close the connection 
	char temp[] = "connection_close";
	strcpy(buffer+1, temp); 
    // if the connection is closed on the request of the client
	if(check == "-1")
	{
		cout << BOLDCYAN << "The connection has been successfully closed." << endl; 
		cout << BOLDCYAN << "Bye!" << endl;
	}
    // writing to the client
	write(clientSocket, buffer, strlen(buffer)); 
}

// Main function
int main(int argc, char *argv[])
{
	// If the port number is not provided, the program terminates.
	if(argc < 3) 
	{
		error("Please enter the port number.");
	}

	// This is the structure of sockaddr_in. It is used to store the address of the server.
    // struct sockaddr_in 
    // {
    //     short            sin_family;  
    //     unsigned short   sin_port;    
    //     struct in_addr   sin_addr;    
    //     char             sin_zero[8];  
    // };
    // sin_family - specifies the type of address family, here, it is AF_INET => iPv4 family
    // sin_port - specifies the port number that the socket will use, here argv[2] is the port number of the server
	struct sockaddr_in server; 

	// Creating a client socket, clientsocket is the file descriptor
	int clientSocket = socket(AF_INET,SOCK_STREAM,0); 
	// Here AF_INET means iPv4 address family
    // Here SOCK_STREAM means TCP protocol
    // If the value of the file descriptor is -1, it means the socket couldn't be created. If it's 0, it's been created.

	char buffer[1500]; // Buffer for storing the message

	// If the client socket couldn't be created
	if(clientSocket == -1)
	{
		error("Oops! Sorry, socket creation failed. Please try again :(");
	} 

	server.sin_family = AF_INET; // For IPv4
	int serverPort = atoi(argv[2]); // Server port
	server.sin_port = htons(serverPort); // The htons() function converts the unsigned short integer hostshort from host byte order to network byte order.
	socklen_t length = sizeof(struct sockaddr_in); // to hold the length of the socket address

	// Checking for ip address
    // Description of inet_aton()
    // inet_aton() converts the Internet host address from the IPv4 numbers-and-dots notation into binary form 
    // (in network byte order) and stores it in the structure that inp points to. inet_aton() returns nonzero if the address
    // is valid, zero if not. The address supplied in cp can have one of the following forms:
	if(inet_aton(argv[1],&server.sin_addr)==0)
	{ 
		error("Oops! Sorry, the given IP address is not valid. Please try again :(");
	} 

	// If the client couldn't connect to the server
    // Description of connect()
    // connect connects the clientsocket to the server. If the connection is successful, it returns 0. Otherwise, it returns -1
	if(connect(clientSocket,(struct sockaddr *) &server, length) == -1)
	{ 
		error("Oops! Sorry, couldn't connect to the server. Please try again :(");
	} 

	cout << endl;
	cout << BOLDMAGENTA << "Hello! You are connected to the server at the port " << serverPort << endl;
	
    string check;

	while(1)
	{
		cout << endl;
		cout << BOLDYELLOW << "To send a message to the server, type 'y'. Other wise, type '-1'." << RESET << endl;
		cin >> check;

		// Closing the client's connection
		if(check != "y")
		{
            // if it's upon request from the client
			if(check == "-1")
			{
                cout << endl;
				cout << BOLDCYAN << "Thanks a lot for the conversation. Hope you enjoyed it!" << endl;
				cout << BOLDCYAN << "Closing the connection......." << endl;
			}
			else
			{
				cout << BOLDRED << "Seems like your reply is not of the expected format. Therefore, we are disconnecting you from the server." << endl;
				cout << BOLDRED << "Sorry, please try again :(" << endl;
			}
            // closing client's connection
			closeConnection(buffer,clientSocket,check);
			break;
		}

		getchar();
		// Taking input
		takeinputfromclient(buffer);
		// Type of the message -> here 1 means a regular message
		buffer[0] =  '1'; 
		// sending to the server
		write(clientSocket, buffer, strlen(buffer)); 
		bzero(buffer, 1500);
		// receiving from the server
		int received = read(clientSocket, buffer, 1500);

		// Checking if the message received is an acknowledgment or not
		if(buffer[0]!='2')
		{
			cout << BOLDGREEN << "Oops! Sorry, couldn't receive the message. Please try again." << endl;
			continue; 
		}
		// If acknowledgement is received from the server
		// cout << BOLDGREEN << "Your message has been sent to the server successfully and you've received a message from the server." << endl;
		cout << BOLDGREEN << "Server: " << RESET;
		printf("%s\n", buffer+1);
	}
	close(clientSocket); 
    return 0;
}
