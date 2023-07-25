#pragma once
#include "node.h"
#include <vector>
#include <string>

using namespace std;

class BPlusTree
{
public:
    BPlusTree(int m) : root(nullptr), _m(m){};
    void insert(string, string);
    bool remove(string);
    string *search(string);
    bool change(string, string);

private:
    const size_t _m; // b+树的度
    Node *root;
    Node *head; // 第一个叶子节点指针
    void insert(Node *, string, string);
    void split(Node *);
    bool remove(string);
    bool update(Node *, string, string);
    void borrow(Node *, Node *, bool);
    void merge(Node *, Node *, bool);
    Node *search(Node *, string);
};