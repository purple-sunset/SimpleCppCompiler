#include "parser.h"


QList<Token> *Parser::getTokens() const
{
    return tokens;
}

void Parser::setTokens(QList<Token> *value)
{
    tokens = value;
}

QList<Error> *Parser::getErrors() const
{
    return errors;
}

void Parser::setErrors(QList<Error> *value)
{
    errors = value;
}

QList<DeclaredVariable> *Parser::getDeclaredVariables() const
{
    return declaredVariables;
}

void Parser::setDeclaredVariables(QList<DeclaredVariable> *value)
{
    declaredVariables = value;
}

Parser::Parser()
{
    tokens = new QList<Token>();
    declaredVariables = new QList<DeclaredVariable>();
    declaredFunctions = new QList<DeclaredFunction>();
    errors = new QList<Error>();
    currentIndex = -1;
    isOk = true;
    isInFunction = false;
    funcValue = ValueType::Nan;
    isReturned = false;
}

Parser::~Parser()
{
    delete tokens;
    delete declaredVariables;
    delete declaredFunctions;
    delete errors;
}

void Parser::parseToken(QString text)
{
    int i=0;
    int l=0;
    int c=0;
    int len = text.size();
    while(i<len)
    {
        Token *tmpToken = new Token();
        QString *tmpString = new QString();

        //check for spaces
        while(text[i] == ' ' || text[i] == '\t')
        {
            i++;
            c++;

        }
        //check for variable or keyword
        if(text[i].isLetter() || text[i] == '_')
        {
            tmpString->append(text[i]);
            tmpToken->setPosition(l, c);
            i++;
            c++;
            while(text[i].isLetterOrNumber() || text[i] == '_')
            {
                tmpString->append(text[i]);
                i++;
                c++;
            }
            //check for keyword
            if(Token::CheckKeyWord(*tmpString))
                tmpToken->createKeyWord(*tmpString);
            else
                tmpToken->createVariable(*tmpString);
        }
        //check for number
        else if(text[i].isNumber())
        {
            tmpString->append(text[i]);
            tmpToken->setPosition(l, c);
            i++;
            c++;
            while(text[i].isNumber())
            {
                tmpString->append(text[i]);
                i++;
                c++;
            }
            if(text[i] == '.')
            {
                tmpString->append(text[i]);
                i++;
                c++;
                while(text[i].isNumber())
                {
                    tmpString->append(text[i]);
                    i++;
                    c++;
                }
            }
            if(text[i] == 'e')
            {
                tmpString->append(text[i]);
                i++;
                c++;
                while(text[i].isNumber())
                {
                    tmpString->append(text[i]);
                    i++;
                    c++;
                }
            }
            tmpToken->createLiteral(*tmpString);
        }
        //check for symbol
        else if(Token::CheckSymbol(text[i]))
        {
            tmpString->append(text[i]);
            tmpToken->setPosition(l, c);
            i++;
            c++;
            //check for +=, -= ...
           if(*tmpString == "+" || *tmpString == "-" || *tmpString == "*" || *tmpString == "/" || *tmpString == "%" || *tmpString == "<" || *tmpString == ">" || *tmpString == "=" || *tmpString == "!")
               if(text[i] == '=')
               {
                   tmpString->append(text[i]);
                   i++;
                   c++;
               }
           //check for ++, --, &&, ||
           if(*tmpString == "+" || *tmpString == "-" || *tmpString == "&" || *tmpString == "|")
               if(QString(text[i]) == tmpString)
               {
                   tmpString->append(text[i]);
                   i++;
                   c++;
               }
           tmpToken->createSymbol(*tmpString);
        }
        //check for eol
        else if(text[i] == '\n')
        {
            tmpToken->setPosition(l, c);
            i++;
            c=0;
            l++;
            tmpToken->createEol("eol");
        }
        else
        {
            errors->append(Error(3, "token", l, c));
            isOk = false;
            i++;
            c++;
            continue;
        }

        //add token to list
        tokens->append(*tmpToken);
    }

}

