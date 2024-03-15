#include <iostream>
#include <cstdlib>
#include <filesystem>
#include "Functions.h"
#include "HuffmanNode.h"
#include "HuffmanTree.h"

namespace fs = std::filesystem;
using namespace std;

// 获取文件中字符的频率，将其存储在一个优先队列中并返回
priority_queue<HuffmanNode, vector<HuffmanNode>, greater<HuffmanNode>>
GetChFreq(const fs::path &file_path, long *file_size)
{
    // 以二进制只读方式打开文件
    ifstream input;
    input.open(file_path, ios::in | ios::binary);

    long array[MaxCharNum] = {0}; // 存放字符频率的数组，包含频率为0的字符
    long size = 0;                // 文件字节数大小
    unsigned char ch;             // 临时变量
    HuffmanNode temp;             // 临时变量

    // 函数最终返回的，存放字符频率的优先队列
    priority_queue<HuffmanNode, vector<HuffmanNode>, greater<HuffmanNode>> p_queue;

    // 循环读取文件中的字符并统计频率
    while (1)
    {
        input.read((char *)&ch, sizeof(ch));
        if (input.eof())
        {
            break;
        }
        array[ch]++;
        size++;
    }

    *file_size = size; // 文件字节数

    // 将字符频率数组转化为优先队列以方便之后构造哈夫曼树
    for (unsigned int i = 0; i < MaxCharNum; i++)
    {
        if (array[i] != 0)
        {
            temp.ch = i;
            temp.num = array[i];
            temp.Isleaf = true;
            temp.Lnode = nullptr;
            temp.Rnode = nullptr;
            p_queue.push(temp);
        }
    }

    // 关闭文件
    input.close();
    return p_queue;
}

// 文件压缩操作
void FileCompress(const fs::path &file_path, const fs::path &zip_path)
{
    // 文件大小
    long file_size = 0;

    // 获取优先队列，文件大小，哈夫曼树以及哈夫曼编码
    priority_queue<HuffmanNode, vector<HuffmanNode>, greater<HuffmanNode>> queue = GetChFreq(file_path, &file_size);
    priority_queue<HuffmanNode, vector<HuffmanNode>, greater<HuffmanNode>> save_code = queue;
    HuffmanTree tree = HuffmanTree(queue);
    vector<HuffmanCode> huffman_code(MaxCharNum);
    huffman_code = tree.GetHuffmanCode();

    // 以二进制方式打开文件
    ifstream input;
    ofstream output;
    input.open(file_path, ios::in | ios::binary);
    output.open(zip_path, ios::out | ios::binary);

    // 先后写入文件大小、文件名字符串长度、文件名字符串
    string file_str_name = file_path.filename().string();
    long str_bytes = file_str_name.size();
    output.write((char *)&file_size, sizeof(file_size));
    output.write((char *)&str_bytes, sizeof(str_bytes));
    output << file_str_name;

    // 先后写入哈夫曼叶节点数、哈夫曼叶节点
    HuffmanNode tempnode;
    long code_bytes = save_code.size();
    output.write((char *)&code_bytes, sizeof(code_bytes));
    while (!save_code.empty())
    {
        tempnode = save_code.top();
        save_code.pop();
        output.write((char *)&tempnode.ch, sizeof(tempnode.ch));
        output.write((char *)&tempnode.num, sizeof(tempnode.num));
    }

    // 临时变量
    unsigned char ch = 0;     // 存放读取的字符
    unsigned char bit_ch = 0; // 存放将要以字符形式写入的8位哈夫曼编码
    int bit_count = 0;        // 记录bit_ch是否满8位

    // 循环读取文件字符并将其按哈夫曼编码转换并写入压缩文件
    while (1)
    {
        // 循环读取字符，直至文件末尾
        input.read((char *)&ch, sizeof(ch));
        if (input.eof())
        {
            break;
        }

        // 引用该字符对应的哈夫曼编码
        string &code = huffman_code[ch].code;
        // 进行循环，每次写入1位，并存放在bit_ch中，满8位写入文件
        for (int i = 0; i < code.size(); i++)
        {
            bit_ch <<= 1;
            if (code[i] == '1')
            {
                bit_ch |= 1;
            }
            bit_count++;
            if (bit_count == 8) // bit_ch满8位写入文件
            {
                output.write((char *)&bit_ch, sizeof(bit_ch));
                bit_count = 0;
            }
        }
    }

    // 处理压缩文件末尾的最后一个字节
    // 若不满8位则剩下的位均填0
    if (bit_count > 0 && bit_count < 8)
    {
        bit_ch <<= (8 - bit_count);
        output.write((char *)&bit_ch, sizeof(bit_ch));
    }

    // 关闭文件
    input.close();
    output.close();
}

