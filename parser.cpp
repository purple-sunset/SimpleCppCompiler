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
    stack = new QStack<QString>();
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
    delete stack;
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
        if(text[i] == ' '){
            tmpString->append(text[i]);
            tmpToken->setPosition(l, c);
            i++;
            c++;
            while(text[i] == ' '){
                tmpString->append(text[i]);
                i++;
                c++;
            }
            tmpToken->createSpace(*tmpString);
        }
        //check for variable or keyword
        else if(text[i].isLetter() || text[i] == '_')
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
            i++;
            c++;
        }

        //add token to list
        tokens->append(*tmpToken);
    }
}

void Parser::check()
{
    getTokenWithSpace();
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
            getTokenWithSpace();
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
        getTokenWithSpace();
        if(currentToken->checkName("include"))
        {
            getTokenWithSpace();
        }
        else
            errors->append(Error(0, "\"include\"", currentToken->getLine(), currentToken->getColumn()));

        if(currentToken->checkName("<"))
        {
            getTokenWithSpace();
            if(currentToken->checkType(TokenType::Variable))
            {
                getTokenWithSpace();
            }
            else
                errors->append(Error(0, "header name", currentToken->getLine(), currentToken->getColumn()));

            if(currentToken->checkName(">"))
            {
                getTokenWithSpace();
            }
            else
                errors->append(Error(0, "\">\"", currentToken->getLine(), currentToken->getColumn()));
        }
        else if(currentToken->checkName("\""))
        {
            getTokenWithSpace();
            if(currentToken->checkType(TokenType::Variable))
            {
                getTokenWithSpace();
                if(currentToken->checkName("."))
                {
                    getTokenWithSpace();
                    if(currentToken->checkName("h"))
                    {
                        getTokenWithSpace();
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
                getTokenWithSpace();
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
            if(currentToken->checkName(";"))
            {
                hasE = true;
                getTokenWithSpace();
                break;
            }
            errors->append(Error(1, "\"" + currentToken->getName() + "\"", currentToken->getLine(), currentToken->getColumn()));
            getTokenWithSpace();
        }

        while (!currentToken->checkType(TokenType::Eol))
        {
            errors->append(Error(1, "\"" + currentToken->getName() + "\"", currentToken->getLine(), currentToken->getColumn()));
            getTokenWithSpace();
        }

        if(!hasE)
            errors->append(Error(0, "\";\"", currentToken->getLine(), currentToken->getColumn()));
        return true;
    }
    else
    {
        while(!currentToken->checkType(TokenType::Eol))
        {
            errors->append(Error(1, "\"" + currentToken->getName() + "\"", currentToken->getLine(), currentToken->getColumn()));
            getTokenWithSpace();
        }
        getTokenWithSpace();
        return true;
    }
    return false;

}

bool Parser::checkDeclareFuncStat()
{
    if(currentToken->checkType(TokenType::Keyword) && !currentToken->checkValue(ValueType::Nan))
    {
        ValueType v = currentToken->getValue();

        getTokenWithSpace();
        if(currentToken->checkType(TokenType::Variable))
        {
            getTokenWithSpace();
            declaredFunctions->append(DeclaredFunction(currentToken->getName(), v));
            isInFunction = true;
            funcValue = v;
        }

        else
            errors->append(Error(0, "function name", currentToken->getLine(), currentToken->getColumn()));

        if(currentToken->checkName("("))
            getTokenWithSpace();
        else
            errors->append(Error(0, "\"(\"", currentToken->getLine(), currentToken->getColumn()));

        if(currentToken->checkType(TokenType::Keyword) && !currentToken->checkValue(ValueType::Nan))
        {
            ValueType v = currentToken->getValue();
            getTokenWithSpace();
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


                while (getTokenWithSpace() && (currentToken->checkName(",")))
                {
                    getTokenWithSpace();
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
            }
        }
        else if(currentToken->checkName(")"))
            getTokenWithSpace();
        else
            errors->append(Error(0, "\")\"", currentToken->getLine(), currentToken->getColumn()));

        checkEol(false);
        return true;
    }
    return false;
}


ValueType Parser::checkDeclareVarStat()
{
    bool b = false;
    if(currentToken->checkName("const"))
    {
        b=true;
        getTokenWithSpace();
    }
    if(currentToken->checkType(TokenType::Keyword) && !currentToken->checkValue(ValueType::Nan))
    {
        ValueType v = currentToken->getValue();
        getTokenWithSpace();
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

            getTokenWithSpace();
            if(currentToken->checkName("="))
            {
                getTokenWithSpace();
                if(!checkExpression(v))
                    errors->append(Error(static_cast<qint32>(v)+5, "Expression", currentToken->getLine(), currentToken->getColumn()));


            }
            else
            {
                while ((currentToken->checkName(",")))
                {
                    getTokenWithSpace();
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
                    getTokenWithSpace();
                }
            }


            checkEol(true);
            return v;
        }
        else
            errors->append(Error(0, "variable name", currentToken->getLine(), currentToken->getColumn()));

    }
    return ValueType::Nan;
}

bool Parser::checkAssignStat(bool b)
{
    if(currentToken->checkType(TokenType::Variable))
    {
        qint32 i = currentIndex;
        Token *t = currentToken;
        getTokenWithSpace();
        if(currentToken->checkName("="))
        {
            if(checkVariable(t))
                errors->append(Error(4, "\"" + t->getName() + "\"", t->getLine(), t->getColumn()));
            if(t->checkValue(ValueType::Void))
                errors->append(Error(5, "\"" +  t->getName() + "\"", t->getLine(), t->getColumn()));
            getTokenWithSpace();
            if(!checkExpression(t->getValue()))
                errors->append(Error(static_cast<qint32>(t->getValue())+5, "Expression", currentToken->getLine(), currentToken->getColumn()));
            if(b)
            {
                checkEol(true);
            }
            return true;
        }
        else
        {
            currentIndex = i-1;
            getTokenWithoutSpace();
        }

        return false;
    }
    return false;
}

bool Parser::checkExpression(ValueType v)
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
        if(!getTokenWithSpace())
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

bool Parser::checkExpressionStat(bool b, ValueType v)
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

bool Parser::checkIf()
{
    if(currentToken->checkName("if"))
    {
        getTokenWithSpace();
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
            if(!getTokenWithSpace())
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
            getTokenWithSpace();
            if(!checkExpression(ValueType::Boolean))
                errors->append(Error(3,"condition", currentToken->getLine(), currentToken->getColumn()));
            if(!currentToken->checkName(")"))
                errors->append(Error(0, ")",currentToken->getLine(), currentToken->getColumn()));
            getTokenWithSpace();
        }

        else if(currentToken->checkType(TokenType::Variable) || currentToken->checkType(TokenType::Literal))
        {
            qint32 i2 = currentIndex;
            getTokenWithSpace();
            if(checkEmptyLine())
            {
                currentIndex = i2;
                currentToken = &(*tokens)[currentIndex];
                if(currentToken->checkValue(ValueType::Boolean))
                {
                    getTokenWithSpace();
                    checkEol(false);
                }
                else
                    errors->append(Error(7, currentToken->getName(), currentToken->getLine(), currentToken->getColumn()));
            }
            else
            {
                currentIndex = i2;
                currentToken = &(*tokens)[currentIndex];
                errors->append(Error(0, "\"(\"", currentToken->getLine(), currentToken->getColumn()));
                if(!checkExpression(ValueType::Boolean))
                    errors->append(Error(3,"condition", currentToken->getLine(), currentToken->getColumn()));
            }

        }

        else
        {
            errors->append(Error(0, "\"(\"", currentToken->getLine(), currentToken->getColumn()));
            getTokenWithSpace();
            if(!checkExpression(ValueType::Boolean))
                errors->append(Error(3,"condition", currentToken->getLine(), currentToken->getColumn()));

        }


        checkEol(false);
        while(checkEmptyLine())
            getTokenWithSpace();
        if(!checkBlockStat())
            errors->append(Error(3, "block statement",currentToken->getLine(), currentToken->getColumn()));

        while(checkEmptyLine() && currentIndex<tokens->size()-1)
            getTokenWithSpace();
        checkElse();
        return true;
    }

    return false;
}

