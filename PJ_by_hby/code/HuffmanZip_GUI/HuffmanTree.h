#ifndef HUFFMANTREE_H
#define HUFFMANTREE_H

#include <string>
#include <queue>
#include <vector>
#include <stack>
#include <fstream>
#include "HuffmanNode.h"

using namespace std;

struct HuffmanCode
{
    unsigned char ch;
    string code;
    HuffmanCode()
        : ch{0}, code{} {}
    HuffmanCode(unsigned char Ch, string Code)
        : ch{Ch}, code{Code} {}
};

class HuffmanTree
{
private:
    HuffmanNode *root;
    int LeafNum;

public:
    HuffmanTree();
    HuffmanTree(priority_queue<HuffmanNode, vector<HuffmanNode>, greater<HuffmanNode>> &queue);
    ~HuffmanTree();
    HuffmanTree(const HuffmanTree &rhs);
    HuffmanTree &operator=(const HuffmanTree &rhs);
    void PostOrderDel(HuffmanNode *node);
    HuffmanNode *PostOrderCopy(const HuffmanNode *rhs_node);
    void PostOrderTravel(HuffmanNode *node, vector<HuffmanCode> &result);
    vector<HuffmanCode> GetHuffmanCode();
    HuffmanNode *GetRoot() { return root; };
};

#endif