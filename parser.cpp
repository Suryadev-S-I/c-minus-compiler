#include "parser.hpp"
#include <utility>


bool PARSER::match(TokenType tokentype)
{   
    if (tokens[current].type == tokentype)
    {
        current++;
        return true;
    }
    else{ return false; }
}

bool PARSER::match(const std::vector<TokenType>& tokentypes)
{
    for(TokenType tokentype: tokentypes)
    {
        if (tokens.at(current).type == tokentype)
        {
            current++;
            return true;
        }
    }
    return false;
}

TOKEN PARSER::previous()
{
    return tokens.at(current -1);
}

std::unique_ptr<ASTnode> PARSER::expression()
{
    return equality(); //variable declaration to be added
}

std::unique_ptr<ASTnode> PARSER::equality()
{
    std::unique_ptr<ASTnode> expr{comparison()};
    while (match({EQUAL_EQUAL, NOT_EQUAL}))
    {
        TOKEN token{previous()};
        std::unique_ptr<ASTnode> right{comparison()};
        expr = std::make_unique<BinaryOp>(expr,token,right);
    }
    return expr;
}

std::unique_ptr<ASTnode> PARSER::comparison()
{
    std::unique_ptr<ASTnode>expr{term()};
    TOKEN token{};
    while(match(std::vector<TokenType>{GREATER, GREATER_EQUAL, LESS, LESS_EQUAL}))
    {
        token = previous();
        std::unique_ptr<ASTnode>right{term()};
        expr = std::move(std::make_unique<BinaryOp>(expr, token, right));//move might be unnecessary
    }
    return expr;
}
 
std::unique_ptr<ASTnode> PARSER::term() 
{
    std::unique_ptr<ASTnode> expr{factor()};
    while(match({MINUS,PLUS}))
    {
        TOKEN token{previous()};
        std::unique_ptr<ASTnode> right{factor()};
        expr = std::make_unique<BinaryOp>(expr, token, right);
    }
    return expr;
}

std::unique_ptr<ASTnode> PARSER::factor()
{
    std::unique_ptr<ASTnode> expr{unary()};
    while(match({SLASH,STAR}))
    {
        TOKEN token{previous()};
        std::unique_ptr<ASTnode> right{unary()};
        expr = std::make_unique<BinaryOp>(expr, token, right);
    }
    return expr;
}

std::unique_ptr<ASTnode> PARSER::unary()
{
    if(match(MINUS))
    {
        TOKEN token{previous()};
        std::unique_ptr<ASTnode> expr{primary()};
        return std::make_unique<UnaryOp>(token, expr);
    }else
    {
        return primary();
    }
}

std::unique_ptr<ASTnode> PARSER::primary()
{
    if(match(INTEGER))
    {
        return std::make_unique<NumberNode>(previous());
    }else if (match(LEFT_PAREN))
    {
        std::unique_ptr<ASTnode> expr {expression()};
        if(match(RIGHT_PAREN)) { return expr; }
        else
        {
            std::cout<< "\nError in primary()\n";
            std::exit(1);
        }//do something to avoid warning about a lack of return, assert won't work
    }
    
}
