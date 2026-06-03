// Task 4 - Item Search & Management Module
// TP074412 Foo Kim Chean

// Loads warehouse item data from a CSV file (database/items.csv)
// at system startup so the warehouse is pre-populated.
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "headerFiles/warehouse.h"

using namespace std;

Item** loadItemsFromCSV(const string& filepath, int& count) {
    // First pass: count the number of data lines
    ifstream countFile(filepath);
    if (!countFile.is_open()) {
        cout << "Error: Could not open file \"" << filepath << "\"" << endl;
        count = 0;
        return nullptr;
    }

    string line;
    count = 0;

    // Skip header row
    getline(countFile, line);

    while (getline(countFile, line)) {
        if (!line.empty()) count++;
    }
    countFile.close();

    if (count == 0) {
        cout << "No items found in file." << endl;
        return nullptr;
    }

    // Allocate array of Item pointers
    Item** items = new Item*[count];

    // Second pass: parse each line
    ifstream dataFile(filepath);
    getline(dataFile, line);    // Skip header row

    int index = 0;
    while (getline(dataFile, line) && index < count) {
        if (line.empty()) continue;

        stringstream ss(line);
        string token;

        // Parse: ID,Name,ZoneID,AisleID,ShelfID
        int id;
        string name;
        char zoneID;
        int aisleID, shelfID;

        getline(ss, token, ',');    // ID
        id = stoi(token);

        getline(ss, token, ',');    // Name
        name = token;

        getline(ss, token, ',');    // ZoneID
        zoneID = token[0];

        getline(ss, token, ',');    // AisleID
        aisleID = stoi(token);

        getline(ss, token, ',');    // ShelfID
        shelfID = stoi(token);

        items[index] = new Item(id, name, zoneID, aisleID, shelfID);
        index++;
    }

    dataFile.close();
    count = index;  // Update count to actual number of items parsed

    cout << "Loaded " << count << " items from \"" << filepath << "\"" << endl;
    return items;
}