void Parser::check()
{
    if(isOk)
    {
        getToken();
        while (currentIndex < tokens->size() - 1)
        {
            if(!isInFunction)
            {
                //check for #include
                if(checkInclude())
                {
                    continue;
                }
            }

            //check for function
            if(checkDeclareFuncStat())
            {
                isInFunction = true;
                if(checkFuncBlock())
                {
                    if(funcValue != ValueType::Void && !isReturned)
                        errors->append(Error(0, "return statement", currentToken->getLine(), currentToken->getColumn()));
                }
                else
                    errors->append(Error(0, "\"{\"", currentToken->getLine(), currentToken->getColumn()));
                continue;
            }

            //check for empty line
            if(checkEmptyLine())
                continue;

            //check for other error
            while(!currentToken->checkType(TokenType::Eol))
            {
                errors->append(Error(1, currentToken->getName(), currentToken->getLine(), currentToken->getColumn()));
                getToken();
            }

        }
    }
}

bool Parser::checkEmptyLine()
{
    if(currentToken->checkType(TokenType::Eol) || currentIndex == tokens->size() -1)
        return true;
    return false;
}

bool Parser::checkInclude()
{
    if(currentToken->checkName("#"))
    {
        getToken();
        if(currentToken->checkName("include"))
        {
            getToken();
        }
        else
            errors->append(Error(0, "\"include\"", currentToken->getLine(), currentToken->getColumn()));

        if(currentToken->checkName("<"))
        {
            getToken();
            if(currentToken->checkType(TokenType::Variable))
            {
                getToken();
            }
            else
                errors->append(Error(0, "header name", currentToken->getLine(), currentToken->getColumn()));

            if(currentToken->checkName(">"))
            {
                getToken();
            }
            else
                errors->append(Error(0, "\">\"", currentToken->getLine(), currentToken->getColumn()));
        }
        else if(currentToken->checkName("\""))
        {
            getToken();
            if(currentToken->checkType(TokenType::Variable))
            {
                getToken();
                if(currentToken->checkName("."))
                {
                    getToken();
                    if(currentToken->checkName("h"))
                    {
                        getToken();
                    }
                    else
                        errors->append(Error(2, "name", currentToken->getLine(), currentToken->getColumn()));
                }
                else
                    errors->append(Error(2, "name", currentToken->getLine(), currentToken->getColumn()));
            }
            else
                errors->append(Error(0, "header name", currentToken->getLine(), currentToken->getColumn()));

            if(currentToken->checkName("\""))
            {
                getToken();
            }
            else
                errors->append(Error(0, "\"\"\"", currentToken->getLine(), currentToken->getColumn()));
        }
        else
            errors->append(Error(0, "\"<\" or \"\"\"", currentToken->getLine(), currentToken->getColumn()));

        checkEol(false);
        return true;
    }
    return false;
}

bool Parser::checkEol(bool b)
{
    if(b)
    {
        bool hasE = false;
        while(!currentToken->checkType(TokenType::Eol))
        {
            if(currentToken->checkName(";") && !hasE)
            {
                hasE = true;
                getToken();
                continue;
            }
            errors->append(Error(1, "\"" + currentToken->getName() + "\"", currentToken->getLine(), currentToken->getColumn()));
            getToken();
        }


        if(!hasE)
            errors->append(Error(0, "\";\"", currentToken->getLine(), currentToken->getColumn()));

        getToken();
        return true;
    }
    else
    {
        while(!currentToken->checkType(TokenType::Eol))
        {
            errors->append(Error(1, "\"" + currentToken->getName() + "\"", currentToken->getLine(), currentToken->getColumn()));
            getToken();
        }
        getToken();
        return true;
    }
    return false;

}