bool Parser::checkElse()
{
    if(currentToken->checkName("else"))
    {
        getTokenWithSpace();

        while(checkEmptyLine())
            getTokenWithSpace();
        if(currentToken->checkName("if"))
            checkIf();
        else
        {
            while(checkEmptyLine())
                getTokenWithSpace();
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
        getTokenWithSpace();
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
            if(!getTokenWithSpace())
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
            getTokenWithSpace();
            if(checkAssignStat(false))
            {
                //getTokenWithSpace();
                if(currentToken->checkName(";"))
                {
                    getTokenWithSpace();
                }
                else
                    errors->append(Error(0, "\";\"", currentToken->getLine(), currentToken->getColumn()));
            }
            else
            {
                errors->append(Error(3, "initialize statement", currentToken->getLine(), currentToken->getColumn()));
            }

            if(checkExpression(ValueType::Boolean))
            {
                //getTokenWithSpace();
                if(currentToken->checkName(";"))
                {
                    getTokenWithSpace();
                }
                else
                    errors->append(Error(0, "\";\"", currentToken->getLine(), currentToken->getColumn()));
            }
            else
            {
                errors->append(Error(3, "condition", currentToken->getLine(), currentToken->getColumn()));
            }

            if(checkExpressionStat(false, ValueType::Void) || checkExpression(ValueType::Void))
            {
                //getTokenWithSpace();
                if(currentToken->checkName(")"))
                {
                    getTokenWithSpace();
                }
            }
            else
            {
                getTokenWithSpace();
                errors->append(Error(3, "afterthought", currentToken->getLine(), currentToken->getColumn()));
            }
        }
        else
        {
            errors->append(Error(0, "\"(\"", currentToken->getLine(), currentToken->getColumn()));
        }

        checkEol(false);
        while(checkEmptyLine())
            getTokenWithSpace();
        if(!checkBlockStat())
            errors->append(Error(3, "block statement",currentToken->getLine(), currentToken->getColumn()));
        while(checkEmptyLine() && currentIndex<tokens->size()-1)
            getTokenWithSpace();
        return true;
    }
    return false;
}

