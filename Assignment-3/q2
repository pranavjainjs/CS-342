// To run
// g++ q2.cpp
// ./a.out 5000 127.0.0.1

// Necessary header files
#include <bits/stdc++.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <mutex>
using namespace std;
// For color printing in linux terminal
#define RESET "\033[0m"
#define BLACK "\033[30m"              /* Black */
#define RED "\033[31m"                /* Red */
#define GREEN "\033[32m"              /* Green */
#define YELLOW "\033[33m"             /* Yellow */
#define BLUE "\033[34m"               /* Blue */
#define MAGENTA "\033[35m"            /* Magenta */
#define CYAN "\033[36m"               /* Cyan */
#define WHITE "\033[37m"              /* White */
#define BOLDBLACK "\033[1m\033[30m"   /* Bold Black */
#define BOLDRED "\033[1m\033[31m"     /* Bold Red */
#define BOLDGREEN "\033[1m\033[32m"   /* Bold Green */
#define BOLDYELLOW "\033[1m\033[33m"  /* Bold Yellow */
#define BOLDBLUE "\033[1m\033[34m"    /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m" /* Bold Magenta */
#define BOLDCYAN "\033[1m\033[36m"    /* Bold Cyan */
#define BOLDWHITE "\033[1m\033[37m"   /* Bold White */
typedef long long ll;

// A class to maintain data about each node
class Node
{
    public:
        int id; // Id 
        int state; // State
        int successful_transmissions; // Number of successful transmissions done
        int collisions; // Number of collisions 
        int backoff_count; // Number of times the node backed-off
        int total_tries; // Total number of times node tried to send
        int start_time; // The time at which the node started transmitting 
        int backoff_interval; // Current backoff interval
        int max_successful_transmissions; // Total number of transmissions the node has to do
        int transmission_time; // Time taken to transmit
        int propagation_time; // Time taken to propagate

    // Constructor
    Node(int id, int state, int successful_transmissions, int collisions, int backoff_count, int total_tries, int start_time, int backoff_interval, int max_successful_transmissions, int transmission_time, int propagation_time)
    {
        this->id = id;
        this->state = state;
        this->successful_transmissions = successful_transmissions;
        this->collisions = collisions;
        this->backoff_count = backoff_count;
        this->total_tries = total_tries;
        this->start_time = start_time;
        this->backoff_interval = backoff_interval;
        this->max_successful_transmissions = max_successful_transmissions;
        this->transmission_time = transmission_time;
        this->propagation_time = propagation_time;
    }
};

vector<Node *> node_data; // Vector to maintain data about each node
vector<int> time_to_transmit; // Time to transmit for each node is stored
vector<int> time_to_propagate; // Time to propagate for each node is stored
vector<int> time_to_done; // Time when the message will be received at the receiver
vector <bool> start_doing; // Boolean variable maintained to give access to a node to do it's work
mutex print_lock; // Lock
int max_backoff_interval; // Maximum backoff interval
int channel_use = -1; // The node that is currently propagating in the channel
int timer = 0; // Global time variable

// Some other necessary variables
bool propagating_rn = false;
bool done = false;
vector<int> temp;
int n;
int done1 = -1;
bool just = false;
int used = -1;

