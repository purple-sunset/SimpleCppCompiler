#ifndef PARSER_H
#define PARSER_H

#include<QString>
#include<QList>
#include<QStack>
#include "token.h"
#include "declaredvariable.h"
#include "error.h"

class Parser
{
private:
    QList<Token> *tokens;
    QList<DeclaredVariable> *declaredVariables;
    QList<Error> *errors;
    qint32 checkingIndex;
    Token *checkingToken;
    QStack<QString> *stack;

public:
    Parser();
    ~Parser();
    void parseToken(QString text);
    void check();
    bool checkForEmptyLine();
    bool checkForInclude();
    bool checkForEol();

    void getTokenWithSpace();
    void getTokenWithoutSpace();
    bool checkForSpace();


    QList<Token> *getTokens() const;
    void setTokens(QList<Token> *value);
    QList<DeclaredVariable> *getDeclaredVariables() const;
    void setDeclaredVariables(QList<DeclaredVariable> *value);
    QList<Error> *getErrors() const;
    void setErrors(QList<Error> *value);

};



#endif // PARSER_H
