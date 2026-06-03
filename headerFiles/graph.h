#ifndef GRAPH_H
#define GRAPH_H

// Task 5 - Warehouse Graph Module
// Defines the physical connections between warehouse locations as an
// undirected weighted graph and provides Dijkstra shortest-path search.
//
// Node scheme (19 nodes total):
//   Node 0          = Base (robot start/end point)
//   Nodes 1 - 18   = warehouse shelf locations
//
//   Formula: nodeID = 1 + (zoneIndex * 6) + (aisleIndex * 3) + shelfIndex
//     zoneIndex  = zone  - 'A'   (0 = A, 1 = B, 2 = C)
//     aisleIndex = aisle - 1     (0 = Aisle 1, 1 = Aisle 2)
//     shelfIndex = shelf - 1     (0 = Shelf 1, 1 = Shelf 2, 2 = Shelf 3)
//
// Edge costs:
//   Adjacent shelves within the same aisle : 1
//   Cross-aisle connection (within a zone) : 2
//   Inter-zone corridor connection         : 2
//   Base -> zone entry points              : 2 / 3

#include <string>
using namespace std;

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
