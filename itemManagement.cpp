// Task 4 - Item Search & Management Module
// TP074412 Foo Kim Chean
// Provides CRUD operations on warehouse items.
// Uses the BST (tree.cpp) for fast search and the shelf linked list
// (storage.cpp) for item storage.
#include <iostream>
#include <iomanip>
#include <string>
#include "warehouse.h"
#include "robot.h"      // For findAvailableRobot() and Robot class


using namespace std;

// Search Item by ID
void searchItem() {
    string input;
    cout << "Enter Item ID to search: ";
    getline(cin, input);

    bool valid = !input.empty();
    for (int i = 0; i < (int)input.length() && valid; i++) {
        if (input[i] < '0' || input[i] > '9') valid = false;
    }
    if (!valid) { cout << "Invalid ID.\n"; return; }

    int searchID = stoi(input);
    Item* found = findItemByID(getWarehouseHead(), searchID);
    if (found == nullptr) {
        cout << "Item with ID " << searchID << " not found.\n";
    } else {
        cout << "Item Found:\n";
        cout << "  ID:    " << found->ID << "\n";
        cout << "  Name:  " << found->name << "\n";
        cout << "  Zone:  " << found->zoneID << "\n";
        cout << "  Aisle: " << found->aisleID << "\n";
        cout << "  Shelf: " << found->shelfID << "\n";
    }
}

// Insert New Item
void insertItem() {
    // Auto-generate ID: largest existing ID + 1, or 1 if database is empty
    int newID = 1;
    TreeNode* maxNode = findMaxNode(bstRoot);
    if (maxNode != nullptr) newID = maxNode->item->ID + 1;

    cout << "Auto-generated Item ID: " << newID << "\n";

    string input;

    cout << "Enter Item Name: ";
    string newName;
    getline(cin, newName);
    if (newName.empty()) { cout << "Name cannot be empty.\n"; return; }

    cout << "Enter Zone ID (A/B/C): ";
    getline(cin, input);
    if (input.length() != 1 || input[0] < 'A' || input[0] > 'C') {
        cout << "Invalid Zone ID. Must be A, B, or C.\n";
        return;
    }
    char zoneID = input[0];

    cout << "Enter Aisle ID (1/2): ";
    getline(cin, input);
    if (input.length() != 1 || input[0] < '1' || input[0] > '2') {
        cout << "Invalid Aisle ID. Must be 1 or 2.\n";
        return;
    }
    int aisleID = input[0] - '0';

    cout << "Enter Shelf ID (1/2/3): ";
    getline(cin, input);
    if (input.length() != 1 || input[0] < '1' || input[0] > '3') {
        cout << "Invalid Shelf ID. Must be 1, 2, or 3.\n";
        return;
    }
    int shelfID = input[0] - '0';

    Item* newItem = new Item(newID, newName, zoneID, aisleID, shelfID);
    storeItem(getWarehouseHead(), newItem);
    cout << "Item [" << newID << "] \"" << newName
         << "\" added to Zone " << zoneID
         << " Aisle " << aisleID
         << " Shelf " << shelfID << ".\n";
}

// ===================== Item Relocation =====================
// Dispatches the next available robot to physically carry the item
// from its old location to the new one.
// Navigation is recorded step-by-step using the robot's stack so
// goBack() returns it to base via the full reverse path.
static void relocateItem(Item* item, char oldZone, int oldAisle, int oldShelf) {
    Robot* robot = findAvailableRobot();
    if (robot == nullptr) {
        cout << "\n[!] No available robots. Item has been updated in the database,\n"
             << "    but no robot was dispatched to physically move it.\n";
        return;
    }

    robot->setStatus("Busy");

    string oldZoneStr  = "Zone ";  oldZoneStr  += oldZone;
    string oldAisleStr = "Aisle " + to_string(oldAisle);
    string oldShelfStr = "Shelf " + to_string(oldShelf);
    string newZoneStr  = "Zone ";  newZoneStr  += item->zoneID;
    string newAisleStr = "Aisle " + to_string(item->aisleID);
    string newShelfStr = "Shelf " + to_string(item->shelfID);

    cout << "\n====================================================\n";
    cout << " Robot [" << robot->getID() << "] | ITEM RELOCATION\n";
    cout << " Item: [" << item->ID << "] \"" << item->name << "\"\n";
    cout << "====================================================\n";

    // Phase 1: Navigate to old location and pick up item
    cout << "\n[Phase 1 - Navigating to Old Location]\n";
    cout << "  From: " << oldZoneStr << " | " << oldAisleStr << " | " << oldShelfStr << "\n\n";

    int step = 1;
    cout << "  Step " << step++ << ": Navigate to " << oldZoneStr << "\n";
    robot->pushStep("Navigate to " + oldZoneStr,  "Return from " + oldZoneStr);
    cout << "  Step " << step++ << ": Navigate to " << oldAisleStr << "\n";
    robot->pushStep("Navigate to " + oldAisleStr, "Return from " + oldAisleStr);
    cout << "  Step " << step++ << ": Navigate to " << oldShelfStr << "\n";
    robot->pushStep("Navigate to " + oldShelfStr, "Return from " + oldShelfStr);
    cout << "  Picking up [" << item->ID << "] \"" << item->name << "\"...\n";

    // Phase 2: Navigate to new location and place item
    cout << "\n[Phase 2 - Navigating to New Location]\n";
    cout << "  To:   " << newZoneStr << " | " << newAisleStr << " | " << newShelfStr << "\n\n";

    cout << "  Step " << step++ << ": Navigate to " << newZoneStr << "\n";
    robot->pushStep("Navigate to " + newZoneStr,  "Return from " + newZoneStr);
    cout << "  Step " << step++ << ": Navigate to " << newAisleStr << "\n";
    robot->pushStep("Navigate to " + newAisleStr, "Return from " + newAisleStr);
    cout << "  Step " << step++ << ": Navigate to " << newShelfStr << "\n";
    robot->pushStep("Navigate to " + newShelfStr, "Return from " + newShelfStr);
    cout << "  Placing item at new location...\n";

    // Return to base via full reverse path
    cout << "\n[Phase 3 - Returning to Base]\n";
    robot->goBack();

    cout << "\nRelocation of [" << item->ID << "] \"" << item->name << "\" complete!\n";
    cout << "====================================================\n";
}

