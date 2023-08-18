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

// Function to close connection
void closeconnection(char buffer[], int clientsocket, string check)
{
    // to send the server
    buffer[0] = '-';
    buffer[1] = '1';

    // if the client is being closed upon the user's request
    if(check == "-1")
	{
		cout << BOLDCYAN << "The connection has been successfully closed." << endl; 
		cout << BOLDCYAN << "Bye!" << endl;
	}

    // sending to the server
    write(clientsocket, buffer, strlen(buffer));
}

// Main function
int main(int argc, char * argv[])
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
    struct sockaddr_in server; char c;

    // Creating a client socket, clientsocket is the file descriptor
    int clientsocket = socket(AF_INET, SOCK_STREAM ,0);
    // Here AF_INET means iPv4 address family
    // Here SOCK_STREAM means TCP protocol
    // If the value of the file descriptor is -1, it means the socket couldn't be created. If it's 0, it's been created.

    char buffer[1500]; // Buffer for storing the message

    // If the client socket couldn't be created
    if(clientsocket == -1)
    {
        error("Oops! Sorry, socket creation failed. Please try again :(");
    }

    server.sin_family = AF_INET; // For IPv4
    int serverport = atoi(argv[2]); // Server port
    server.sin_port = htons(serverport); // The htons() function converts the unsigned short integer hostshort from host byte order to network byte order.
    socklen_t length = sizeof(struct sockaddr_in); // to hold the length of the socket address

    // Checking for ip address
    // Description of inet_aton()
    // inet_aton() converts the Internet host address from the IPv4 numbers-and-dots notation into binary form 
    // (in network byte order) and stores it in the structure that inp points to. inet_aton() returns nonzero if the address
    // is valid, zero if not. The address supplied in cp can have one of the following forms:
    if(inet_aton(argv[1], &server.sin_addr) == 0)
    {
       error("Oops! Sorry, the given IP address is not valid. Please try again :(");
    }

    // If the client couldn't connect to the server
    // Description of connect()
    // connect connects the clientsocket to the server. If the connection is successful, it returns 0. Otherwise, it returns -1
    if(connect(clientsocket, (struct sockaddr *) &server, length) == -1)
    {
        error("Oops! Sorry, couldn't connect to the server. Please try again :(");
    }

    cout << endl;
	cout << BOLDMAGENTA << "Hello! You are connected to the server at the port " << serverport << endl;
	
    string check;

    while(1)
    {
        cout << endl;
		cout << BOLDYELLOW << "To evaluate a binary expression, type 'y'. Other wise, type '-1'." << RESET << endl;
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
			closeconnection(buffer,clientsocket,check);
			break;
		}

        cout << BOLDGREEN << "Please enter your expression in the following manner" << endl;
        cout << BOLDGREEN << "operand1 operator operand2" << endl;
        cout << BOLDYELLOW << "Valid operators are +, -, *, /, ^" << RESET << endl;

        // Taking in input
        string a,b,c; cin >> a >> b >> c;

        // Modifying the input
        int i = 0; char buff[a.size()+c.size()+4];
        for(int j = 0; j < min(int(a.size()),1500); j++)
        {
            buff[i] = a[j];
            i++;
        }
        buff[i] = ' '; i++;
        buff[i] = b[0];
        i++;
        buff[i] = ' '; i++;
        for(int j = 0; j < min(int(c.size()),1500); j++)
        {
            buff[i] = c[j];
            i++;
        }

        // sending the expression to the server for evaluation
        write(clientsocket, buff, strlen(buff));

        bzero(buffer, 1500);

        // received the answer
        int received = read(clientsocket, buffer, 1500);

        cout << BOLDCYAN << "The expression has been evaluated by the server: ";
        cout << BOLDCYAN << a << " " << b << " " << c << " = " << buffer << endl;
    }
    close(clientsocket);
    return 0;
}