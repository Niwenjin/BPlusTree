#include "bpt.h"
#include <iostream>
#include <string>
#include <time.h>
#include <readline/readline.h>
#include <readline/history.h>
using namespace std;

void help();
void readfile(BPlusTree &);
void serialize(BPlusTree &);
void insert(BPlusTree &);
void remove(BPlusTree &);
void search(BPlusTree &);
void change(BPlusTree &);
void searchall(BPlusTree &);

int main(int argc, char **argv)
{
    if (argc != 2 && argc != 3)
    {
        cout << "Please input m of B+tree." << endl;
        return -1;
    }
    int m = stoi(argv[1]);
    BPlusTree bpt(m);
    if (argc == 3)
    {
        string filename = argv[2];
        clock_t start, end;
        start = clock();
        bpt.deserialize(filename);
        end = clock();
        cout << "time: " << end - start << "μs" << endl;
    }
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
            serialize(bpt);
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
        case 't':
            bpt.show();
            break;
        case 'p':
            bpt.printall();
            break;
        case 'x':
            searchall(bpt);
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
    cout << "r--read key-value from file" << endl;
    cout << "s--serialize to file" << endl;
    cout << "i--insert" << endl;
    cout << "d--remove" << endl;
    cout << "f--search" << endl;
    cout << "c--change" << endl;
    cout << "t--print tree" << endl;
    cout << "p--print key-values" << endl;
    cout << "q--quit" << endl;
}

// 从文件读取键值对
void readfile(BPlusTree &bpt)
{
    cout << "Read from: " << endl;
    string filename;
    cin >> filename;
    clock_t start, end;
    start = clock();
    bpt.read(filename);
    end = clock();
    cout << "time: " << end - start << "μs" << endl;
}

// 序列化b+树到文件
void serialize(BPlusTree &bpt)
{
    string filename = readline("Save to: ");
    clock_t start, end;
    start = clock();
    bpt.serialize(filename);
    end = clock();
    cout << "time: " << end - start << "μs" << endl;
}

// 从命令行插入键值对
void insert(BPlusTree &bpt)
{
    string key = readline("Key: ");
    string value = readline("Value: ");
    clock_t start, end;
    start = clock();
    bpt.insert(key, value);
    end = clock();
    cout << "time: " << end - start << "μs" << endl;
}

// 删除键值对
void remove(BPlusTree &bpt)
{
    string key = readline("Key: ");
    clock_t start, end;
    start = clock();
    if (!bpt.remove(key))
        cerr << "key: " << key << " not exist!" << endl;
    else
        cout << "remove key: " << key << " successfully!" << endl;
    end = clock();
    cout << "time: " << end - start << "μs" << endl;
}

// 查询关键字对应的值
void search(BPlusTree &bpt)
{
    string key = readline("Key: ");
    clock_t start, end;
    start = clock();
    string *ans = bpt.search(key);
    if (ans == nullptr)
        cerr << "key: " << key << " not exist!" << endl;
    else
        cout << "Value of " << key << ": " << *ans << endl;
    end = clock();
    cout << "time: " << end - start << "μs" << endl;
}

// 将关键字对应的值更新
void change(BPlusTree &bpt)
{
    string key = readline("Key: ");
    string value = readline("Value: ");
    clock_t start, end;
    start = clock();
    if (!bpt.change(key, value))
        cerr << "key: " << key << " not exist!" << endl;
    else
        cout << "change success!" << endl;
    end = clock();
    cout << "time: " << end - start << "μs" << endl;
}

// 测试查询全部节点的平均时间
void searchall(BPlusTree &bpt)
{
    const int MAX = 10000000;
    clock_t start, end;
    start = clock();
    for (int i = 0; i < MAX; i++)
        bpt.search(to_string(i));
    end = clock();
    cout << "time: " << end - start << "μs" << endl;
    // cout << "avg time: " << (end - start) / MAX << "μs" << endl;
}