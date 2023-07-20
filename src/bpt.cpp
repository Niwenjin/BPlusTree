#include "bpt.h"
#include "node.h"
using namespace std;

// 在B+树中插入键值对
void BPlusTree::insert(int key, string value)
{
    if (root == nullptr)
    {
        // 根节点为空，创建根节点
        root = new Node(true);
        root->keys.push_back(key);
        root->values.push_back(value);
    }
    else
    {
        if (root->keys.size() == _m)
        {
            // 根节点已满，需要分裂
            branchNode *newRoot = new branchNode();
            newRoot->child.push_back(root);
            splitChild(newRoot, 0, root);
            root = newRoot;
        }
        insertNonFull(root, key);
    }
}

// 在非满节点中插入键值对
void BPlusTree::insertNonFull(BPlusTreeNode *node, int key)
{
    int i = node->keys.size() - 1;
    if (node->isleaf)
    {
        // 叶子节点，直接插入到正确位置
        while (i >= 0 && key < node->keys[i])
        {
            node->keys[i + 1] = node->keys[i];
            i--;
        }
        node->keys[i + 1] = key;
    }
    else
    {
        // 非叶子节点，向下递归找到插入位置
        while (i >= 0 && key < node->keys[i])
        {
            i--;
        }
        i++;
        if (node->child[i]->keys.size() == _m - 1)
        {
            // 子节点已满，需要分裂
            splitChild(node, i, node->child[i]);
            if (key > node->keys[i])
            {
                i++;
            }
        }
        insertNonFull(node->child[i], key);
    }
}

// 分裂节点
void BPlusTree::splitChild(BPlusTreeNode *parentNode, int childIndex, BPlusTreeNode *childNode)
{
    BPlusTreeNode *newChild = new BPlusTreeNode(childNode->isleaf);

    // 将childNode的右半部分键值对移动到newChild
    for (int i = _m / 2; i < childNode->keys.size(); i++)
    {
        newChild->keys.push_back(childNode->keys[i]);
    }

    // 如果childNode是非叶子节点，则也需要移动相应的子节点指针
    if (!childNode->isleaf)
    {
        for (int i = _m / 2; i < childNode->child.size(); i++)
        {
            newChild->child.push_back(childNode->child[i]);
        }
    }

    // 调整childNode的键值对数量
    childNode->keys.resize(_m / 2);

    // 在parentNode中插入newChild的键和指针
    parentNode->keys.insert(parentNode->keys.begin() + childIndex, childNode->keys[ORDER / 2 - 1]);
    parentNode->child.insert(parentNode->child.begin() + childIndex + 1, newChild);
}

// 在B+树中删除键值对
void BPlusTree::remove(int key)
{
    if (root == nullptr)
    {
        return;
    }

    remove(root, key);

    // 如果根节点只有一个子节点，则将根节点替换为该子节点
    if (root->keys.empty() && !root->isleaf)
    {
        BPlusTreeNode *newRoot = root->child[0];
        delete root;
        root = newRoot;
    }
}

// 在节点中删除键值对
void BPlusTree::remove(BPlusTreeNode *node, int key)
{
    int i = 0;
    while (i < node->keys.size() && key > node->keys[i])
    {
        i++;
    }

    if (i < node->keys.size() && key == node->keys[i])
    {
        // 键值对在当前节点中
        if (node->isleaf)
        {
            // 叶子节点，直接删除
            node->keys.erase(node->keys.begin() + i);
        }
        else
        {
            // 非叶子节点，将后继键值对替换当前键值对，然后递归删除后继节点中的后继键值对
            BPlusTreeNode *successor = getSuccessor(node, i);
            int nextKey = successor->keys[0];
            remove(successor, nextKey);
            node->keys[i] = nextKey;
        }
    }
    else
    {
        if (node->isleaf)
        {
            // 键值对不在当前节点中，且当前节点为叶子节点，不做任何操作
            return;
        }

        // 键值对不在当前节点中，向下递归到合适的子节点
        BPlusTreeNode *child = node->child[i];
        if (child->keys.size() == _m / 2 - 1)
        {
            // 子节点的键值对数量不足，需要借或合并
            if (i > 0 && node->child[i - 1]->keys.size() >= ORDER / 2)
            {
                // 从左兄弟节点借一个键值对
                borrowFromLeftSibling(node, i);
            }
            else if (i < node->keys.size() && node->child[i + 1]->keys.size() >= ORDER / 2)
            {
                // 从右兄弟节点借一个键值对
                borrowFromRightSibling(node, i);
            }
            else
            {
                // 与左或右兄弟节点合并
                if (i > 0)
                {
                    mergeWithLeftSibling(node, i);
                    child = node->child[i - 1];
                }
                else
                {
                    mergeWithRightSibling(node, i);
                }
            }
        }
        remove(child, key);
    }
}

