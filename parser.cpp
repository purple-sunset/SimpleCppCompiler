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
}

Parser::~Parser()
{
    delete tokens;
    delete declaredVariables;
    delete declaredFunctions;
    delete errors;
    delete currentToken;
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
    while (currentIndex < tokens->size() - 1)
    {
        getTokenWithSpace();

        if(!isInFunction)
        {
            //check for #include
            if(checkInclude())
            {
                if(!currentToken->checkType(TokenType::Eol))
                    getTokenWithSpace();
                    while(!currentToken->checkType(TokenType::Eol))
                    {
                        errors->append(Error(1, currentToken->getName(), currentToken->getLine(), currentToken->getColumn()));
                        getTokenWithSpace();
                    }
                continue;
            }
        }

        //check for function
        if(checkDeclareFuncStat())
        {
            isInFunction = true;
            if(!checkBlockStat(true))
                errors->append(Error(3, "block statement", currentToken->getLine(), currentToken->getColumn()));
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
            if(currentToken->checkName("<"))
            {
                stack->push("<");
                getTokenWithSpace();
                if(currentToken->checkType(TokenType::Variable))
                {
                    getTokenWithSpace();
                    while(!currentToken->checkName(">"))
                    {
                        if(currentToken->checkType(TokenType::Eol) || currentIndex == tokens->size() - 1)
                        {
                            stack->pop();
                            errors->append(Error(0, "\">\"", currentToken->getLine(), currentToken->getColumn()));
                            return true;
                        }
                        errors->append(Error(1, currentToken->getName(), currentToken->getLine(), currentToken->getColumn()));
                        getTokenWithSpace();
                    }
                    stack->pop();
                }
                else
                    errors->append(Error(0, "header name", currentToken->getLine(), currentToken->getColumn()));
            }
            else if(currentToken->checkName("\""))
            {
                stack->push("\"");
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
                            while(!currentToken->checkName("\""))
                            {
                                if(currentToken->checkType(TokenType::Eol) || currentIndex == tokens->size() - 1)
                                {
                                    stack->pop();
                                    errors->append(Error(0, "\"\"\"", currentToken->getLine(), currentToken->getColumn()));
                                    return true;
                                }
                                errors->append(Error(1, currentToken->getName(), currentToken->getLine(), currentToken->getColumn()));
                                getTokenWithSpace();
                            }
                            stack->pop();
                        }
                        else
                            errors->append(Error(0, "header name", currentToken->getLine(), currentToken->getColumn()));
                    }
                    else
                        errors->append(Error(0, "header name", currentToken->getLine(), currentToken->getColumn()));
                }
                else
                    errors->append(Error(0, "header name", currentToken->getLine(), currentToken->getColumn()));
            }
            else
                errors->append(Error(0, "\"<\" or \"\"\"", currentToken->getLine(), currentToken->getColumn()));
        }
        else
            errors->append(Error(0, "\"include\"", currentToken->getLine(), currentToken->getColumn()));
        return true;
    }
    return false;
}

bool Parser::checkEol()
{
    if(currentToken->checkName(";"))
    {
        getTokenWithSpace();
        while(!currentToken->checkType(TokenType::Eol))
        {
            errors->append(Error(1, currentToken->getName(), currentToken->getLine(), currentToken->getColumn()));
        }

        return true;
    }
    errors->append(Error(0, "\";\"", currentToken->getLine(), currentToken->getColumn()));
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
            QString *s = new QString(currentToken->getName());
            getTokenWithSpace();
            if(currentToken->checkName("("))
            {
                getTokenWithSpace();
                if(currentToken->checkName(")"))
                {
                    getTokenWithSpace();
                    while(!currentToken->checkType(TokenType::Eol))
                    {
                        errors->append(Error(1, currentToken->getName(), currentToken->getLine(), currentToken->getColumn()));
                        getTokenWithSpace();
                    }
                    declaredFunctions->append(DeclaredFunction(*s, v));
                    funcValue = v;
                }
                else
                    errors->append(Error(0, "\")\"", currentToken->getLine(), currentToken->getColumn()));
            }
            else
                errors->append(Error(0, "\"(\"", currentToken->getLine(), currentToken->getColumn()));
        }
        else
            errors->append(Error(0, "function name", currentToken->getLine(), currentToken->getColumn()));
        return true;
    }
    return false;
}

