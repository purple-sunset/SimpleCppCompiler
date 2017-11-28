#ifndef ERROR_H
#define ERROR_H

#include<QString>
#include<QStringList>

class Error
{
private:
    static const QStringList errors;
    QString name;
    qint32 line;
    qint32 column;

public:
    Error(qint32 i, QString n, qint32 l, qint32 c);
    QString Print();
    qint32 getLine() const;
    void setLine(const qint32 &value);
    qint32 getColumn() const;
    void setColumn(const qint32 &value);
};

#endif // ERROR_H
