#ifndef ERROR_H
#define ERROR_H

#include<QString>

class Error
{
private:
    QString name;
    qint32 line;
    qint32 column;

public:
    Error(QString n, qint32 l, qint32 c);
    QString Print();
    qint32 getLine() const;
    void setLine(const qint32 &value);
    qint32 getColumn() const;
    void setColumn(const qint32 &value);
};

#endif // ERROR_H
