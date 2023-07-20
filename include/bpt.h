#pragma once
#include "node.h"
#include <vector>
#include <string>

using namespace std;

class BPlusTree
{
public:
    void insert(int, string);
    void remove(int);
    bool search(int);
    void change(int, int);
    void read(const string &);
    void save(const string &);
    void show();
    BPlusTree(int m) : root(nullptr), _m(m){};

private:
    const size_t _m;
    Node *root;
    void splitChild(Node *, int, Node *);
    void insertNonFull(Node *, int);
    void remove(Node *, int);
    string *search(Node *, int);
    void show(Node *);
};