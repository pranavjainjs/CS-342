#include <bits/stdc++.h>
using namespace std;

// Function to generate values to form an exponential distribution
double generateExponentialTime(double lambda)
{
    random_device rd;
    mt19937 gen(rd());
    exponential_distribution<double> exponential(lambda);
    return exponential(gen);
}

// Main function
int main()
{
    // Taking input from the user
    int num_of_servers, size_of_buffer;
    cout << "Please enter the number of security scanners in the system: ";
    cin >> num_of_servers;
    cout << "Please enter the size of buffer: ";
    cin >> size_of_buffer;

    double arrival_rate, service_rate;
    cout << "Please enter the arrival rate: ";
    cin >> arrival_rate;
    cout << "Please enter the service rate: ";
    cin >> service_rate;

    // Taking number of passengers
    cout << "Please enter the number of passengers that will arrive: ";
    int total_p;
    cin >> total_p;

    cout << endl;
    cout << "Passengers are going to arrive at time = :-" << endl;
    cout << "0" << endl;
    vector<double> arrival_times;
    arrival_times.push_back(0);
    double current_time = 0;

    // Generate arrival times for passengers based on exponential distribution
    while (arrival_times.size() < total_p)
    {
        double x = generateExponentialTime(arrival_rate);
        current_time += x;
        arrival_times.push_back(current_time);
        cout << current_time << endl;
    }

    // Taking time till which passengers can arrive or join the queue
    // double total_time;
    // cout << "Please enter the time till which passengers can arrive: ";
    // cin >> total_time;

    // cout << endl;
    // cout << "Passengers are going to arrive at time = :-" << endl;
    // cout << "0" << endl;
    // vector <double> arrival_times;
    // arrival_times.push_back(0);
    // double current_time = 0;
    // while(1)
    // {
    //     double x = generateExponentialTime(arrival_rate);
    //     if(current_time + x <= total_time)
    //     {
    //         current_time += x;
    //         arrival_times.push_back(current_time);
    //         cout << current_time << endl;
    //     }
    //     else
    //     {
    //         break;
    //     }
    // }

    cout << endl;

    // Simulation
    cout << "Starting the simulation..." << endl;
    sleep(1);
    cout << endl;

    // Data structures to store information about passengers
    vector<queue<double>> occupied(num_of_servers);
    vector<bool> server_busy(num_of_servers, false);
    vector<double> completion_time(num_of_servers, double(-1));
    vector<double> waiting_time(num_of_servers, double(0));
    vector<int> passengers_serviced(num_of_servers, 0);
    vector<double> queue_length(num_of_servers, double(0));
    vector<double> enter_queue_time(num_of_servers);
    vector<double> time_done(num_of_servers);
    vector<double> active_time(num_of_servers, double(0));
    vector<double> start_time(num_of_servers, double(-1));
    int total_passengers = 0;
    double total_time_end = double(0);
    int index = 0;
    int dropped_passengers = 0;

    // While loop for simulation
    while (index < arrival_times.size()) // Runs till there are passengers arrive
    {
        for (int i = 0; i < num_of_servers; i++)
        {
            // Check each server for completed service
            while (completion_time[i] != -1 && completion_time[i] <= arrival_times[index])
            {
                if (occupied[i].size() == 0)
                {
                    // Server completed servicing a passenger
                    cout << "Time " << completion_time[i] << ": Server " << i << " has completed servicing a passenger." << endl;
                    time_done[i] = completion_time[i];
                    passengers_serviced[i]++;
                    total_passengers++;
                    completion_time[i] = -1;
                    server_busy[i] = false;
                }
                else
                {
                    // Server completed servicing a passenger and started servicing another
                    cout << "Time " << completion_time[i] << ": Server " << i << " has completed servicing a passenger and now has started servicing a passenger who had";
                    cout << " arrived at time = " << occupied[i].front() << " and the passenger's servicing will be completed at time = ";
                    queue_length[i] += (completion_time[i] - enter_queue_time[i]) * occupied[i].size();
                    enter_queue_time[i] = completion_time[i];
                    double x = generateExponentialTime(service_rate);
                    completion_time[i] += x;
                    active_time[i] += x;
                    cout << completion_time[i] << "." << endl;
                    waiting_time[i] += (arrival_times[index] - occupied[i].front());
                    occupied[i].pop();
                    passengers_serviced[i]++;
                    total_passengers++;
                }
            }
        }

        // Find free servers
        vector<int> free_servers;
        for (int i = 0; i < num_of_servers; i++)
        {
            if (server_busy[i] == false)
            {
                free_servers.push_back(i);
            }
        }

        // Assign a passenger to a free server or buffer
        if (free_servers.size() > 0)
        {
            int x = rand() % (free_servers.size());
            double y = generateExponentialTime(service_rate);
            active_time[free_servers[x]] += y;
            completion_time[free_servers[x]] = arrival_times[index] + y;
            server_busy[free_servers[x]] = true;
            if(start_time[free_servers[x]] == -1) 
            {
                start_time[free_servers[x]] = arrival_times[index];
            }
            cout << "Time " << arrival_times[index] << ": Server " << free_servers[x] << " has started servicing the passenger who just arrived. Servicing will be completed at time ";
            cout << completion_time[free_servers[x]] << "." << endl;
            enter_queue_time[free_servers[x]] = arrival_times[index];
        }
        else
        {
            // Find a server with the least occupied buffer
            int ind = -1;
            int least = INT_MAX;
            for (int i = 0; i < num_of_servers; i++)
            {
                if (occupied[i].size() < size_of_buffer)
                {
                    if (occupied[i].size() < least)
                    {
                        least = occupied[i].size();
                        ind = i;
                    }
                }
            }
            if (ind == -1)
            {
                // All buffers are filled and all servers are busy, drop the passenger
                cout << "Time " << arrival_times[index] << ": All the buffers are filled and all the servers are busy. Thus the passenger who just arrived is being dropped." << endl;
                dropped_passengers++;
            }
            else
            {
                // The passenger has joined the queue of a server
                cout << "Time " << arrival_times[index] << ": The passenger has joined the queue of the server " << ind << "." << endl;
                queue_length[ind] += (arrival_times[index] - enter_queue_time[ind]) * occupied[ind].size();
                occupied[ind].push(arrival_times[index]);
                enter_queue_time[ind] = arrival_times[index];
            }
        }
        index++;
    }

    // Process remaining completion times after all passengers arrive
    cout << endl;
    cout << "Arrivals done!!!" << endl;
    cout << endl;

    double time_ended = double(0);
    for (int i = 0; i < num_of_servers; i++)
    {
        while (completion_time[i] != -1)
        {
            if (occupied[i].size() == 0)
            {
                // Server completed servicing a passenger
                cout << "Time " << completion_time[i] << ": Server " << i << " has completed servicing a passenger." << endl;
                time_done[i] = completion_time[i];
                completion_time[i] = -1;
                server_busy[i] = false;
                passengers_serviced[i]++;
                total_passengers++;
            }
            else
            {
                // Server completed servicing a passenger and started servicing another
                double x = occupied[i].front();
                cout << "Time " << completion_time[i] << ": Server " << i << " has completed servicing a passenger and now has started servicing a passenger who had arrived ";
                queue_length[i] += (completion_time[i] - enter_queue_time[i]) * (occupied[i].size());
                enter_queue_time[i] = completion_time[i];
                waiting_time[i] += (completion_time[i] - x);
                double y = generateExponentialTime(service_rate);
                active_time[i] += y;
                completion_time[i] += y;
                cout << "at time = " << x << " and the servicing of the passenger will be completed at " << completion_time[i] << endl;
                occupied[i].pop();
                passengers_serviced[i]++;
                total_passengers++;
            }
        }
    }

    for (int i = 0; i < num_of_servers; i++)
    {
        total_time_end = max(total_time_end, time_done[i]);
    }

    cout << endl;
    cout << "Simulation done!!!" << endl;
    cout << endl;
    cout << "Printing statistics:" << endl;
    cout << endl;
    cout << "The number of security scanners in the system: ";
    cout << num_of_servers;
    cout << "\nThe size of buffer: ";
    cout << size_of_buffer;

    cout << "\nArrival rate: ";
    cout << arrival_rate;
    cout << "\nService rate: ";
    cout << service_rate;
    cout << endl;
    // Taking number of passengers

    cout << "Total number of passengers arrived: " << arrival_times.size() << endl;
    cout << "Total number of passengers serviced: " << total_passengers << endl;
    cout << "Total number of passengers dropped: " << dropped_passengers << endl;
    cout << endl;
    double total_waiting_time = 0;
    for (int i = 0; i < num_of_servers; i++)
    {
        cout << "Total waiting time for queue of Server " << i << ": " << waiting_time[i] << endl;
        cout << "Total numbers of passengers serviced by the Server " << i << ": " << passengers_serviced[i] << endl;
        double x = waiting_time[i] / double(passengers_serviced[i]);
        total_waiting_time += waiting_time[i];
        cout << "Average waiting time of the queue of Server " << i << ": " << x << endl;
        cout << endl;
    }
    cout << "Average waiting time: " << total_waiting_time / double(total_passengers) << endl;

    cout << endl;
    cout << "Total time of simulation: " << total_time_end << endl;
    cout << endl;
    double total_queue_length = 0;
    for (int i = 0; i < num_of_servers; i++)
    {
        cout << "Total queue length for queue of Server " << i << ": " << queue_length[i] << endl;
        cout << "Total time till which Server " << i << " was servicing: " << time_done[i] << endl;
        double x = queue_length[i] / double(time_done[i]-start_time[i]);
        total_queue_length += x;
        cout << "Average queue length of the queue of Server " << i << ": " << x << endl;
        cout << endl;
    }
    cout << "Average queue length: " << total_queue_length / num_of_servers << endl;
    cout << endl;

    for (int i = 0; i < num_of_servers; i++)
    {
        cout << "Total time Server " << i << " was active: " << active_time[i] << endl;
        cout << "Percentage of time Server " << i << " was active: " << (active_time[i] / total_time_end) * 100 << endl;
    }

    cout << endl;
}