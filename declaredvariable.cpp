#include "declaredvariable.h"

QString DeclaredVariable::getName() const
{
    return name;
}

void DeclaredVariable::setName(const QString &value)
{
    name = value;
}

ValueType DeclaredVariable::getValue() const
{
    return value;
}

void DeclaredVariable::setValue(const ValueType &v)
{
    value = v;
}

bool DeclaredVariable::getIsConst() const
{
    return isConst;
}

void DeclaredVariable::setIsConst(bool value)
{
    isConst = value;
}

DeclaredVariable::DeclaredVariable(QString n, ValueType v, bool i)
{
    name = n;
    value = v;
    isConst = i;
}
