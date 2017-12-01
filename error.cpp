#include "error.h"

const QStringList Error::errors({"Missing ", "Extra token ", "Wrong header ", "Invalid ", "Cannot assign to a constant ", " is not declared" , "is declared", " is not a boolean", " is not an integer", " is not a float", " is not a double"});

qint32 Error::getLine() const
{
    return line;
}

void Error::setLine(const qint32 &value)
{
    line = value;
}

qint32 Error::getColumn() const
{
    return column;
}

void Error::setColumn(const qint32 &value)
{
    column = value;
}

Error::Error(qint32 i, QString n, qint32 l, qint32 c)
{
    if(i < 5)
        name = QString(errors[i]).append(n);
    else
        name = QString(n).append(errors[i]);
    line = l;
    column = c;
}

QString Error::Print()
{
    QString *s = new QString(name);
    s->append(" at line ");
    s->append(QString::number(line+1));
    s->append(", column ");
    s->append(QString::number(column+1));
    return *s;
}