// 文件解压缩操作
void FileUncompress(const fs::path &zip_path, const fs::path &folder_path)
{
    // 以二进制形式打开文件
    ifstream input;
    ofstream output;
    input.open(zip_path, ios::in | ios::binary);

    // 读取文件大小、文件名字符串长度、文件名字符串
    long file_size = 0;
    long str_bytes;
    string file_name = "";
    unsigned char file_ch = 0;
    input.read((char *)&file_size, sizeof(file_size));
    input.read((char *)&str_bytes, sizeof(str_bytes));
    for (int i = 0; i < str_bytes; i++)
    {
        input.read((char *)&file_ch, sizeof(file_ch));
        file_name += file_ch;
    }

    // 通过上面读取的文件名构造路径并打开文件
    fs::path file_path = folder_path / file_name;
    output.open(file_path, ios::out | ios::binary);

    // 变量定义
    long code_bytes;              // 哈夫曼编码组数
    long array[MaxCharNum] = {0}; // 存放字符频率的数组
    unsigned char ch = 0;         // 临时变量，存放字符
    long num = 0;                 // 临时变量，存放字符频率
    HuffmanNode temp;             // 临时变量，存放哈夫曼节点

    priority_queue<HuffmanNode, vector<HuffmanNode>, greater<HuffmanNode>> p_queue; // 存放字符频率的优先队列

    // 循环读取哈夫曼叶节点并构造优先队列
    input.read((char *)&code_bytes, sizeof(code_bytes));
    for (long i = 0; i < code_bytes; i++)
    {
        input.read((char *)&ch, sizeof(ch));
        input.read((char *)&num, sizeof(num));
        array[ch] = num;
    }
    for (unsigned int i = 0; i < MaxCharNum; i++)
    {
        if (array[i] != 0)
        {
            temp.ch = i;
            temp.num = array[i];
            temp.Isleaf = true;
            temp.Lnode = nullptr;
            temp.Rnode = nullptr;
            p_queue.push(temp);
        }
    }

    // 构造哈夫曼树
    HuffmanTree tree = HuffmanTree(p_queue);

    // 临时变量
    unsigned char bit_ch = 0;      // 以8位字符形式读取压缩文件中的信息
    unsigned char first_bit = 128; // 检查第一位的值用：0b10000000
    int bit_count = 0;             // 记录bit_ch信息已用的位数
    long bytes_count = 0;          // 记录目前已还原的字节数
    HuffmanNode *find_leaf = tree.GetRoot();

    while (1)
    {
        // 循环读取字符，直至文件末尾
        input.read((char *)&bit_ch, sizeof(bit_ch));
        if (input.eof())
        {
            break;
        }

        // 以8位字符形式读取压缩文件中的信息并还原为原文件的字符并写入
        while (bit_count < 8)
        {
            // 根据哈夫曼编码寻找叶节点
            while ((!(find_leaf->Isleaf)) && (bit_count < 8))
            {
                if ((first_bit & bit_ch) == first_bit) // 若检查位为1则移动到右子节点
                {
                    find_leaf = find_leaf->Rnode;
                    bit_ch <<= 1;
                    bit_count++;
                }
                else // 若检查位为0则移动到左子节点
                {
                    find_leaf = find_leaf->Lnode;
                    bit_ch <<= 1;
                    bit_count++;
                }

                if (find_leaf == nullptr) // 若遍历到了空节点则退出循环
                {
                    break;
                }
            }

            // 若遍历到了空节点或文件已完全还原则退出循环
            if (find_leaf == nullptr || file_size == bytes_count)
            {
                break;
            }

            // 若找到叶节点且文件还原并未完成则写入
            if ((find_leaf->Isleaf) && (file_size != bytes_count))
            {
                output << find_leaf->ch;
                bytes_count++;
                find_leaf = tree.GetRoot();
            }
        }
        bit_count = 0;
    }

    // 关闭文件
    input.close();
    output.close();
}