bool Parser::checkDeclareFuncStat()
{
    if(currentToken->checkType(TokenType::Keyword) && !currentToken->checkValue(ValueType::Nan))
    {
        ValueType v = currentToken->getValue();
        funcValue = v;

        getToken();
        if(currentToken->checkType(TokenType::Variable))
        {
            getToken();
            declaredFunctions->append(DeclaredFunction(currentToken->getName(), v));
            isInFunction = true;

        }

        else
            errors->append(Error(0, "function name", currentToken->getLine(), currentToken->getColumn()));

        if(currentToken->checkName("("))
            getToken();
        else
            errors->append(Error(0, "\"(\"", currentToken->getLine(), currentToken->getColumn()));

        if(currentToken->checkType(TokenType::Keyword) && !currentToken->checkValue(ValueType::Nan))
        {
            ValueType v1 = currentToken->getValue();
            getToken();
            if(currentToken->checkType(TokenType::Variable))
            {
                checkVariable(currentToken);
                if(!currentToken->checkValue(ValueType::Void) && currentToken->checkValue(v))
                    errors->append(Error(6, currentToken->getName(), currentToken->getLine(), currentToken->getColumn()));
                else
                {
                    declaredVariables->append(DeclaredVariable(currentToken->getName(), v1, true));
                    checkVariable(currentToken);
                }
            }
        }


        while (!currentToken->checkName(")") && currentToken->checkName(",") && !currentToken->checkType(TokenType::Eol))
        {
            getToken();
            if(currentToken->checkType(TokenType::Variable))
            {
                checkVariable(currentToken);
                if(!currentToken->checkValue(ValueType::Void) && currentToken->checkValue(v))
                        errors->append(Error(6, currentToken->getName(), currentToken->getLine(), currentToken->getColumn()));
                else
                {
                    declaredVariables->append(DeclaredVariable(currentToken->getName(), v, true));
                    checkVariable(currentToken);
                }
            }
            else
                break;
        }

        if(currentToken->checkName(")"))
            getToken();
        else
            errors->append(Error(0, "\")\"", currentToken->getLine(), currentToken->getColumn()));
        checkEol(false);
        return true;
    }
    return false;
}


bool Parser::checkDeclareVarStat(bool isCheck)
{
    bool b = false;
    if(currentToken->checkName("const"))
    {
        b=true;
        getToken();
    }
    if(currentToken->checkType(TokenType::Keyword) && !currentToken->checkValue(ValueType::Nan))
    {
        ValueType v = currentToken->getValue();
        getToken();
        if(currentToken->checkType(TokenType::Variable))
        {
            checkVariable(currentToken);
            if(!currentToken->checkValue(ValueType::Void) && currentToken->checkValue(v))
                    errors->append(Error(6, currentToken->getName(), currentToken->getLine(), currentToken->getColumn()));
            else
            {
                declaredVariables->append(DeclaredVariable(currentToken->getName(), v, b));
                checkVariable(currentToken);
            }

            getToken();
            if(currentToken->checkName("="))
            {
                getToken();
                if(!checkExpression(v))
                    errors->append(Error(static_cast<qint32>(v)+5, "Expression", currentToken->getLine(), currentToken->getColumn()));
            }
            else
            {
                while ((currentToken->checkName(",")))
                {
                    getToken();
                    if(currentToken->checkType(TokenType::Variable))
                    {
                        checkVariable(currentToken);
                        if(!currentToken->checkValue(ValueType::Void) && currentToken->checkValue(v))
                            errors->append(Error(6, currentToken->getName(), currentToken->getLine(), currentToken->getColumn()));
                        else
                        {
                            declaredVariables->append(DeclaredVariable(currentToken->getName(), v, b));
                            checkVariable(currentToken);
                        }
                    }
                    else
                        break;
                    getToken();
                }
            }

            if(isCheck)
                checkEol(true);

        }
        else
            errors->append(Error(0, "variable name", currentToken->getLine(), currentToken->getColumn()));

        return v!=ValueType::Nan;

    }
    return false;
}

