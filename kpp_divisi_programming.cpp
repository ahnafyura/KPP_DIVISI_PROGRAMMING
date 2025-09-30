#include <bits/stdc++.h>
using namespace std;

const int energiMaks = 1000;

class Edge {
public:
    int tujuan;
    int panjang;
    int halangan;
    Edge(int t, int l, int o) : tujuan(t), panjang(l), halangan(o) {}
};

class Graph {
public:
    int N;
    unordered_map<string,int> NamakeID;
    vector<string> IDkenama;
    vector<vector<Edge>> adj;
    vector<char> isRest, isCharge;

    Graph(int n=0){ init(n); }

    void init(int n){
        N=n;
        adj.assign(N+1,{});
        isRest.assign(N+1,0);
        isCharge.assign(N+1,0);
        IDkenama.assign(N+1,"");
    }

    int getId(const string &s){
        if(NamakeID.count(s)) return NamakeID[s];
        int id=(int)NamakeID.size()+1;
        NamakeID[s]=id;
        if((int)IDkenama.size()<=id) IDkenama.resize(id+1);
        IDkenama[id]=s;
        return id;
    }

    void addEdge(const string &u,const string &v,int w,int o){
        int a=getId(u), b=getId(v);
        if((int)adj.size()<=max(a,b)){
            adj.resize(max(a,b)+1);
            isRest.resize(max(a,b)+1);
            isCharge.resize(max(a,b)+1);
        }
        adj[a].push_back(Edge(b,w,o));
        adj[b].push_back(Edge(a,w,o));
    }
};

class RobotNavigator {
public:
    Graph &graph;
    int startHour;
    int idStart, idTarget;

    RobotNavigator(Graph &g,int h):graph(g),startHour(h){}

    bool shortestPath(const string &S,const string &T){
        idStart=graph.getId(S);
        idTarget=graph.getId(T);

        using P=pair<int,int>;
        vector<int> dist(graph.N+5,INT_MAX);
        vector<int> energy(graph.N+5,energiMaks);
        vector<int> parent(graph.N+5,-1);
        vector<int> timeArr(graph.N+5,-1);

        priority_queue<P,vector<P>,greater<P>> pq;
        dist[idStart]=0;
        timeArr[idStart]=0;
        pq.push({0,idStart});

        while(!pq.empty()){
            auto [d,u]=pq.top(); pq.pop();
            if(d>dist[u]) continue;

            if(u==idTarget){
                printResult(dist,parent,timeArr);
                return true;
            }

            if(graph.isRest[u]){
                int newTime=timeArr[u]+60;
                int newCost=dist[u];
                if(newCost<dist[u]){}
            }

            for(auto &e:graph.adj[u]){
                int base=e.panjang+e.halangan;
                int nowHour=(startHour*60+timeArr[u])/60;
                double factor=(nowHour%2==0)?0.8:1.3;
                int cost=(int)ceil(base*factor);

                if(cost>energiMaks) continue;
                if(cost>energy[u]) continue;

                int newDist=dist[u]+cost;
                int v=e.tujuan;
                int newTime=timeArr[u]+2;

                if(graph.isCharge[v]){
                    energy[v]=energiMaks;
                } else {
                    energy[v]=energy[u]-cost;
                }

                if(newDist<dist[v]){
                    dist[v]=newDist;
                    parent[v]=u;
                    timeArr[v]=newTime;
                    pq.push({dist[v],v});
                }
            }
        }
        cout<<"Robot gagal dalam mencapai tujuan :(\n";
        return false;
    }

    void printResult(vector<int>&dist,vector<int>&parent,vector<int>&timeArr){
        cout<<"Total energi minimum: "<<dist[idTarget]<<"\n";
        vector<int> path;
        int cur=idTarget;
        while(cur!=-1){ path.push_back(cur); cur=parent[cur]; }
        reverse(path.begin(),path.end());

        cout<<"Jalur: ";
        for(size_t i=0;i<path.size();i++){
            if(i) cout<<" -> ";
            cout<<graph.IDkenama[path[i]];
        }
        cout<<"\nWaktu tiba:\n";
        for(size_t i=0;i<path.size();i++){
            cout<<graph.IDkenama[path[i]]<<" (menit "<<timeArr[path[i]]<<")\n";
        }
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int N, M;
    cin >> N >> M;
    cout << "======PLAY=====" << endl;

    Graph g(N);
    for (int i = 0; i < M; i++) {
        string u, v;
        int w, o;
        cin >> u >> v >> w >> o;
        g.addEdge(u, v, w, o);
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
            if (s == "-") {
                marker.assign(marker.size(), 0);
                return;
            }
            int id = g.getId(s);
            if ((int)marker.size() <= id) marker.resize(id + 1);
            marker[id] = 1;
        }
    };

    readList(g.isRest);
    readList(g.isCharge);
    vector<char> dummy;
    readList(dummy);
    readList(dummy);

    int startHour;
    cin >> startHour;

    RobotNavigator robot(g, startHour);
    robot.shortestPath(S, T);

    return 0;
}