// 文件夹压缩操作内部的遍历目录递归函数
void inFolderCompressTravelDir(const fs::path &folder_path, ofstream &output)
{
    // 变量定义
    static int file_str_bytes = 0; // 文件名字符串长度
    static string tempstr = "";    // 文件名字符串
    static int folder_tag = 0;     // 是否为文件夹的标志
    static int right = -1;         // 文件夹目录遍历结束的标志

    // 读取文件或文件夹的名称信息并先后写入文件夹标志、文件名字符串长度、文件名字符串
    folder_tag = 1;
    tempstr = folder_path.filename().string();
    file_str_bytes = tempstr.size();
    output.write((char *)&folder_tag, sizeof(folder_tag));
    output.write((char *)&file_str_bytes, sizeof(file_str_bytes));
    output << tempstr;

    for (const fs::directory_entry &entry : fs::directory_iterator(folder_path))
    {
        if (entry.is_directory()) // 若为文件夹则递归遍历
        {
            inFolderCompressTravelDir(entry.path(), output);
        }
        else // 若为文件则写入文件名称信息
        {
            folder_tag = 0;
            tempstr = entry.path().filename().string();
            file_str_bytes = tempstr.size();
            output.write((char *)&folder_tag, sizeof(file_str_bytes));
            output.write((char *)&file_str_bytes, sizeof(file_str_bytes));
            output << tempstr;
        }
    }
    // 写入文件夹目录遍历结束的标志
    output.write((char *)&right, sizeof(right));
}

// 文件夹压缩操作内部的对单个文件压缩的操作
void inFolderCompressFile(const fs::path &file_path, ofstream &output)
{
    // 注释参考void FileCompress(const fs::path &file_path, const fs::path &zip_path);
    // 只有小部分改动

    ifstream input;
    input.open(file_path, ios::in | ios::binary);

    long file_size = 0;

    priority_queue<HuffmanNode, vector<HuffmanNode>, greater<HuffmanNode>> queue = GetChFreq(file_path, &file_size);
    priority_queue<HuffmanNode, vector<HuffmanNode>, greater<HuffmanNode>> save_code = queue;
    HuffmanTree tree = HuffmanTree(queue);
    vector<HuffmanCode> huffman_code(MaxCharNum);
    huffman_code = tree.GetHuffmanCode();

    output.write((char *)&file_size, sizeof(file_size));

    HuffmanNode tempnode;
    long code_bytes = save_code.size();

    output.write((char *)&code_bytes, sizeof(code_bytes));

    while (!save_code.empty())
    {
        tempnode = save_code.top();
        save_code.pop();

        output.write((char *)&tempnode.ch, sizeof(tempnode.ch));
        output.write((char *)&tempnode.num, sizeof(tempnode.num));
    }

    unsigned char ch = 0;
    unsigned char bit_ch = 0;
    int bit_count = 0;
    while (1)
    {
        input.read((char *)&ch, sizeof(ch));
        if (input.eof())
        {
            break;
        }
        string &code = huffman_code[ch].code;
        for (int i = 0; i < code.size(); i++)
        {
            bit_ch <<= 1;
            if (code[i] == '1')
            {
                bit_ch |= 1;
            }
            bit_count++;
            if (bit_count == 8)
            {
                output.write((char *)&bit_ch, sizeof(bit_ch));
                bit_count = 0;
            }
        }
    }
    if (bit_count > 0 && bit_count < 8)
    {
        bit_ch <<= (8 - bit_count);
        output.write((char *)&bit_ch, sizeof(bit_ch));
    }
    input.close();
}

