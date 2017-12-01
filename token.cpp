#include "token.h"

const QStringList Token::keyWords({"include", "return", "using", "namespace", "case", "switch", "if", "else", "for", "while", "do", "char", "bool", "int", "void", "float", "double", "true", "false"});
const QStringList Token::symbols({"#", "+", "-", "*", "/", "%", "<", ">", "=", "!", "&", "|", "(", ")", "{", "}",  "[", "]", "\"", "?", ".", ",", ":", ";"});
const QStringList Token::tokenType({"Keyword", "Symbol", "Literal", "Variable", "Eol"});
const QStringList Token::valueType({"Nan", "Void", "Boolean", "Interger", "Float", "Double"});


TokenType Token::getType() const
{
    return type;
}

void Token::setType(const TokenType &value)
{
    type = value;
}

ValueType Token::getValue() const
{
    return value;
}

void Token::setValue(const ValueType &_value)
{
    value = _value;
}

qint32 Token::getLine() const
{
    return line;
}

void Token::setLine(const qint32 &value)
{
    line = value;
}

qint32 Token::getColumn() const
{
    return column;
}

void Token::setColumn(const qint32 &value)
{
    column = value;
}

QString Token::getName() const
{
    return name;
}

void Token::setName(const QString &value)
{
    name = value;
}

bool Token::CheckKeyWord(QString s)
{
    return keyWords.contains(s);
}

bool Token::CheckSymbol(QChar c)
{
    return symbols.contains(QString(c));
}

qint32 Token::GetPriority(QString s)
{
    if(s=="!" || s=="||" || s=="&&")
        return 1;
    else if(s=="<" || s==">" || s=="<=" || s==">=" || s=="==" || s=="!=")
        return 2;
    else if(s=="+" || s=="-" )
        return 3;
    else if(s=="%" || s=="*" || s=="/")
        return 4;
    else if( s=="++" || s=="--")
        return 5;
    else if(s=="(" || s==")")
        return 0;
    else
        return 0;
}

Token::Token()
{
}

QString Token::Print()
{
    QString *s = new QString();
    s->append("Line: ");
    s->append(QString::number(line+1));
    s->append(", Column: ");
    s->append(QString::number(column+1));
    s->append(", Token: ");
    s->append(name);
    s->append(", Type: ");
    s->append(tokenType[static_cast<qint32>(type)]);
    s->append(", ValueType: ");
    s->append(valueType[static_cast<qint32>(value)]);
    return *s;
}

void Token::setPosition(int l, int c)
{
    line = l;
    column = c;
}

bool Token::checkName(QString s)
{
    return (name == s);
}

bool Token::checkType(TokenType t)
{
    return type == t;
}

bool Token::checkValue(ValueType v)
{
    return value == v;
}

void Token::createKeyWord(QString n)
{
    name = n;
    type = TokenType::Keyword;
    if(name=="true" || name=="false")
    {
        type = TokenType::Literal;
        value = ValueType::Boolean;
    }
    else if(name=="void")
        value = ValueType::Void;
    else if(name=="bool")
        value = ValueType::Boolean;
    else if(name=="int")
        value = ValueType::Interger;
    else if(name=="float")
        value = ValueType::Float;
    else if(name=="double")
        value = ValueType::Double;
    else
        value = ValueType::Nan;
}

void Token::createVariable(QString n)
{
    name = n;
    type = TokenType::Variable;
    value = ValueType::Void;
}

void Token::createLiteral(QString n)
{
    name = n;
    type = TokenType::Literal;
    bool ok;
    n.toInt(&ok);
    if(ok)
        value = ValueType::Interger;
    else
    {
        n.toFloat(&ok);
        if(ok)
            value = ValueType::Float;
        else
        {
            n.toDouble(&ok);
            if(ok)
                value = ValueType::Double;
        }
    }
}


void Token::createSymbol(QString n)
{
    name = n;
    type = TokenType::Symbol;
    if(name=="!" || name=="||" || name=="&&" || name=="<" || name==">" || name=="<=" || name==">=" || name=="==" || name=="!=")
        value = ValueType::Boolean;
    else if(name=="+" || name=="-" || name=="++" || name=="--" || name=="%" || name=="*" || name=="/")
        value = ValueType::Interger;
    else if(name=="(" || name==")")
        value = ValueType::Void;
    else
        value=ValueType::Nan;
}

void Token::createEol(QString n)
{
    name = n;
    type = TokenType::Eol;
    value = ValueType::Nan;
}