bool Parser::checkWhile()
{
    if(currentToken->checkName("while"))
    {
        getTokenWithSpace();
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
            if(!getTokenWithSpace())
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
            getTokenWithSpace();
            if(!checkExpression(ValueType::Boolean))
                errors->append(Error(3,"condition", currentToken->getLine(), currentToken->getColumn()));
            if(!currentToken->checkName(")"))
                errors->append(Error(0, ")",currentToken->getLine(), currentToken->getColumn()));
            getTokenWithSpace();
        }

        else if(currentToken->checkType(TokenType::Variable) || currentToken->checkType(TokenType::Literal))
        {
            qint32 i2 = currentIndex;
            getTokenWithSpace();
            if(checkEmptyLine())
            {
                currentIndex = i2;
                currentToken = &(*tokens)[currentIndex];
                if(currentToken->checkValue(ValueType::Boolean))
                {
                    getTokenWithSpace();
                    checkEol(false);
                }
                else
                    errors->append(Error(7, currentToken->getName(), currentToken->getLine(), currentToken->getColumn()));
            }
            else
            {
                currentIndex = i2;
                currentToken = &(*tokens)[currentIndex];
                errors->append(Error(0, "\"(\"", currentToken->getLine(), currentToken->getColumn()));
                if(!checkExpression(ValueType::Boolean))
                    errors->append(Error(3,"condition", currentToken->getLine(), currentToken->getColumn()));
            }

        }

        else
        {
            errors->append(Error(0, "\"(\"", currentToken->getLine(), currentToken->getColumn()));
            getTokenWithSpace();
            if(!checkExpression(ValueType::Boolean))
                errors->append(Error(3,"condition", currentToken->getLine(), currentToken->getColumn()));

        }


        checkEol(false);
        while(checkEmptyLine())
            getTokenWithSpace();
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
        getTokenWithSpace();
        checkEol(false);
        while(checkEmptyLine())
            getTokenWithSpace();
        if(!checkBlockStat())
            errors->append(Error(3, "block statement",currentToken->getLine(), currentToken->getColumn()));

        while(checkEmptyLine() && currentIndex<tokens->size()-1)
            getTokenWithSpace();
        if(currentToken->checkName("while"))
        {
            getTokenWithSpace();
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
                if(!getTokenWithSpace())
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
                getTokenWithSpace();
                if(!checkExpression(ValueType::Boolean))
                    errors->append(Error(3,"condition", currentToken->getLine(), currentToken->getColumn()));
                if(!currentToken->checkName(")"))
                    errors->append(Error(0, ")",currentToken->getLine(), currentToken->getColumn()));
                getTokenWithSpace();
            }

            else if(currentToken->checkType(TokenType::Variable) || currentToken->checkType(TokenType::Literal))
            {
                qint32 i2 = currentIndex;
                getTokenWithSpace();
                if(checkEmptyLine())
                {
                    currentIndex = i2;
                    currentToken = &(*tokens)[currentIndex];
                    if(currentToken->checkValue(ValueType::Boolean))
                    {
                        getTokenWithSpace();
                        checkEol(false);
                    }
                    else
                        errors->append(Error(7, currentToken->getName(), currentToken->getLine(), currentToken->getColumn()));
                }
                else
                {
                    currentIndex = i2;
                    currentToken = &(*tokens)[currentIndex];
                    errors->append(Error(0, "\"(\"", currentToken->getLine(), currentToken->getColumn()));
                    if(!checkExpression(ValueType::Boolean))
                        errors->append(Error(3,"condition", currentToken->getLine(), currentToken->getColumn()));
                }

            }

            else
            {
                errors->append(Error(0, "\"(\"", currentToken->getLine(), currentToken->getColumn()));
                getTokenWithSpace();
                if(!checkExpression(ValueType::Boolean))
                    errors->append(Error(3,"condition", currentToken->getLine(), currentToken->getColumn()));

            }

            //getTokenWithSpace();
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
        getTokenWithSpace();
        if(v == ValueType::Void)
        {
            getTokenWithSpace();
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
    if(currentToken->checkName("{") && getTokenWithSpace())
    {
        while(!currentToken->checkName("}"))
        {
            if(currentToken->checkType(TokenType::Eol))
            {
                if(!getTokenWithSpace())
                    break;
                continue;
            }
            if(checkDeclareVarStat() == ValueType::Nan)
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
                                                getTokenWithSpace();
                                            }

        }
        if(currentIndex == tokens->size() -1)
            errors->append(Error(0,"\"}\"", currentToken->getLine(), currentToken->getColumn()));
        else
        {
            getTokenWithSpace();
            checkEol(false);
        }

        return true;
    }
    return false;
}