// 文件夹压缩操作的内部递归函数
void inFolderCompressDir(const fs::path &folder_path, ofstream &output)
{
    for (const fs::directory_entry &entry : fs::directory_iterator(folder_path))
    {
        if (entry.is_directory())
        {
            // 若为文件夹则递归遍历
            inFolderCompressDir(entry.path(), output);
        }
        else
        {
            // 若为文件则执行压缩操作
            inFolderCompressFile(entry.path(), output);
        }
    }
}

// 文件夹压缩操作
void FolderCompress(const fs::path &folder_path, const fs::path &zip_path)
{
    // 以二进制形式打开文件
    ofstream output;
    output.open(zip_path, ios::out | ios::binary);

    // 写入其为文件夹的标志
    long FolderTag = -1;
    output.write((char *)&FolderTag, sizeof(FolderTag));

    // 递归遍历目录并写入经序列化的文件夹树形结构
    inFolderCompressTravelDir(folder_path, output);

    // 写入目录遍历结束的标志
    int right = -1;
    output.write((char *)&right, sizeof(right));

    // 递归遍历目录并压缩文件
    inFolderCompressDir(folder_path, output);

    // 关闭文件
    output.close();
}

// 文件夹解压缩操作内部的遍历目录递归函数
void inFolderUncompressTravelDir(ifstream &input, const fs::path &folder_path)
{
    // 注释参考 void inFolderCompressTravelDir(const fs::path &folder_path, ofstream &output);
    // 基本上只是相反的操作
    static int file_str_bytes = 0;
    static unsigned char tempch = 0;
    static int folder_tag = 0;

    input.read((char *)&folder_tag, sizeof(folder_tag));

    while (folder_tag != -1)
    {
        if (folder_tag == 0)
        {
            string file_str;
            input.read((char *)&file_str_bytes, sizeof(file_str_bytes));
            for (int i = 0; i < file_str_bytes; i++)
            {
                input.read((char *)&tempch, sizeof(tempch));
                file_str += tempch;
            }
            fs::path new_file(folder_path / file_str);
            fstream file(new_file, ios::out | ios::trunc);
            file.close();
        }
        else if (folder_tag == 1)
        {
            string folder_str;
            input.read((char *)&file_str_bytes, sizeof(file_str_bytes));
            for (int i = 0; i < file_str_bytes; i++)
            {
                input.read((char *)&tempch, sizeof(tempch));
                folder_str += tempch;
            }
            fs::path new_folder(folder_path / folder_str);
            fs::create_directory(new_folder);
            inFolderUncompressTravelDir(input, new_folder);
        }
        input.read((char *)&folder_tag, sizeof(folder_tag));
    }
}

// 文件夹解压缩操作内部的对单个文件压缩的操作
void inFolderUncompressFile(ifstream &input, const fs::path &file_path)
{
    // 注释参考void FileUncompress(const fs::path &zip_path, const fs::path &folder_path);
    // 只有小部分改动

    ofstream output;
    output.open(file_path, ios::out | ios::binary);

    long file_size = 0;
    input.read((char *)&file_size, sizeof(file_size));

    long code_bytes;
    unsigned char ch = 0;
    long num = 0;
    long array[MaxCharNum] = {0};
    HuffmanNode temp;
    priority_queue<HuffmanNode, vector<HuffmanNode>, greater<HuffmanNode>> p_queue;

    input.read((char *)&code_bytes, sizeof(code_bytes));
    for (long i = 0; i < code_bytes; i++)
    {
        input.read((char *)&ch, sizeof(ch));
        input.read((char *)&num, sizeof(num));
        array[ch] = num;
    }
    for (unsigned int i = 0; i < MaxCharNum; i++)
    {
        if (array[i] != 0)
        {
            temp.ch = i;
            temp.num = array[i];
            temp.Isleaf = true;
            temp.Lnode = nullptr;
            temp.Rnode = nullptr;
            p_queue.push(temp);
        }
    }

    HuffmanTree tree = HuffmanTree(p_queue);
    unsigned char bit_ch = 0;
    unsigned char first_bit = 128; // 0b10000000
    int bit_count = 0;
    long bytes_count = 0;
    HuffmanNode *find_leaf = tree.GetRoot();

    while (file_size != bytes_count)
    {
        input.read((char *)&bit_ch, sizeof(bit_ch));
        while (bit_count < 8)
        {
            while ((!(find_leaf->Isleaf)) && (bit_count < 8))
            {
                if ((first_bit & bit_ch) == first_bit)
                {
                    find_leaf = find_leaf->Rnode;
                    bit_ch <<= 1;
                    bit_count++;
                }
                else
                {
                    find_leaf = find_leaf->Lnode;
                    bit_ch <<= 1;
                    bit_count++;
                }
                if (find_leaf == nullptr)
                {
                    break;
                }
            }
            if (find_leaf == nullptr || file_size == bytes_count)
            {
                break;
            }
            if ((find_leaf->Isleaf) && (file_size != bytes_count))
            {
                output << find_leaf->ch;
                bytes_count++;
                find_leaf = tree.GetRoot();
            }
        }
        bit_count = 0;
    }
    output.close();
}

