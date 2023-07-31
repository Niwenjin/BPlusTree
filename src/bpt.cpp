#include "bpt.h"
#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

// 在B+树中插入键值对
void BPlusTree::insert(string key, string value)
{
    if (search(key) != nullptr)
    {
        cout << "Key: " << key << "exists" << endl;
        return;
    }
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
    p->keys.insert(p->keys.begin() + i, key);
    p->values.insert(p->values.begin() + i, value);
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
        p->parent->keys.insert(p->parent->keys.begin() + i, p->keys[t]);
        p->parent->child.insert(p->parent->child.begin() + i, p);
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

// 根据关键字删除键值对
bool BPlusTree::remove(string key)
{
    Node *p = search(root, key);
    if (p == nullptr)
        return false; // 目标关键字不存在
    size_t i = 0;
    while (i < p->n && key > p->keys[i])
        i++;
    if (i == p->n || p->keys[i] != key)
        return false; // 目标关键字不存在
    // 删除的是节点中最大值
    if (i == p->n - 1)
        update(p->parent, key, p->keys[i - 1]);
    // 删除键值对
    p->keys.erase(p->keys.begin() + i);
    p->values.erase(p->values.begin() + i);
    p->n--;
    size_t t = (_m + 1) / 2;
    // 节点中关键字数量不足
    if (p->n < t)
    {
        if (p != root)
        {
            // 寻找兄弟
            i = 0;
            while (p->parent->keys[i] < p->keys[p->n - 1])
                i++;
            Node *bro;
            if (i == 0)
            {
                bro = p->parent->child[i + 1];
                if (bro->n > t)
                    borrow(p, bro, true, false);
                else
                    merge(p, bro, true, false);
            }
            else
            {
                bro = p->parent->child[i - 1];
                if (bro->n > t)
                    borrow(p, bro, true, true); // 左兄弟有多余
                else if (i != p->parent->n - 1 && p->parent->child[i + 1]->n > t)
                    borrow(p, p->parent->child[i + 1], true, false); // 右兄弟有多余
                else
                    merge(p, bro, true, true);
            }
        }
        else if (p->n == 0)
        {
            free(p);
            root = nullptr;
            head = nullptr;
        }
    }
    return true;
}

// 向兄弟节点借键值对
void BPlusTree::borrow(Node *p, Node *bro, bool leaf, bool flag)
{
    // 为叶子节点
    if (leaf)
    {
        if (flag)
        {
            p->keys.insert(p->keys.begin(), bro->keys[bro->n - 1]);
            p->values.insert(p->values.begin(), bro->values[bro->n - 1]);
            update(p->parent, bro->keys[bro->n - 1], bro->keys[bro->n - 2]);
            bro->keys.pop_back();
            bro->values.pop_back();
        }
        else
        {
            p->keys.push_back(bro->keys[0]);
            p->values.push_back(bro->values[0]);
            update(p->parent, p->keys[p->n - 1], p->keys[p->n]);
            bro->keys.erase(bro->keys.begin());
            bro->values.erase(bro->values.begin());
        }
    }
    // 为分支节点
    else
    {
        if (flag)
        {
            p->keys.insert(p->keys.begin(), bro->keys[bro->n - 1]);
            p->child.insert(p->child.begin(), bro->child[bro->n - 1]);
            update(p->parent, bro->keys[bro->n - 1], bro->keys[bro->n - 2]);
            bro->keys.pop_back();
            bro->child.pop_back();
        }
        else
        {
            p->keys.push_back(bro->keys[0]);
            p->child.push_back(bro->child[0]);
            update(p->parent, p->keys[p->n - 1], p->keys[p->n]);
            bro->keys.erase(bro->keys.begin());
            bro->child.erase(bro->child.begin());
        }
    }
    p->n++;
    bro->n--;
}

// 从叶子节点向上合并
void BPlusTree::merge(Node *p, Node *bro, bool leaf, bool flag)
{
    Node *parent = p->parent;
    if (parent == nullptr)
        return;
    // 合并叶子节点
    if (leaf)
    {
        if (flag)
        {
            string key = bro->keys[bro->n - 1];
            // 向左合并
            for (int i = 0; i < p->n; i++)
            {
                bro->keys.push_back(p->keys[i]);
                bro->values.push_back(p->values[i]);
                bro->n++;
            }
            size_t i = 0;
            while (parent->keys[i] != key)
                i++;
            parent->keys.erase(parent->keys.begin() + i);
            parent->child.erase(parent->child.begin() + i);
            parent->n--;
            bro->next = p->next;
            free(p);
        }
        else
        {
            string key = p->keys[p->n - 1];
            // 向右合并
            for (int i = 0; i < bro->n; i++)
            {
                p->keys.push_back(bro->keys[i]);
                p->values.push_back(bro->values[i]);
                p->n++;
            }
            size_t i = 0;
            while (parent->keys[i] != key)
                i++;
            parent->keys.erase(parent->keys.begin() + i);
            parent->child.erase(parent->child.begin() + i);
            parent->n--;
            p->next = bro->next;
            free(bro);
        }
    }
    // 合并非叶子节点
    else
    {
        if (flag)
        {
            string key = bro->keys[bro->n - 1];
            // 向左合并
            for (int i = 0; i < p->n; i++)
            {
                bro->keys.push_back(p->keys[i]);
                bro->child.push_back(p->child[i]);
                bro->n++;
            }
            size_t i = 0;
            while (parent->keys[i] != key)
                i++;
            parent->keys.erase(parent->keys.begin() + i);
            parent->child.erase(parent->child.begin() + i);
            parent->n--;
            bro->next = p->next;
            free(p);
        }
        else
        {
            string key = p->keys[p->n - 1];
            // 向右合并
            for (int i = 0; i < bro->n; i++)
            {
                p->keys.push_back(bro->keys[i]);
                p->child.push_back(bro->child[i]);
                p->n++;
            }
            size_t i = 0;
            while (parent->keys[i] != key)
                i++;
            parent->keys.erase(parent->keys.begin() + i);
            parent->child.erase(parent->child.begin() + i);
            parent->n--;
            p->next = bro->next;
            free(bro);
        }
    }
    size_t t = (_m + 1) / 2;
    // 父节点元素不足
    if (parent->n < t)
    {
        // 不为根节点
        if (parent != root)
        {
            string key = parent->keys[parent->n - 1];
            size_t i = 0;
            while (parent->parent->keys[i] != key)
                i++;
            // 有左右兄弟
            if (i != 0 && i != parent->parent->n - 1)
            {
                if (parent->parent->child[i - 1]->n > t)
                    borrow(parent, parent->parent->child[i - 1], false, true);
                else if (parent->parent->child[i + 1]->n > t)
                    borrow(parent, parent->parent->child[i + 1], false, false);
                else
                    merge(parent, parent->parent->child[i - 1], false, true);
            }
            // 只有左兄弟
            else if (i != 0)
            {
                if (parent->parent->child[i - 1]->n > t)
                    borrow(parent, parent->parent->child[i - 1], false, true);
                else
                    merge(parent, parent->parent->child[i - 1], false, true);
            }
            // 只有右兄弟
            else
            {
                if (parent->parent->child[i + 1]->n > t)
                    borrow(parent, parent->parent->child[i + 1], false, false);
                else
                    merge(parent, parent->parent->child[i + 1], false, false);
            }
        }
        // 根节点只有一个元素
        else if (parent->n == 1)
        {
            root = parent->child[0];
            root->parent = nullptr;
            free(parent);
        }
    }
}

// 从分支节点向上更新关键字
void BPlusTree::update(Node *p, string key, string newkey)
{
    while (p)
    {
        size_t i = 0;
        while (i < p->n && key > p->keys[i])
            i++;
        if (i == p->n || p->keys[i] != key)
            return;
        p->keys[i] = newkey;
        p = p->parent;
    }
}

// 查询键值对所在的叶子节点
Node *BPlusTree::search(Node *root, string key)
{
    Node *p = root;
    if (!p)
        return nullptr;
    while (!p->isleaf)
    {
        size_t i = 0;
        while (i < p->n && key > p->keys[i])
            i++;
        if (i == p->n)
            return nullptr; // 关键字超出节点范围
        else
            p = p->child[i];
    }
    return p;
}

// 在叶子节点中查询键值对
string *BPlusTree::search(string key)
{
    Node *p = search(root, key);
    if (p == nullptr)
        return nullptr;
    size_t i = 0;
    while (i < p->n && key > p->keys[i])
        i++;
    if (p->keys[i] != key)
        return nullptr; // 未找到键值对
    else
        return &p->values[i]; // 找到键值对
}

// 更新键值对
bool BPlusTree::change(string key, string value)
{
    Node *p = search(root, key);
    if (p == nullptr)
        return false;
    size_t i = 0;
    while (i < p->n && key > p->keys[i])
        i++;
    if (p->keys[i] != key)
        return false; // 未找到键值对
    else
        p->values[i] = value; // 找到键值对
    return true;
}

// 遍历打印所有键值对
void BPlusTree::show()
{
    Node *p = head;
    if (p)
        cout << "--------" << endl;
    else
        cout << "No data" << endl;
    while (p)
    {
        for (int i = 0; i < p->n; i++)
            cout << p->keys[i] << " = " << p->values[i] << endl;
        cout << "--------" << endl;
        p = p->next;
    }
}

// 从文件读取键值对
void BPlusTree::read(const string &filename)
{
    ifstream FILE(filename);
    if (!FILE)
    {
        cerr << "Fail to open " << filename << endl;
        return;
    }
    string line;
    while (getline(FILE, line))
    {
        size_t pos = line.find('=');
        if (pos != string::npos)
        {
            string key = line.substr(0, pos - 1);
            string value = line.substr(pos + 2);
            insert(key, value);
        }
    }
    FILE.close();
}

// 存储键值对到文件
void BPlusTree::save(const string &filename)
{
    ofstream FILE(filename);
    Node *p = head;
    while (p)
    {
        for (int i = 0; i < p->n; i++)
            FILE << p->keys[i] << " = " << p->values[i] << endl;
        p = p->next;
    }
    FILE.close();
}