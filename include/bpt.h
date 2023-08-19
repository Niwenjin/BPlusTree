#pragma once
#include <vector>
#include <string>
using namespace std;

class Node
{
public:
    Node(bool isleaf) : isleaf(isleaf),
                        n(0), parent(nullptr), next(nullptr){};
    ~Node()
    {
        keys.clear();
        values.clear();
        child.clear();
    }
    bool isleaf;
    size_t n;
    vector<string> keys;
    vector<string> values;
    vector<Node *> child;
    Node *parent;
    Node *next;
};

class BPlusTree
{
public:
    BPlusTree(int m) : root(nullptr),
                       head(nullptr), _m(m){};
    ~BPlusTree() {
        deleteTree(root);
    }
    void insert(string, string);
    bool remove(string);
    string *search(string);
    bool change(string, string);
    void show();
    void printall();
    void read(const string &);
    void save(const string &);
    void serialize(const string &);
    void deserialize(const string &);

private:
    const size_t _m; // b+树的度
    Node *root;
    Node *head; // 第一个叶子节点指针
    void insert(Node *, string, string);
    void split(Node *);
    void update(Node *, string, string);
    void borrow(Node *, Node *, bool, bool);
    void merge(Node *, Node *, bool, bool);
    Node *search(Node *, string);
    size_t findkey(Node *, string);
    void print(Node *);
    void deleteTree(Node *);
};