bool Parser::checkAssignStat(bool b)
{
    if(currentToken->checkType(TokenType::Variable))
    {
        qint32 i = currentIndex;
        Token *t = currentToken;
        getToken();
        if(currentToken->checkName("="))
        {
            if(checkVariable(t))
                errors->append(Error(4, "\"" + t->getName() + "\"", t->getLine(), t->getColumn()));
            if(t->checkValue(ValueType::Void))
                errors->append(Error(5, "\"" +  t->getName() + "\"", t->getLine(), t->getColumn()));
            getToken();
            if(!checkExpression(t->getValue()))
                errors->append(Error(static_cast<qint32>(t->getValue())+5, "Expression", currentToken->getLine(), currentToken->getColumn()));
            if(b)
            {
                checkEol(true);
            }
            return true;
        }
        else if(currentToken->checkName("+=") || currentToken->checkName("-=") || currentToken->checkName("*=") || currentToken->checkName("/="))
        {
            if(checkVariable(t))
                errors->append(Error(4, "\"" + t->getName() + "\"", t->getLine(), t->getColumn()));
            if(t->checkValue(ValueType::Void))
                errors->append(Error(5, "\"" +  t->getName() + "\"", t->getLine(), t->getColumn()));

            getToken();
            if(t->checkValue(ValueType::Boolean))
            {
                errors->append(Error(8, t->getName(), t->getLine(), t->getColumn()));
                checkExpression(ValueType::Void);
            }
            else if(!checkExpression(t->getValue()))
                errors->append(Error(static_cast<qint32>(t->getValue())+5, "Expression", currentToken->getLine(), currentToken->getColumn()));
            if(b)
            {
                checkEol(true);
            }
            return true;
        }
        else if(currentToken->checkName("%="))
        {
            if(checkVariable(t))
                errors->append(Error(4, "\"" + t->getName() + "\"", t->getLine(), t->getColumn()));
            if(t->checkValue(ValueType::Void))
                errors->append(Error(5, "\"" +  t->getName() + "\"", t->getLine(), t->getColumn()));

            getToken();
            if(!t->checkValue(ValueType::Interger))
                errors->append(Error(8, t->getName(), t->getLine(), t->getColumn()));
            if(!checkExpression(ValueType::Interger))
                errors->append(Error(8, "Expression", currentToken->getLine(), currentToken->getColumn()));
            if(b)
            {
                checkEol(true);
            }
            return true;
        }
        else
        {
            currentIndex = i-1;
            getToken();
        }

        return false;
    }
    return false;
}

bool Parser::checkExpression2(ValueType v)
{
    if(currentToken->checkType(TokenType::Variable))
        checkVariable(currentToken);
    ValueType minV = currentToken->getValue();
    ValueType maxV = currentToken->getValue();
    QStack<Token> *s1 = new QStack<Token>();
    QStack<Token> *s2 = new QStack<Token>();
    bool isValid = true;
    while(!currentToken->checkType(TokenType::Eol) && !currentToken->checkType(TokenType::Keyword) && !currentToken->checkValue(ValueType::Nan))
    {
        if(currentToken->checkName("("))
        {
            s1->push(*currentToken);
        }
        else if(currentToken->checkName(")"))
        {
            if(s1->size()>0)
            {
                s1->pop();
            }
            else
                errors->append(Error(1,"\")\"", currentToken->getLine(), currentToken->getColumn()));
        }
        else if(currentToken->checkType(TokenType::Variable))
        {
            checkVariable(currentToken);
            if(currentToken->checkValue(ValueType::Void))
                errors->append(Error(5, currentToken->getName(), currentToken->getLine(), currentToken->getColumn()));
            s2->push(*currentToken);
            minV = EnumMath::Min(minV, currentToken->getValue());
            maxV = EnumMath::Max(maxV, currentToken->getValue());

        }
        else if(currentToken->checkType(TokenType::Literal))
        {
            s2->push(*currentToken);
            minV = EnumMath::Min(minV, currentToken->getValue());
            maxV = EnumMath::Max(maxV, currentToken->getValue());
        }
        else if (   currentToken->checkName("+") || currentToken->checkName("-") || currentToken->checkName("*") || currentToken->checkName("/") || currentToken->checkName("%")
                 || currentToken->checkName("++") || currentToken->checkName("--"))
        {
            if(s2->size()>0)
            {
                Token t = s2->pop();
                minV = EnumMath::Min(minV, currentToken->getValue());
                maxV = EnumMath::Max(maxV, currentToken->getValue());
            }
            else
                errors->append(Error(3,"expression", currentToken->getLine(), currentToken->getColumn()));
        }
        else if (currentToken->checkName("<") || currentToken->checkName(">") || currentToken->checkName("==") || currentToken->checkName("!=") || currentToken->checkName("<=")
                || currentToken->checkName(">=") || currentToken->checkName("||") || currentToken->checkName("&&"))
        {
            if(s2->size()>0)
            {
                Token t = s2->pop();
                minV = ValueType::Boolean;
                maxV = ValueType::Boolean;
            }
            else
                errors->append(Error(3,"expression", currentToken->getLine(), currentToken->getColumn()));
        }
        else
        {
            isValid = false;
            errors->append(Error(3,"expression", currentToken->getLine(), currentToken->getColumn()));
            break;
        }
        if(!getToken())
            break;
    }
    while(s1->size()>0)
    {
        Token t=s1->pop();
        errors->append(Error(1,"\"(\"", t.getLine(), t.getColumn()));
    }
    if(isValid)
    {
        if(v == ValueType::Void)
            return ((minV == ValueType::Boolean) && (maxV >= ValueType::Boolean)) || ((minV > ValueType::Boolean) && (maxV >= v));
        else if(v == ValueType::Boolean)
            return (minV == ValueType::Boolean) && (maxV >= ValueType::Boolean);
        else
            return (minV > ValueType::Boolean) && (minV <= v) && (maxV <= v);

    }
    return false;
}

