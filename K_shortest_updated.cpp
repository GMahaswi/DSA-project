#include <bits/stdc++.h>
#include <vector>
#include <unordered_map>
#include <iostream>
#include "graph.hpp"
using namespace std;

struct Edge {
    int v;
    int to;
    int w;
};

unordered_map<int,vector<Edge>>graph;
unordered_map<int,unordered_map<int,double>>edgeweight;
    for(auto &[v,adj]:graph){
            for(auto &e:adj){
        edgeweight[v][e.v]=e.w;
        }
        }
vector<int> dijkstraPath(int src, int dest, double &cost) {
    unordered_map<int, double> dist;
    unordered_map<int, int> parent;
    for (auto &p : graph) dist[p.first] = 1e18;
    using P = pair<double,int>;
    priority_queue<P, vector<P>, greater<P>> pq;
    dist[src] = 0;
    pq.push({0, src});
    
    while (!pq.empty()) {
        auto [d, u] = pq.top(); pq.pop();
        if (u == dest) break;
        if (d > dist[u]) continue;
        for (auto &e : graph[u]) {
            if (dist[e.v] > d + e.w) {
                dist[e.v] = d + e.w;
                parent[e.v] = u;
                pq.push({dist[e.v], e.v});
            }
        }
    }

    vector<int> path;
    if (dist[dest] == 1e18) return path;
    for (int v = dest; v != src; v = parent[v]) path.push_back(v);
    path.push_back(src);
    reverse(path.begin(), path.end());
    cost = dist[dest];
    return path;
}

vector<pair<vector<int>, double>> yenKShortest(int src,int dest,int k){
    vector<pair<vector<int>,double>>kpaths;
    vector<pair<vector<int>,double>>B;
    /*paths that can be part*/
    double first;
    vector<int>firstpath=dijkstra(src,dest,first);
    if(firstpath.empty()){
        return kpaths;
    }
    kpaths.push_back({firstpath,first});
    for(int i=1;i<k;i++){
        vector<int>lastpath=kpaths[i-1].first;
        int size=lastpath.size();
        for(int j=0;j<size;j++){
            int u=lastpath[j];
            vector<int>prefixpath(lastpath.begin(),lastpath.begin()+j+1);
            //weshouldn't consider the edges that share same 
            vector<pair<int,int>>removed;
            for(auto &p:kpaths){
                auto &path=p.first;
                if(path.size()>j && equal(prefixpath.begin(),prefixpath.end(),path.begin())){
                    int u=path[j];
                    int v=path[j+1];
                    auto &adj = graph[u];
                    auto it = find_if(adj.begin(), adj.end(), [&](Edge &e){return e.v == v;});
                    if(it!=adj.end()){
                        removed.push_back({u,v});
                        adj.erase(it);
                    }
                }
            }
            double suffixcost;
            
            vector<int>suppath=dijkstra(u,dest,suffixcost);

        }
    }


}

