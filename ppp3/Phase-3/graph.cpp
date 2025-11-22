#include "Graph.hpp"
#include <queue>
#include <fstream>
#include <limits>

void Graph::loadGraph(const string& filename){
    ifstream f(filename);
    json j; f>>j;

    for(auto& n : j["nodes"])
        coords[n["id"]] = {n["lat"], n["lon"]};

    for(auto& e : j["edges"])
        addEdge(e["u"],e["v"],e["length"],e["avg_time"],e["oneway"]);
}

void Graph::addEdge(int u,int v,double length,double avgTime,bool oneway){
    Edge e = {int(edge_map.size()), u, v, length, avgTime, oneway, ""};
    adj[u][v] = e;
    edge_map[e.id] = e;
    if(!oneway) adj[v][u] = e;
}

double Graph::euclideanDistance(int a, int b) const{
    auto [x1,y1]=coords.at(a);
    auto [x2,y2]=coords.at(b);
    return sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
}

tuple<bool,double,vector<int>> Graph::shortestPath(int source,int target,const string& mode) const{
    unordered_map<int,double> dist;
    unordered_map<int,int> parent;

    for(auto&p:coords) dist[p.first]=1e18;
    dist[source]=0;

    using P=pair<double,int>;
    priority_queue<P,vector<P>,greater<P>> pq;
    pq.push({0,source});

    while(!pq.empty()){
        auto [d,u]=pq.top(); pq.pop();
        if(u==target) break;
        if(d>dist[u]) continue;

        for(auto &[v,e]:adj.at(u)){
            double cost= (mode=="average")?e.average_time:e.length;
            if(dist[u]+cost<dist[v]){
                dist[v]=dist[u]+cost;
                parent[v]=u;
                pq.push({dist[v],v});
            }
        }
    }

    vector<int>path;
    if(dist[target]>=1e18) return {false,dist[target],path};

    for(int cur=target;cur!=source;cur=parent[cur]) path.push_back(cur);
    path.push_back(source);
    reverse(path.begin(),path.end());
    return {true,dist[target],path};
}

double Graph::getDist(int a, int b){
    if(distCache.count(a) && distCache[a].count(b))
        return distCache[a][b];

    auto [ok, d, p] = shortestPath(a,b,"average");
    distCache[a][b]=d;
    return d;
}

vector<Cluster> Graph::clusterOrders(vector<Order>& orders,int maxClusterSize){
    vector<Cluster> clusters;
    vector<bool> used(orders.size(),false);
    int cid=0;

    for(int i=0;i<orders.size();i++){
        if(used[i]) continue;

        Cluster c; c.id=cid++;
        c.orders.push_back(orders[i]);
        used[i]=true;

        while(c.orders.size()<maxClusterSize){
            double best=1e18;
            int idx=-1;

            for(int j=0;j<orders.size();j++){
                if(used[j]) continue;

                for(auto&o:c.orders){
                    double d = euclideanDistance(o.pickup, orders[j].pickup) +
                               euclideanDistance(o.drop, orders[j].drop);
                    if(d<best){ best=d; idx=j; }
                }
            }

            if(idx==-1) break;
            used[idx]=true;
            c.orders.push_back(orders[idx]);
        }
        clusters.push_back(c);
    }
    return clusters;
}

double Graph::getClusterDistance(const Cluster& c, int depot){
    double total=0;
    int cur=depot;

    for(auto&o:c.orders){
        total+=getDist(cur,o.pickup);
        total+=getDist(o.pickup,o.drop);
        cur=o.drop;
    }
    return total;
}

double Graph::computeClusterScore(const Cluster& c){
    return getClusterDistance(c);
}

vector<Assignment> Graph::assignClustersToDrivers(vector<Cluster>& clusters, vector<Driver>& drivers){
    vector<Assignment> res(drivers.size());

    for(int i=0;i<drivers.size();i++)
        res[i].driverId=drivers[i].id;

    int di=0;
    for(auto&c:clusters){
        res[di].assignedOrders.insert(res[di].assignedOrders.end(),
                                      c.orders.begin(),c.orders.end());
        di=(di+1)%drivers.size();
    }
    return res;
}