bool Parser::checkExpression(ValueType v)
{
    if(currentToken->checkType(TokenType::Variable))
        checkVariable(currentToken);
    //QList<> *prefix = new QList();
    QStack<BinaryTreeNode*> *operatorStack = new QStack<BinaryTreeNode*>();
    QStack<BinaryTreeNode*> *nodeStack = new QStack<BinaryTreeNode*>();
    //bool isValid = true;
    qint32 i=0;
    while(!currentToken->checkType(TokenType::Eol) && !currentToken->checkType(TokenType::Keyword) && !currentToken->checkValue(ValueType::Nan))
    {
        if(currentToken->checkType(TokenType::Variable))
        {
            checkVariable(currentToken);
            if(currentToken->checkValue(ValueType::Void))
                errors->append(Error(5, currentToken->getName(), currentToken->getLine(), currentToken->getColumn()));
            nodeStack->push(new BinaryTreeNode(currentToken->getName(), currentToken->getValue()));
        }
        else if(currentToken->checkType(TokenType::Literal))
        {
            nodeStack->push(new BinaryTreeNode(currentToken->getName(), currentToken->getValue()));
        }
        else if(currentToken->checkName("("))
        {
            operatorStack->push(new BinaryTreeNode(currentToken->getName(), ValueType::Void));
        }
        else if(currentToken->checkName(")"))
        {
            if(!operatorStack->isEmpty())
            {
                while (operatorStack->top()->name != "(")
                {
                    BinaryTreeNode::CreateSubTree(operatorStack, nodeStack);
                    if(operatorStack->isEmpty())
                        break;
                }
                if(!operatorStack->isEmpty())
                    operatorStack->pop();
                else
                    errors->append(Error(1,"\")\"", currentToken->getLine(), currentToken->getColumn()));
            }
            else
                errors->append(Error(1,"\")\"", currentToken->getLine(), currentToken->getColumn()));
        }
        else if(Token::GetPriority(currentToken->getName()) > 0)
        {
            if(currentToken->checkName("-") && (i==0 || (i>0 && (*tokens)[currentIndex-1].checkName("("))))
            {
                getToken();
                if(currentToken->checkType(TokenType::Variable))
                    checkVariable(currentToken);
                nodeStack->push(new BinaryTreeNode("-" + currentToken->getName(), currentToken->getValue()));
            }
            else
            {
                while (operatorStack->size() > 0 && Token::GetPriority(operatorStack->top()->name) >= Token::GetPriority(currentToken->getName()))
                    BinaryTreeNode::CreateSubTree(operatorStack, nodeStack);

                operatorStack->push(new BinaryTreeNode(currentToken->getName(), ValueType::Void));
            }
        }

        i++;
        if(!getToken())
            break;
    }

    while (operatorStack->size() > 0)
        if(operatorStack->top()->name == "(")
        {
            operatorStack->pop();
            errors->append(Error(0,"\")\"", currentToken->getLine(), currentToken->getColumn()));
        }
        else
            BinaryTreeNode::CreateSubTree(operatorStack, nodeStack);

    ValueType result = ValueType::Nan;
    if(!nodeStack->isEmpty())
        result = BinaryTreeNode::EvaluateExpressionTree(nodeStack->top());
    delete operatorStack;
    delete nodeStack;
    if(result != ValueType::Nan)
    {
        if(v==ValueType::Void)
            return (result != ValueType::Void);
        else if(v==ValueType::Boolean)
            return (result == ValueType::Boolean);
        else
            return (EnumMath::Max(result, ValueType::Interger) == result) && (EnumMath::Max(result, v) == v);
    }
    else
    {
        errors->append(Error(3,"expression", currentToken->getLine(), currentToken->getColumn()));
        return true;
    }
}

