#ifndef WAREHOUSE_H
#define WAREHOUSE_H

// Shared header used by Tasks 3, 4, and 5.
// Declares all warehouse structs (Item, Shelf, Aisle, Zone, TreeNode)
// and function prototypes for storage, BST, file handling, and item management.

// Note: std:: prefix used throughout - 'using namespace std' is omitted
// from header files to avoid polluting the global namespace.

#include <iostream>
#include <string>

// ==================== Warehouse Structs ====================

struct Item {
    int ID;
    std::string name;

    char zoneID;
    int aisleID;
    int shelfID;

    Item* left;
    Item* right;
    Item* next;         // For shelf linked list

    Item(int id, std::string itemName, char zID, int aID, int sID) {
        ID = id;
        name = itemName;
        zoneID = zID;
        aisleID = aID;
        shelfID = sID;
        left = nullptr;
        right = nullptr;
        next = nullptr;
    }
};

struct Shelf {      // Level 1 
    int ID;
    Shelf* nextShelf;
    Item* headItem;     // Head of item linked list on this shelf

    Shelf(int sID) {
        ID = sID;
        nextShelf = nullptr;
        headItem = nullptr;
    }
};

struct Aisle {      // Level 2
    int ID;
    Aisle* nextAisle;
    Shelf* headShelf;
    
    Aisle(int aID, Shelf* head) {
        ID = aID;
        headShelf = head;
        nextAisle = nullptr;
    }
};

struct Zone {       // Level 3
    char ID;
    Zone* nextZone;
    Aisle* headAisle;

    Zone(char zID, Aisle* head) {
        ID = zID;
        nextZone = nullptr;
        headAisle = head;
    }
};

struct TreeNode {
    Item* item;
    TreeNode* left;
    TreeNode* right;

    TreeNode(Item* newItem) {
        item = newItem;
        left = nullptr;
        right = nullptr;
    }
};

// ==================== BST (defined in tree.cpp) ====================

// Global BST root
extern TreeNode* bstRoot;

// BST operations
TreeNode* createNode(Item* item);
TreeNode* insertBST(TreeNode* root, Item* item);
TreeNode* searchBST(TreeNode* root, int itemID);
TreeNode* deleteBST(TreeNode* root, int itemID);
TreeNode* findMinNode(TreeNode* root);
TreeNode* findMaxNode(TreeNode* root);
void inOrderTraversal(TreeNode* root);
void displayItemsSorted();

// ==================== File Handling ====================

// Parse items from a CSV file
// Returns a dynamically allocated array of Item pointers, sets 'count' to the number of items loaded
Item** loadItemsFromCSV(const std::string& filepath, int& count);

// ==================== Storage ====================

// Initialize the warehouse structure (Zones -> Aisles -> Shelves)
void initWarehouse();

// Store a single item into the correct shelf in the warehouse
void storeItem(Zone* warehouseHead, Item* item);

// Display the full warehouse inventory (Zone -> Aisle -> Shelf -> Items)
void displayWarehouse(Zone* warehouseHead);

// Get the global warehouse head pointer
Zone* getWarehouseHead();

// Find an item by its ID across all zones/aisles/shelves
Item* findItemByID(Zone* head, int itemID);

// Delete an item by its ID, returns true if found and deleted
bool deleteItem(Zone* head, int itemID);

// Unlink an item from its shelf without deleting it or touching the BST
// Used when relocating an item to a different shelf
bool removeItemFromShelf(Zone* head, int itemID);

// Item CRUD functions (defined in itemManagement.cpp)
void searchItem();
void insertItem();
void updateItem();
void deleteItemMenu();
void displayAllItems();     // Bubble-sorted table: by ID or by Name

#endif
