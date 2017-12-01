#include "binarytreenode.h"


BinaryTreeNode::BinaryTreeNode(QString n, ValueType v)
{
    name = n;
    value = v;
    leftChild = NULL;
    rightChild = NULL;
}

bool BinaryTreeNode::IsLeaf()
{
    return (leftChild==NULL && rightChild==NULL);
}

void BinaryTreeNode::CreateSubTree(QStack<BinaryTreeNode *> *opStack, QStack<BinaryTreeNode *> *nodeStack)
{
    BinaryTreeNode *node = opStack->pop();
    node->rightChild = nodeStack->pop();
    if(!nodeStack->isEmpty() && node->name != "++" && node->name != "--" && node->name!="!")
        node->leftChild = nodeStack->pop();
    nodeStack->push(node);
}

ValueType BinaryTreeNode::EvaluateExpressionTree(BinaryTreeNode *node)
{
    ValueType t = ValueType::Void;
    ValueType min = ValueType::Void;
    ValueType max = ValueType::Void;
    if (node->IsLeaf())
        t = node->value;
    else
    {
        ValueType x = ValueType::Nan;
        if(node->leftChild != NULL)
            x=EvaluateExpressionTree(node->leftChild);
        ValueType y = ValueType::Nan;
        if(node->rightChild != NULL)
            y=EvaluateExpressionTree(node->rightChild);
        min = EnumMath::Min(x,y);
        max = EnumMath::Max(x,y);

        if(node->name=="+" || node->name=="-" || node->name=="*" || node->name=="/")
        {
            if(min == ValueType::Nan)
                t=min;
            else if(min!=ValueType::Void && min!=ValueType::Boolean)
                t=max;
        }
        else if(node->name=="%")
        {
            if(min == ValueType::Nan)
                t=min;
            else if(min==ValueType::Interger && max==ValueType::Interger)
                t=max;
        }
        else if(node->name=="++" || node->name=="--")
        {
            if(max == ValueType::Nan)
                t=max;
            else if(max==ValueType::Interger)
                t=max;
        }
        else if(node->name=="<" || node->name==">" || node->name=="==" || node->name=="!=" || node->name=="<=" || node->name==">=" || node->name=="||" || node->name=="&&")
        {
            if(min == ValueType::Nan)
                t=min;
            else if(min!=ValueType::Boolean && max!=ValueType::Boolean)
                t=ValueType::Boolean;
        }
        else if(node->name=="!")
        {
            if(max == ValueType::Nan)
                t=max;
            else if(max==ValueType::Boolean)
                t=max;
        }
    }
    return t;
}