bool Parser::checkExpressionStat(bool b, ValueType v)
{
    if(!currentToken->checkValue(ValueType::Nan))
    {
        if(checkExpression(v))
        {
            if(b)
            {
                checkEol(true);
            }
            return true;
        }
        return false;
    }
    return false;
}

bool Parser::checkIf()
{
    if(currentToken->checkName("if"))
    {
        getToken();
        currentToken->setValue(ValueType::Nan);
        qint32 i = currentIndex;
        Token *t;
        bool hasE = false;
        while(!currentToken->checkType(TokenType::Eol))
        {
            if(currentToken->checkName(")"))
            {
                hasE = true;
                t=currentToken;
            }
            if(!getToken())
                break;
        }
        if(hasE)
            t->setValue(ValueType::Nan);
        else
            errors->append(Error(0, "\")\"", currentToken->getLine(), currentToken->getColumn()));

        currentIndex = i;
        currentToken = &(*tokens)[currentIndex];

        if(currentToken->checkName("("))
            getToken();
        else
            errors->append(Error(0, "\"(\"", currentToken->getLine(), currentToken->getColumn()));



        if(!checkExpression(ValueType::Boolean))
            errors->append(Error(3,"condition", currentToken->getLine(), currentToken->getColumn()));

        if(!currentToken->checkName(")"))
            errors->append(Error(0, ")",currentToken->getLine(), currentToken->getColumn()));
        getToken();

        checkEol(false);
        while(checkEmptyLine())
            getToken();
        if(!checkBlockStat())
            errors->append(Error(3, "block statement",currentToken->getLine(), currentToken->getColumn()));

        while(checkEmptyLine() && currentIndex<tokens->size()-1)
            getToken();
        checkElse();
        return true;
    }

    return false;
}

bool Parser::checkElse()
{
    if(currentToken->checkName("else"))
    {
        getToken();

        while(checkEmptyLine())
            getToken();
        if(currentToken->checkName("if"))
            checkIf();
        else
        {
            while(checkEmptyLine())
                getToken();
            if(!checkBlockStat())
                errors->append(Error(3, "block statement",currentToken->getLine(), currentToken->getColumn()));
        }
        return true;
    }
    return false;
}

bool Parser::checkFor()
{
    if(currentToken->checkName("for"))
    {
        getToken();
        currentToken->setValue(ValueType::Nan);
        qint32 i = currentIndex;
        Token *t;
        bool hasE = false;
        while(!currentToken->checkType(TokenType::Eol))
        {
            if(currentToken->checkName(")"))
            {
                hasE = true;
                t=currentToken;
            }
            if(!getToken())
                break;
        }
        if(hasE)
            t->setValue(ValueType::Nan);
        else
            errors->append(Error(0, "\")\"", currentToken->getLine(), currentToken->getColumn()));

        currentIndex = i;
        currentToken = &(*tokens)[currentIndex];

        if(currentToken->checkName("("))
        {
            getToken();
            if(checkDeclareVarStat(false))
            {
                //getToken();
                if(currentToken->checkName(";"))
                {
                    getToken();
                }
                else
                    errors->append(Error(0, "\";\"", currentToken->getLine(), currentToken->getColumn()));
            }
            else if(checkAssignStat(false))
            {
                //getToken();
                if(currentToken->checkName(";"))
                {
                    getToken();
                }
                else
                    errors->append(Error(0, "\";\"", currentToken->getLine(), currentToken->getColumn()));
            }
            else
            {
                errors->append(Error(3, "initialize statement", currentToken->getLine(), currentToken->getColumn()));
                do
                {
                    getToken();
                } while(currentToken->checkName(";"));

            }

            if(checkExpression(ValueType::Boolean))
            {
                //getToken();
                if(currentToken->checkName(";"))
                {
                    getToken();
                }
                else
                    errors->append(Error(0, "\";\"", currentToken->getLine(), currentToken->getColumn()));
            }
            else
            {
                errors->append(Error(3, "condition", currentToken->getLine(), currentToken->getColumn()));
                do
                {
                    getToken();
                } while(currentToken->checkName(";"));

            }

            if(checkAssignStat(false))
            {
                //getToken();
                if(currentToken->checkName(")"))
                {
                    getToken();
                }
            }
            else if(checkExpressionStat(false, ValueType::Void))
            {
                //getToken();
                if(currentToken->checkName(")"))
                {
                    getToken();
                }
            }
            else
            {
                getToken();
                errors->append(Error(3, "afterthought", currentToken->getLine(), currentToken->getColumn()));
            }
        }
        else
        {
            errors->append(Error(0, "\"(\"", currentToken->getLine(), currentToken->getColumn()));
        }

        checkEol(false);
        while(checkEmptyLine())
            getToken();
        if(!checkBlockStat())
            errors->append(Error(3, "block statement",currentToken->getLine(), currentToken->getColumn()));
        while(checkEmptyLine() && currentIndex<tokens->size()-1)
            getToken();
        return true;
    }
    return false;
}

