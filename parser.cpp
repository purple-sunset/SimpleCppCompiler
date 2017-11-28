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
    errors = new QList<Error>();
    checkingIndex = 0;
    stack = new QStack<QString>();
    checkingToken = new Token();

}

Parser::~Parser()
{
    delete tokens;
    delete declaredVariables;
    delete errors;
    delete checkingToken;
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
            tmpToken->createSymbol("eol");
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
    while (checkingIndex < tokens->size())
    {

        //check for empty line
        if(checkForEmptyLine())
            continue;

        //check for #include
        if(checkForInclude())
        {
            //if(!checkForEol())
               errors->append(Error("Missing \";\"", checkingToken->getLine(), checkingToken->getColumn()));
            continue;
        }
    }
}

bool Parser::checkForEmptyLine()
{
    getTokenWithSpace();
    if(checkingToken->checkType(TokenType::Eol))
        return true;
    return false;
}

bool Parser::checkForInclude()
{
    getTokenWithSpace();
    if(checkingToken->checkName("#"))
    {
        getTokenWithSpace();
        if(checkingToken->checkName("include"))
        {
            getTokenWithSpace();
            if(checkingToken->checkName("<"))
            {
                stack->push("<");
                getTokenWithSpace();
                if(checkingToken->checkType(TokenType::Variable))
                {
                    getTokenWithSpace();
                    if(checkingToken->checkName(">"))
                       stack->pop();
                    else
                        errors->append(Error("Missing \">\"", checkingToken->getLine(), checkingToken->getColumn()));
                }
                else
                    errors->append(Error("Wrong header", checkingToken->getLine(), checkingToken->getColumn()));
            }
            else if(checkingToken->checkName("\""))
            {
                stack->push("\"");
                getTokenWithSpace();
                if(checkingToken->checkType(TokenType::Variable))
                {
                    getTokenWithSpace();
                    if(checkingToken->checkName("."))
                    {
                        getTokenWithSpace();
                        if(checkingToken->checkName("h"))
                        {
                            getTokenWithSpace();
                            if(checkingToken->checkName("\""))
                               stack->pop();
                            else
                                errors->append(Error("Missing \"\"\"", checkingToken->getLine(), checkingToken->getColumn()));
                        }
                        else
                            errors->append(Error("Wrong header", checkingToken->getLine(), checkingToken->getColumn()));
                    }
                    else
                        errors->append(Error("Wrong header", checkingToken->getLine(), checkingToken->getColumn()));
                }
                else
                    errors->append(Error("Wrong header", checkingToken->getLine(), checkingToken->getColumn()));
            }
            else
                errors->append(Error("Missing \"<\" or \"\"\"", checkingToken->getLine(), checkingToken->getColumn()));
        }
        else
            errors->append(Error("Missing \"include\"", checkingToken->getLine(), checkingToken->getColumn()));
        return true;
    }
    return false;
}

bool Parser::checkForEol()
{
    getTokenWithSpace();
    if(checkingToken->checkName(";"))
    {
        getTokenWithSpace();
        if(!checkingToken->checkType(TokenType::Eol))
            errors->append(Error("Missing End of Line", checkingToken->getLine(), checkingToken->getColumn()));
        return true;
    }
    return false;

}

void Parser::getTokenWithSpace()
{
    if(checkingIndex < tokens->size())
    {
        *checkingToken = tokens->at(checkingIndex);
        if(checkingToken->checkType(TokenType::Space))
        {
            checkingIndex++;
            if(checkingIndex < tokens->size())
                *checkingToken = tokens->at(checkingIndex);
        }
        checkingIndex++;
    }
}

void Parser::getTokenWithoutSpace()
{
    if(checkingIndex < tokens->size())
    {
        *checkingToken = tokens->at(checkingIndex);
        checkingIndex++;
    }
}

bool Parser::checkForSpace()
{
    if(checkingIndex < tokens->size())
    {
        *checkingToken = tokens->at(checkingIndex);
        checkingIndex++;
        return checkingToken->checkType(TokenType::Space);
    }
    return false;

}
