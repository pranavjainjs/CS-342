// To run
// g++ q1_weighted.cpp
// ./a.out 

#include <bits/stdc++.h>
using namespace std;

// Defining infinity for Dijkstra's application
int inf = 1e9;

//Router Class
class Router
{
    public:
        int Router_id;                              // Id
        vector<pair<Router *, int>> Neighbours;     // Neighbours
        vector<int> Routing_Table_distances;        // Distance of each node
        vector<int> Routing_Table;                  // Final Table

    Router(int id, int n)                           // Constructor
    {
        this->Router_id = id;
        Routing_Table_distances.resize(n + 1, inf);
        Routing_Table.resize(n + 1, -1);
    }

    // Add a neighbour to this router
    void Add_Neighbour(Router *neighbour, int weight)
    {
        Neighbours.push_back({neighbour, weight});
    }

    // Calculate the Routing Table according via Dijkstra
    void Update_Routing_Table()
    {
        Routing_Table_distances[this->Router_id] = 0;
        Routing_Table[this->Router_id] = Router_id;
        multiset<pair<int, Router *>> Dijkstra;         // Min heap
        Dijkstra.insert({0, this});                 

        while (Dijkstra.size() != 0)
        {
            pair<int, Router *> vertex = *(Dijkstra.begin());
            Dijkstra.erase(Dijkstra.begin());
            int distance = vertex.first;
            Router *router = vertex.second;
            for (auto child : router->Neighbours)
            {
                if (Routing_Table_distances[(child.first)->Router_id] > distance + child.second)
                {
                    Routing_Table[(child.first)->Router_id] = router->Router_id;
                    Routing_Table_distances[(child.first)->Router_id] = distance + child.second;
                    Dijkstra.insert({distance + child.second, child.first});
                }
            }
        }
    }

    // Print the Routing table
    void Print_Routing_Table()
    {
        int n = Routing_Table.size() - 1;
        for (int i = 1; i <= n; i++)
        {
            vector<int> path;
            if (i == Router_id)
            {
                cout << "Router " << i << ": ";
                cout << "Already in destination!" << endl;
                continue;
            }

            int link;
            if (Routing_Table[i] == this->Router_id)
            {
                link = i;
                path.push_back(link);
            }
            else
            {
                link = i;
                while (Routing_Table[link] != this->Router_id)
                {
                    path.push_back(link);
                    link = Routing_Table[link];
                }
                path.push_back(link);
            }

            path.push_back(this->Router_id);
            reverse(path.begin(), path.end());
            cout << "Router " << i << ": "
                 << "take exit via link connected to " << link << endl;
            cout << "Path: ";

            for (auto i : path)
            {
                cout << i << " ";
            }
            cout << endl;
        }
    }
};

int main()
{
    cout << "Enter the number of nodes and total edges in the system " << endl;
    int n, m;
    cin >> n >> m;

    vector<Router *> Routers(n + 1);
    for (int i = 1; i <= n; i++)
    {
        Routers[i] = new Router(i, n);
    }

    cout << "Enter the edges of the system (input m lines of the form a b c, indicating that nodes a and b are connected by an edge of weight c): " << endl;
    
    for (int i = 0; i < m; i++)
    {
        int node1, node2, weight;
        cin >> node1 >> node2 >> weight;
        Routers[node1]->Add_Neighbour(Routers[node2], weight);
        Routers[node2]->Add_Neighbour(Routers[node1], weight);
    }
    cout << endl;

    for (int i = 1; i <= n; i++)
    {
        Routers[i]->Update_Routing_Table();
    }
    
    for (int i = 1; i <= n; i++)
    {
        cout << "Printing Routing Table for " << i << endl;
        Routers[i]->Print_Routing_Table();
        cout << endl;
    }
    return 0;
}
