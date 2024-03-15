#ifndef HUFFMANNODE_H
#define HUFFMANNODE_H

/**
 * 21302010042
 * 侯斌洋
 * https://gitee.com/hby_star/Code/tree/master/DataStructure/project/HuffmanZip_CLI
 */

//定义字符集最大种类数为256
const int MaxCharNum = 256;

//定义哈夫曼树的节点
struct HuffmanNode
{
    long num;           //节点字符的频率
    unsigned char ch;   //节点字符
    bool Isleaf;        //节点是否为叶节点
    HuffmanNode *Lnode; //节点的左子节点
    HuffmanNode *Rnode; //节点的右子节点

    //构造函数以及重载比较运算符的声明
    HuffmanNode()
        : ch{0}, num{0}, Isleaf{false}, Lnode{nullptr}, Rnode{nullptr} {}
    HuffmanNode(unsigned char Ch, long Num, bool isleaf, HuffmanNode *lnode, HuffmanNode *rnode)
        : ch{Ch}, num{Num}, Isleaf{isleaf}, Lnode{lnode}, Rnode{rnode} {}
    bool operator==(const HuffmanNode &rhs) const;
    bool operator>=(const HuffmanNode &rhs) const;
    bool operator<=(const HuffmanNode &rhs) const;
    bool operator>(const HuffmanNode &rhs) const;
    bool operator<(const HuffmanNode &rhs) const;
};

#endif