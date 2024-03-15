#include "HuffmanTree.h"

//默认构造函数
HuffmanTree::HuffmanTree()
{
    root = nullptr;
    LeafNum = 0;
}

//通过存储哈夫曼树节点的优先队列构造哈夫曼树的函数
HuffmanTree::HuffmanTree(priority_queue<HuffmanNode, vector<HuffmanNode>, greater<HuffmanNode>> &queue)
{
    LeafNum = queue.size(); //叶节点数

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
        //不断构造子树直至队列中只有一个节点
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

//析构函数
HuffmanTree::~HuffmanTree()
{
    PostOrderDel(root);
}

//复制构造函数
HuffmanTree::HuffmanTree(const HuffmanTree &rhs)
{
    LeafNum = rhs.LeafNum;
    PostOrderCopy(rhs.root);
}

//重载赋值运算符
HuffmanTree &HuffmanTree::operator=(const HuffmanTree &rhs)
{
    LeafNum = rhs.LeafNum;
    root = PostOrderCopy(rhs.root);
    return *this;
}

//析构函数内部的遍历释放节点空间的递归函数
void HuffmanTree::PostOrderDel(HuffmanNode *node)
{
    if (node != nullptr)
    {
        //后序遍历
        PostOrderDel(node->Lnode);
        PostOrderDel(node->Rnode);
        delete node;
    }
}

//复制构造函数内部的遍历复制节点的递归函数
HuffmanNode *HuffmanTree::PostOrderCopy(const HuffmanNode *rhs_node)
{
    if (rhs_node != nullptr) //若节点不为空则构造子树并返回子树的根
    {
        HuffmanNode *temp, *temp1, *temp2;
        temp1 = PostOrderCopy(rhs_node->Lnode);
        temp2 = PostOrderCopy(rhs_node->Rnode);
        temp = new HuffmanNode(rhs_node->ch, rhs_node->num, rhs_node->Isleaf, temp1, temp2);
        return temp;
    }
    else //节点为空返回空指针
    {
        return nullptr;
    }
}

//获取哈夫曼编码的函数内部的递归函数
void HuffmanTree::PostOrderTravel(HuffmanNode *node, vector<HuffmanCode> &result)
{
    static string s; //存储哈夫曼编码的字符串
    if (node != nullptr)
    {
        //后序遍历
        s.push_back('0');
        PostOrderTravel(node->Lnode, result);
        s.pop_back();
        s.push_back('1');
        PostOrderTravel(node->Rnode, result);
        s.pop_back();

        if (node->Isleaf) //遇到叶节点便存储编码
        {
            result[node->ch] = HuffmanCode(node->ch, s);
        }
    }
}

//获取哈夫曼编码并返回的函数
vector<HuffmanCode> HuffmanTree::GetHuffmanCode()
{
    vector<HuffmanCode> result(MaxCharNum);
    PostOrderTravel(root, result);
    return result;
}