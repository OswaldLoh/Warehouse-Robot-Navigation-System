// Task 5 - Warehouse Graph Module
// File: graph.cpp
//
// Implements the warehouse as an undirected weighted graph and provides
// Dijkstra's algorithm for computing optimal robot paths.
//
// The graph is built as a static edge list; no dynamic allocation is
// needed because the warehouse layout is fixed (3 zones x 2 aisles x 3 shelves).
// Dijkstra runs in O(V^2) which is negligible for 19 nodes.

#include <iostream>
#include <string>
#include "headerFiles/graph.h"
#include "headerFiles/robot.h"

using namespace std;

// ===================== Edge List =====================
// All edges are undirected (robot can travel in both directions).
// Edges are grouped by connection type for readability.

struct Edge {
    int from, to, cost;
};

static const Edge EDGES[] = {
    // ---- Base entry points ----
    // The robot enters Zone A from the main corridor at two points.
    {0,  1,  2},  // Base  <-> Zone A | Aisle 1 | Shelf 1   (main entrance)
    {0,  4,  3},  // Base  <-> Zone A | Aisle 2 | Shelf 1   (side entrance)

    // ---- Zone A internal connections ----
    {1,  2,  1},  // A|Aisle1|S1 <-> A|Aisle1|S2
    {2,  3,  1},  // A|Aisle1|S2 <-> A|Aisle1|S3
    {4,  5,  1},  // A|Aisle2|S1 <-> A|Aisle2|S2
    {5,  6,  1},  // A|Aisle2|S2 <-> A|Aisle2|S3
    {1,  4,  2},  // A|Aisle1|S1 <-> A|Aisle2|S1  (cross-aisle at shelf 1)
    {3,  6,  2},  // A|Aisle1|S3 <-> A|Aisle2|S3  (cross-aisle at shelf 3)

    // ---- Zone B internal connections ----
    {7,  8,  1},
    {8,  9,  1},
    {10, 11, 1},
    {11, 12, 1},
    {7,  10, 2},  // B|Aisle1|S1 <-> B|Aisle2|S1
    {9,  12, 2},  // B|Aisle1|S3 <-> B|Aisle2|S3

    // ---- Zone C internal connections ----
    {13, 14, 1},
    {14, 15, 1},
    {16, 17, 1},
    {17, 18, 1},
    {13, 16, 2},  // C|Aisle1|S1 <-> C|Aisle2|S1
    {15, 18, 2},  // C|Aisle1|S3 <-> C|Aisle2|S3

    // ---- Inter-zone corridor (Aisle 1 and Aisle 2 corridors) ----
    {1,  7,  2},  // ZoneA|A1|S1 <-> ZoneB|A1|S1
    {4,  10, 2},  // ZoneA|A2|S1 <-> ZoneB|A2|S1
    {7,  13, 2},  // ZoneB|A1|S1 <-> ZoneC|A1|S1
    {10, 16, 2},  // ZoneB|A2|S1 <-> ZoneC|A2|S1
};

static const int EDGE_COUNT = (int)(sizeof(EDGES) / sizeof(EDGES[0]));

// ===================== Helper Functions =====================

int locationToNode(char zone, int aisle, int shelf) {
    return 1 + ((zone - 'A') * 6) + ((aisle - 1) * 3) + (shelf - 1);
}

string nodeLabel(int nodeID) {
    if (nodeID == 0) return "Base";
    int  idx   = nodeID - 1;
    char zone  = 'A' + (idx / 6);
    int  aisle = 1   + (idx % 6) / 3;
    int  shelf = 1   + (idx % 3);
    string s = "Zone ";
    s += zone;
    s += " | Aisle " + to_string(aisle) + " | Shelf " + to_string(shelf);
    return s;
}

