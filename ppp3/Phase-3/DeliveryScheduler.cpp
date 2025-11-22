#include "DeliveryScheduler.hpp"

json processDeliverySchedule(Graph& g, const json& query){

    vector<Order> orders;
    for(auto&o:query["orders"])
        orders.push_back({o["order_id"],o["pickup"],o["dropoff"]});

    int depot=query["fleet"]["depot_node"];
    int k=query["fleet"]["num_delievery_guys"];

    vector<Driver> drivers;
    for(int i=0;i<k;i++) drivers.push_back({i,0,0});

    auto clusters=g.clusterOrders(orders,5);
    auto assignments=g.assignClustersToDrivers(clusters,drivers);

    json out;
    json arr=json::array();

    double total=0,maxTime=0;

    for(auto&a:assignments){
        json obj;
        obj["driver_id"]=a.driverId;

        vector<int> orderIds;
        vector<int> route;

        int cur=depot;
        route.push_back(cur);

        double t=0;

        for(auto&o:a.assignedOrders){
            orderIds.push_back(o.id);

            auto [_,d1,p1]=g.shortestPath(cur,o.pickup);
            route.insert(route.end(),p1.begin()+1,p1.end());
            t+=d1;

            auto [__,d2,p2]=g.shortestPath(o.pickup,o.drop);
            route.insert(route.end(),p2.begin()+1,p2.end());
            t+=d2;

            cur=o.drop;
        }

        total+=t;
        maxTime=max(maxTime,t);

        obj["route"]=route;
        obj["order_ids"]=orderIds;
        arr.push_back(obj);
    }

    out["assignments"]=arr;
    out["metrics"]={
        {"total_delivery_time_s",total},
        {"max_delivery_time_s",maxTime}
    };

    return out;
}