// ===================== Display All Items =====================
// Gathers every item from all shelf linked lists into a local array,
// sorts with Bubble Sort (by ID or Name), then prints a setw() table.
void displayAllItems() {
    cout << "\n================= DISPLAY ALL ITEM ==================\n";
    cout << "1. Sort by ID\n";
    cout << "2. Sort by Name\n";
    cout << "3. Cancel\n";
    cout << "====================================================\n";
    cout << ">>> ";

    string input;
    getline(cin, input);

    if (input.length() != 1 || input[0] < '1' || input[0] > '3') {
        cout << "Invalid choice.\n";
        return;
    }
    int sortChoice = input[0] - '0';
    if (sortChoice == 3) return;

    // Collect all items from every shelf linked list
    const int MAX_ITEMS = 200;
    Item* items[MAX_ITEMS];
    int count = 0;

    Zone* z = getWarehouseHead();
    while (z != nullptr && count < MAX_ITEMS) {
        Aisle* a = z->headAisle;
        while (a != nullptr && count < MAX_ITEMS) {
            Shelf* s = a->headShelf;
            while (s != nullptr && count < MAX_ITEMS) {
                Item* it = s->headItem;
                while (it != nullptr && count < MAX_ITEMS) {
                    items[count++] = it;
                    it = it->next;
                }
                s = s->nextShelf;
            }
            a = a->nextAisle;
        }
        z = z->nextZone;
    }

    if (count == 0) {
        cout << "No items in the warehouse.\n";
        return;
    }

    // Bubble Sort
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - 1 - i; j++) {
            bool doSwap = (sortChoice == 1)
                          ? (items[j]->ID > items[j + 1]->ID)
                          : (items[j]->name > items[j + 1]->name);
            if (doSwap) {
                Item* tmp      = items[j];
                items[j]       = items[j + 1];
                items[j + 1]   = tmp;
            }
        }
    }

    string sortLabel = (sortChoice == 1) ? "ID" : "Name";
    cout << "\n====================================================\n";
    cout << " All Items | Sorted by: " << sortLabel
         << " | Total: " << count << "\n";
    cout << "====================================================\n";

    // Column header
    cout << left
         << setw(6)  << "ID"
         << setw(24) << "NAME"
         << setw(7)  << "ZONE"
         << setw(8)  << "AISLE"
         << "SHELF"  << "\n";
    cout << string(52, '-') << "\n";

    // Data rows
    for (int i = 0; i < count; i++) {
        cout << left
             << setw(6)  << items[i]->ID
             << setw(24) << items[i]->name
             << setw(7)  << items[i]->zoneID
             << setw(8)  << items[i]->aisleID
             << items[i]->shelfID << "\n";
    }
    cout << "====================================================\n";
}

