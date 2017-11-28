#include "error.h"

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

Error::Error(QString n, qint32 l, qint32 c)
{
    name = n;
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
