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
    string search(string);

private:
    const size_t _m;
    Node *root;
    Node *head; // 第一个叶子节点指针
    void insert(Node *, string, string);
    void split(Node *);
    string search(Node *, string);
};