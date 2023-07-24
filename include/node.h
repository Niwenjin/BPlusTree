#pragma once
#include <vector>
#include <string>

using namespace std;

class Node
{
public:
    Node(bool isleaf) : isleaf(isleaf), n(0), parent(nullptr), next(nullptr){};
    bool isleaf;
    size_t n;
    vector<string> keys;
    vector<string> values;
    vector<Node *> child;
    Node *parent;
    Node *next;
};