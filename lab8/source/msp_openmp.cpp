#include <iostream>
#include <vector>
#include <queue>
#include <fstream>
#include <sstream>
#include <omp.h>
#include <limits>
#include <unordered_map>
#include <unordered_set>
using namespace std;

const float INF = numeric_limits<float>::infinity();

struct Edge {
    int to;
    float weight;
};

void read_graph(const string& filename, unordered_map<int, vector<Edge>>& graph, int& max_node) {
    ifstream file(filename);
    string line;
    getline(file, line); // skip header

    int u, v;
    float w;
    max_node = 0;
    while (getline(file, line)) {
        stringstream ss(line);
        string token;
        getline(ss, token, ','); if (token.empty()) continue;
        u = stoi(token);
        getline(ss, token, ',');
        v = stoi(token);
        getline(ss, token, ',');
        w = stof(token);

        graph[u].push_back({v, w});
        graph[v].push_back({u, w}); // undirected
        max_node = max(max_node, max(u, v));
    }
}

vector<float> dijkstra(int src, const unordered_map<int, vector<Edge>>& graph, int V) {
    vector<float> dist(V + 1, INF);
    priority_queue<pair<float, int>, vector<pair<float, int>>, greater<>> pq;
    dist[src] = 0;
    pq.push({0, src});
    while (!pq.empty()) {
        auto [d, u] = pq.top(); pq.pop();
        if (d > dist[u]) continue;
        if (graph.count(u) == 0) continue;
        for (auto& edge : graph.at(u)) {
            if (dist[edge.to] > dist[u] + edge.weight) {
                dist[edge.to] = dist[u] + edge.weight;
                pq.push({dist[edge.to], edge.to});
            }
        }
    }
    return dist;
}

int main(int argc, char* argv[]) {
    string adj_file = argv[1];
    string test_file = argv[2];
    int num_threads = atoi(argv[3]);

    unordered_map<int, vector<Edge>> graph;
    int V1;
    read_graph(adj_file, graph, V1);

    unordered_set<int> source_set;
    vector<pair<int, int>> test_pairs;
    int V2 = 0;

    {
        ifstream tf(test_file);
        string line;
        getline(tf, line); // skip header

        while (getline(tf, line)) {
            stringstream ss(line);
            string token;
            int s, t;

            getline(ss, token, ','); if (token.empty()) continue;
            s = stoi(token);
            getline(ss, token, ','); t = stoi(token);
            // ignore third column (distance)

            test_pairs.emplace_back(s, t);
            source_set.insert(s);
            V2 = max(V2, max(s, t));
        }
    }

    int V = max(V1, V2);  // ensure vector size covers both graph + test data
    vector<int> sources(source_set.begin(), source_set.end());
    vector<vector<float>> all_dist(V + 1);

    double start = omp_get_wtime();

    #pragma omp parallel for num_threads(num_threads) schedule(dynamic)
    for (int i = 0; i < sources.size(); ++i) {
        int s = sources[i];
        vector<float> dist = dijkstra(s, graph, V);
        all_dist[s] = dist;
    }

    double end = omp_get_wtime();
    cout << "Time taken: " << (end - start) * 1000 << " ms\n";

    // for (auto& [s, t] : test_pairs) {
    //     if (s <= V && t <= V)
    //         cout << s << " " << t << " " << all_dist[s][t] << endl;
    // }

    return 0;
}
