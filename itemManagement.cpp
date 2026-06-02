// Task 4 - Item Search & Management Module
//
// Provides CRUD operations on warehouse items.
// Uses the BST (tree.cpp) for fast search and the shelf linked list
// (storage.cpp) for item storage.
#include <iostream>
#include <string>
#include "headerFiles/warehouse.h"

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
    string input;
    cout << "Enter Item ID: ";
    getline(cin, input);

    bool valid = !input.empty();
    for (int i = 0; i < (int)input.length() && valid; i++) {
        if (input[i] < '0' || input[i] > '9') valid = false;
    }
    if (!valid) { cout << "Invalid ID.\n"; return; }

    int newID = stoi(input);

    // Check for duplicate ID
    if (findItemByID(getWarehouseHead(), newID) != nullptr) {
        cout << "Item with ID " << newID << " already exists.\n";
        return;
    }

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

// Update Item
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

    cout << "Current Details:\n";
    cout << "  ID:    " << item->ID << "\n";
    cout << "  Name:  " << item->name << "\n";
    cout << "  Zone:  " << item->zoneID << "\n";
    cout << "  Aisle: " << item->aisleID << "\n";
    cout << "  Shelf: " << item->shelfID << "\n";

    cout << "\nEnter new name (or press Enter to keep \"" << item->name << "\"): ";
    string newName;
    getline(cin, newName);
    if (!newName.empty()) {
        item->name = newName;
    }

    cout << "Item [" << item->ID << "] updated to \"" << item->name << "\".\n";
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
