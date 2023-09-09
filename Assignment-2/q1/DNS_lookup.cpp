// DNS Query Program on Linux

// Header Files
#include <iostream>     // cin, cout, endl
#include <string.h>     // strlen
#include <sys/socket.h> // UDP sockets
#include <arpa/inet.h>  // inet_addr , inet_ntoa , ntohs htons
#include <unistd.h>     // getpid
#include "dns.h"        // custom header file
#include <string>

using std::string;

char dns_server[20];          // dns server
char IPaddress[10][20];       // list of IP addresses received
int k = 0;                    // number of IP addresses received
struct RES_RECORD answers[8]; // the replies from the DNS server

struct LinkedList
{
    string url;
    char *results;
    LinkedList *next;

    LinkedList() // constructor
    {
        url = "";
        next = NULL;
    }
};

LinkedList *cache = new LinkedList();

// Check if a node exists in current Linked List or not...
LinkedList *In_Cache(string url)
{
    LinkedList *root = cache;
    while (root != NULL)
    {
        if (root->url == url)
        {
            return root;
        }
        root = root->next;
    }
    return NULL;
}

// Insert a node at start of Linked List....
void Insert(LinkedList *&node)
{
    if (cache->next == NULL)
    {
        cache->next = node;
        return;
    }
    LinkedList *temp = cache->next;
    cache->next = node;
    node->next = temp;
}

// Delete a node....
void Delete(LinkedList *&node)
{
    LinkedList *root = cache;
    LinkedList *prev = root;
    LinkedList *ahead = node->next;
    while (root != node)
    {
        prev = root;
        root = root->next;
    }
    prev->next = ahead;
}

// Evict the least recently used node from end of list..
void LRU_evict()
{
    LinkedList *root = cache;
    LinkedList *prev = root;
    while (root->next != NULL)
    {
        prev = root;
        root = root->next;
    }
    prev->next = NULL;
}

void gethostbyname(unsigned char *host, int query_type, LinkedList *&node);

int sz = 0;

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cout << "Enter the correct format" << std::endl;
        std::cout << "Format : ./exe <DNS server IP>" << std::endl;
        exit(1);
    }

    // Get the DNS server from 1st argument
    strcpy(dns_server, argv[1]);

    int max_cache_size;
    std::cout << "Enter the desired cache size : ";
    std::cin >> max_cache_size;

    unsigned char hostname[100] = {0};
    while (1)
    {
        std::cout << "\n------------------------------ NEW QUERY ------------------------------\n"
                  << std::endl;
        // hostname[15] = 0;
        printf(">>> Enter Hostname/exit to Lookup/cancel : ");
        scanf("%s", hostname);
        if (strcmp((const char *)hostname, "exit") == 0) // exit command
        {
            break;
        }
        k = 0;

        std::string s = (char *)hostname;
        LinkedList *found = In_Cache(s);

        if (found != NULL) // if found in cache
        {
            std::cout << "\033[31m"
                      << "Hostname found in cache"
                      << "\033[0m" << std::endl;
            LinkedList *node;
            char *buf;
            LinkedList *root = cache;
            while (root != NULL)
            {
                if (root->url == s)
                {
                    node = root;
                    buf = root->results;
                }
                root = root->next;
            }
            LinkedList *temp = node;
            Delete(node);
            Insert(temp);
            extract_result(buf);
        }
        else if (found == NULL) // if not present in cache
        {
            LinkedList *node = new LinkedList();
            node->url = (char *)hostname;

            // Now perform the IPv4 query for this hostname
            gethostbyname(hostname, 1, node);

            if (sz == max_cache_size) // size full
            {
                std::cout << "Cache Full...Evicting" << std::endl;
                LRU_evict();
                std::cout << "LRU Eviction done" << std::endl;
                Insert(node);
            }
            else
            {
                std::cout << "Cache Not Full...Inserting" << std::endl;
                Insert(node);
                sz++;
            }
        }
    }
    return 0;
}

// extract from node and print IPs
void extract_result(char *res)
{
    std::cout << "\033[32m"
              << "\nFound the following IPs :-"
              << "\033[0m" << std::endl;
    int i = 0;
    while (res[i] != 0)
    {
        if (res[i] == '#')
        {
            if (i)
                putc('\n', stdout);
            putc('\t', stdout);
        }
        else
        {
            putc(res[i], stdout);
        }
        i++;
    }
    putc('\n', stdout);
}

