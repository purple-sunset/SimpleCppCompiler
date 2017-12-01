#ifndef TOKEN_H
#define TOKEN_H

#include<QChar>
#include<QStringList>
#include<QString>
#include "tokentype.h"
#include "valuetype.h"

class Token
{

private:
    static const QStringList keyWords;
    static const QStringList symbols;
    static const QStringList tokenType;
    static const QStringList valueType;

    QString name;
    qint32 line;
    qint32 column;
    TokenType type;
    ValueType value;

public:
    static bool CheckKeyWord(QString s);
    static bool CheckSymbol(QChar c);

public:
    static qint32 GetPriority(QString s);
    Token();
    QString Print();
    void createKeyWord(QString n);
    void createVariable(QString n);
    void createLiteral(QString n);
    void createSymbol(QString n);
    void createEol(QString n);
    void setPosition(int l, int c);

    bool checkName(QString s);
    bool checkType(TokenType t);
    bool checkValue(ValueType v);

    TokenType getType() const;
    void setType(const TokenType &value);
    ValueType getValue() const;
    void setValue(const ValueType &_value);
    qint32 getLine() const;
    void setLine(const qint32 &value);
    qint32 getColumn() const;
    void setColumn(const qint32 &value);
    QString getName() const;
    void setName(const QString &value);
};



#endif // TOKEN_H
