#include <iostream>
#include <fstream>
#include <filesystem>
#include <wchar.h>
#include <string>
#include <cstdlib>
#include "HuffmanNode.h"
#include "HuffmanTree.h"
#include "Functions.h"

/**
 * 21302010042
 * 侯斌洋
 * https://gitee.com/hby_star/Code/tree/master/DataStructure/project/HuffmanZip_CLI
 */

namespace fs = std::filesystem;
using namespace std;

int main(int argc, char **argv)
{
    string opt; //存放具体操作的字符串

    switch (argc)
    {
    case 2: //若命令行有2个参数
    {
        opt = argv[1];
        if (opt == "--h")
            HelpPage(); //打印帮助界面
    }
    break;
    case 3: //若命令行有3个参数
    {
        opt = argv[1];
        fs::path zip_path = argv[2]; //压缩包路径
        if (opt == "unzip_hby")
            UnzipOpt(zip_path); //进行解压
        else if (opt == "prev_hby")
            PrevOpt(zip_path); //进行压缩包预览
        else
            InvalidInputs(); //无效输入
    }
    break;
    case 4: //若命令行有4个参数
    {
        opt = argv[1];
        fs::path zip_path = argv[2];
        fs::path file_path = argv[3];
        if (opt == "zip_hby")
            ZipOpt(file_path, zip_path); //进行压缩
        else
            InvalidInputs(); //无效输入
    }
    break;
    default:
    {
        InvalidInputs(); //无效输入
    }
    break;
    }
    return 0;
}