// 从左兄弟节点借一个键值对
void BPlusTree::borrowFromLeftSibling(BPlusTreeNode *parentNode, int childIndex)
{
    BPlusTreeNode *child = parentNode->child[childIndex];
    BPlusTreeNode *leftSibling = parentNode->child[childIndex - 1];

    // 将父节点的键值对插入child的首位置
    child->keys.insert(child->keys.begin(), parentNode->keys[childIndex - 1]);

    // 将左兄弟节点的最后一个键值对移动到父节点
    parentNode->keys[childIndex - 1] = leftSibling->keys.back();
    leftSibling->keys.pop_back();

    // 如果child是非叶子节点，则还需要移动相应的子节点指针
    if (!child->isleaf)
    {
        child->child.insert(child->child.begin(), leftSibling->children.back());
        leftSibling->children.pop_back();
    }
}

// 从右兄弟节点借一个键值对
void BPlusTree::borrowFromRightSibling(BPlusTreeNode *parentNode, int childIndex)
{
    BPlusTreeNode *child = parentNode->children[childIndex];
    BPlusTreeNode *rightSibling = parentNode->children[childIndex + 1];

    // 将父节点的键值对插入child的末尾位置
    child->keys.push_back(parentNode->keys[childIndex]);

    // 将右兄弟节点的第一个键值对移动到父节点
    parentNode->keys[childIndex] = rightSibling->keys.front();
    rightSibling->keys.erase(rightSibling->keys.begin());

    // 如果child是非叶子节点，则还需要移动相应的子节点指针
    if (!child->isLeaf)
    {
        child->children.push_back(rightSibling->children.front());
        rightSibling->children.erase(rightSibling->children.begin());
    }
}

// 与左兄弟节点合并
void BPlusTree::mergeWithLeftSibling(BPlusTreeNode *parentNode, int childIndex)
{
    BPlusTreeNode *child = parentNode->child[childIndex];
    BPlusTreeNode *leftSibling = parentNode->child[childIndex - 1];

    // 将父节点的键值对插入左兄弟节点的末尾位置
    leftSibling->keys.push_back(parentNode->keys[childIndex - 1]);

    // 将child的键值对移动到左兄弟节点的末尾位置
    leftSibling->keys.insert(leftSibling->keys.end(), child->keys.begin(), child->keys.end());
    child->keys.clear();

    // 如果child是非叶子节点，则还需要移动相应的子节点指针
    if (!child->isleaf)
    {
        leftSibling->child.insert(leftSibling->child.end(), child->children.begin(), child->children.end());
        child->child.clear();
    }

    // 删除父节点的键值对和子节点指针
    parentNode->keys.erase(parentNode->keys.begin() + childIndex - 1);
    parentNode->child.erase(parentNode->child.begin() + childIndex);
    delete child;
}

// 与右兄弟节点合并
void BPlusTree::mergeWithRightSibling(BPlusTreeNode *parentNode, int childIndex)
{
    BPlusTreeNode *child = parentNode->child[childIndex];
    BPlusTreeNode *rightSibling = parentNode->child[childIndex + 1];

    // 将父节点的键值对插入child的末尾位置
    child->keys.push_back(parentNode->keys[childIndex]);

    // 将右兄弟节点的键值对移动到child的末尾位置
    child->keys.insert(child->keys.end(), rightSibling->keys.begin(), rightSibling->keys.end());
    rightSibling->keys.clear();

    // 如果child是非叶子节点，则还需要移动相应的子节点指针
    if (!child->isleaf)
    {
        child->child.insert(child->child.end(), rightSibling->child.begin(), rightSibling->child.end());
        rightSibling->child.clear();
    }

    // 删除父节点的键值对和子节点指针
    parentNode->keys.erase(parentNode->keys.begin() + childIndex);
    parentNode->child.erase(parentNode->child.begin() + childIndex + 1);
    delete rightSibling;
}

// 在B+树中查询关键字
bool BPlusTree::search(int key)
{
    return search(root, key);
}

// 在节点中查询键值对
string *BPlusTree::search(Node *node, int key)
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
        return &node->value[i]; // 找到键值对
}