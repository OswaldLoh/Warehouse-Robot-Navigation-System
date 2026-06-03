#ifndef GRAPH_H
#define GRAPH_H

// Task 5 - Warehouse Graph Module
// TP086085 Oswald Loh Kar Tzun
// Defines the physical connections between warehouse locations as an
// undirected weighted graph and provides Dijkstra shortest-path search.

using namespace std;

#include <string>

const int GRAPH_NODES = 19;
const int BASE_NODE   = 0;
const int INF_COST    = 999999;

// Convert a warehouse location to its graph node ID
int locationToNode(char zone, int aisle, int shelf);

// Return a human-readable label for a node ID
// e.g.  nodeLabel(7)  -> "Zone B | Aisle 1 | Shelf 1"
//       nodeLabel(0)  -> "Base"
string nodeLabel(int nodeID);

// Dijkstra shortest path from sourceNode to targetNode.
//   Pass BASE_NODE as sourceNode for standard Base -> shelf routing.
//   path[] is filled with node IDs in order: source, ..., target.
//   Returns the number of nodes in the path (0 if unreachable).
int dijkstra(int sourceNode, int targetNode, int path[GRAPH_NODES]);

// Print the full adjacency list with edge costs (for the warehouse menu)
void printWarehouseGraph();

// Prompt the user for start/end locations and simulate Dijkstra navigation
void routeGenerationSimulation();

#endif
