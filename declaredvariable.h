#ifndef DECLAREDVARIABLE_H
#define DECLAREDVARIABLE_H

#include<QString>
#include "valuetype.h"

class DeclaredVariable
{
private:
    QString name;
    ValueType value;
    bool isConst;

public:
    DeclaredVariable();
};

#endif // DECLAREDVARIABLE_H
