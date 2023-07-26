#include <iostream>
#include <unordered_map>
#include <string>
#include <fstream>
#include <readline/readline.h>
#include <readline/history.h>
#include <getopt.h>

using namespace std;

unordered_map<string, string> keyValues;

// 打印帮助信息
void printhelp()
{
    cout << "KEY_VALUE" << endl;
    cout << "Usage:program [options]" << endl;
    cout << "options:" << endl;
    cout << "  -a[key=value]    Add key_value pair" << endl;
    cout << "  -q[key]          Query value for a key" << endl;
    cout << "  -f file          Read key_value pair from file" << endl;
    cout << "  -d key           Delete a key_value pair" << endl;
    cout << "  -h               Print help massage" << endl;
}

// 从文件读取键值对
void readfromFile(const string &filename)
{
    ifstream file;
    file.open("../"+filename);
    if (!file)
    {
        cerr << "Fail to open " << filename << endl;
        return;
    }
    string line;
    while (getline(file, line))
    {
        size_t pos = line.find('=');
        if (pos != string::npos)
        {
            string key = line.substr(0, pos);
            string value = line.substr(pos + 1);
            keyValues[key] = value;
        }
    }
    file.close();
}

// 交互式添加键值对
void addwithReadline()
{
    string key, value;
    char *input = readline("Key: ");
    if (input)
    {
        key = input;
        add_history(input);
        free(input);
    }
    input = readline("Value: ");
    if (input)
    {
        value = input;
        add_history(input);
        free(input);
    }
    keyValues[key] = value;
}

// 命令行参数添加键值对
void addwithParm(const string &str)
{
    size_t pos = str.find('=');
    if (pos != string::npos)
    {
        string key, value;
        key = str.substr(0, pos);
        value = str.substr(pos + 1);
        keyValues[key] = value;
    }
    else
        cout << "illegal input!" << endl;
}

// 交互式查询键值对
void querywithReadline()
{
    string key;
    char *input = readline("Key: ");
    if (input)
    {
        key = input;
        add_history(input);
        free(input);
    }
    auto it = keyValues.find(key);
    if (it != keyValues.end())
        cout << "Value of " << key << ": " << it->second << endl;
    else
        cout << "Not found value of " << key << endl;
}

// 命令行参数查询键值对
void querywithParm(const string &key)
{
    auto it = keyValues.find(key);
    if (it != keyValues.end())
        cout << "Value of " << key << ": " << it->second << endl;
    else
        cout << "Not found value of " << key << endl;
}

// 删除键值对
void deleteKey(const string &key)
{
    auto n = keyValues.erase(key);
    if (n)
        cout << "delete key: " << key << " successfully" << endl;
    else
        cout << "Key: " << key << " not exist" << endl;
}

// 存储键值对到文件
void savetoFile()
{
    ofstream file("../backup.txt");
    if (!file)
    {
        std::cerr << "Failed to open file: backup.txt" << endl;
        return;
    }
    for (const auto &kv : keyValues)
    {
        file << kv.first << "=" << kv.second << endl;
    }
    file.close();
}

int main(int argc, char **argv)
{
    const char *options = "a::q::f:d:h";
    string filename;
    int opt;
    readfromFile("backup.txt");
    while ((opt = getopt(argc, argv, options)) != -1)
    {
        switch (opt)
        {
        case 'a':
            if (optarg)
                addwithParm(optarg);
            else
                addwithReadline();
            break;
        case 'q':
            if (optarg)
                querywithParm(optarg);
            else
                querywithReadline();
            break;
        case 'f':
            readfromFile(optarg);
            break;
        case 'd':
            deleteKey(optarg);
            break;
        case 'h':
            printhelp();
            break;
        default:
            cout << "use option -h to get help" << endl;
            return 1;
        }
    }
    savetoFile();
    return 0;
}