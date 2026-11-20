// Lab 10 - Q2: AVL insertion and rotations
// Name: Ayesha Kashaf Aslam  Reg No: 515191  Section: BSCS-15D
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <initializer_list>
using namespace std;

struct Node {
    int data;
    Node *left, *right;
    int height;
};

Node* newNode(int v) { return new Node{v, nullptr, nullptr, 1}; }

int height(Node* n)  { return n ? n->height : 0; }                 // height(nullptr) = 0
int balance(Node* n) { return n ? height(n->left) - height(n->right) : 0; }
void update(Node* n) { n->height = 1 + max(height(n->left), height(n->right)); }

bool verbose = true;                                                // print rotation messages
void say(const string& s) { if (verbose) cout << "    * " << s << "\n"; }

// Right rotation (y = unbalanced node, x = its left child, T2 = x's right subtree):
//   before:  y(left=x(left=A, right=T2), right=C)
//   after:   x(left=A, right=y(left=T2, right=C))
Node* rightRotate(Node* y) {
    Node* x = y->left;
    Node* t2 = x->right;
    x->right = y;
    y->left = t2;
    update(y);          // old root first (it is now lower)
    update(x);          // then the new root
    return x;
}

// Mirror image of rightRotate
Node* leftRotate(Node* x) {
    Node* y = x->right;
    Node* t2 = y->left;
    y->left = x;
    x->right = t2;
    update(x);
    update(y);
    return y;
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

Node* insertAVL(Node* root, int v) {
    if (!root) return newNode(v);
    if (v < root->data)      root->left  = insertAVL(root->left, v);
    else if (v > root->data) root->right = insertAVL(root->right, v);
    else return root;                                  // ignore duplicates

    update(root);
    int bf = balance(root);
    string at = "at node " + to_string(root->data) + " (balance " + to_string(bf) + "): ";

    if (bf > 1 && v < root->left->data) {              // LL
        say(at + "LL case -> right rotate " + to_string(root->data));
        return rightRotate(root);
    }
    if (bf < -1 && v > root->right->data) {            // RR
        say(at + "RR case -> left rotate " + to_string(root->data));
        return leftRotate(root);
    }
    if (bf > 1 && v > root->left->data) {              // LR
        say(at + "LR case -> left rotate " + to_string(root->left->data) + ", then right rotate " + to_string(root->data));
        root->left = leftRotate(root->left);
        return rightRotate(root);
    }
    if (bf < -1 && v < root->right->data) {            // RL
        say(at + "RL case -> right rotate " + to_string(root->right->data) + ", then left rotate " + to_string(root->data));
        root->right = rightRotate(root->right);
        return leftRotate(root);
    }
    return root;
}

void preorder(Node* r) { if (r) { cout << r->data << " "; preorder(r->left); preorder(r->right); } }
void inorder(Node* r)  { if (r) { inorder(r->left); cout << r->data << " "; inorder(r->right); } }
void destroy(Node* r)  { if (r) { destroy(r->left); destroy(r->right); delete r; } }

// Stored heights correct and every |balance factor| <= 1
bool isBalanced(Node* r) {
    if (!r) return true;
    return r->height == 1 + max(height(r->left), height(r->right)) &&
           abs(balance(r)) <= 1 && isBalanced(r->left) && isBalanced(r->right);
}

void table(Node* r) {// inorder walk: key / height / balance factor
    if (!r) return;
    table(r->left);
    cout << "      " << setw(4) << r->data << setw(8) << r->height << setw(9) << (balance(r) > 0 ? "+" : "") + to_string(balance(r)) << "\n";
    table(r->right);
}

void report(Node* r) {
    cout << "  Tree:\n";
    drawTree(r);
    cout << "  Preorder: "; preorder(r);
    cout << "\n  Inorder : "; inorder(r);
    cout << "\n  Node table:\n      " << setw(4) << "key" << setw(8) << "height" << setw(9) << "balance" << "\n";
    table(r);
    cout << "  All nodes balanced? " << (isBalanced(r) ? "YES" : "NO") << "\n";
}

void runCase(const char* name, initializer_list<int> keys) {
    cout << "\n-----------------------------------------------\n"
         << " Sequence " << name << "\n"
         << "-----------------------------------------------\n";
    Node* root = nullptr;
    for (int k : keys) {
        cout << "  Insert " << k << "\n";
        root = insertAVL(root, k);
    }
    report(root);
    destroy(root); // release all nodes
}

int main() {
    runCase("(a)  30, 20, 10", {30, 20, 10});
    runCase("(b)  10, 20, 30", {10, 20, 30});
    runCase("(c)  30, 10, 20", {30, 10, 20});
    runCase("(d)  10, 30, 20", {10, 30, 20});
    runCase("extra: 10, 20, 30, 40, 50, 25, 25 (duplicate ignored)", {10, 20, 30, 40, 50, 25, 25});
    return 0;
}