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
#include "binarytreenode.h"

class Parser
{
private:
    QList<Token> *tokens;
    QList<DeclaredVariable> *declaredVariables;
    QList<DeclaredFunction> *declaredFunctions;
    QList<Error> *errors;
    qint32 currentIndex;
    Token *currentToken;
    bool isOk;
    bool isInFunction;
    ValueType funcValue;
    bool isReturned;

public:
    Parser();
    ~Parser();
    void parseToken(QString text);

    void check();
    bool checkEmptyLine();
    bool checkInclude();
    bool checkEol(bool b);
    bool checkDeclareFuncStat();
    bool checkDeclareVarStat(bool isCheck);
    bool checkAssignStat(bool b);
    bool checkExpression(ValueType v);
    bool checkExpression2(ValueType v);
    bool checkExpressionStat(bool b, ValueType v);
    bool checkIf();
    bool checkElse();
    bool checkFor();
    bool checkWhile();
    bool checkDo();
    bool checkReturn(ValueType v);
    bool checkFuncBlock();
    bool checkBlockStat();

    bool getToken();
    bool checkVariable(Token *t);

    QList<Token> *getTokens() const;
    void setTokens(QList<Token> *value);
    QList<DeclaredVariable> *getDeclaredVariables() const;
    void setDeclaredVariables(QList<DeclaredVariable> *value);
    QList<Error> *getErrors() const;
    void setErrors(QList<Error> *value);

};



#endif // PARSER_H