// Function for each node
void simulation(int i)
{
    // Runs till all the transmissions take place
    while (node_data[i]->successful_transmissions < node_data[i]->max_successful_transmissions)
    {
        // State 
        // 0 - The node is not doing anything
        // 1 - The state is about to transmit
        // 2 - The node is transmittingh
        // 3 - The node has completed transmission and is about to propagate
        // 4 - The node is propagating

        if(start_doing[i])
        {
            // Node goes into state 1 from state 0 when it has to transmit
            if (node_data[i]->state == 0)
            {
                if (time_to_transmit[i] == timer)
                {
                    node_data[i]->state = 1;
                }
            }

            // Node goes into state 3 from state 2 when it has to propagate
            if (node_data[i]->state == 2)
            {
                if (time_to_propagate[i] == timer)
                {
                    node_data[i]->state = 3;
                }
            }

            if (node_data[i]->state == 0)
            {
                time_to_propagate[i] = -1;
                time_to_done[i] = -1;
            }
            else if (node_data[i]->state == 1)
            {
                node_data[i]->total_tries++;

                // If channel is in use, the node doesn't transmit. Otherwise, it does
                if (channel_use == -1)
                {
                    node_data[i]->state = 2;
                    time_to_propagate[i] = timer + node_data[i]->transmission_time;

                    print_lock.lock();
                    cout << BOLDWHITE << "Node " << i << BLUE << ": Beginning transmission. Transmission will be completed at time = " << time_to_propagate[i] << RESET<< endl;
                    print_lock.unlock();
                }
                else
                {
                    node_data[i]->backoff_count++;

                    node_data[i]->backoff_interval = min(node_data[i]->backoff_interval * 2, max_backoff_interval);
                    time_to_transmit[i] = timer + rand() % node_data[i]->backoff_interval + 1;
                    time_to_propagate[i] = -1;
                    time_to_done[i] = -1;

                    print_lock.lock();
                    if(used != -1)
                        cout << BOLDWHITE << "Node " << i << BOLDRED << ": Couldn't transmit because channel wasn't free as it is being used by Node " << used << ". Will try to transmit again at " << time_to_transmit[i] << "." << RESET << endl;
                    else 
                        cout << BOLDWHITE << "Node " << i << BOLDRED << ": Couldn't transmit because channel wasn't free as it is being used by Node " << channel_use << ". Will try to transmit again at " << time_to_transmit[i] << "." << RESET << endl;
                    print_lock.unlock();

                    node_data[i]->state = 0;
                }
            }
            else if (node_data[i]->state == 2)
            {
                
            }
            else if (node_data[i]->state == 3)
            {
                // When a node is in use, the node doesn't propagate. Otherwise, it does.
                if (channel_use == -1)
                {
                    node_data[i]->state = 4;
                    channel_use = i;
                    time_to_done[i] = timer + node_data[i]->propagation_time;
                    time_to_propagate[i] = -1;
                    time_to_transmit[i] = -1;

                    print_lock.lock();
                    cout << BOLDWHITE << "Node " << i << MAGENTA << ": Found the channel free. Started propagation. Will be done at time = " << time_to_done[i] << RESET << endl;
                    print_lock.unlock();
                }
                else 
                {
                    node_data[i]->collisions++;
                    node_data[i]->backoff_count++;
                    node_data[i]->backoff_interval = min(node_data[i]->backoff_interval * 2, max_backoff_interval);
                    time_to_transmit[i] = timer + rand() % node_data[i]->backoff_interval + 1;
                    time_to_propagate[i] = -1;
                    time_to_done[i] = -1;

                    print_lock.lock();
                    cout << BOLDWHITE << "Node " << i << BOLDRED << ": Couldn't propagate because of a collision. Will try to transmit again at " << time_to_transmit[i] << ". " << RESET << endl;
                    print_lock.unlock();
                    
                    node_data[i]->state = 0;
                }
            }
            else if (node_data[i]->state == 4)
            {
                // The node has successfully propagated
                if (time_to_done[i] == timer)
                {
                    channel_use = -1;
                    node_data[i]->successful_transmissions++;
                    node_data[i]->state = 0;
                    time_to_propagate[i] = -1;
                    time_to_done[i] = -1;

                    print_lock.lock();
                    cout << BOLDWHITE << "Node " << i << BOLDGREEN << ": Successful propagation! ";
                    if (node_data[i]->successful_transmissions < node_data[i]->max_successful_transmissions)
                    {
                        time_to_transmit[i] = timer + 1;
                        cout << "Next data packet will be transmitted at time = " << time_to_transmit[i] << "." << RESET << endl;
                    }
                    else
                    {
                        time_to_transmit[i] = -1;
                        cout << "Done for now. No packets left to transmit." << RESET << endl;
                    }
                    print_lock.unlock();
                }
                else
                {
                    // When some other node comes for propagating, while this node is propagting. Both of them backoff.
                    if (propagating_rn == true)
                    {
                        propagating_rn = false;
                        node_data[i]->collisions++;
                        node_data[i]->backoff_count++;
                        node_data[i]->backoff_interval = min(node_data[i]->backoff_interval * 2, max_backoff_interval);
                        time_to_transmit[i] = timer + rand() % node_data[i]->backoff_interval + 1;
                        time_to_propagate[i] = -1;
                        time_to_done[i] = -1;
                        channel_use = -1;

                        print_lock.lock();
                        cout << BOLDWHITE << "Node " << i << BOLDRED << ": Couldn't propagate because of a collision. Will try to transmit again at " << time_to_transmit[i] << "." << RESET << endl;
                        node_data[i]->state = 0;
                        print_lock.unlock();
                    }
                }
            }
            start_doing[i] = false;
        }
    }
}