// 文件夹解压缩操作的内部递归函数
void inFolderUncompressDir(ifstream &input, const fs::path &folder_path)
{
    for (const fs::directory_entry &entry : fs::directory_iterator(folder_path))
    {
        if (entry.is_directory())
        {
            // 若为文件夹则递归遍历
            inFolderUncompressDir(input, entry.path());
        }
        else
        {
            // 若为文件则进行解压
            inFolderUncompressFile(input, entry.path());
        }
    }
}

// 文件夹解压缩操作
void FolderUncompress(const fs::path &zip_path, const fs::path &folder_path)
{
    // 以二进制形式打开文件
    ifstream input;
    input.open(zip_path, ios::in | ios::binary);

    int new_folder_str_bytes; // 存放解压之后的文件夹名字符串长度
    string new_folder = "";   // 存放解压之后的文件夹名称
    unsigned char tempch;     // 临时变量

    // 文件流定位到合适的位置以读取文件夹名称
    input.seekg(sizeof(long) + sizeof(int), ios::beg);

    // 先后读取文件夹名字符串长度、文件夹名字符串并构造文件夹路径
    input.read((char *)&new_folder_str_bytes, sizeof(new_folder_str_bytes));
    for (int i = 0; i < new_folder_str_bytes; i++)
    {
        input.read((char *)&tempch, sizeof(tempch));
        new_folder += tempch;
    }
    fs::path new_folder_path(folder_path / new_folder);

    // 文件流再次定位到合适的位置
    input.seekg(sizeof(long), ios::beg);

    // 读取文件夹树形结构并递归生成文件夹及其内含的文件
    inFolderUncompressTravelDir(input, folder_path);

    // 递归对文件夹内的文件进行解压
    inFolderUncompressDir(input, new_folder_path);

    // 关闭文件
    input.close();
}

// 解压缩，自动识别压缩包为文件还是文件夹
void Uncompress(const fs::path &zip_path, const fs::path &folder_path)
{
    // 识别该压缩包存放的是文件还是文件夹
    ifstream input;
    input.open(zip_path, ios::in | ios::binary);
    long FolderTag = 0;
    input.read((char *)&FolderTag, sizeof(FolderTag));
    input.close();

    if (FolderTag == -1)
    {
        // 若为文件夹
        FolderUncompress(zip_path, folder_path);
    }
    else
    {
        // 若为文件
        FileUncompress(zip_path, folder_path);
    }
}