// ===================== Dijkstra =====================
// Standard O(V^2) Dijkstra using arrays (fine for V = 19).
// Fills path[] with the sequence of node IDs from sourceNode to targetNode.
// Pass BASE_NODE as sourceNode for the standard Base -> shelf route.
// Returns the number of nodes in the path, or 0 if unreachable.
int dijkstra(int sourceNode, int targetNode, int path[GRAPH_NODES]) {
    int  dist[GRAPH_NODES];
    int  prev[GRAPH_NODES];
    bool visited[GRAPH_NODES];

    for (int i = 0; i < GRAPH_NODES; i++) {
        dist[i]    = INF_COST;
        prev[i]    = -1;
        visited[i] = false;
    }
    dist[sourceNode] = 0;

    for (int iter = 0; iter < GRAPH_NODES; iter++) {
        // Select unvisited node with smallest tentative distance
        int u = -1;
        for (int i = 0; i < GRAPH_NODES; i++) {
            if (!visited[i] && (u == -1 || dist[i] < dist[u])) u = i;
        }
        if (u == -1 || dist[u] == INF_COST) break;
        visited[u] = true;

        // Relax all edges that touch u (undirected graph)
        for (int e = 0; e < EDGE_COUNT; e++) {
            int a = EDGES[e].from, b = EDGES[e].to, w = EDGES[e].cost;
            int v = (a == u) ? b : (b == u) ? a : -1;
            if (v == -1 || visited[v]) continue;
            if (dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                prev[v] = u;
            }
        }
    }

    if (dist[targetNode] == INF_COST) return 0;  // unreachable

    // Reconstruct path by following prev[] pointers backwards
    int tmp[GRAPH_NODES];
    int len = 0;
    for (int cur = targetNode; cur != -1; cur = prev[cur]) tmp[len++] = cur;

    // Reverse so path goes sourceNode -> ... -> targetNode
    for (int i = 0; i < len; i++) path[i] = tmp[len - 1 - i];
    return len;
}

// ===================== Print Graph =====================
void printWarehouseGraph() {
    cout << "\n====================================================\n";
    cout << " WAREHOUSE CONNECTION GRAPH\n";
    cout << " Nodes: " << GRAPH_NODES << "  |  Edges: " << EDGE_COUNT << "\n";
    cout << "====================================================\n";

    for (int u = 0; u < GRAPH_NODES; u++) {
        cout << "\n [" << u << "] " << nodeLabel(u) << "\n";
        for (int e = 0; e < EDGE_COUNT; e++) {
            int a = EDGES[e].from, b = EDGES[e].to, w = EDGES[e].cost;
            int v = (a == u) ? b : (b == u) ? a : -1;
            if (v == -1) continue;
            cout << "      -- (cost " << w << ") --> [" << v << "] "
                 << nodeLabel(v) << "\n";
        }
    }
    cout << "\n====================================================\n";
}