// Function to control the system of nodes
void increment_time()
{
    while (!done)
    {  
        timer++; // Incrementing the timer

        temp.clear();
        done1 = -1;
        just = false;
        used = -1;

        print_lock.lock();
        cout << endl;
        cout << BOLDCYAN << "Time " << timer << RESET << endl;
        print_lock.unlock();

        // Finding the nodes which are about to propagate
        for(int i = 0; i < n; i++)
        {
            if(timer == time_to_propagate[i])
            {
                temp.push_back(i);
            }
            if(timer == time_to_done[i])
            {
                start_doing[i] = true;
                done1 = i;
                while(start_doing[i]);
            }
        }

        // If no nodes are going to start propagating now
        if(temp.size() == 0)
        {
            for(int i = 0; i < n; i++)
            {
                if(done1 != i){
                    start_doing[i] = true;
                } 
            }
        }
        else if(temp.size() == 1) // If one node is wants to start propagating
        {
            // If channel is not in use and otherwise
            if(channel_use == -1)
            {
                start_doing[temp[0]] = true;
                while(start_doing[temp[0]]);
                just = true;
            }
            else
            {
                propagating_rn = true;
                just = true;
                used = channel_use;
                start_doing[temp[0]] = true;
                while(start_doing[temp[0]]);
                start_doing[channel_use] = true;
                while(start_doing[channel_use]);
            }
        }
        else
        {
            // If more than one node wants to propagate
            // If channel is not in use and otherwise
            if(channel_use == -1)
            {
                channel_use = -2;
                for(int i = 0; i < temp.size(); i++)
                {
                    start_doing[temp[i]] = true;
                    while(start_doing[temp[i]]);
                }
                channel_use = -1;
            }
            else
            {
                propagating_rn = true;
                just = true;
                used = channel_use;
                start_doing[channel_use] = true;
                while(start_doing[channel_use]);
                channel_use = -2;
                for(int i = 0; i < temp.size(); i++)
                {
                    start_doing[temp[i]] = true;
                    while(start_doing[temp[i]]);
                }
                channel_use = -1;
            }
        }

        // For nodes that are not going to start propagating now
        if(channel_use == -1)
        {
            if(just) channel_use = -2;
            for(int i = 0; i < n; i++)
            {
                if(time_to_transmit[i] == timer) 
                {
                    start_doing[i] = true;
                    while(start_doing[i]);
                }
            }
            channel_use = -1;
        }
        else
        {
            int x = channel_use;
            if(just)
            {
                channel_use = -1;
            }
            for(int i = 0; i < n; i++)
            {
                if(time_to_transmit[i] == timer) 
                {
                    start_doing[i] = true;
                    while(start_doing[i]);
                }
            }
            channel_use = x;
        }
        sleep(1);
    }
}

int main()
{
    int init_backoff, latest, trans_time, prop_time;
    vector <int> max_transmissions_each_node;

    // Taking input from the user
    cout << BOLDCYAN << "Enter the number of nodes for which CSMA has to be simulated." << RESET << endl;
    cin >> n;
    cout << endl;

    cout << BOLDCYAN << "Enter the number of messages each node has to send." << RESET << endl;
    for(int i = 0; i < n; i++)
    {
        int x; cin >> x;
        max_transmissions_each_node.push_back(x);
    }
    cout << endl;

    cout << BOLDCYAN << "Enter the maximum backoff delay." << RESET << endl;
    cin >> max_backoff_interval;
    cout << endl;

    cout << BOLDCYAN << "Enter the maximum intial backoff interval." << RESET << endl;
    cin >> init_backoff;
    cout << endl;

    cout << BOLDCYAN << "Enter the latest time by which each node should start transmitting." << RESET << endl;
    cin >> latest;
    cout << endl;

    cout << BOLDCYAN << "Enter the maximum possible transmission time." << RESET << endl;
    cin >> trans_time;
    cout << endl;

    cout << BOLDCYAN << "Enter the maximum possible propagation time." << RESET << endl;
    cin >> prop_time;
    cout << endl;

    cout << BOLDCYAN << "Start time, Initial backoff interval, Transmission time, Propagation time of each node will be generated randomly taking the given inputs into consideration." << RESET << endl;
    cout << endl;

    // Resizing vectors
    node_data.resize(n);
    time_to_done.resize(n,-1);
    time_to_propagate.resize(n,-1);
    time_to_transmit.resize(n);
    start_doing.resize(n,false);

    // Initializing the data values for all the nodes
    for (int i = 0; i < n; i++)
    {
        node_data[i] = new Node(i, 0, 0, 0, 0, 0, rand() % latest + 1, rand() % init_backoff + 1, max_transmissions_each_node[i], rand() % trans_time + 1, rand() % prop_time + 1);
        time_to_transmit[i] = node_data[i]->start_time;
        cout << BOLDMAGENTA << "The node " << i << " will transmit it's data for the first time at time = " << node_data[i]->start_time << " and the initial backoff-interval is " << node_data[i]->backoff_interval << RESET << endl;
    }

    // Creating a thread for each node
    vector<thread> node(n);

    for (int i = 0; i < n; i++)
    {
        node[i] = thread(simulation, i);
    }

    // A master thread for controlling
    thread master(increment_time);
  
    for (int i = 0; i < n; i++)
    {
        node[i].join();
    }

    done = true;

    master.join();

    // Printing the statistics
    cout << endl;
    cout << endl;
    cout << BOLDCYAN << "Id\tSuccessful transmissions\tStart time\tTransmission time\tPropagation time\tCollisions\tBackoff Count\tTotal Tries" << RESET << endl;
    for (int i = 0; i < n; i++)
    {
        cout << node_data[i]->id << "\t\t";
        cout << node_data[i]->successful_transmissions << "\t\t\t     ";
        cout << node_data[i]->start_time << "\t\t\t";
        cout << node_data[i]->transmission_time << "\t\t\t";
        cout << node_data[i]->propagation_time << "\t\t     ";
        cout << node_data[i]->collisions << "\t\t     ";
        cout << node_data[i]->backoff_count << "\t\t     ";
        cout << node_data[i]->total_tries << "\t\t\t";
        cout << endl;
    }
    cout << endl;
}
