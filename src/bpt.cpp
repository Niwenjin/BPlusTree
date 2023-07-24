#include "bpt.h"
#include "node.h"
#include <iostream>
#include <vector>
using namespace std;

// 在B+树中插入键值对
void BPlusTree::insert(string key, string value)
{
    // 根节点为空
    if (root == nullptr)
    {
        root = new Node(true);
        insert(root, key, value);
        head = root;
    }
    // 根节点为叶子节点
    else if (root->isleaf)
        insert(root, key, value);
    // 查找插入的叶子节点
    else
    {
        Node *p = root;
        size_t i = 0;
        while (i < p->n && key > p->keys[i])
            i++;
        // 插入的关键字大于最大值
        if (p->n == i)
        {
            while (!p->isleaf)
            {
                p->keys[p->n - 1] = key; // 沿途更新key值
                p = p->child[p->n - 1];
            }
            insert(p, key, value);
        }
        // 插入的关键字不大于最大值
        else
        {
            while (!p->isleaf)
            {
                p = p->child[i];
                i = 0;
                while (i < p->n && key > p->keys[i])
                    i++;
            }
            insert(p, key, value);
        }
    }
}

// 在叶子节点中插入键值对
void BPlusTree::insert(Node *p, string key, string value)
{
    size_t i = 0;
    while (i < p->n && key > p->keys[i])
        i++;
    auto it = p->keys.begin();
    p->keys.insert(it + i, key);
    it = p->values.begin();
    p->values.insert(it + i, value);
    p->n++;
    // 节点满，分裂
    if (p->n == _m + 1)
        split(p);
}

// 从叶子节点向上分裂
void BPlusTree::split(Node *p)
{
    size_t t = _m / 2; // 分裂位置
    Node *bro = new Node(p->isleaf);
    // 分裂root节点
    if (p->parent == nullptr)
    {
        // 创建新root节点
        Node *newroot = new Node(false);
        root = newroot;
        root->keys.push_back(p->keys[t]);
        root->child.push_back(p);
        root->keys.push_back(p->keys[_m]);
        root->child.push_back(bro);
        root->n = 2;

        p->parent = root;
        bro->parent = root;
    }
    else
    {
        // 将中间元素提到父节点
        bro->parent = p->parent;
        size_t i = 0;
        while (p->keys[t] > p->parent->keys[i])
            i++;
        auto kit = p->parent->keys.begin();
        p->parent->keys.insert(kit + i, p->keys[t]);
        auto cit = p->parent->child.begin();
        p->parent->child.insert(cit + i, p);
        p->parent->child[i + 1] = bro;
        p->parent->n++;
    }

    // 分裂叶子节点
    if (p->isleaf)
    {
        bro->next = p->next;
        p->next = bro;
        for (size_t i = 0; i < (_m + 1) / 2; i++)
        {
            bro->keys.push_back(p->keys[t + i + 1]);
            bro->values.push_back(p->values[t + i + 1]);
            bro->n++;
        }
        for (size_t i = 0; i < (_m + 1) / 2; i++)
        {
            p->keys.pop_back();
            p->values.pop_back();
            p->n--;
        }
    }
    // 分裂分支节点
    else
    {
        for (int i = 0; i < (_m + 1) / 2; i++)
        {
            bro->keys.push_back(p->keys[t + i + 1]);
            bro->child.push_back(p->child[t + i + 1]);
            bro->n++;
        }
        for (int i = 0; i < (_m + 1) / 2; i++)
        {
            p->keys.pop_back();
            p->child.pop_back();
            p->n--;
        }
    }
    // 分裂后父节点满，继续分裂
    if (p->parent->n == _m + 1)
        split(p->parent);
}

// 在B+树中根据关键字查询键值对
string BPlusTree::search(string key)
{
    return search(root, key);
}

// 在节点中根据关键字查询键值对
string BPlusTree::search(Node *node, string key)
{
    size_t i = 0;
    while (i < node->keys.size() && key > node->keys[i])
        i++;
    if (i == node->keys.size())
        return nullptr; // 关键字超出节点范围
    if (!node->isleaf)
        return search(node->child[i], key); // 从孩子节点递归查找
    else if (node->keys[i] != key)
        return nullptr; // 未找到键值对
    else
        return node->values[i]; // 找到键值对
}