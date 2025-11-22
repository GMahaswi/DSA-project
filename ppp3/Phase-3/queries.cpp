#include "queries.hpp"
#include "DeliveryScheduler.hpp"

json process_query(Graph& g,const json& query){

    if(query["type"]=="delivery_schedule")
        return processDeliverySchedule(g,query);

    json r;
    r["id"]=query.value("id",-1);
    r["error"]="unknown query type";
    return r;
}
