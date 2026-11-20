// Task 3: AVL insertion and deletion
// Name: Ayesha Kashaf Aslam   Reg No: 515191  Section: BSCS-15D
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

Node* minNode(Node* n) { while (n && n->left) n = n->left; return n; }

// Rebalance a node whose children are already valid AVL trees.
// The rotation is chosen from the CHILD's balance factor (works for insert and delete).
Node* rebalance(Node* root) {
    update(root);
    int bf = balance(root);
    string at = "at node " + to_string(root->data) + " (balance " + to_string(bf) + "): ";
    if (bf > 1) {                                      // left heavy
        int cb = balance(root->left);
        if (cb < 0) {                                  // child leans right -> LR
            say(at + "left child " + to_string(root->left->data) + " has balance " + to_string(cb) +
                " -> left rotate " + to_string(root->left->data) + ", then right rotate " + to_string(root->data));
            root->left = leftRotate(root->left);
        } else {                                       // child balance >= 0 -> single right
            say(at + "left child " + to_string(root->left->data) + " has balance " + to_string(cb) +
                " (>= 0) -> single right rotate " + to_string(root->data));
        }
        return rightRotate(root);
    }
    if (bf < -1) {                                     // right heavy
        int cb = balance(root->right);
        if (cb > 0) {                                  // child leans left -> RL
            say(at + "right child " + to_string(root->right->data) + " has balance " + to_string(cb) +
                " -> right rotate " + to_string(root->right->data) + ", then left rotate " + to_string(root->data));
            root->right = rightRotate(root->right);
        } else {                                       // child balance <= 0 -> single left
            say(at + "right child " + to_string(root->right->data) + " has balance " + to_string(cb) +
                " (<= 0) -> single left rotate " + to_string(root->data));
        }
        return leftRotate(root);
    }
    return root;                                       // already balanced
}

Node* insertAVL(Node* root, int v) {
    if (!root) return newNode(v);
    if (v < root->data)      root->left  = insertAVL(root->left, v);
    else if (v > root->data) root->right = insertAVL(root->right, v);
    else return root;                                  // ignore duplicates
    return rebalance(root);
}

Node* deleteAVL(Node* root, int v) {
    if (!root) return nullptr;                         // empty tree / key missing
    if (v < root->data)      root->left  = deleteAVL(root->left, v);
    else if (v > root->data) root->right = deleteAVL(root->right, v);
    else {
        if (!root->left || !root->right) {             // 0 or 1 child
            Node* child = root->left ? root->left : root->right;
            delete root;
            return child;                              // nullptr if it was a leaf
        }
        Node* s = minNode(root->right);                // 2 children: inorder successor
        root->data = s->data;
        root->right = deleteAVL(root->right, s->data);
    }
    return rebalance(root);                            // runs at every ancestor on the way up
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

void table(Node* r) {                                // inorder walk: key / height / balance factor
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

void banner(const string& t) {
    cout << "\n-------------------------------------------------\n " << t
         << "\n-------------------------------------------------\n";
}

int main() {
    banner("Main test: insert 30, 20, 40, 10, 25, 50, 5");
    Node* root = nullptr;
    verbose = false;                                    // keep insertion quiet; no rotations happen anyway
    for (int k : {30, 20, 40, 10, 25, 50, 5}) root = insertAVL(root, k);
    verbose = true;
    report(root);

    cout << "\n  --- Delete 50 ---\n";
    root = deleteAVL(root, 50);
    report(root);

    cout << "\n  --- Delete 40 ---\n";
    root = deleteAVL(root, 40);
    cout << "    * no node became unbalanced -> no rotation needed\n";
    report(root);

    banner("Edge cases");
    cout << "  Delete missing key 999:\n";
    root = deleteAVL(root, 999);
    cout << "    tree unchanged. Inorder: "; inorder(root); cout << "\n";
    destroy(root);

    Node* e = deleteAVL(nullptr, 7);
    cout << "  Delete from empty tree      -> returns nullptr? " << (e == nullptr ? "YES" : "NO") << "\n";
    Node* one = insertAVL(nullptr, 42);
    one = deleteAVL(one, 42);
    cout << "  Delete the only node (42)   -> returns nullptr? " << (one == nullptr ? "YES" : "NO") << "\n";

    banner("Heavy child with balance factor 0:  30, 20, 40, 10, 25  then delete 40");
    Node* t = nullptr;
    verbose = false;
    for (int k : {30, 20, 40, 10, 25}) t = insertAVL(t, k);
    verbose = true;
    cout << "  Before:\n"; drawTree(t);
    cout << "\n  --- Delete 40 ---\n";
    t = deleteAVL(t, 40);
    cout << "    Why single: node 20 has two equal-height subtrees (balance 0), so one\n"
            "    right rotation fixes 30. A double rotation would not.\n";
    report(t);
    destroy(t);
    return 0;
}