bool Parser::checkWhile()
{
    if(currentToken->checkName("while"))
    {
        getToken();
        currentToken->setValue(ValueType::Nan);
        qint32 i = currentIndex;
        Token *t;
        bool hasE = false;
        while(!currentToken->checkType(TokenType::Eol))
        {
            if(currentToken->checkName(")"))
            {
                hasE = true;
                t=currentToken;
            }
            if(!getToken())
                break;
        }
        if(hasE)
            t->setValue(ValueType::Nan);
        else
            errors->append(Error(0, "\")\"", currentToken->getLine(), currentToken->getColumn()));

        currentIndex = i;
        currentToken = &(*tokens)[currentIndex];

        if(currentToken->checkName("("))
            getToken();
        else
            errors->append(Error(0, "\"(\"", currentToken->getLine(), currentToken->getColumn()));



        if(!checkExpression(ValueType::Boolean))
            errors->append(Error(3,"condition", currentToken->getLine(), currentToken->getColumn()));

        if(!currentToken->checkName(")"))
            errors->append(Error(0, ")",currentToken->getLine(), currentToken->getColumn()));
        getToken();

        checkEol(false);
        while(checkEmptyLine())
            getToken();
        if(!checkBlockStat())
            errors->append(Error(3, "block statement",currentToken->getLine(), currentToken->getColumn()));
        return true;
    }

    return false;
}

bool Parser::checkDo()
{
    if(currentToken->checkName("do"))
    {
        getToken();
        checkEol(false);
        while(checkEmptyLine())
            getToken();
        if(!checkBlockStat())
            errors->append(Error(3, "block statement",currentToken->getLine(), currentToken->getColumn()));

        while(checkEmptyLine() && currentIndex<tokens->size()-1)
            getToken();
        if(currentToken->checkName("while"))
        {
            getToken();
            currentToken->setValue(ValueType::Nan);
            qint32 i = currentIndex;
            Token *t;
            bool hasE = false;
            while(!currentToken->checkType(TokenType::Eol))
            {
                if(currentToken->checkName(")"))
                {
                    hasE = true;
                    t=currentToken;
                }
                if(!getToken())
                    break;
            }
            if(hasE)
                t->setValue(ValueType::Nan);
            else
                errors->append(Error(0, "\")\"", currentToken->getLine(), currentToken->getColumn()));

            currentIndex = i;
            currentToken = &(*tokens)[currentIndex];

            if(currentToken->checkName("("))
                getToken();
            else
                errors->append(Error(0, "\"(\"", currentToken->getLine(), currentToken->getColumn()));



            if(!checkExpression(ValueType::Boolean))
                errors->append(Error(3,"condition", currentToken->getLine(), currentToken->getColumn()));

            if(!currentToken->checkName(")"))
                errors->append(Error(0, ")",currentToken->getLine(), currentToken->getColumn()));


            //getToken();
            checkEol(true);
        }
        else
            errors->append(Error(0, "while",currentToken->getLine(), currentToken->getColumn()));
        return true;
    }
    return false;
}

