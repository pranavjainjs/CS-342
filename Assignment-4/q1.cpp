#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

// Class HttpRequest..
class HttpRequest
{
public:
    int ID;
    int Website_ID;
    int processing_time;
    HttpRequest(int ID, int Website_ID, int processing_time)
    {
        this->ID = ID;
        this->Website_ID = Website_ID;
        this->processing_time = processing_time;
    }
};

// Class Website..
class Website
{
public:
    int ID;
    string owner;
    queue<HttpRequest *> requests;
    int processing_power;
    int bandwidth;
    Website(int ID, string owner, int bandwidth, int processing_power)
    {
        this->ID = ID;
        this->owner = owner;
        while (this->requests.size() != 0)
        {
            this->requests.pop();
        }
        this->bandwidth = bandwidth;
        this->processing_power = processing_power;
    }
};

// Class LoadBalancer..
class LoadBalancer
{
public:
    vector<Website *> All_Websites;         // Maintains all websites belonging to server.
    queue<HttpRequest *> All_HttpRequests;  // FIFO entry of reuquests.
    Website *dequeue_from;                  // Website from which http request is to be dequed.
    vector<pair<int,pair<int,int>>> Order;                      // Order in which requests are processed.
    map<pair<int,int>,int>processing_time_req;
    // Add a new website to the server.
    void Add_Website(int web_id, string owner, int bandwidth, int processing_power)
    {
        Website *Web = new Website(web_id, owner, bandwidth, processing_power);
        All_Websites.push_back(Web);
    }

    // Enqueue a new http request to the appropriate website.
    void Enqueue_Request(HttpRequest *Request)
    {
        Website *Web;
        for (int i = 0; i < All_Websites.size(); i++)
        {
            if (All_Websites[i]->ID == Request->Website_ID)
            {
                Web = All_Websites[i];
                // cout<<All_Websites[i]->ID<<endl;
                break;
            }
        }
        // cout<<"Done"<<endl;
        Web->requests.push(Request);
        All_HttpRequests.push(Request);
        processing_time_req[{Web->ID,Request->ID}]=Request->processing_time;
    }

    // Dequeue the newly served http request from the appropriate website.
    void Dequeue_Request()
    {
        dequeue_from->requests.pop();
    }

    // Implemented Weighted Fair Queuing.
    void WFQ_Scheduling()
    {
        map<int, int> latest_finished_time;             // Maintain latest time when a http request was finished processing by each website.
        map<int, set<pair<int, int>>> finished;         // For each time interval maintain http requests which were finished processing at that instant.
        int tot_band_pow = 0;
        for (auto i : All_Websites)
        {
            tot_band_pow += (i->bandwidth) + (i->processing_power);
        }

        // Assume all requests arrived at same time.
        while (All_HttpRequests.size() != 0)
        {
            HttpRequest *Request = All_HttpRequests.front();
            All_HttpRequests.pop();
            Website *Web;
            for (auto i : All_Websites)
            {
                if (i->ID == Request->Website_ID)
                {
                    Web = i;
                    break;
                }
            }
            int web_id = Web->ID;
            int req_id = Request->ID;
            double transfer_time = double(Request->processing_time * tot_band_pow) / (Web->bandwidth + Web->processing_power);
            if (transfer_time != int(transfer_time))
            {
                transfer_time = int(transfer_time) + 1;
            }
            cout<<req_id<<" "<<transfer_time<<endl;
            int time_taken = latest_finished_time[web_id] + int(transfer_time);
            latest_finished_time[web_id] = time_taken;
            finished[time_taken].insert({web_id, req_id});
            dequeue_from = Web;
            Dequeue_Request();
        }
        for (auto t : finished)
        {
            for (auto req : t.second)
            {
                Order.push_back({t.first,{req.first,req.second}});
            }
        }
    }
    
    // Function to Print the final order in which the http requests where served..
    void Print_Order()
    {
        if (Order.size()==0)
        {
            cout<<"NO Http REQUEST HAS BEEN ENTERED!"<<endl;
            return;
        }
        
        sort(Order.begin(),Order.end());
        int cur_time=0;
        cout << "Requests Served in this Order: "<<endl;
        for (auto i : Order)
        {
            // int t=i.first;
            int web_id=i.second.first;
            int req_id=i.second.second;
            cur_time+=processing_time_req[{web_id,req_id}];
            cout<<"Http Request with Http Request ID : "<<req_id<<" has been served by Website with Website ID : "<<web_id<<" at time "<<cur_time<<endl;
        }
        cout << endl;
    }
};

int main()
{
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    LoadBalancer Master;
    int n, m;
    cout << "Enter the number of Websites : "<<endl;
    cin >> n;
    for (int i = 0; i < n; i++)
    {
        int ID;
        string owner;
        int processing_power;
        int bandwidth;
        cout << "Enter the details of Website (ID, Owner, Bandwidth, Processing Power) " << i + 1 << " "<<endl;
        cin >> ID >> owner >> bandwidth >> processing_power;
        Master.Add_Website(ID, owner, bandwidth, processing_power);
    }
    cout << "Enter the number of Http Requests : "<<endl;
    cin >> m;
    // cout<<m<<endl;
    for (int i = 0; i < m; i++)
    {
        int ID;
        int Website_ID;
        int processing_time;
        cout << "Enter the details of Http Request (ID, Website ID, Processing Time) " << i + 1 << " "<<endl;
        cin >> ID >> Website_ID >> processing_time;
        // cout << endl;
        HttpRequest *Req = new HttpRequest(ID, Website_ID, processing_time);
        Master.Enqueue_Request(Req);
    }
    Master.WFQ_Scheduling();
    Master.Print_Order();

    return 0;
}


// Test Case 1
//  2 
//  1 a 1 1
//  2 b 1 1
//  4
//  1 1 1
//  2 1 1
//  3 2 1
//  4 2 1
// Expected Output : 1 3 2 4

// Test Case 2
//  2 
//  1 a 1 1
//  2 b 2 1
//  4
//  1 1 1
//  2 1 1
//  3 2 1
//  4 2 1
// Expected Output : 3 1 4 2

// Test Case 3
//  2 
//  1 a 1 1
//  2 b 1 2
//  4
//  1 1 1
//  2 1 1
//  3 2 1
//  4 2 1
// Expected Output : 3 1 4 2

// Test Case 4
//  4 
//  1 a 1 1
//  2 b 1 1
//  3 c 1 1
//  4 d 1 1
//  6
//  1 1 1
//  2 1 1
//  3 2 1
//  4 2 1
//  5 3 1
//  6 4 1
// Expected Output : 1 3 5 6 2 4

// Test Case 5
//  2 
//  1 a 1 1
//  2 b 1 1
//  12
//  1 1 1
//  2 1 1
//  3 2 1
//  4 2 1
//  5 1 2
//  6 2 3
//  7 1 2
//  8 2 3
//  9 1 7
//  10 2 4
//  11 1 5
//  12 2 5
// Expected Output : 1 3 2 4 5 6 7 8 10 9 12 11

// Test Case 6
//  2 
//  1 a 1 1
//  2 b 1 1
//  0
// Expected Output : -

// Test Case 7
//  2 
//  1 a 1 2
//  2 b 2 3
//  4
//  1 1 2
//  2 1 1
//  3 2 2
//  4 2 2
// Expected Output :  3 1 4 2

// Test Case 8
//  1 
//  1 a 1 1
//  4
//  1 1 1
//  2 1 2
//  3 1 1
//  4 1 2
// Expected Output : 1 2 3 4

