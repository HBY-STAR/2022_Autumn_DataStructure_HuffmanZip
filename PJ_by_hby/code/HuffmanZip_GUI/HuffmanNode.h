#ifndef HUFFMANNODE_H
#define HUFFMANNODE_H

const int MaxCharNum = 256;

struct HuffmanNode
{
    long num;
    unsigned char ch;
    bool Isleaf;
    HuffmanNode *Lnode;
    HuffmanNode *Rnode;

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