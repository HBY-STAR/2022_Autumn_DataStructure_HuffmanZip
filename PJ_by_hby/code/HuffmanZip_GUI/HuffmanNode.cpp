#include"HuffmanNode.h"

bool HuffmanNode::operator==(const HuffmanNode &rhs) const
{
    return num == rhs.num;
}

bool HuffmanNode::operator>=(const HuffmanNode &rhs) const
{
    return num >= rhs.num;
}

bool HuffmanNode::operator<=(const HuffmanNode &rhs) const
{
    return num <= rhs.num;
}

bool HuffmanNode::operator>(const HuffmanNode &rhs) const
{
    return num > rhs.num;
}

bool HuffmanNode::operator<(const HuffmanNode &rhs) const
{
    return num < rhs.num;
}