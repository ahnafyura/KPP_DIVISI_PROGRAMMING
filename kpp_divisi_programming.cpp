#include <bits/stdc++.h>
using namespace std;

const int MAX_ENERGY = 1000;

struct Edge {
    int to;
    int length;
    int obstacle;
};

struct State {
    int node;
    int minuteMod60;
    int hourParity;
    int consumed;
    bool operator==(const State &o) const {
        return node == o.node && minuteMod60 == o.minuteMod60 &&
               hourParity == o.hourParity && consumed == o.consumed;
    }
};

struct StateHash {
    size_t operator()(const State &s) const noexcept {
        return ((size_t)s.node * 1000003u) ^ ((size_t)s.minuteMod60 * 1009u) ^
               ((size_t)s.hourParity * 9176u) ^ ((size_t)s.consumed * 1315423911u);
    }
};

struct ParentInfo {
    State prev;
    int prevNode;
    int arrivalMinute;
};

class Graph {
public:
    int N;
    unordered_map<string, int> nameToId;
    vector<string> idToName;
    vector<vector<Edge>> adj;
    vector<char> isRestPoint, isCharge;

    Graph(int n = 0) { init(n); }

    void init(int n) {
        N = n;
        adj.assign(N + 1, {});
        isRestPoint.assign(N + 1, 0);
        isCharge.assign(N + 1, 0);
        idToName.assign(N + 1, "");
    }

    int getId(const string &name) {
        if (nameToId.count(name)) return nameToId[name];
        int id = (int)nameToId.size() + 1;
        nameToId[name] = id;
        if ((int)idToName.size() < id + 1) idToName.resize(id + 1);
        idToName[id] = name;
        return id;
    }

    void addEdge(const string &u, const string &v, int w, int o) {
        int a = getId(u), b = getId(v);
        if ((int)adj.size() <= max(a, b)) {
            adj.resize(max(a, b) + 1);
            isRestPoint.resize(max(a, b) + 1);
            isCharge.resize(max(a, b) + 1);
        }
        adj[a].push_back({b, w, o});
        adj[b].push_back({a, w, o});
    }
};

class RobotNavigator {
public:
    Graph &graph;
    int startHour;
    int idStart, idTarget;

    RobotNavigator(Graph &g, int hour) : graph(g), startHour(hour) {}

    bool shortestPath(const string &start, const string &target) {
        idStart = graph.getId(start);
        idTarget = graph.getId(target);

        using Item = tuple<int, int, State>; 
        struct Cmp {
            bool operator()(const Item &a, const Item &b) const {
                if (get<0>(a) != get<0>(b)) return get<0>(a) > get<0>(b);
                return get<1>(a) > get<1>(b);
            }
        };

        priority_queue<Item, vector<Item>, Cmp> pq;
        unordered_map<State, int, StateHash> best;
        unordered_map<State, ParentInfo, StateHash> parent;

        State startState{idStart, 0, startHour % 2, 0};
        best[startState] = 0;
        parent[startState] = ParentInfo{startState, -1, 0};
        pq.push({0, 0, startState});

        bool found = false;
        State goalState;

        while (!pq.empty()) {
            auto [totalEnergy, totalMinutes, st] = pq.top(); pq.pop();
            if (best.find(st) != best.end() && best[st] < totalEnergy) continue;

            if (st.node == idTarget) {
                found = true;
                goalState = st;
                break;
            }

            // recharge
            if (graph.isCharge[st.node] && st.consumed != 0) {
                State ns = st; ns.consumed = 0;
                if (!best.count(ns) || best[ns] > totalEnergy) {
                    best[ns] = totalEnergy;
                    parent[ns] = {st, st.node, totalMinutes};
                    pq.push({totalEnergy, totalMinutes, ns});
                }
            }

            if (graph.isRestPoint[st.node]) {
                int toNextHour = (60 - st.minuteMod60) % 60;
                if (toNextHour == 0) toNextHour = 60;
                int newMinutes = totalMinutes + toNextHour;
                int newMod60 = (st.minuteMod60 + toNextHour) % 60;
                int newParity = 1 - st.hourParity;
                State ns{st.node, newMod60, newParity, st.consumed};
                if (!best.count(ns) || best[ns] > totalEnergy) {
                    best[ns] = totalEnergy;
                    parent[ns] = {st, st.node, totalMinutes}; 
                    pq.push({totalEnergy, newMinutes, ns});
                }
            }

            for (auto &e : graph.adj[st.node]) {
                int baseLen = e.length + e.obstacle;
                int energyCost = baseLen;

                if (st.consumed + energyCost > MAX_ENERGY) continue;

                int timeCost = 2;
                int newMinutes = totalMinutes + timeCost;

                int absoluteMinutes = startHour * 60 + newMinutes;
                int newParity = (absoluteMinutes / 60) % 2;
                int newMod60 = newMinutes % 60;

                int newConsumed = st.consumed + energyCost;
                if (graph.isCharge[e.to]) newConsumed = 0;

                int newTotalEnergy = totalEnergy + energyCost;
                State ns{e.to, newMod60, newParity, newConsumed};
                if (!best.count(ns) || best[ns] > newTotalEnergy) {
                    best[ns] = newTotalEnergy;
                    parent[ns] = {st, st.node, newMinutes};
                    pq.push({newTotalEnergy, newMinutes, ns});
                }
            }
        }

        if (!found) {
            cout << "Robot gagal dalam mencapai tujuan :(\n";
            return false;
        }

        // reconstruct path
        vector<pair<int,int>> path;
        State cur = goalState;
        while (true) {
            ParentInfo pi = parent[cur];
            path.push_back({cur.node, pi.arrivalMinute});
            if (pi.prevNode == -1) break;
            cur = pi.prev;
        }
        reverse(path.begin(), path.end());

        cout << "Total minimum energy: " << best[goalState] << "\n";
        cout << "Path: ";
        for (size_t i = 0; i < path.size(); i++) {
            if (i) cout << " -> ";
            cout << graph.idToName[path[i].first];
        }
        cout << "\n";
        cout << "Arrival times:\n";
        for (auto &p : path) {
            cout << graph.idToName[p.first] << " (minute " << p.second << ")\n";
        }
        return true;
    }
};

int main() {
    cout << "N x M: ";
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int N, M;
    cin >> N >> M;
    cout << "======PLAY=====" << endl;

    Graph graph(N);
    for (int i = 0; i < M; i++) {
        string u, v;
        int w, o;
        cin >> u >> v >> w >> o;
        graph.addEdge(u, v, w, o);
    }

    string S, T;
    cin >> S >> T;

    auto readList = [&](vector<char> &marker) {
        string line;
        cin >> ws;
        getline(cin, line);
        stringstream ss(line);
        string s;
        while (ss >> s) {
            if (s == "-") { marker.assign(marker.size(), 0); return; }
            int id = graph.getId(s);
            if ((int)marker.size() <= id) marker.resize(id + 1);
            marker[id] = 1;
        }
    };

    readList(graph.isRestPoint);
    readList(graph.isCharge);
    vector<char> dummy;
    readList(dummy);
    readList(dummy);

    int startHour;
    cin >> startHour;

    RobotNavigator robot(graph, startHour);
    robot.shortestPath(S, T);

    return 0;
}