ValueType Parser::checkDeclareVarStat()
{
    bool b = false;
    if(currentToken->checkType(TokenType::Keyword) && !currentToken->checkValue(ValueType::Nan))
    {
        ValueType v = currentToken->getValue();
        getTokenWithSpace();
        if(currentToken->checkType(TokenType::Variable))
        {
            QString *s = new QString(currentToken->getName());
            getTokenWithSpace();
            checkEol();
            declaredVariables->append(DeclaredVariable(*s, v, b));
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
                errors->append(Error(4, t->getName(), t->getLine(), t->getColumn()));
            if(t->checkValue(ValueType::Nan))
                errors->append(Error(5, t->getName(), t->getLine(), t->getColumn()));
            getTokenWithSpace();
            if(checkExpression(t->getValue()))
                errors->append(Error(static_cast<qint32>(t->getValue()), currentToken->getName(), currentToken->getLine(), currentToken->getColumn()));
            if(b)
            {
                getTokenWithSpace();
                checkEol();
            }
            return true;
        }
        else
            currentIndex = i;
        return false;
    }
    return false;
}

bool Parser::checkExpression(ValueType v)
{
    ValueType minV = ValueType::Nan;
    ValueType maxV = ValueType::Nan;
    QStack<Token> *s = new QStack<Token>();
    bool isValid = true;
    while(!currentToken->checkType(TokenType::Eol))
    {
        if(currentToken->checkName("("))
        {
            s->push(*currentToken);
        }
        else if(currentToken->checkType(TokenType::Variable))
        {
            if(checkVariable(currentToken))
                errors->append(Error(4, currentToken->getName(), currentToken->getLine(), currentToken->getColumn()));
            if(s->size()>0)
            {
                Token t = s->pop();
                if(t.checkName("("))
                {
                    s->push(t);
                    s->push(*currentToken);
                    minV = EnumMath::Min(minV, currentToken->getValue());
                    maxV = EnumMath::Min(minV, currentToken->getValue());
                }
                else
                {
                    isValid = false;
                    errors->append(Error(3,"expression", currentToken->getLine(), currentToken->getColumn()));
                    break;
                }
            }

        }
        else if(currentToken->checkType(TokenType::Literal))
        {
            if(s->size()>0)
            {
                Token t = s->pop();
                if(t.checkName("("))
                {
                    s->push(t);
                    s->push(*currentToken);
                    minV = EnumMath::Min(minV, currentToken->getValue());
                    maxV = EnumMath::Min(minV, currentToken->getValue());
                }
                else
                {
                    isValid = false;
                    errors->append(Error(3,"expression", currentToken->getLine(), currentToken->getColumn()));
                    break;
                }
            }
        }
        else if (   currentToken->checkName("+") || currentToken->checkName("-") || currentToken->checkName("*") || currentToken->checkName("/") || currentToken->checkName("%")
                 || currentToken->checkName("<") || currentToken->checkName(">") || currentToken->checkName("==") || currentToken->checkName("!=") || currentToken->checkName("<=")
                 || currentToken->checkName(">=") || currentToken->checkName("||") || currentToken->checkName("&&"))
        {
            if(s->size()>0)
            {
                Token t = s->pop();
                if(t.checkType(TokenType::Literal) || t.checkType(TokenType::Variable))
                {
                    minV = EnumMath::Min(minV, currentToken->getValue());
                    maxV = EnumMath::Min(minV, currentToken->getValue());
                }
                else
                {
                    isValid = false;
                    errors->append(Error(3,"expression", currentToken->getLine(), currentToken->getColumn()));
                    break;
                }

            }
        }
        else
        {
            isValid = false;
            errors->append(Error(3,"expression", currentToken->getLine(), currentToken->getColumn()));
            break;
        }
    }
    if(isValid)
    {
        if(v == ValueType::Boolean)
            return (minV == ValueType::Boolean) && (maxV == ValueType::Boolean);
        else
            return (minV > ValueType::Boolean) && (minV <= v) && (maxV >= v);

    }
    return false;
}