// ===================== Update Item =====================
void updateItem() {
    string input;
    cout << "Enter Item ID to update: ";
    getline(cin, input);

    bool valid = !input.empty();
    for (int i = 0; i < (int)input.length() && valid; i++) {
        if (input[i] < '0' || input[i] > '9') valid = false;
    }
    if (!valid) { cout << "Invalid ID.\n"; return; }

    int updateID = stoi(input);
    Item* item = findItemByID(getWarehouseHead(), updateID);
    if (item == nullptr) {
        cout << "Item with ID " << updateID << " not found.\n";
        return;
    }

    cout << "\nCurrent Details:\n";
    cout << "  ID:    " << item->ID    << "\n";
    cout << "  Name:  " << item->name  << "\n";
    cout << "  Zone:  " << item->zoneID  << "\n";
    cout << "  Aisle: " << item->aisleID << "\n";
    cout << "  Shelf: " << item->shelfID << "\n";

    cout << "\n========== WHAT WOULD YOU LIKE TO EDIT? ===========\n";
    cout << "1. Name\n";
    cout << "2. Zone  (A / B / C)\n";
    cout << "3. Aisle (1 / 2)\n";
    cout << "4. Shelf (1 / 2 / 3)\n";
    cout << "5. Cancel\n";
    cout << "====================================================\n";
    cout << "Enter choice: ";
    getline(cin, input);

    if (input.length() != 1 || input[0] < '1' || input[0] > '5') {
        cout << "Invalid choice.\n";
        return;
    }
    int choice = input[0] - '0';

    if (choice == 5) {
        cout << "Update cancelled.\n";
        return;
    }

    // Snapshot old location before any change
    char oldZone  = item->zoneID;
    int  oldAisle = item->aisleID;
    int  oldShelf = item->shelfID;
    bool locationChanged = false;

    if (choice == 1) {
        cout << "Enter new name (current: \"" << item->name << "\"): ";
        string newName;
        getline(cin, newName);
        if (newName.empty()) {
            cout << "Name cannot be empty. Update cancelled.\n";
            return;
        }
        item->name = newName;
        cout << "Item [" << item->ID << "] name updated to \"" << item->name << "\".\n";

    } else if (choice == 2) {
        cout << "Enter new Zone ID (A/B/C, current: " << item->zoneID << "): ";
        getline(cin, input);
        if (input.length() != 1 || input[0] < 'A' || input[0] > 'C') {
            cout << "Invalid Zone ID. Must be A, B, or C.\n";
            return;
        }
        if (input[0] == item->zoneID) { cout << "Zone unchanged.\n"; return; }
        item->zoneID = input[0];
        locationChanged = true;

    } else if (choice == 3) {
        cout << "Enter new Aisle ID (1/2, current: " << item->aisleID << "): ";
        getline(cin, input);
        if (input.length() != 1 || input[0] < '1' || input[0] > '2') {
            cout << "Invalid Aisle ID. Must be 1 or 2.\n";
            return;
        }
        int newAisle = input[0] - '0';
        if (newAisle == item->aisleID) { cout << "Aisle unchanged.\n"; return; }
        item->aisleID = newAisle;
        locationChanged = true;

    } else if (choice == 4) {
        cout << "Enter new Shelf ID (1/2/3, current: " << item->shelfID << "): ";
        getline(cin, input);
        if (input.length() != 1 || input[0] < '1' || input[0] > '3') {
            cout << "Invalid Shelf ID. Must be 1, 2, or 3.\n";
            return;
        }
        int newShelf = input[0] - '0';
        if (newShelf == item->shelfID) { cout << "Shelf unchanged.\n"; return; }
        item->shelfID = newShelf;
        locationChanged = true;
    }

    if (locationChanged) {
        // 1. Detach from old shelf (BST untouched — ID hasn't changed)
        removeItemFromShelf(getWarehouseHead(), item->ID);
        // 2. Attach to new shelf (insertBST is a no-op for duplicate IDs)
        storeItem(getWarehouseHead(), item);

        cout << "Item [" << item->ID << "] \"" << item->name << "\" moved:\n";
        cout << "  From: Zone " << oldZone  << " | Aisle " << oldAisle << " | Shelf " << oldShelf  << "\n";
        cout << "  To:   Zone " << item->zoneID << " | Aisle " << item->aisleID << " | Shelf " << item->shelfID << "\n";

        // 3. Dispatch a robot to physically perform the move
        relocateItem(item, oldZone, oldAisle, oldShelf);
    }
}

// Delete Item
void deleteItemMenu() {
    string input;
    cout << "Enter Item ID to delete: ";
    getline(cin, input);

    bool valid = !input.empty();
    for (int i = 0; i < (int)input.length() && valid; i++) {
        if (input[i] < '0' || input[i] > '9') valid = false;
    }
    if (!valid) { cout << "Invalid ID.\n"; return; }

    int deleteID = stoi(input);

    // Show the item before confirming deletion
    Item* item = findItemByID(getWarehouseHead(), deleteID);
    if (item == nullptr) {
        cout << "Item with ID " << deleteID << " not found.\n";
        return;
    }

    cout << "Deleting: [" << item->ID << "] \"" << item->name
         << "\" (Zone " << item->zoneID
         << " Aisle " << item->aisleID
         << " Shelf " << item->shelfID << ")\n";
    cout << "Confirm delete? (y/n): ";
    getline(cin, input);

    if (input == "y" || input == "Y") {
        if (deleteItem(getWarehouseHead(), deleteID)) {
            cout << "Item deleted successfully.\n";
        } else {
            cout << "Error: Could not delete item.\n";
        }
    } else {
        cout << "Deletion cancelled.\n";
    }
}
