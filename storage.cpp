// Task 4 — Item Search & Management Module
// TP074412 Foo Kim Chean

// Models the warehouse as a 3-level hierarchical linked structure:
//   Zone -> Aisle -> Shelf -> Items (linked list)
// Supports item storage, display, lookup, and deletion.
#include <iostream>
#include <string>
#include "warehouse.h"

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

// ==================== Display Warehouse (Visual Tree) ====================
// Prints a textual tree of the warehouse hierarchy using box-drawing
// characters so zones, aisles, shelves and items are visually connected.

void displayWarehouse(Zone* head) {
    cout << "\n====================================================\n";
    cout << "           WAREHOUSE LAYOUT (VISUAL TREE)\n";
    cout << "====================================================\n";
    cout << "WAREHOUSE\n";

    Zone* currentZone = head;
    while (currentZone != nullptr) {
        bool lastZone = (currentZone->nextZone == nullptr);
        cout << (lastZone ? "\xc0\xc4\xc4 " : "\xc3\xc4\xc4 ")   // +-- or L--
             << "Zone " << currentZone->ID << "\n";

        string zonePrefix = lastZone ? "    " : "\xb3   ";         // "    " or "|   "

        Aisle* currentAisle = currentZone->headAisle;
        while (currentAisle != nullptr) {
            bool lastAisle = (currentAisle->nextAisle == nullptr);
            cout << zonePrefix
                 << (lastAisle ? "\xc0\xc4\xc4 " : "\xc3\xc4\xc4 ")
                 << "Aisle " << currentAisle->ID << "\n";

            string aislePrefix = zonePrefix + (lastAisle ? "    " : "\xb3   ");

            Shelf* currentShelf = currentAisle->headShelf;
            while (currentShelf != nullptr) {
                bool lastShelf = (currentShelf->nextShelf == nullptr);
                cout << aislePrefix
                     << (lastShelf ? "\xc0\xc4\xc4 " : "\xc3\xc4\xc4 ")
                     << "Shelf " << currentShelf->ID;

                // Count items so we know whether to show them inline or as children
                int itemCount = 0;
                Item* tmp = currentShelf->headItem;
                while (tmp != nullptr) { itemCount++; tmp = tmp->next; }

                if (itemCount == 0) {
                    cout << "  (empty)\n";
                } else if (itemCount <= 2) {
                    // Short shelf: list items inline
                    cout << ":  ";
                    Item* it = currentShelf->headItem;
                    bool first = true;
                    while (it != nullptr) {
                        if (!first) cout << ",  ";
                        cout << "[" << it->ID << "] " << it->name;
                        first = false;
                        it = it->next;
                    }
                    cout << "\n";
                } else {
                    // Long shelf: list items as sub-children
                    cout << "  (" << itemCount << " items)\n";
                    string shelfPrefix = aislePrefix + (lastShelf ? "    " : "\xb3   ");
                    Item* it = currentShelf->headItem;
                    while (it != nullptr) {
                        bool lastItem = (it->next == nullptr);
                        cout << shelfPrefix
                             << (lastItem ? "\xc0\xc4\xc4 " : "\xc3\xc4\xc4 ")
                             << "[" << it->ID << "] " << it->name << "\n";
                        it = it->next;
                    }
                }

                currentShelf = currentShelf->nextShelf;
            }
            currentAisle = currentAisle->nextAisle;
        }
        currentZone = currentZone->nextZone;
    }
    cout << "====================================================\n";
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

// ==================== Remove Item from Shelf (no BST change) ====================
// Unlinks an item from its current shelf linked list.
// Does NOT delete the Item object and does NOT modify the BST.
// Used before relocating an item to a new shelf.
bool removeItemFromShelf(Zone* head, int itemID) {
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
                        if (prev == nullptr)
                            s->headItem = item->next;
                        else
                            prev->next = item->next;
                        item->next = nullptr;   // Detach cleanly
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
