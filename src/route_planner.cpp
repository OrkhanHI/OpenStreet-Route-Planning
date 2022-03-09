#include "route_planner.h"
#include <algorithm>

RoutePlanner::RoutePlanner(RouteModel &model, float start_x, float start_y, float end_x, float end_y): m_Model(model) {
    // Convert inputs to percentage:
    start_x *= 0.01;
    start_y *= 0.01;
    end_x *= 0.01;
    end_y *= 0.01;

    // Find the closest nodes to the starting and ending coordinates
    this->start_node = &m_Model.FindClosestNode(start_x, start_y);
    this->end_node = &m_Model.FindClosestNode(end_x, end_y);
}


float RoutePlanner::CalculateHValue(RouteModel::Node const *node) {
    return node->distance(*end_node);
}

void RoutePlanner::AddNeighbors(RouteModel::Node *current_node) {
    current_node->FindNeighbors();
    for(auto &node : current_node->neighbors){
        node->parent = current_node;
        node->g_value = current_node->g_value + current_node->distance(*node); 
        node->h_value = CalculateHValue(node);
        node->visited = true;
        open_list.push_back(node);
    }
}

RouteModel::Node *RoutePlanner::NextNode() {
    sort(open_list.begin(), open_list.end(), [](const RouteModel::Node *a, const RouteModel::Node *b){return a->g_value + a->h_value > b->g_value + b->h_value;});
    auto lowestSumNode = open_list.back();
    open_list.pop_back();
    return lowestSumNode;
}

std::vector<RouteModel::Node> RoutePlanner::ConstructFinalPath(RouteModel::Node *current_node) {
    // Create path_found vector
    distance = 0.0f;
    std::vector<RouteModel::Node> path_found;

    // TODO: Implement your solution here.
    while(true){
        //add distance from current node to its parent node
        distance += current_node->parent->distance(*current_node);
        //attach the current node to the path_found
        path_found.emplace_back(*current_node);
        //set parent node as current
        current_node = current_node->parent;
        //break the loop once current node is the starting node
        if(current_node == start_node){
            //add the start point before break
            path_found.emplace_back(*start_node);
            break;
        }
    }
    //reverse the order of the vector
    std::reverse(path_found.begin(), path_found.end());

    distance *= m_Model.MetricScale(); // Multiply the distance by the scale of the map to get meters.
    return path_found;

}

void RoutePlanner::AStarSearch() {
    RouteModel::Node *current_node = nullptr;

    //Declare finalPath as a vector of nodes
    std::vector<RouteModel::Node> finalPath; 
    //Set the current node to the start node
    current_node = this->start_node;
    open_list.emplace_back(current_node);
    //Make it visited
    current_node->visited = true;

    while(!open_list.empty()){
        current_node = NextNode();
        if(current_node == this->end_node){
            finalPath = ConstructFinalPath(current_node);
            break;
        }
        AddNeighbors(current_node);
    }
    m_Model.path = finalPath;
}