/* Perform a DNS query by sending a packet */
void gethostbyname(unsigned char *host, int query_type, LinkedList *&node)
{
    unsigned char buf[65536] = {0}, *qname, *reader;
    int i, s;
    struct sockaddr_in serv;
    struct DNS_HEADER *dns = NULL;
    struct QUESTION *qinfo = NULL;

    std::cout << "Resolving " << host;

    s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); // UDP connection with DNS server

    serv.sin_family = AF_INET;
    serv.sin_port = htons(53);
    serv.sin_addr.s_addr = inet_addr(dns_server); // dns servers

    // initializing the dns packet
    dns = (struct DNS_HEADER *)&buf;

    // Set the DNS header fields:
    dns->id = (unsigned short)htons(getpid()); // Set the ID field with a unique identifier
    dns->qr = 0;                               // This is a query (not a response)
    dns->opcode = 0;                           // This is a standard query
    dns->aa = 0;                               // Not Authoritative
    dns->tc = 0;                               // Not truncated
    dns->rd = 1;                               // Recursion desired
    dns->ra = 0;                               // Recursion not available
    dns->z = 0;                                // Reserved field, set to 0
    dns->ad = 0;                               // Not authenticated data
    dns->cd = 0;                               // No checking disabled
    dns->rcode = 0;                            // Response code set to 0 (no error)
    dns->q_count = htons(1);                   // Number of questions (1 question)
    dns->ans_count = 0;                        // Number of answers (0 answers)
    dns->auth_count = 0;                       // Number of authority resource records (0 authorities)
    dns->add_count = 0;                        // Number of additional resource records (0 additional)

    // point to the query portion
    qname = (unsigned char *)&buf[sizeof(struct DNS_HEADER)];
    ChangetoDnsNameFormat(qname, host);

    // query field
    qinfo = (struct QUESTION *)&buf[sizeof(struct DNS_HEADER) + (strlen((const char *)qname) + 1)]; // fill it

    qinfo->qtype = htons(query_type); // type of the query , A , MX , CNAME , NS etc
    qinfo->qclass = htons(1);         // internet

    // Sending the query
    std::cout << "\nSending Packet... ";
    if (sendto(s, (char *)buf, sizeof(struct DNS_HEADER) + (strlen((const char *)qname) + 1) + sizeof(struct QUESTION), 0, (struct sockaddr *)&serv, sizeof(serv)) < 0)
    {
        perror("sendto failed");
    }
    std::cout << "Done";

    // Receive the answer
    i = sizeof(serv);
    std::cout << "\nReceiving answer... ";
    if (recvfrom(s, (char *)buf, 65536, 0, (struct sockaddr *)&serv, (socklen_t *)&i) < 0)
    {
        perror("recvfrom failed");
    }
    std::cout << "Done";

    // move ahead of the dns header, hostname and question fields
    reader = &buf[sizeof(struct DNS_HEADER) + (strlen((const char *)qname) + 1) + sizeof(struct QUESTION)];

    dns = (struct DNS_HEADER *)buf;

    // read answers
    read_answers(buf, reader, dns);

    int p = 0;
    char *arr = new char[150];
    char *seq = arr;
    for (int i = 0; i < k; i++)
    {
        *seq = '#';
        int len = strlen(IPaddress[i]);
        strcpy(seq + 1, IPaddress[i]);
        seq = (seq + 1) + len;
    }
    *(seq + 1) = 0;

    // print answers
    extract_result(arr);

    node->results = arr;

    close(s);
}

// Reads the DNS response to queries
void read_answers(unsigned char buf[], unsigned char *reader, struct DNS_HEADER *dns)
{
    int stop = 0;
    int i, j;
    struct sockaddr_in a;

    // Start reading answers
    for (i = 0; i < ntohs(dns->ans_count); i++)
    {
        int len;
        answers[i].name = ReadName(reader, buf, &stop);
        reader = reader + stop;

        answers[i].resource = (struct R_DATA *)(reader);
        reader = reader + sizeof(struct R_DATA);

        if (ntohs(answers[i].resource->type) == 1) // if its an IPv4 address
        {
            len = ntohs(answers[i].resource->data_len);
            answers[i].rdata = new unsigned char[len];

            for (j = 0; j < len; j++)
            {
                answers[i].rdata[j] = reader[j];
            }
            answers[i].rdata[len] = '\0';

            long *p;
            p = (long *)answers[i].rdata;
            a.sin_addr.s_addr = (*p); // working without ntohl
            if (k < 10)
                strcpy(IPaddress[k++], inet_ntoa(a.sin_addr));

            reader = reader + len;
        }
        else // if its not an IPv4 address
        {
            answers[i].rdata = ReadName(reader, buf, &stop);
            reader = reader + stop;
        }
    }
}

// Read website names in DNS formats and converts them into hostname
unsigned char *ReadName(unsigned char *reader, unsigned char *buffer, int *count)
{
    unsigned char *name = new unsigned char[256];
    unsigned int p = 0, jumped = 0, offset;
    int i, j;

    *count = 1;

    name[0] = '\0';

    // read the names in 3www8facebook3com format
    while (*reader != 0)
    {
        if (*reader >= 192)
        {
            offset = (*reader) * 256 + *(reader + 1) - 49152; // 49152 = 11000000 00000000 ;)
            reader = buffer + offset - 1;
            jumped = 1; // we have jumped to another location so counting wont go up!
        }
        else
        {
            name[p++] = *reader;
        }

        reader = reader + 1;

        if (jumped == 0)
            *count = *count + 1; // if we havent jumped to another location then we can count up
    }

    name[p] = '\0'; // string complete
    if (jumped == 1)
        *count = *count + 1; // number of steps we actually moved forward in the packet

    // now convert 3www8facebook3com to www.facebook.com
    for (i = 0; i < (int)strlen((const char *)name); i++)
    {
        p = name[i];
        for (j = 0; j < (int)p; j++)
        {
            name[i] = name[i + 1];
            i = i + 1;
        }
        name[i] = '.';
    }
    name[i - 1] = '\0'; // remove the last dot
    return name;
}

/* This will convert www.facebook.com to 3www8facebook3com */
void ChangetoDnsNameFormat(unsigned char *dns, unsigned char *host)
{
    int lock = 0, i;
    strcat((char *)host, ".");

    for (i = 0; i < strlen((char *)host); i++)
    {
        if (host[i] == '.')
        {
            *dns++ = i - lock;
            for (; lock < i; lock++)
            {
                *dns++ = host[lock];
            }
            lock++; // or lock=i+1;
        }
    }
    *dns++ = '\0';
}