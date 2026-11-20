// Task 1: BST deletion
// Name: Ayesha Kashaf Aslam   Reg No: 515191  Section: BSCS-15D
#include <iostream>
#include <string>
#include<vector>
using namespace std;

struct Node {
    int data;
    Node *left, *right;
    int height;   
};

Node* newNode(int v) { return new Node{v, nullptr, nullptr, 1}; }

// Plain BST insert; duplicates ignored
Node* insertBST(Node* root, int v) {
    if (!root) return newNode(v);
    if (v < root->data) root->left = insertBST(root->left, v);
    else if (v > root->data) root->right = insertBST(root->right, v);
    return root;
}

// Helper: smallest node of a subtree (inorder successor of a node with 2 children)
Node* minNode(Node* n) {
    while (n && n->left) n = n->left;
    return n;
}

// Recursive deletion. Always returns the (possibly new) subtree root.
Node* deleteBST(Node* root, int value) {
    if (!root) return nullptr;                       // empty tree / key not found
    if (value < root->data)      root->left  = deleteBST(root->left, value);
    else if (value > root->data) root->right = deleteBST(root->right, value);
    else {                                           // found the node
        if (!root->left && !root->right) {           // Case 1: leaf
            delete root;
            return nullptr;
        }
        if (!root->left) {                           // Case 2: only right child
            Node* r = root->right;
            delete root;
            return r;
        }
        if (!root->right) {                          // Case 2: only left child
            Node* l = root->left;
            delete root;
            return l;
        }
        // Case 3: two children -> copy inorder successor, delete it from right subtree
        Node* s = minNode(root->right);
        root->data = s->data;
        root->right = deleteBST(root->right, s->data);
    }
    return root;
}

void destroy(Node* r)  { if (r) { destroy(r->left); destroy(r->right); delete r; } }

// Inorder traversal collected into a string, e.g. "20 30 40"
string inorderStr(Node* r) {
    if (!r) return "";
    string l = inorderStr(r->left), rt = inorderStr(r->right);
    return (l.empty() ? "" : l + " ") + to_string(r->data) + (rt.empty() ? "" : " " + rt);
}

// ---------- Tree drawing (top-down, root centred over its children) ----------
struct Box { vector<string> rows; int w; int c; };   // c = column of the root label's centre

Box build(Node* n) {
    string s = to_string(n->data);
    int len = (int)s.size();
    if (!n->left && !n->right) return Box{{s}, len, len / 2};
    Box L{{}, 0, 0}, R{{}, 0, 0};
    if (n->left)  L = build(n->left);
    if (n->right) R = build(n->right);
    int lx = 0, rx = n->left ? L.w + 3 : 0;          // 3 spaces between sibling subtrees
    int lc = lx + L.c, rc = rx + R.c, c;
    if (n->left && n->right) c = (lc + rc) / 2;
    else if (n->left)        c = lc + 2;
    else                     c = rc - 2;
    int start = c - len / 2, shift = start < 0 ? -start : 0;
    lx += shift; rx += shift; lc += shift; rc += shift; c += shift; start += shift;
    int w = max(start + len, max(n->left ? lx + L.w : 0, n->right ? rx + R.w : 0));
    string top(w, ' '), slash(w, ' ');
    if (n->left)  slash[lc] = '/';
    if (n->right) slash[rc] = '\\';
    if (n->left && n->right) {                       // horizontal bar joins the two branches
        for (int i = lc + 1; i < start; i++) top[i] = '_';
        for (int i = start + len; i < rc; i++) top[i] = '_';
    }
    for (int i = 0; i < len; i++) top[start + i] = s[i];
    Box b{{top, slash}, w, c};
    size_t h = max(L.rows.size(), R.rows.size());
    for (size_t i = 0; i < h; i++) {
        string line(w, ' ');
        if (i < L.rows.size()) line.replace(lx, L.rows[i].size(), L.rows[i]);
        if (i < R.rows.size()) line.replace(rx, R.rows[i].size(), R.rows[i]);
        b.rows.push_back(line);
    }
    b.w = w; b.c = c;
    return b;
}

void drawTree(Node* root, const string& indent = "    ") {
    if (!root) { cout << indent << "(empty tree)\n"; return; }
    Box b = build(root);
    for (auto& r : b.rows) {
        size_t e = r.find_last_not_of(' ');
        cout << indent << (e == string::npos ? "" : r.substr(0, e + 1)) << "\n";
    }
}

// One deletion step: case, prediction, actual result, match check
Node* step(Node* root, int key, const char* caseName, const string& predicted) {
    root = deleteBST(root, key);
    string actual = inorderStr(root);
    cout << "Delete " << key << "  [" << caseName << "]\n"
         << "  predicted: " << predicted << "\n"
         << "  actual   : " << (actual.empty() ? "(empty)" : actual) << "\n"
         << "  match    : " << (actual == predicted ? "YES" : "NO") << "\n";
    cout << "  tree:\n";
    drawTree(root);
    cout << "\n";
    return root;
}

int main() {
    Node* root = nullptr;
    int keys[] = {50, 30, 70, 20, 40, 60, 80, 65};
    for (int k : keys) root = insertBST(root, k);

    cout << "----- Initial tree -----\n";
    drawTree(root);
    cout << "  inorder: " << inorderStr(root) << "\n\n";

    cout << "----- Task 1: deletions -----\n";
    root = step(root, 20, "case 1: leaf",         "30 40 50 60 65 70 80");
    root = step(root, 60, "case 2: one child",    "30 40 50 65 70 80");
    root = step(root, 50, "case 3: two children, successor = 65", "30 40 65 70 80");

    cout << "----- Boundary cases -----\n";
    root = step(root, 999, "missing key, tree unchanged", "30 40 65 70 80");

    Node* e = deleteBST(nullptr, 5);
    cout << "Delete from empty tree  -> returns nullptr? " << (e == nullptr ? "YES" : "NO") << "\n";

    Node* one = insertBST(nullptr, 42);
    one = deleteBST(one, 42);
    cout << "Delete only node (42)   -> returns nullptr? " << (one == nullptr ? "YES" : "NO") << "\n";

    destroy(root);   // release remaining nodes
    return 0;
}