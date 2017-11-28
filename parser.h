#ifndef PARSER_H
#define PARSER_H

#include<QString>
#include<QList>
#include<QStack>
#include "token.h"
#include "declaredvariable.h"
#include "declaredfunction.h"
#include "error.h"
#include "enummath.h"

class Parser
{
private:
    QList<Token> *tokens;
    QList<DeclaredVariable> *declaredVariables;
    QList<DeclaredFunction> *declaredFunctions;
    QList<Error> *errors;
    qint32 currentIndex;
    Token *currentToken;
    QStack<QString> *stack;
    bool isInFunction;
    ValueType funcValue;

public:
    Parser();
    ~Parser();
    void parseToken(QString text);

    void check();
    bool checkEmptyLine();
    bool checkInclude();
    bool checkEol();
    bool checkDeclareFuncStat();
    ValueType checkDeclareVarStat();
    bool checkAssignStat(bool b);
    bool checkExpression(ValueType v);
    bool checkIf();
    bool checkElse();
    bool checkFor();
    bool checkWhile();
    bool checkDo();
    bool checkReturn(ValueType v);
    bool checkBlockStat(bool isFunc);

    bool getTokenWithSpace();
    bool getTokenWithoutSpace();
    bool checkSpace();
    bool checkVariable(Token *t);

    QList<Token> *getTokens() const;
    void setTokens(QList<Token> *value);
    QList<DeclaredVariable> *getDeclaredVariables() const;
    void setDeclaredVariables(QList<DeclaredVariable> *value);
    QList<Error> *getErrors() const;
    void setErrors(QList<Error> *value);

};



#endif // PARSER_H
