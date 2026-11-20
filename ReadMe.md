# Data Structures & Algorithms — Lab 10: BST & AVL Trees

**Author:** Ayesha Kashaf Aslam<br>
**Registration No:** 515191<br>
**Section:** BSCS-15D<br>

## Overview
This repository contains C++ implementations of Binary Search Tree (BST) and self-balancing AVL Tree operations. Each program includes an ASCII top-down tree visualizer, traversal outputs, and automated verification of structural invariants and edge cases.

## Repository Structure

### 1. `Task1.cpp` — Binary Search Tree (BST) Deletion
Implements recursive node deletion (`deleteBST`) covering all three standard structural cases alongside boundary conditions:
* **Case 1 (Leaf Node):** Removes node with zero children directly.
* **Case 2 (Single Child):** Replaces node with its sole left or right child.
* **Case 3 (Two Children):** Replaces node value with its inorder successor (`minNode` in the right subtree) and recursively deletes the successor.
* **Verification:** Compares predicted vs. actual inorder traversal strings after each step and tests empty-tree, single-node, and missing-key edge cases.

### 2. `Task2.cpp` — AVL Tree Insertion & Rotations
Implements self-balancing AVL insertion (`insertAVL`) with dynamic height tracking and balance factor computation:
* **Single Rotations:** Handles Left-Left (`LL`) via `rightRotate` and Right-Right (`RR`) via `leftRotate`.
* **Double Rotations:** Handles Left-Right (`LR`) and Right-Left (`RL`) imbalances.
* **Diagnostics:** Prints step-by-step rotation logs, preorder/inorder traversals, a per-node height and balance factor table, and an automated `isBalanced` check.

### 3. `Task3.cpp` — Complete AVL Tree (Insertion & Deletion)
Extends the AVL implementation with self-balancing node deletion (`deleteAVL`) using a unified `rebalance` helper:
* **Child Balance Factor Routing:** Determines single vs. double rotations after deletion by inspecting the heavy child's balance factor (including the `balance == 0` single-rotation case).
* **Edge Cases:** Validates deletions resulting in no imbalance, missing keys, single-node trees, and empty trees.

## Compilation & Execution

Requires any standard C++ compiler supporting C++11 or later (`g++`, `clang++`, or MSVC).
