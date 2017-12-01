#ifndef BINARYTREENODE_H
#define BINARYTREENODE_H

#include<QString>
#include<QStack>
#include "valuetype.h"
#include "enummath.h"

class BinaryTreeNode
{
public:
    QString name;
    ValueType value;
    BinaryTreeNode *leftChild;
    BinaryTreeNode *rightChild;
public:
    BinaryTreeNode(QString n, ValueType v);
    bool IsLeaf();
    static void CreateSubTree(QStack<BinaryTreeNode*> *opStack, QStack<BinaryTreeNode*> *nodeStack);
    static ValueType EvaluateExpressionTree(BinaryTreeNode* node);
};

#endif // BINARYTREENODE_H
