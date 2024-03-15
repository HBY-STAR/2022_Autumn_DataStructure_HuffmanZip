#include "HuffmanTree.h"

HuffmanTree::HuffmanTree()
{
    root = nullptr;
    LeafNum = 0;
}
HuffmanTree::HuffmanTree(priority_queue<HuffmanNode, vector<HuffmanNode>, greater<HuffmanNode>> &queue)
{
    LeafNum = queue.size();
    if (LeafNum == 0)
    {
        root = nullptr;
    }
    else if (LeafNum == 1)
    {
        root = new HuffmanNode(queue.top());
        queue.pop();
    }
    else
    {
        HuffmanNode *temp, *temp1, *temp2;
        while (queue.size() >= 2)
        {
            temp1 = new HuffmanNode(queue.top());
            queue.pop();
            temp2 = new HuffmanNode(queue.top());
            queue.pop();
            temp = new HuffmanNode(0, temp1->num + temp2->num, false, temp1, temp2);
            queue.push(*temp);
        }
        root = temp;
    }
}
HuffmanTree::~HuffmanTree()
{
    PostOrderDel(root);
}
HuffmanTree::HuffmanTree(const HuffmanTree &rhs)
{
    LeafNum = rhs.LeafNum;
    PostOrderCopy(rhs.root);
}
HuffmanTree &HuffmanTree::operator=(const HuffmanTree &rhs)
{
    LeafNum = rhs.LeafNum;
    root = PostOrderCopy(rhs.root);
    return *this;
}

void HuffmanTree::PostOrderDel(HuffmanNode *node)
{
    if (node != nullptr)
    {
        PostOrderDel(node->Lnode);
        PostOrderDel(node->Rnode);
        delete node;
    }
}

HuffmanNode *HuffmanTree::PostOrderCopy(const HuffmanNode *rhs_node)
{
    if (rhs_node != nullptr)
    {
        HuffmanNode *temp, *temp1, *temp2;
        temp1 = PostOrderCopy(rhs_node->Lnode);
        temp2 = PostOrderCopy(rhs_node->Rnode);
        temp = new HuffmanNode(rhs_node->ch, rhs_node->num, rhs_node->Isleaf, temp1, temp2);
        return temp;
    }
    else
    {
        return nullptr;
    }
}

void HuffmanTree::PostOrderTravel(HuffmanNode *node, vector<HuffmanCode> &result)
{
    static string s;
    if (node != nullptr)
    {
        s.push_back('0');
        PostOrderTravel(node->Lnode,result);
        s.pop_back();
        s.push_back('1');
        PostOrderTravel(node->Rnode,result);
        s.pop_back();
        if (node->Isleaf)
        {
            result[node->ch] = HuffmanCode(node->ch, s);
        }
    }
}

vector<HuffmanCode> HuffmanTree::GetHuffmanCode()
{
    vector<HuffmanCode> result(MaxCharNum);
    PostOrderTravel(root, result);
    return result;
}