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
    if (peek().type == tokentype)
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
    //use try catch and use synchronize() outside the function
    if(!match(type))
    {
        error(peek(), message);
    }
    return previous();
}

std::unique_ptr<ASTnode> PARSER::parse_program()
{
    std::unique_ptr<CompoundStmt> statement{std::make_unique<CompoundStmt>()};
    while(PARSER::current < PARSER::tokens.size() && peek().type != RIGHT_BRACE)
    {
        //use try catch and synchronise here?
        statement->statements.push_back(std::move(parse_declaration()));
    }
    if(PARSER::current < PARSER::tokens.size())
    {
        consume(RIGHT_BRACE, "Expected a '}'.\n");
    }
    return statement;
}

std::unique_ptr<ASTnode> PARSER::parse_declaration()
{
    //add try catch here?
    TOKEN type{peek()};
    if(match({INT,VOID}))
    {
        if(match(VAR_NAME))
        {
            return varDeclare(type);
        }else if(match(FUNC_NAME))
        {
            return funcDeclare(type);
        }else
        {
            error(peek(), "Expected an identifier");
            return nullptr;
        }
    }else if(match(LEFT_BRACE))
    {
        auto expr{ parse_program() };
        return expr;
    }else if(match(IF))
    {
        return parse_ifStmt();
    }else if(match(WHILE))
    {
        return parse_whileStmt();
    }else if(match(SEMICOLON))
    {
        return nullptr;
    }else //check for var or func name for redefinition or function call
    { 
        auto a = parse_expression();
        consume(SEMICOLON, "Expected a semicolon.\n");
        return a;
    }
}

std::unique_ptr<ASTnode> PARSER::funcDeclare(TOKEN funcType)
{
    TOKEN funcName { previous() };
    std::unique_ptr<ASTnode> expr{};
    consume(LEFT_PAREN, "Expected '('.\n");

    auto parameters{parse_parameters()};

    consume(RIGHT_PAREN, "Expected ')'.\n");
    if(match(LEFT_BRACE))
    {
        expr = std::move(parse_program());
    }else consume(SEMICOLON, "Expected ';'.\n"); // if no braces

    return std::make_unique<FuncDeclaration>(funcType, funcName, parameters, expr);
}
std::unique_ptr<ASTnode> PARSER::parse_parameters()
{
    if(peek().type == RIGHT_PAREN)
    {
        return nullptr;
    }
    auto parameters {std::make_unique<Parameter>()};
    do{
        if(!match(INT))
        {
            consume(VOID, "Expected  a type.\n");
        }
        TOKEN varType{previous()};
        consume(VAR_NAME, "Expected  a variable name.\n");
        TOKEN varName{previous()};
        parameters->parameters.push_back(std::move(std::make_unique<VarDeclaration>(varType, varName)));
    } while (match(COMMA));
    return parameters;
}

std::unique_ptr<ASTnode> PARSER::varDeclare(TOKEN varType)
{
    TOKEN varName { previous() };
    std::unique_ptr<ASTnode> initializer = nullptr;
    if(match(EQUAL))
    {
        initializer = std::move(parse_expression());
    }
    consume(SEMICOLON, "Expected a semicolon.\n");
    return std::make_unique<VarDeclaration>(varType, varName, initializer);
}


std::unique_ptr<ASTnode> PARSER::parse_ifStmt()
{
    consume(LEFT_PAREN, "Expected '(' after 'if'.\n");
    auto condition { parse_expression() };
    consume(RIGHT_PAREN, "Expected ')' after 'if' condition.\n");
    auto thenBranch { match(LEFT_BRACE)?parse_program():parse_declaration() };
    std::unique_ptr<ASTnode> elseBranch{};
    if(match(ELSE))
    {
        elseBranch = std::move(parse_declaration());
    }
    return std::make_unique<IfStmt>(condition, thenBranch, elseBranch);
}

std::unique_ptr<ASTnode> PARSER::parse_whileStmt()
{
    consume(LEFT_PAREN, "Expected '(' after 'if'.\n");
    auto condition { parse_expression() };
    consume(RIGHT_PAREN, "Expected ')' after 'if' condition.\n");
    consume(LEFT_BRACE, "Expected '{' after 'while'.\n");
    auto loop_body { parse_program() };

    return std::make_unique<WhileStmt>(condition, loop_body);
}

std::unique_ptr<ASTnode> PARSER::parse_expression()
{
    auto expr {equality()};
    return expr;
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
    }
    else if (match(LEFT_PAREN))
    {
        std::unique_ptr<ASTnode> expr {parse_expression()};
        consume(RIGHT_PAREN, "missing ')'\n");
        return expr;
    }
    else if(match(VAR_NAME))
    {
        return std::make_unique<Variable>(previous());
    }
    else if(match(FUNC_NAME))
    {
        return std::make_unique<Call>(previous());
    }
    else
    {
        error(peek(), "expected an expression");
        std::exit(1);
        return nullptr; //might cause problems
    }
    
}

void CompoundStmt::print(int indent) const
{
    indent++;
    if (indent != 0)
    {
        for (int i{}; i < indent * 4; i++)
        {
            std::cout << " ";
        }
        std::cout << "{\n";
    }
    for(int i{}; i<statements.size() ;i++)
    {
        for(int j{}; j < indent*4; j++)
        {
            std::cout << " ";
        }
        if(statements.at(i))
        {    
            statements.at(i)->print(indent);
            std::cout << "\n";
        }
    }
    if (indent != 0)
    {
        for (int i{}; i < indent * 4; i++)
        {
            std::cout << " ";
        }
        std::cout << "{\n";
    }
}

void VarDeclaration::print(int indent) const
{
    std::cout <<  var_type.value << " " << var_name.value;
    if (expr)
    {
        std::cout << " = ";
        VarDeclaration::expr->print(indent);
    }
}

void FuncDeclaration::print(int indent) const
{
    std::cout << func_type.value << " " << func_name.value << "(";
    if(FuncDeclaration::parameters)
    {
        parameters->print(indent);
    }
    std::cout << ")";
    if (expr)
    {
        std::cout << "\n";
        FuncDeclaration::expr->print(indent);  
    }
}

void Parameter::print(int indent) const
{
    for(int i{}; i<parameters.size(); i++)
    {
        if(parameters.at(i))
        {    
            parameters.at(i)->print(indent);
        }
    }
}

void IfStmt::print(int indent) const
{
    std::cout << "if(";
    condition->print(indent);
    std::cout << ")\n";
    for(int i{}; i < indent*4; i++){ std::cout << " "; }
    std::cout << "{\n";
    thenBranch->print(indent);
    for(int i{}; i < indent*4; i++){ std::cout << " "; }
    std::cout << "}";
    if(elseBranch)
    {
        std::cout << "else\n";
        elseBranch->print(indent); 
    }
}

void WhileStmt::print(int indent) const
{
    std::cout << "while(";
    condition->print(indent);
    std::cout << ")\n";
    if(loop_body)
    {
        loop_body->print(indent);
    }
}

void BinaryOp::print(int indent) const
{
    std::cout << "(";
    BinaryOp::left->print(indent);
    std::cout <<" " << BinaryOp::token.value << " ";
    BinaryOp::right->print(indent);
    std::cout << ")";
}

void UnaryOp::print(int indent) const
{
        std::cout<<"(-"; UnaryOp::right->print(indent); std::cout<<")";
}

void Variable::print(int indent) const
{
    std::cout << Variable::value;
}

void Call::print(int indent) const
{
    std::cout << Call::value << "()";
}

void NumberNode::print(int indent) const
{
    std::cout<< value;
}