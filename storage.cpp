// Task 5 - Warehouse Layout Module
//
// Models the warehouse as a 3-level hierarchical linked structure:
//   Zone -> Aisle -> Shelf -> Items (linked list)
// Supports item storage, display, lookup, and deletion.
#include <iostream>
#include <string>
#include "headerFiles/warehouse.h"

using namespace std;

// Global warehouse head pointer
Zone* warehouseHead = nullptr;

// ==================== Warehouse Initialization ====================

void initWarehouse() {
    // Build structure bottom-up: Shelves -> Aisles -> Zones
    // 3 Zones (A, B, C) x 2 Aisles (1, 2) x 3 Shelves (1, 2, 3)

    Zone* prevZone = nullptr;

    for (int z = 0; z < 3; z++) {
        char zoneID = 'A' + z;
        Aisle* firstAisle = nullptr;
        Aisle* prevAisle = nullptr;

        for (int a = 1; a <= 2; a++) {
            Shelf* firstShelf = nullptr;
            Shelf* prevShelf = nullptr;

            for (int s = 1; s <= 3; s++) {
                Shelf* newShelf = new Shelf(s);
                if (firstShelf == nullptr) {
                    firstShelf = newShelf;
                } else {
                    prevShelf->nextShelf = newShelf;
                }
                prevShelf = newShelf;
            }

            Aisle* newAisle = new Aisle(a, firstShelf);
            if (firstAisle == nullptr) {
                firstAisle = newAisle;
            } else {
                prevAisle->nextAisle = newAisle;
            }
            prevAisle = newAisle;
        }

        Zone* newZone = new Zone(zoneID, firstAisle);
        if (warehouseHead == nullptr) {
            warehouseHead = newZone;
        } else {
            prevZone->nextZone = newZone;
        }
        prevZone = newZone;
    }

    cout << "Warehouse initialized: 3 Zones x 2 Aisles x 3 Shelves" << endl;
}

// ==================== Store Item ====================

void storeItem(Zone* head, Item* item) {
    // Iterate through zones
    Zone* currentZone = head;
    while (currentZone != nullptr) {
        if (currentZone->ID == item->zoneID) {
            // Found matching zone, iterate through aisles
            Aisle* currentAisle = currentZone->headAisle;
            while (currentAisle != nullptr) {
                if (currentAisle->ID == item->aisleID) {
                    // Found matching aisle, iterate through shelves
                    Shelf* currentShelf = currentAisle->headShelf;
                    while (currentShelf != nullptr) {
                        if (currentShelf->ID == item->shelfID) {
                            // Found matching shelf, append item to linked list
                            item->next = nullptr;

                            if (currentShelf->headItem == nullptr) {
                                // Shelf is empty, item becomes head
                                currentShelf->headItem = item;
                            } else {
                                // Traverse to end of linked list and append
                                Item* temp = currentShelf->headItem;
                                while (temp->next != nullptr) {
                                    temp = temp->next;
                                }
                                temp->next = item;
                            }
                            // Also insert into BST for fast lookup
                            bstRoot = insertBST(bstRoot, item);
                            return;
                        }
                        currentShelf = currentShelf->nextShelf;
                    }
                    cout << "Warning: Shelf " << item->shelfID
                         << " not found in Zone " << item->zoneID
                         << " Aisle " << item->aisleID << endl;
                    return;
                }
                currentAisle = currentAisle->nextAisle;
            }
            cout << "Warning: Aisle " << item->aisleID
                 << " not found in Zone " << item->zoneID << endl;
            return;
        }
        currentZone = currentZone->nextZone;
    }
    cout << "Warning: Zone " << item->zoneID << " not found!" << endl;
}

// ==================== Display Warehouse ====================

void displayWarehouse(Zone* head) {
    cout << "\n=============== WAREHOUSE INVENTORY ===============\n";

    Zone* currentZone = head;
    while (currentZone != nullptr) {
        cout << "\n[Zone " << currentZone->ID << "]" << endl;

        Aisle* currentAisle = currentZone->headAisle;
        while (currentAisle != nullptr) {
            cout << "  Aisle " << currentAisle->ID << ":" << endl;

            Shelf* currentShelf = currentAisle->headShelf;
            while (currentShelf != nullptr) {
                cout << "    Shelf " << currentShelf->ID << ": ";

                Item* currentItem = currentShelf->headItem;
                if (currentItem == nullptr) {
                    cout << "(empty)";
                } else {
                    bool first = true;
                    while (currentItem != nullptr) {
                        if (!first) cout << ", ";
                        cout << "[" << currentItem->ID << "] " << currentItem->name;
                        first = false;
                        currentItem = currentItem->next;
                    }
                }
                cout << endl;

                currentShelf = currentShelf->nextShelf;
            }

            currentAisle = currentAisle->nextAisle;
        }

        currentZone = currentZone->nextZone;
    }

    cout << "\n===================================================\n";
}

// ==================== Accessor ====================

Zone* getWarehouseHead() {
    return warehouseHead;
}

// ==================== Item Search (BST) ====================

Item* findItemByID(Zone* head, int itemID) {
    // 'head' parameter kept for API consistency — BST gives O(log n) lookup
    (void)head;     // Suppress unused-parameter warning
    TreeNode* result = searchBST(bstRoot, itemID);
    if (result != nullptr) {
        return result->item;
    }
    return nullptr;
}

// ==================== Item Delete ====================

bool deleteItem(Zone* head, int itemID) {
    Zone* z = head;
    while (z != nullptr) {
        Aisle* a = z->headAisle;
        while (a != nullptr) {
            Shelf* s = a->headShelf;
            while (s != nullptr) {
                Item* prev = nullptr;
                Item* item = s->headItem;
                while (item != nullptr) {
                    if (item->ID == itemID) {
                        // Unlink from shelf's linked list
                        if (prev == nullptr) {
                            s->headItem = item->next;
                        } else {
                            prev->next = item->next;
                        }
                        // Also remove from BST
                        bstRoot = deleteBST(bstRoot, itemID);
                        delete item;
                        return true;
                    }
                    prev = item;
                    item = item->next;
                }
                s = s->nextShelf;
            }
            a = a->nextAisle;
        }
        z = z->nextZone;
    }
    return false;
}
