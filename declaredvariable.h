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
    DeclaredVariable(QString n, ValueType v, bool i);
    QString getName() const;
    void setName(const QString &value);
    ValueType getValue() const;
    void setValue(const ValueType &v);
    bool getIsConst() const;
    void setIsConst(bool value);
};

#endif // DECLAREDVARIABLE_H