// ===================== Route Generation Simulation =====================
// Prompts the user for a start and end warehouse location,
// runs Dijkstra between them, creates a temporary simulation robot,
// pushes steps onto its navigation stack, prints the full nav log,
// then executes the reverse path back to start.
void routeGenerationSimulation() {
    cout << "\n====================================================\n";
    cout << "       ROUTE GENERATION SIMULATION\n";
    cout << "====================================================\n";
    cout << " Valid Zones  : A, B, C\n";
    cout << " Valid Aisles : 1, 2\n";
    cout << " Valid Shelves: 1, 2, 3\n";
    cout << "====================================================\n";

    // --- Starting location ---
    cout << "\n[Starting Location]\n";

    char startZone;
    int  startAisle, startShelf;

    // Prompt for zone
    cout << "  Zone   (A/B/C): ";
    string inp;
    getline(cin, inp);
    if (inp.empty()) { cout << "Invalid zone.\n"; return; }
    startZone = (char)toupper((unsigned char)inp[0]);
    if (startZone < 'A' || startZone > 'C') { cout << "Invalid zone. Must be A, B, or C.\n"; return; }

    // Prompt for aisle
    cout << "  Aisle  (1/2)  : ";
    getline(cin, inp);
    if (inp.empty() || (inp[0] != '1' && inp[0] != '2')) { cout << "Invalid aisle. Must be 1 or 2.\n"; return; }
    startAisle = inp[0] - '0';

    // Prompt for shelf
    cout << "  Shelf  (1/2/3): ";
    getline(cin, inp);
    if (inp.empty() || inp[0] < '1' || inp[0] > '3') { cout << "Invalid shelf. Must be 1, 2, or 3.\n"; return; }
    startShelf = inp[0] - '0';

    // --- Ending location ---
    cout << "\n[Ending Location]\n";

    char endZone;
    int  endAisle, endShelf;

    // Prompt for zone
    cout << "  Zone   (A/B/C): ";
    getline(cin, inp);
    if (inp.empty()) { cout << "Invalid zone.\n"; return; }
    endZone = (char)toupper((unsigned char)inp[0]);
    if (endZone < 'A' || endZone > 'C') { cout << "Invalid zone. Must be A, B, or C.\n"; return; }

    // Prompt for aisle
    cout << "  Aisle  (1/2)  : ";
    getline(cin, inp);
    if (inp.empty() || (inp[0] != '1' && inp[0] != '2')) { cout << "Invalid aisle. Must be 1 or 2.\n"; return; }
    endAisle = inp[0] - '0';

    // Prompt for shelf
    cout << "  Shelf  (1/2/3): ";
    getline(cin, inp);
    if (inp.empty() || inp[0] < '1' || inp[0] > '3') { cout << "Invalid shelf. Must be 1, 2, or 3.\n"; return; }
    endShelf = inp[0] - '0';

    // --- Convert locations to graph node IDs ---
    int startNode = locationToNode(startZone, startAisle, startShelf);
    int endNode   = locationToNode(endZone,   endAisle,   endShelf);

    cout << "\n  From : [" << startNode << "] " << nodeLabel(startNode) << "\n";
    cout << "  To   : [" << endNode   << "] " << nodeLabel(endNode)   << "\n";

    if (startNode == endNode) {
        cout << "\n  Start and end are the same location. No route needed.\n";
        cout << "====================================================\n";
        return;
    }

    // --- Run Dijkstra from start -> end ---
    // Because our Dijkstra always starts from BASE_NODE (0), we use a
    // two-pass approach: base->start and base->end, then derive the
    // intermediate sub-path by walking from endNode back through prev[]
    // until we reach startNode (or Base if start is not on the path).
    // For a cleaner simulation we run Dijkstra from startNode directly
    // by temporarily treating it as the source via a modified dist init.

    int dist[GRAPH_NODES];
    int prev[GRAPH_NODES];
    bool visited[GRAPH_NODES];

    for (int i = 0; i < GRAPH_NODES; i++) {
        dist[i]    = INF_COST;
        prev[i]    = -1;
        visited[i] = false;
    }
    dist[startNode] = 0;   // Source is the user's start location

    for (int iter = 0; iter < GRAPH_NODES; iter++) {
        int u = -1;
        for (int i = 0; i < GRAPH_NODES; i++) {
            if (!visited[i] && (u == -1 || dist[i] < dist[u])) u = i;
        }
        if (u == -1 || dist[u] == INF_COST) break;
        visited[u] = true;

        for (int e = 0; e < EDGE_COUNT; e++) {
            int a = EDGES[e].from, b = EDGES[e].to, w = EDGES[e].cost;
            int v = (a == u) ? b : (b == u) ? a : -1;
            if (v == -1 || visited[v]) continue;
            if (dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                prev[v] = u;
            }
        }
    }

    if (dist[endNode] == INF_COST) {
        cout << "\n  [!] No path found between the two locations.\n";
        cout << "====================================================\n";
        return;
    }

    // Reconstruct path: endNode -> ... -> startNode (then reverse)
    int tmp[GRAPH_NODES];
    int len = 0;
    for (int cur = endNode; cur != -1; cur = prev[cur]) {
        tmp[len++] = cur;
        if (cur == startNode) break;
    }

    int path[GRAPH_NODES];
    for (int i = 0; i < len; i++) path[i] = tmp[len - 1 - i];

    cout << "\n  Shortest path: " << len - 1 << " hop(s)  |  Total cost: " << dist[endNode] << "\n";

    // --- Create a temporary simulation robot ---
    Robot* simRobot = new Robot(99);   // ID 99 = simulation robot (not in main pool)
    simRobot->setStatus("Busy");

    // --- Push one step per hop ---
    cout << "\n[Forward Navigation Steps]\n";
    for (int i = 1; i < len; i++) {
        string fwd = "Move to "      + nodeLabel(path[i]);
        string bwd = "Return from " + nodeLabel(path[i]);
        cout << "  Step " << i << ": " << fwd << "\n";
        simRobot->pushStep(fwd, bwd);
    }

    // Save nav record before goBack() clears the stack
    string routeLabel = "Route Sim (" + nodeLabel(startNode) + " -> " + nodeLabel(endNode) + ")";

    // --- Execute return journey ---
    cout << "\n[Reverse Navigation]\n";
    simRobot->goBack();

    // --- Clean up ---
    delete simRobot;

    cout << "====================================================\n";
}
