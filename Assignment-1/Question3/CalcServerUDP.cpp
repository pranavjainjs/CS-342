// Necessary header files
#include <bits/stdc++.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <vector>
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
    cout << message << endl;
    exit(0);
}

// To calculate the expression given in the message
double calculator(char buffer[])
{
    // Finding the first operand
    string operand1 = "";
    int i = 0;
    while(buffer[i] != ' ' && i < 1500)
    {
        operand1 += buffer[i];
        i++;
    }
    // converting it into a double value
    double val1 = stod(operand1);

    // Finding the operator
    i++;
    char op = buffer[i];
    i += 2;

    // Finding the second operand
    string operand2 = "";
    while(buffer[i] != '\0' && buffer[i] !=' ' && i < 1500)
    {
        operand2 += buffer[i];
        i++;
    }
    // converting it into a double value
    double val2 = stod(operand2);

    cout << BOLDMAGENTA << "Received from the client: ";
    cout << BOLDYELLOW << val1 << " " << op << " " << val2 << endl;

    // Calculating the answer
    if(op == '+')
    {
        cout << BOLDMAGENTA << "Sending to the client: ";
        cout << BOLDYELLOW << val1 + val2 << endl;
        return val1+val2;
    }
    if(op == '-')
    {
        cout << BOLDMAGENTA << "Sending to the client: ";
        cout << BOLDYELLOW << val1-val2<< endl;
        return val1-val2;
    }
    if(op == '*')
    {
        cout << BOLDMAGENTA << "Sending to the client: ";
        cout << BOLDYELLOW << val1*val2<< endl;
        return val1*val2;
    }
    if(op == '/')
    {
        cout << BOLDMAGENTA << "Sending to the client: ";
        cout << BOLDYELLOW << val1/val2 << endl;
        return val1/val2;
    }
    if(op == '^')
    {
        cout << BOLDMAGENTA << "Sending to the client: ";
        cout << BOLDYELLOW << pow(val1, val2) << endl;
        return pow(val1, val2);
    }
    return -1;
}


// Main function
int main(int argc, char * argv[])
{
    // If the correct number of arguments are not passed
	if(argc < 2)
	{
		error("Please provide the port number.");
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
    // sin_addr - ip address of the server
    // sin_zero - used to balance the size
    struct sockaddr_in server, client;

    // Creating a server socket, serversocket is the file descriptor
    int serversocket = socket(AF_INET, SOCK_DGRAM, 0);
    // Here AF_INET means iPv4 address family
    // Here SOCK_DGRAM means UDP protocol
    // If the value of the file descriptor is -1, it means the socket couldn't be created. If it's 0, it's been created.

    // If the server could not be created
    if(serversocket == -1)
    {
        error("Oops! Sorry, socket creation failed. Please try again :(");
    }

    server.sin_family = AF_INET; // For iPv4 address family
    server.sin_addr.s_addr = INADDR_ANY; // iP address of the host, binds the socket to all local interfaces
    server.sin_port = htons(atoi(argv[1])); // port number of the server
    memset(&server.sin_zero,8,0); // makes them zero
    socklen_t length = sizeof(struct sockaddr_in); // to hold the length of the socket address

    // The server socket is binded to the the ip address of the server by the bind function.
    // If bind is successful, it return 0. Otherwise, it returns -1
    if(bind(serversocket, (struct sockaddr*) &server, length) < 0)
    {
        error("Binding failed");
    }   

    // Server has finally started working!!
	cout << endl;
	cout << BOLDGREEN << "Server is working successfully!" <<  RESET << endl;

    char buffer[1500];

    while(1)
    {
        bzero(buffer,1500);

        // recvfrom gets the message from client
        int clientsocket = recvfrom(serversocket, buffer,1500,0,(struct sockaddr *)&client, &length);
       
        // if it's less than 0, it means connection couldn't be established
        if(clientsocket < 0)
        {
            error("Oops! Sorry, server - client connection couldn't be establised. Please try again :(");
        }

        // To close the connection
        if(buffer[0] == '-' && buffer[1] == '1') 
        {   
            close(clientsocket);
            break;
        }

        // evaluating the answer
        double answer = calculator(buffer);

        bzero(buffer,1500);

        // converting answer to a string
        string result = to_string(answer);

        for(int i = 0; i < result.size(); i++)
        {
            buffer[i] = result[i];
        }
     
        // sending it to the client
        sendto(serversocket,buffer,1500,0,(struct sockaddr *)&client,length);
    }
     
    return 0;
}