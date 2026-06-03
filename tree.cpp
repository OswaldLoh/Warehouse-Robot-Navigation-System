// Task 4 - Item Search & Management Module
// TP074412 Foo Kim Chean

// Implements a Binary Search Tree (BST) for fast item lookup by ID.
// The BST runs alongside the shelf linked lists and gives O(log n)
// search instead of O(n) linear scan.

#include <iostream>
#include <string>
#include "warehouse.h"

using namespace std;

// Global BST root
TreeNode* bstRoot = nullptr;

// Create a new tree node
TreeNode* createNode(Item* item) {
    return new TreeNode(item);
}

// Insert an item into the BST (sorted by Item ID)
TreeNode* insertBST(TreeNode* root, Item* item) {
    if (root == nullptr) {
        return createNode(item);
    }

    if (item->ID < root->item->ID) {
        root->left = insertBST(root->left, item);
    } else if (item->ID > root->item->ID) {
        root->right = insertBST(root->right, item);
    }
    // Duplicate IDs are not inserted

    return root;
}

// Search for an item by ID in the BST - O(log n)
TreeNode* searchBST(TreeNode* root, int itemID) {
    if (root == nullptr) return nullptr;

    if (itemID == root->item->ID) {
        return root;
    } else if (itemID < root->item->ID) {
        return searchBST(root->left, itemID);
    } else {
        return searchBST(root->right, itemID);
    }
}

// Find the node with the minimum value (leftmost node)
TreeNode* findMinNode(TreeNode* root) {
    TreeNode* current = root;
    while (current != nullptr && current->left != nullptr) {
        current = current->left;
    }
    return current;
}

// Find the node with the maximum value (rightmost node)
TreeNode* findMaxNode(TreeNode* root) {
    TreeNode* current = root;
    while (current != nullptr && current->right != nullptr) {
        current = current->right;
    }
    return current;
}

// Delete a node by Item ID from the BST
TreeNode* deleteBST(TreeNode* root, int itemID) {
    if (root == nullptr) return nullptr;

    if (itemID < root->item->ID) {
        root->left = deleteBST(root->left, itemID);
    } else if (itemID > root->item->ID) {
        root->right = deleteBST(root->right, itemID);
    } else {
        // Found the node to delete

        // Case 1: No children (leaf node)
        if (root->left == nullptr && root->right == nullptr) {
            delete root;
            return nullptr;
        }

        // Case 2: One child
        if (root->left == nullptr) {
            TreeNode* temp = root->right;
            delete root;
            return temp;
        }
        if (root->right == nullptr) {
            TreeNode* temp = root->left;
            delete root;
            return temp;
        }

        // Case 3: Two children - replace with in-order successor
        TreeNode* successor = findMinNode(root->right);
        root->item = successor->item;       // Copy the successor's item pointer
        root->right = deleteBST(root->right, successor->item->ID);
    }

    return root;
}

// In-order traversal - displays items sorted by ID
void inOrderTraversal(TreeNode* root) {
    if (root == nullptr) return;

    inOrderTraversal(root->left);
    cout << "  [" << root->item->ID << "] "
         << root->item->name
         << " (Zone " << root->item->zoneID
         << " Aisle " << root->item->aisleID
         << " Shelf " << root->item->shelfID << ")\n";
    inOrderTraversal(root->right);
}

// Display all items sorted by ID using BST in-order traversal
void displayItemsSorted() {
    if (bstRoot == nullptr) {
        cout << "No items in the system.\n";
        return;
    }

    cout << "\n=============== ITEMS (Sorted by ID) ===============\n";
    inOrderTraversal(bstRoot);
    cout << "====================================================\n";
}