// 压缩包预览的内部函数
void inFolderPreviewTravel(ifstream &input, string &preview_graph)
{
    static int file_str_bytes = 0;   // 文件名字符串长度
    static unsigned char tempch = 0; // 临时变量
    static int folder_tag = 0;       // 文件夹标志
    static int level = 0;            // 文件在文件夹树形结构中的深度
    static bool first = true;        // 主目录的标志

    // 识别为文件还是文件夹
    input.read((char *)&folder_tag, sizeof(folder_tag));

    // 遍历整个目录直至遇见目录结束标志
    while (folder_tag != -1)
    {
        if (folder_tag == 0) // 若为文件
        {
            // 读取文件名长度及文件名
            string file_str;
            input.read((char *)&file_str_bytes, sizeof(file_str_bytes));
            for (int i = 0; i < file_str_bytes; i++)
            {
                input.read((char *)&tempch, sizeof(tempch));
                file_str += tempch;
            }

            // 添加修饰符
            for (int i = 0; i < level - 1; i++)
            {
                preview_graph += "  |\t";
            }
            preview_graph += "  |----";
            preview_graph += file_str;
            preview_graph += '\n';
        }
        else if (folder_tag == 1) // 若为文件夹
        {
            // 读取文件夹名长度及文件夹名
            string folder_str;
            input.read((char *)&file_str_bytes, sizeof(file_str_bytes));
            for (int i = 0; i < file_str_bytes; i++)
            {
                input.read((char *)&tempch, sizeof(tempch));
                folder_str += tempch;
            }

            // 添加修饰符
            for (int i = 0; i < level - 1; i++)
            {
                preview_graph += "  |";
                preview_graph += '\t';
            }
            if (first)
            {
                first = false;
            }
            else
            {
                preview_graph += "  |----";
            }
            preview_graph += folder_str;
            preview_graph += '\n';

            // 对文件夹内部进行递归
            level++;
            inFolderPreviewTravel(input, preview_graph);
            level--;
        }
        input.read((char *)&folder_tag, sizeof(folder_tag));
    }
}

// 压缩包预览
void zipPreview(const fs::path &zip_path, string &preview_graph)
{
    // 打开文件并识别为文件或文件夹
    ifstream input;
    input.open(zip_path, ios::in | ios::binary);
    long FolderTag = 0;
    input.read((char *)&FolderTag, sizeof(FolderTag));

    if (FolderTag == -1)
    {
        // 若为文件夹则进行递归遍历
        inFolderPreviewTravel(input, preview_graph);
    }
    else
    {
        // 若为文件则直接读取文件名
        string file_name = "";
        unsigned char file_ch = 0;
        long str_bytes;
        input.read((char *)&str_bytes, sizeof(str_bytes));
        for (int i = 0; i < str_bytes; i++)
        {
            input.read((char *)&file_ch, sizeof(file_ch));
            file_name += file_ch;
        }
        preview_graph = file_name;
        preview_graph += '\n';
    }

    // 关闭文件
    input.close();
}

// 获取压缩文件中的原文件名
string GetZipFileName(const fs::path &zip_path)
{
    // 打开文件
    ifstream input;
    input.open(zip_path, ios::in | ios::binary);

    // 识别为文件还是文件夹
    long FolderTag = 0;
    input.read((char *)&FolderTag, sizeof(FolderTag));

    if (FolderTag == -1) // 若为文件夹
    {
        string folder_str = "";
        int file_str_bytes = 0;
        unsigned char tempch = 0;
        input.read((char *)&file_str_bytes, sizeof(file_str_bytes));
        file_str_bytes = 0;
        input.read((char *)&file_str_bytes, sizeof(file_str_bytes));
        for (int i = 0; i < file_str_bytes; i++)
        {
            input.read((char *)&tempch, sizeof(tempch));
            folder_str += tempch;
        }
        input.close();
        return folder_str;
    }
    else // 若为文件
    {
        string file_name = "";
        unsigned char file_ch = 0;
        long str_bytes;
        input.read((char *)&str_bytes, sizeof(str_bytes));
        for (int i = 0; i < str_bytes; i++)
        {
            input.read((char *)&file_ch, sizeof(file_ch));
            file_name += file_ch;
        }
        input.close();
        return file_name;
    }
}

// 帮助界面
void HelpPage()
{
    cout << "---------------------------------Help---------------------------------" << endl;
    cout << "| Read commands.txt for details                                      |" << endl;
    cout << "| Compress:  .\\HuffmanZip_CLI  zip_hby  anyname.hby  file_or_folder  |" << endl;
    cout << "| Uncompress:.\\HuffmanZip_CLI  unzip_hby  zip_file                   |" << endl;
    cout << "| Preview:   .\\HuffmanZip_CLI  prev_hby   zip_file                   |" << endl;
    cout << "|--------------------------------End---------------------------------|" << endl;
}

