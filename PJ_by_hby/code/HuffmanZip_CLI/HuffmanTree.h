#ifndef HUFFMANTREE_H
#define HUFFMANTREE_H

#include <string>
#include <queue>
#include <vector>
#include <stack>
#include <fstream>
#include "HuffmanNode.h"

/**
 * 21302010042
 * 侯斌洋
 * https://gitee.com/hby_star/Code/tree/master/DataStructure/project/HuffmanZip_CLI
 */

using namespace std;

//定义字符与哈夫曼编码相对应的结构
struct HuffmanCode
{
    unsigned char ch; //字符
    string code;      //对应的哈夫曼编码
    HuffmanCode()
        : ch{0}, code{} {}
    HuffmanCode(unsigned char Ch, string Code)
        : ch{Ch}, code{Code} {}
};

//定义哈夫曼树
class HuffmanTree
{
private:
    HuffmanNode *root; //哈夫曼树的根
    int LeafNum;       //哈夫曼树叶节点的个数

public:
    HuffmanTree(); //默认构造函数

    //通过存储哈夫曼树节点的优先队列构造哈夫曼树的函数
    HuffmanTree(priority_queue<HuffmanNode, vector<HuffmanNode>, greater<HuffmanNode>> &queue);

    ~HuffmanTree();                                                       //析构函数
    HuffmanTree(const HuffmanTree &rhs);                                  //复制构造函数
    HuffmanTree &operator=(const HuffmanTree &rhs);                       //重载赋值运算符
    void PostOrderDel(HuffmanNode *node);                                 //析构函数内部的遍历释放节点空间的递归函数
    HuffmanNode *PostOrderCopy(const HuffmanNode *rhs_node);              //复制构造函数内部的遍历复制节点的递归函数
    void PostOrderTravel(HuffmanNode *node, vector<HuffmanCode> &result); //获取哈夫曼编码的函数内部的递归函数
    vector<HuffmanCode> GetHuffmanCode();                                 //获取哈夫曼编码并返回的函数
    HuffmanNode *GetRoot() { return root; };                              //获取哈夫曼树的根的函数
};

#endif