#include "parser.hpp"
#include <utility>

void PARSER::error(const TOKEN& token, const std::string& message)
{
    if(PARSER::current == PARSER::tokens.size())
    {
        std::cout << token.line << "-" << token.column << ": at the end " << message << "\n";
    }else
    {
        std::cout << token.line << "-" << token.column << ": token '" << token.value << "', " << message << "\n";
    }
    PARSER::hasError = true;
}

void PARSER::synchronize()
{
    advance();

    while (PARSER::current != PARSER::tokens.size()) {
      if (previous().type == SEMICOLON) return;

      switch (peek().type) {
        case INT:
        case VOID:
        case IF:
        case ELSE:
        case WHILE:
        case INPUT:
        case OUTPUT:
        case RETURN:
          return;
      }

      advance();
    }
}

TOKEN PARSER::advance()
{
    if(current != PARSER::tokens.size()) current++;
    return previous();
}

TOKEN PARSER::previous() const
{
    return tokens.at(current -1);
}

TOKEN PARSER::peek() const
{
    return PARSER::tokens[current];
}

bool PARSER::match(TokenType tokentype)
{   
    if (tokens[current].type == tokentype)
    {
        advance();
        return true;
    }
    else{ return false; }
}

bool PARSER::match(const std::vector<TokenType>& tokentypes)
{
    for(int i{}; i<tokentypes.size();i++)
    {
        if (peek().type == tokentypes.at(i))
        {
            advance();
            return true;
        }
    }
    return false;
}

TOKEN PARSER::consume(TokenType type, const std::string& message)
{
    //use try catch and use synchronize() outside
    if(!match(type))
    {
        error(peek(), message);
    }
    return previous();
}

std::unique_ptr<ASTnode> PARSER::parse_program()
{
    //std::vector<std::unique_ptr<ASTnode>> statements{};
    std::unique_ptr<CompoundStmt> statement{std::make_unique<CompoundStmt>()};
    while(PARSER::current != PARSER::tokens.size())
    {
        //(statement->a).push_back(3);
        std::cout<<"\ndddd\n"; 
        statement->statements.push_back(std::move(parse_declaration()));
    }
    return statement;
}

std::unique_ptr<ASTnode> PARSER::parse_declaration()
{
    TOKEN next_token{peek()};
    if(match({INT,VOID}))
    {
        return varDeclare(next_token);
        
    }
    else //check for var or func name for redefinition or function call
    {
        auto a = parse_expression();
        return a;
    }
}

std::unique_ptr<ASTnode> PARSER::varDeclare(TOKEN varType)
{
    TOKEN varName { consume(VAR_NAME, "Expected a variable name\n") };
    std::unique_ptr<ASTnode> initializer = nullptr;
    if(match(EQUAL))
    {
        initializer = std::move(parse_expression());
    }
    return std::make_unique<VarDeclaration>(varType, varName, initializer);
}

std::unique_ptr<ASTnode> PARSER::parse_expression()
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
    while(match({GREATER, GREATER_EQUAL, LESS, LESS_EQUAL}))
    {
        token = previous();
        std::unique_ptr<ASTnode>right{term()};
        expr = std::make_unique<BinaryOp>(expr, token, right);//move might be unnecessary
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
        std::unique_ptr<ASTnode> expr {parse_expression()};
        if(!match(RIGHT_PAREN)) { error(peek(), "missing ')'\n"); }
       
        return expr;
    }
    else
    {
        error(peek(), "expected an expression");
        return nullptr; //might cause problems
    }
    
}

void VarDeclaration::print() const
{
    std::cout << var_type.value << " " << var_name.value << " = ";
    if (expr)
        VarDeclaration::expr->print();
    std::cout << "\n";
}

void BinaryOp::print() const
{
    std::cout << " (";
    BinaryOp::left->print();
    std::cout <<" " << BinaryOp::token.value << " ";
    BinaryOp::right->print();
    std::cout << " ) ";
}

void UnaryOp::print() const
{
        std::cout<<"(-"; UnaryOp::right->print(); std::cout<<")";
}

void NumberNode::print() const
{
    std::cout<< value;
}