// 处理压缩命令
void ZipOpt(const fs::path &file_path, const fs::path &zip_path)
{
    // 识别压缩文件后缀是否正确
    string zip_str_name = zip_path.filename().string();
    string post_fix = zip_str_name.substr(zip_str_name.rfind('.'));
    if (post_fix != ".hby")
    {
        cout << post_fix << " is not the right suffix." << endl;
        cout << "The file suffix should be .hby" << endl;
        system("pause");
        exit(1);
    }

    char c_or_q = 'c';

    if (!fs::exists(file_path))
    {
        cout << "There is no file: " << file_path.filename() << endl;
    }
    if (fs::exists(zip_path))
    {
        cout << "There is already a file: " << zip_path.filename() << endl;
        cout << "Press c to cover the old file, any other key to quit" << endl;
        cin >> c_or_q;
    }

    if (c_or_q == 'c')
    {
        fs::directory_entry file_entry(file_path);
        fs::remove_all(zip_path);
        if (file_entry.is_directory())
        {
            cout << "Please wait a moment..." << endl;
            FolderCompress(file_path, zip_path);
            cout << "Finish!" << endl;
        }
        else
        {
            cout << "Please wait a moment..." << endl;
            FileCompress(file_path, zip_path);
            cout << "Finish!" << endl;
        }
    }
    else
    {
        exit(0);
    }
}

// 处理解压命令
void UnzipOpt(const fs::path &zip_path, const fs::path &folder_path)
{
    // 识别压缩文件后缀是否正确
    string zip_str_name = zip_path.filename().string();
    string post_fix = zip_str_name.substr(zip_str_name.rfind('.'));
    if (post_fix != ".hby")
    {
        cout << post_fix << " is not the right suffix." << endl;
        cout << "The file suffix should be .hby" << endl;
        system("pause");
        exit(1);
    }

    if (!fs::exists(zip_path))
    {
        cout << "There is no file: " << zip_path.filename() << endl;
        system("pause");
        exit(1);
    }

    string filename = GetZipFileName(zip_path);
    fs::path file_path = folder_path / filename;

    char c_or_q = 'c';

    if (fs::exists(file_path))
    {
        cout << "There is already a file: " << file_path.filename() << endl;
        cout << "Press c to cover the old file, any other key to quit" << endl;
        cin >> c_or_q;
    }

    if (c_or_q == 'c')
    {
        if (fs::exists(file_path))
        {
            fs::remove_all(file_path);
        }
        cout << "Please wait a moment..." << endl;
        Uncompress(zip_path, folder_path);
        cout << "Finish!" << endl;
    }
    else
    {
        exit(0);
    }
}

// 处理压缩包预览命令
void PrevOpt(const fs::path &zip_path)
{
    // 识别压缩文件后缀是否正确
    string zip_str_name = zip_path.filename().string();
    string post_fix = zip_str_name.substr(zip_str_name.rfind('.'));
    if (post_fix != ".hby")
    {
        cout << post_fix << " is not the right suffix." << endl;
        cout << "The file suffix should be .hby" << endl;
        system("pause");
        exit(1);
    }

    string preview_graph = "";
    if (!fs::exists(zip_path))
    {
        cout << "There is no file: " << zip_path.filename() << endl;
        system("pause");
        exit(1);
    }
    zipPreview(zip_path, preview_graph);
    cout << preview_graph;
    system("pause");
}

// 处理无效命令
void InvalidInputs()
{
    cout << "Invalid inputs, please check your command." << endl;
    exit(1);
}

void initEditBox(sys_edit *editBox, int x, int y, int width, int height)
{
    editBox->create(true);
    editBox->size(width, height);
    editBox->setbgcolor(EGERGB(0xff, 0xff, 0xff));
    editBox->setcolor(EGERGB(0x00, 0x00, 0x00));
    editBox->setfont(20, 0, "黑体");
    editBox->move(x, y);
    editBox->setmaxlen(1024); // 设置允许输入的字符数
    editBox->visible(true);
}

