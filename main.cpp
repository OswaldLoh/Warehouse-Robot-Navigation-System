#include <iostream>
#include <string>
#include "headerFiles/robot.h"
#include "headerFiles/order.h"
#include "headerFiles/warehouse.h"

using namespace std;

// ==================== Module Sub-Menus ====================


// Module 1: Order Management
void orderManagementMenu() {
    string input;
    int choice;

    do {
        cout << "\n===== ORDER MANAGEMENT MODULE =====\n";
        cout << "1. Add New Order\n";
        cout << "2. View All Order Status\n";
        cout << "3. Back to Main Menu\n";
        cout << "===================================\n";
        cout << "Enter choice: ";
        getline(cin, input);

        if (input.length() != 1 || input[0] < '1' || input[0] > '3') {
            cout << "\nInvalid input. Please enter 1, 2, or 3.\n";
            continue;
        }

        choice = input[0] - '0';
        cout << "\n";

        switch (choice) {
            case 1: addOrder(); break;
            case 2: viewOrderStatus(); break;
            case 3: break;
        }
    } while (choice != 3);
}

// Module 2: Robot Assignment
void robotAssignmentMenu() {
    string input;
    int choice;

    do {
        cout << "\n===== ROBOT ASSIGNMENT MODULE =====\n";
        cout << "1. Assign Pending Orders to Robots\n";
        cout << "2. View Robot Status\n";
        cout << "3. Robot Maintenance\n";
        cout << "4. Back to Main Menu\n";
        cout << "===================================\n";
        cout << "Enter choice: ";
        getline(cin, input);

        if (input.length() != 1 || input[0] < '1' || input[0] > '4') {
            cout << "\nInvalid input. Please enter 1, 2, 3, or 4.\n";
            continue;
        }

        choice = input[0] - '0';
        cout << "\n";

        switch (choice) {
            case 1: assignOrders(); break;
            case 2: displayRobotStatus(); break;
            case 3: robotMaintenance(); break;
            case 4: break;
        }
    } while (choice != 4);
}

// Module 3: Robot Navigation & Path Tracking
void navigationMenu() {
    string input;
    int choice;

    do {
        cout << "\n===== ROBOT NAVIGATION & PATH TRACKING MODULE =====\n";
        cout << "1. Complete Next In-Progress Order\n";
        cout << "2. Back to Main Menu\n";
        cout << "====================================================\n";
        cout << "Enter choice: ";
        getline(cin, input);

        if (input.length() != 1 || input[0] < '1' || input[0] > '2') {
            cout << "\nInvalid input. Please enter 1 or 2.\n";
            continue;
        }

        choice = input[0] - '0';
        cout << "\n";

        switch (choice) {
            case 1: completeOrder(); break;
            case 2: break;
        }
    } while (choice != 2);
}

// Module 4: Item Search & Management
void itemSearchMenu() {
    string input;
    int choice;

    do {
        cout << "\n===== ITEM SEARCH & MANAGEMENT MODULE =====\n";
        cout << "1. View Warehouse Inventory\n";
        cout << "2. Search Item by ID\n";
        cout << "3. Insert New Item\n";
        cout << "4. Update Item\n";
        cout << "5. Delete Item\n";
        cout << "6. Display Items (Sorted by ID)\n";
        cout << "7. Back to Main Menu\n";
        cout << "============================================\n";
        cout << "Enter choice: ";
        getline(cin, input);

        if (input.length() != 1 || input[0] < '1' || input[0] > '7') {
            cout << "\nInvalid input. Please enter a number between 1 and 7.\n";
            continue;
        }

        choice = input[0] - '0';
        cout << "\n";

        switch (choice) {
            case 1: displayWarehouse(getWarehouseHead()); break;
            case 2: searchItem(); break;
            case 3: insertItem(); break;
            case 4: updateItem(); break;
            case 5: deleteItemMenu(); break;
            case 6: displayItemsSorted(); break;
            case 7: break;
        }
    } while (choice != 7);
}

// Module 5: Warehouse Layout & Navigation
void warehouseLayoutMenu() {
    string input;
    int choice;

    do {
        cout << "\n===== WAREHOUSE LAYOUT & NAVIGATION MODULE =====\n";
        cout << "1. View Warehouse Layout\n";
        cout << "2. Back to Main Menu\n";
        cout << "=================================================\n";
        cout << "Enter choice: ";
        getline(cin, input);

        if (input.length() != 1 || input[0] < '1' || input[0] > '2') {
            cout << "\nInvalid input. Please enter 1 or 2.\n";
            continue;
        }

        choice = input[0] - '0';
        cout << "\n";

        switch (choice) {
            case 1: displayWarehouse(getWarehouseHead()); break;
            case 2: break;
        }
    } while (choice != 2);
}

// ==================== Main ====================

int main() {
    initRobots(3);      // Create 3 robots in a circular queue
    cout << "Warehouse Robot Navigation System initialized with " << robotCount << " robots.\n";

    // Initialize warehouse structure and load items from CSV
    initWarehouse();
    int itemCount = 0;
    Item** items = loadItemsFromCSV("database/items.csv", itemCount);
    if (items != nullptr) {
        for (int i = 0; i < itemCount; i++) {
            storeItem(getWarehouseHead(), items[i]);
        }
        delete[] items;     // Free the array (items themselves are now in the warehouse)
    }

    int choice;
    string input;

    do {
        cout << "\n========== WAREHOUSE ROBOT NAVIGATION SYSTEM ==========\n";
        cout << "1. Order Management Module\n";
        cout << "2. Robot Assignment Module\n";
        cout << "3. Robot Navigation & Path Tracking Module\n";
        cout << "4. Item Search & Management Module\n";
        cout << "5. Warehouse Layout & Navigation Module\n";
        cout << "0. Exit\n";
        cout << "========================================================\n";
        cout << "Enter choice: ";
        getline(cin, input);

        // Input validation: must be a single digit 0-5
        if (input.length() != 1 || input[0] < '0' || input[0] > '5') {
            cout << "\nInvalid input. Please enter a number between 0 and 5.\n";
            continue;
        }

        choice = input[0] - '0';       // Convert char to int

        switch (choice) {
            case 1: orderManagementMenu(); break;
            case 2: robotAssignmentMenu(); break;
            case 3: navigationMenu(); break;
            case 4: itemSearchMenu(); break;
            case 5: warehouseLayoutMenu(); break;
            case 0: cout << "\nExiting system. Goodbye!\n"; break;
        }
    } while (choice != 0);

    return 0;
}