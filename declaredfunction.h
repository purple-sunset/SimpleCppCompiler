#ifndef DECLAREDFUNCTION_H
#define DECLAREDFUNCTION_H

#include<QString>
#include "valuetype.h"

class DeclaredFunction
{
private:
    QString name;
    ValueType value;
public:
    DeclaredFunction(QString n, ValueType v);
    QString getName() const;
    void setName(const QString &value);
    ValueType getValue() const;
    void setValue(const ValueType &v);
};

#endif // DECLAREDFUNCTION_H