void loadImages(PIMAGE *images, int num)
{
    string image_half_path = "image/rect";
    string image_path;
    for (int i = 0; i < num; i++)
    {
        images[i] = newimage();
        image_path = image_half_path + to_string(i) + ".png";
        const char *path = image_path.data();
        getimage(images[i], path);
    }
}

void releaseImages(PIMAGE *images, int num)
{
    for (int i = 0; i < num; i++)
    {
        delimage(images[i]);
    }
}

void drawMainInterface(PIMAGE *images)
{
    setfont(30, 0, "隶书", 0, 0, 6, false, false, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH);
    setcolor(EGERGB(0x00, 0x00, 0x00));

    outtextxy(30, 30, "本程序基于哈夫曼编码实现对于单个文件以及");
    outtextxy(30, 70, "文件夹的压缩与解压缩，压缩包后缀为.hby。");
    setfont(18, 0, "微软雅黑", 0, 0, FW_DONTCARE, false, false, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH);
    outtextxy(30, 120, "https://github.com/HBY-STAR/Code/tree/master/DataStructure/project/HuffmanZip");

    putimage_withalpha(NULL, images[0], 70, 210);
    putimage_withalpha(NULL, images[1], 370, 210);
    putimage_withalpha(NULL, images[2], 70, 330);
    putimage_withalpha(NULL, images[3], 370, 330);
}

void drawFileCompressInterface(PIMAGE *images)
{
    putimage_withalpha(NULL, images[4], 500, 20);
    putimage_withalpha(NULL, images[5], 40, 380);
    setfont(30, 0, "方正启功行楷 简", 0, 0, 6, false, false, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH);
    outtextxy(30, 20, "文件压缩:");
    outtextxy(70, 60, "请输入要压缩的文件路径");
    outtextxy(70, 210, "请输入压缩包的路径(后缀为.hby)");
    setfont(20, 0, "等线", 0, 0, 6, false, false, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH);
    outtextxy(180, 360, "程序状态:(read only)");
}

void drawFolderCompressInterface(PIMAGE *images)
{
    putimage_withalpha(NULL, images[4], 500, 20);
    putimage_withalpha(NULL, images[5], 40, 380);
    setfont(30, 0, "方正启功行楷 简", 0, 0, 6, false, false, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH);
    outtextxy(30, 20, "文件夹压缩:");
    outtextxy(70, 60, "请输入要压缩的文件夹路径");
    outtextxy(70, 210, "请输入压缩包的路径(后缀为.hby)");
    setfont(20, 0, "等线", 0, 0, 6, false, false, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH);
    outtextxy(180, 360, "程序状态:(read only)");
}

void drawUncompressInterface(PIMAGE *images)
{
    putimage_withalpha(NULL, images[4], 500, 20);
    putimage_withalpha(NULL, images[6], 40, 380);
    setfont(30, 0, "方正启功行楷 简", 0, 0, 6, false, false, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH);
    outtextxy(30, 20, "解压缩:");
    outtextxy(70, 60, "请输入压缩包的路径(后缀为.hby)");
    outtextxy(70, 210, "请输入存储解压文件的文件夹路径");
    setfont(20, 0, "等线", 0, 0, 6, false, false, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH);
    outtextxy(180, 360, "程序状态:(read only)");
}

void drawPreviewInterface(PIMAGE *images)
{
    putimage_withalpha(NULL, images[4], 500, 20);
    putimage_withalpha(NULL, images[7], 40, 380);
    setfont(30, 0, "方正启功行楷 简", 0, 0, 6, false, false, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH);
    outtextxy(30, 20, "压缩包预览:");
    outtextxy(70, 60, "请输入压缩包的路径");
    outtextxy(70, 210, "压缩包结构如下：(read only)");
    setfont(20, 0, "等线", 0, 0, 6, false, false, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH);
    outtextxy(180, 360, "程序状态:(read only)");
}