bool Parser::checkReturn(ValueType v)
{
    if(currentToken->checkName("return"))
    {
        getToken();
        if(v == ValueType::Void)
        {
            checkEol(true);
        }
        else
        {
            if(checkExpression(v))
            {
                checkEol(true);
            }
            else
                errors->append(Error(3, "return", currentToken->getLine(), currentToken->getColumn()));
        }
        return true;
    }
    return false;
}

bool Parser::checkFuncBlock()
{
    if(!currentToken->checkName("{"))
    {
        errors->append(Error(0,"\"{\"", currentToken->getLine(), currentToken->getColumn()));
        checkEol(false);
    }
    else
        getToken();


    while(!currentToken->checkName("}"))
    {
        if(currentToken->checkType(TokenType::Eol))
        {
            if(!getToken())
                break;
            continue;
        }
        if(!checkDeclareVarStat(true))
            if(!checkAssignStat(true))
                if(!checkExpressionStat(true, ValueType::Void))
                    if(!checkIf())
                        if(!checkFor())
                            if(!checkDo())
                                if(!checkWhile())
                                    if(checkReturn(funcValue))
                                        isReturned = true;
                                    else if(!checkEmptyLine())
                                        {
                                            errors->append(Error(3,"statement", currentToken->getLine(), currentToken->getColumn()));
                                            while(!currentToken->checkType(TokenType::Eol))
                                            getToken();
                                        }

    }
    if(currentIndex == tokens->size() -1)
        errors->append(Error(0,"\"}\"", currentToken->getLine(), currentToken->getColumn()));
    else
    {
        getToken();
        checkEol(false);
    }

    return true;

    return false;
}

bool Parser::checkBlockStat()
{
    if(currentToken->checkName("{") && getToken())
    {
        bool hasStat = false;
        while(!currentToken->checkName("}"))
        {
            if(currentToken->checkType(TokenType::Eol))
            {
                if(!getToken())
                    break;
                continue;
            }
            if(checkDeclareVarStat(true))
                hasStat = true;
                else if(checkAssignStat(true))
                hasStat = true;
                    else if(checkExpressionStat(true, ValueType::Void))
                    hasStat = true;
                        else if(checkIf())
                        hasStat = true;
                            else if(checkFor())
                            hasStat = true;
                                else if(checkDo())
                                hasStat = true;
                                    else if(checkWhile())
                                    hasStat = true;
                                        else if(checkReturn(funcValue))
                                        hasStat = true;
                                            else if(checkEmptyLine())
                                            hasStat = true;
                                                else
                                                {
                                                    errors->append(Error(3,"statement", currentToken->getLine(), currentToken->getColumn()));
                                                    while(!currentToken->checkType(TokenType::Eol))
                                                    getToken();
                                                }

        }
        if(!hasStat)
            errors->append(Error(0,"statement in block", currentToken->getLine(), currentToken->getColumn()));
        if(currentIndex == tokens->size() -1)
            errors->append(Error(0,"\"}\"", currentToken->getLine(), currentToken->getColumn()));
        else
        {
            getToken();
            checkEol(false);
        }

        return true;
    }
    else if(currentIndex<tokens->size()-1)
    {
        if(!checkDeclareVarStat(true))
            if(!checkAssignStat(true))
                if(!checkExpressionStat(true, ValueType::Void))
                    if(!checkIf())
                        if(!checkFor())
                            if(!checkDo())
                                if(!checkWhile())
                                    if(!checkEmptyLine())
                                        if(!checkReturn(funcValue))
                                            return false;
        return true;
    }
    else
        return false;
}


bool Parser::getToken()
{
    if(currentIndex < tokens->size() - 1)
    {
        currentIndex++;
        currentToken = &(*tokens)[currentIndex];
        return true;
    }
    return false;
}

//check variable token is declared and return isConst
bool Parser::checkVariable(Token *t)
{
    for(int i=0; i<declaredVariables->size();i++)
    {
        DeclaredVariable v = declaredVariables->at(i);
        if(t->checkName(v.getName()))
        {
            t->setValue(v.getValue());
            return v.getIsConst();
        }
    }
    return false;
}
