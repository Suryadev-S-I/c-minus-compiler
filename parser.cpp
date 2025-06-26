#include "parser.hpp"

//error ->factor_

bool PARSER::match(TokenType tokentype)
{
    //std::cout<<"current: "<<current<<" ";
    //std::cout << tokens[current].value <<"\n";//<<tokens[3].value;
    //std::exit(1);
    
    if (tokens[current].type == tokentype)
    {
        current++;
        return true;
    }
    else{ return false; }
}

bool PARSER::match(std::vector<TokenType> tokentypes)
{
    for(TokenType tokentype: tokentypes)
    {
        if (tokens.at(current).type == tokentype)
        {
            current++;
            return true;
        }
    }
    //std::cout << "match(tokens)\n";
    return false;
}

TOKEN PARSER::previous()
{
    return tokens.at(current -1);
}

std::unique_ptr<ASTnode> PARSER::expression()
{
    return simple_expr(); //variable declaration to be added
}

/*std::unique_ptr<ASTnode> PARSER::equality()
{
    std::unique_ptr<ASTnode> expr = comparison();

    while (match(NOT_EQUAL) || match(EQUAL_EQUAL))
    {
        TOKEN op {previous()};
        std::unique_ptr<ASTnode> right = comparison();
        expr = std::make_unique<BinaryOp>(BinaryOp {std::make_unique<BinaryOp>(expr),op, std::make_unique<BinaryOp>(right)});
    }

    return expr;
}

std::unique_ptr<ASTnode> PARSER::comparison()
{
    std::unique_ptr<ASTnode> expr{term()};
    while(match(std::vector<TokenType>{GREATER, GREATER_EQUAL, LESS, LESS_EQUAL}))
    {
        TOKEN op{previous()};
        std::unique_ptr<ASTnode> right{term()};
        expr = std::make_unique<BinaryOp>(BinaryOp {std::make_unique<BinaryOp>(expr),op, std::make_unique<BinaryOp>(right)});
    }
    return expr;
}

std::unique_ptr<ASTnode> PARSER::term()
{
    std::unique_ptr<ASTnode> expr{factor()};

    while (match(MINUS) || match(PLUS))
    {
        TOKEN op{previous()};
        std::unique_ptr<ASTnode> right{factor()};
        expr = std::make_unique<BinaryOp>(BinaryOp {std::make_unique<BinaryOp>(expr),op, std::make_unique<BinaryOp>(right)});
    }
    return expr;
}

std::unique_ptr<ASTnode> PARSER::factor()
{
    std::unique_ptr<ASTnode> expr{std::move(unary())};
    while (match(SLASH) || match(STAR))
    {
        TOKEN op{previous()};
        std::unique_ptr<ASTnode> right{std::move(unary())};
        expr =  std::move(std::make_unique<BinaryOp>( BinaryOp{expr,op, right}));
    }
    return expr;
}

std::unique_ptr<ASTnode> PARSER::unary()
{
    if(match(MINUS))
    {
        TOKEN op { previous() };
        std::unique_ptr<ASTnode> right {std::move(unary())};

        return std::unique_ptr<ASTnode> {std::make_unique<UnaryOp>(UnaryOp {op, right})};
        // ???? right is supposed to be unique_ptr<ASTnode> for Unaryop constructor
    }
    return primary();
}

std::unique_ptr<ASTnode> PARSER::primary()
{
    if (match(INTEGER))
    {
        return std::unique_ptr<ASTnode> {std::make_unique<NumberNode> (NumberNode{previous()})};
    }
    if(match(LEFT_PAREN))
    {
        std::unique_ptr<ASTnode> expr = expression();
        if(match(RIGHT_PAREN))
        {
            return expr;
        }else
        {
            //error
        }
    }
}*/

std::unique_ptr<ASTnode> PARSER::simple_expr()
{
    std::unique_ptr<ASTnode>left{std::move(arithmetic_expr())};
    std::cout<<" c ";
    TOKEN token{};
    if(match(std::vector<TokenType>{GREATER, GREATER_EQUAL, LESS, LESS_EQUAL}))
    {
        token = previous();
    }
    std::cout<<" d ";
    std::unique_ptr<ASTnode>right{std::move(arithmetic_expr())};
    //std::cout<<"  simple_expr  \n"; 
    std::cout<<" e \n";
    //BinaryOp expr {left, token, right};
    return std::unique_ptr<ASTnode>{std::make_unique<BinaryOp>(left, token, right)};
}

std::unique_ptr<ASTnode> PARSER::arithmetic_expr()
{
    if (match(MINUS))
    {
        TOKEN op {previous()};
        std::unique_ptr<ASTnode> expr {factor_()};
        return std::unique_ptr<ASTnode> {std::make_unique<UnaryOp>(UnaryOp{op, expr})};
    }else //if (match(INTEGER))
    {
        std::unique_ptr<ASTnode> left_expr {factor_()};
        //left_expr->print();
        match(PLUS)||match(MINUS); //increments current
        //std::cout<<current<<" \n";
        TOKEN op {previous()};
        std::unique_ptr<ASTnode> right_expr {factor_()};
        std::cout<<" b\n";
        //BinaryOp expr {left_expr, op, right_expr};
        auto a = std::make_unique<BinaryOp>(left_expr, op, right_expr);
        std::cout<<"j";
        return a;

    }
}

std::unique_ptr<ASTnode> PARSER::factor_()
{
    //return value seems to get deleted
    if (match(INTEGER))
    {
        //std::cout<<current <<": test\n";
        auto k {(std::make_unique<NumberNode> (previous()))};
        return std::unique_ptr<ASTnode>(std::move(k));
    }
    if(match(LEFT_PAREN))
    {
        std::unique_ptr<ASTnode> expr = expression();
        if(match(RIGHT_PAREN))
        {
            return expr;
        }
    }else if (match(SEMICOLON))
        {
            std::cout<<"End of file\n";
            std::exit(1);
        }
    return std::unique_ptr<ASTnode>{};
}