bool Parser::checkBlockStat()
{
    if(currentToken->checkName("{") && getTokenWithSpace())
    {
        bool hasStat = false;
        while(!currentToken->checkName("}"))
        {
            if(currentToken->checkType(TokenType::Eol))
            {
                if(!getTokenWithSpace())
                    break;
                continue;
            }
            if(checkDeclareVarStat() != ValueType::Nan)
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
                                                    getTokenWithSpace();
                                                }

        }
        if(!hasStat)
            errors->append(Error(0,"statement in block", currentToken->getLine(), currentToken->getColumn()));
        if(currentIndex == tokens->size() -1)
            errors->append(Error(0,"\"}\"", currentToken->getLine(), currentToken->getColumn()));
        else
        {
            getTokenWithSpace();
            checkEol(false);
        }

        return true;
    }
    else if(currentIndex<tokens->size()-1)
    {
        if(checkDeclareVarStat() == ValueType::Nan)
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

bool Parser::getTokenWithSpace()
{
    if(currentIndex < tokens->size() - 1)
    {
        currentIndex++;
        currentToken = &(*tokens)[currentIndex];
        if(currentToken->checkType(TokenType::Space) && currentIndex < tokens->size() - 1)
        {
            currentIndex++;
            currentToken = &(*tokens)[currentIndex];
        }
        return true;
    }
    return false;
}

bool Parser::getTokenWithoutSpace()
{
    if(currentIndex < tokens->size() - 1)
    {
        currentIndex++;
        currentToken = &(*tokens)[currentIndex];
        return true;
    }
    return false;
}

bool Parser::checkSpace()
{
    if(currentIndex < tokens->size() - 1)
    {
        currentIndex++;
        currentToken = &(*tokens)[currentIndex];
        return currentToken->checkType(TokenType::Space);
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