bool Parser::checkIf()
{
    if(currentToken->checkName("if"))
    {
        getTokenWithSpace();
        if(checkExpression(ValueType::Boolean))
        {
            getTokenWithSpace();
            if(!checkEmptyLine())
                while(!currentToken->checkType(TokenType::Eol))
                {
                    errors->append(Error(1, currentToken->getName(), currentToken->getLine(), currentToken->getColumn()));
                    getTokenWithSpace();
                }
            if(!checkBlockStat(false))
                errors->append(Error(3, "block statement", currentToken->getLine(), currentToken->getColumn()));
            checkElse();
        }
        else
        {
            errors->append(Error(3,"condition", currentToken->getLine(), currentToken->getColumn()));
        }
        return true;
    }
    return false;
}

bool Parser::checkElse()
{
    if(currentToken->checkName("else"))
    {
        getTokenWithSpace();
        if(currentToken->checkName("if"))
            checkIf();
        else if(!checkEmptyLine())
            while(!currentToken->checkType(TokenType::Eol))
            {
                errors->append(Error(1, currentToken->getName(), currentToken->getLine(), currentToken->getColumn()));
                getTokenWithSpace();
            }
        if(!checkBlockStat(false))
            errors->append(Error(3, "block statement", currentToken->getLine(), currentToken->getColumn()));
        return true;
    }
    return false;
}

bool Parser::checkFor()
{
    if(currentToken->checkName("for"))
    {
        getTokenWithSpace();
        if(currentToken->checkName("("))
        {
            getTokenWithSpace();
            if(checkAssignStat(false))
            {
                getTokenWithSpace();
                if(currentToken->checkName(";"))
                {
                    if(checkExpression(ValueType::Boolean))
                    {
                        getTokenWithSpace();
                        if(currentToken->checkName(";"))
                        {
                            if(checkAssignStat(false))
                            {
                                getTokenWithSpace();
                                if(currentToken->checkName(")"))
                                {
                                    getTokenWithSpace();
                                    if(!checkEmptyLine())
                                        while(!currentToken->checkType(TokenType::Eol))
                                        {
                                            errors->append(Error(1, currentToken->getName(), currentToken->getLine(), currentToken->getColumn()));
                                            getTokenWithSpace();
                                        }
                                    if(!checkBlockStat(false))
                                        errors->append(Error(3, "block statement",currentToken->getLine(), currentToken->getColumn()));
                                }
                                else
                                    errors->append(Error(0, ")",currentToken->getLine(), currentToken->getColumn()));
                            }
                            else
                               errors->append(Error(0, "statement", currentToken->getLine(), currentToken->getColumn()));

                        }
                        else
                            errors->append(Error(0, "\";\"", currentToken->getLine(), currentToken->getColumn()));
                    }
                    else
                        errors->append(Error(0, "statement", currentToken->getLine(), currentToken->getColumn()));
                }
                else
                    errors->append(Error(0, "\";\"", currentToken->getLine(), currentToken->getColumn()));
            }
            else
                errors->append(Error(0, "statement", currentToken->getLine(), currentToken->getColumn()));
        }
        else
            errors->append(Error(0, "(",currentToken->getLine(), currentToken->getColumn()));

        return true;
    }
    return false;
}

