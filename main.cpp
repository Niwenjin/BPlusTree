#include <iostream>
#include "bpt.h"

using namespace std;

int main() {
    BPlusTree bPlusTree;

    // 插入键值对
    bPlusTree.insert(10);
    bPlusTree.insert(20);
    bPlusTree.insert(30);
    bPlusTree.insert(40);
    bPlusTree.insert(50);

    // 查询键值对
    cout << "Search 30: " << (bPlusTree.search(30) ? "Found" : "Not Found") << std::endl;
    cout << "Search 60: " << (bPlusTree.search(60) ? "Found" : "Not Found") << std::endl;

    // 删除键值对
    bPlusTree.remove(30);

    // 查询键值对
    cout << "Search 30: " << (bPlusTree.search(30) ? "Found" : "Not Found") << std::endl;

    return 0;
}