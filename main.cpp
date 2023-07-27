#include "bpt.h"
#include <iostream>
#include <string>
#include <readline/readline.h>
#include <readline/history.h>
using namespace std;

void help();
void readfile(BPlusTree &);
void save(BPlusTree &);
void insert(BPlusTree &);
void remove(BPlusTree &);
void search(BPlusTree &);
void change(BPlusTree &);

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        cout << "Please input m of B+tree." << endl;
        return -1;
    }
    int m = stoi(argv[1]);
    BPlusTree bpt(m);
    char opt;
    cout << "Input a choice: " << endl;
    while (cin >> opt)
    {
        switch (opt)
        {
        case 'h':
            help();
            break;
        case 'r':
            readfile(bpt);
            break;
        case 's':
            save(bpt);
            break;
        case 'i':
            insert(bpt);
            break;
        case 'd':
            remove(bpt);
            break;
        case 'f':
            search(bpt);
            break;
        case 'c':
            change(bpt);
            break;
        case 'p':
            bpt.show();
            break;
        case 'q':
            return 0;
            break;
        default:
            cout << "Invaild choice." << endl;
            break;
        }
        cout << "Input a choice: " << endl;
    }
    return -1;
}

// 打印帮助文本
void help()
{
    cout << "h--help" << endl;
    cout << "r--read from file" << endl;
    cout << "s--save to file" << endl;
    cout << "i--insert" << endl;
    cout << "d--remove" << endl;
    cout << "f--search" << endl;
    cout << "c--change" << endl;
    cout << "p--print all" << endl;
    cout << "q--quit" << endl;
}

// 从文件读取键值对
void readfile(BPlusTree &bpt)
{
    string filename = readline("Read from: ");
    bpt.read(filename);
}

// 保存键值对到文件
void save(BPlusTree &bpt)
{
    string filename = readline("Save to: ");
    bpt.save(filename);
}

// 从命令行插入键值对
void insert(BPlusTree &bpt)
{
    string key = readline("Key: ");
    string value = readline("Value: ");
    bpt.insert(key, value);
}

// 删除键值对
void remove(BPlusTree &bpt)
{
    string key = readline("Key: ");
    if (!bpt.remove(key))
        cerr << "key: " << key << " not exist!" << endl;
    else
        cout << "remove key: " << key << " successfully!" << endl;
}

// 查询关键字对应的值
void search(BPlusTree &bpt)
{
    string key = readline("Key: ");
    string *ans = bpt.search(key);
    if (ans == nullptr)
        cerr << "key: " << key << " not exist!" << endl;
    else
        cout << "Value of " << key << ": " << *ans << endl;
}

// 将关键字对应的值更新
void change(BPlusTree &bpt)
{
    string key = readline("Key: ");
    string value = readline("Value: ");
    if (!bpt.change(key, value))
        cerr << "key: " << key << " not exist!" << endl;
    else
        cout << "change success!" << endl;
}