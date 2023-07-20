#pragma once
#include <vector>
#include <string>

using namespace std;

class Node
{
public:
    Node(bool isleaf) : isleaf(isleaf), next(nullptr){};
    bool isleaf;
    vector<int> keys;
    vector<string> values;
    vector<Node *> child;
    Node *next;
};