bool Parser::checkWhile()
{
    if(currentToken->checkName("while"))
    {
        getTokenWithSpace();
        if(currentToken->checkName("("))
        {
            getTokenWithSpace();
            if(checkExpression(ValueType::Boolean))
            {
                getTokenWithSpace();
                if(currentToken->checkName(")"))
                {
                    getTokenWithSpace();
                    if(!checkEmptyLine())
                        while(!currentToken->checkType(TokenType::Eol))
                        {
                            errors->append(Error(1, currentToken->getName(), currentToken->getLine(), currentToken->getColumn()));
                            getTokenWithSpace();
                        }
                    if(!checkBlockStat(false))
                        errors->append(Error(3, "block statement",currentToken->getLine(), currentToken->getColumn()));
                }
                else
                    errors->append(Error(0, ")",currentToken->getLine(), currentToken->getColumn()));
            }
            else
                errors->append(Error(0, "condition", currentToken->getLine(), currentToken->getColumn()));
        }
        else
            errors->append(Error(0, "(",currentToken->getLine(), currentToken->getColumn()));
        return true;
    }
    return false;
}

bool Parser::checkDo()
{
    if(currentToken->checkName("do"))
    {
        getTokenWithSpace();
        if(!checkEmptyLine())
            while(!currentToken->checkType(TokenType::Eol))
            {
                errors->append(Error(1, currentToken->getName(), currentToken->getLine(), currentToken->getColumn()));
                getTokenWithSpace();
            }
        if(!checkBlockStat(false))
            errors->append(Error(3, "block statement",currentToken->getLine(), currentToken->getColumn()));
        getTokenWithSpace();
        if(currentToken->checkName("while"))
        {
            getTokenWithSpace();
            if(currentToken->checkName("("))
            {
                getTokenWithSpace();
                if(checkExpression(ValueType::Boolean))
                {
                    getTokenWithSpace();
                    if(currentToken->checkName(")"))
                    {
                        getTokenWithSpace();
                        if(!checkEmptyLine())
                            while(!currentToken->checkType(TokenType::Eol))
                            {
                                errors->append(Error(1, currentToken->getName(), currentToken->getLine(), currentToken->getColumn()));
                                getTokenWithSpace();
                            }
                    }
                    else
                        errors->append(Error(0, ")",currentToken->getLine(), currentToken->getColumn()));
                }
                else
                    errors->append(Error(0, "condition", currentToken->getLine(), currentToken->getColumn()));
            }
            else
                errors->append(Error(0, "(",currentToken->getLine(), currentToken->getColumn()));
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
            checkEol();
        }
        else
        {
            if(checkExpression(v))
            {
                checkEol();
            }
            else
                errors->append(Error(3, "return", currentToken->getLine(), currentToken->getColumn()));
        }
        return true;
    }
    return false;
}

bool Parser::checkBlockStat(bool isFunc)
{
    if(currentToken->checkName("{"))
    {
        bool isH = false;
        while(!currentToken->checkType(TokenType::Eol))
        {
            getTokenWithSpace();
            if(currentToken->checkName("}"))
            {
                isH = true;
                break;
            }
            if(checkDeclareVarStat() == ValueType::Nan)
                if(!checkAssignStat(true))
                    if(!checkIf())
                        if(!checkFor())
                            if(!checkDo())
                                if(!checkWhile())
                                    if(!checkReturn(funcValue))
                                        if(!checkEmptyLine())
                                        {
                                            errors->append(Error(3,"statement", currentToken->getLine(), currentToken->getColumn()));
                                            while(!currentToken->checkType(TokenType::Eol))
                                            getTokenWithSpace();
                                        }

        }
        if(!isH)
            errors->append(Error(0,"\"}\"", currentToken->getLine(), currentToken->getColumn()));
    }
    else if(!isFunc)
    {
        if(checkDeclareVarStat() == ValueType::Nan)
            if(!checkAssignStat(true))
                if(!checkIf())
                    if(!checkFor())
                        if(!checkDo())
                            if(!checkWhile())
                                if(!checkEmptyLine())
                                    if(!checkReturn(funcValue))
                                        return false;
        return true;
    }
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
