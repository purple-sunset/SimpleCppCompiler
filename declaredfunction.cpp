#include "declaredfunction.h"

QString DeclaredFunction::getName() const
{
    return name;
}

void DeclaredFunction::setName(const QString &value)
{
    name = value;
}

ValueType DeclaredFunction::getValue() const
{
    return value;
}

void DeclaredFunction::setValue(const ValueType &v)
{
    value = v;
}

DeclaredFunction::DeclaredFunction(QString n, ValueType v)
{
    name = n;
    value = v;
}
