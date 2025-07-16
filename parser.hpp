#include "lexer.hpp"
#include <memory>

class Visitor;

class ASTnode
{
public:
    virtual ~ASTnode() = default;
    virtual void print(int indent = -1) const =0;
    virtual void accept(Visitor &visitor) =0; 
};

class VarDeclaration : public ASTnode
{   
    TOKEN var_type{};
    TOKEN var_name{};
    std::unique_ptr<ASTnode> expr{};

    public:
    void print(int indent = -1) const override;
    void accept(Visitor &visitor) override{}
    
    VarDeclaration(TOKEN type, TOKEN name, std::unique_ptr<ASTnode>&node)
    :var_type(type), var_name(name), expr(std::move(node)){}

    VarDeclaration(TOKEN type, TOKEN name)
    :var_type(type), var_name(name){}
};

class FuncDeclaration : public ASTnode
{
    TOKEN func_type{};
    TOKEN func_name{};
    std::unique_ptr<ASTnode> parameters{};
    std::unique_ptr<ASTnode> expr{};

    public:
    void print(int indent = -1) const override;
    void accept(Visitor &visitor) override{}
    
    FuncDeclaration(TOKEN type, TOKEN name, std::unique_ptr<ASTnode>& parameters_,std::unique_ptr<ASTnode>&node)
    :func_type(type), func_name(name), parameters(std::move(parameters_)), expr(std::move(node)){}
};

class Parameter : public ASTnode
{
    public:
    std::vector<std::unique_ptr<ASTnode>> parameters{};
    void print(int indent = -1) const override;
    void accept(Visitor &visitor) override{}
};

class CompoundStmt : public ASTnode
{
    public:
    std::vector<std::unique_ptr<ASTnode>> statements{};
    void print(int indent = -1) const override;
    void accept(Visitor &visitor) override{}
};

class IfStmt : public ASTnode
{
    std::unique_ptr<ASTnode> condition{};
    std::unique_ptr<ASTnode> thenBranch{};
    std::unique_ptr<ASTnode> elseBranch{};

    public:
    void print(int indent = -1) const override;
    void accept(Visitor &visitor) override{}

    IfStmt(std::unique_ptr<ASTnode>& condition_,std::unique_ptr<ASTnode>& then_,std::unique_ptr<ASTnode>& else_)
    :condition(std::move(condition_)), thenBranch(std::move(then_)), elseBranch(std::move(else_)){}
};

class WhileStmt : public ASTnode
{
    std::unique_ptr<ASTnode> condition{};
    std::unique_ptr<ASTnode> loop_body{};

    public:
    void print(int indent = -1) const override;
    void accept(Visitor &visitor) override{}

    WhileStmt(std::unique_ptr<ASTnode>& condition_, std::unique_ptr<ASTnode>& loop_body_)
    : condition(std::move(condition_)), loop_body(std::move(loop_body_)){}
};

class ReturnStmt : public ASTnode
{
    std::unique_ptr<ASTnode> expr{};
    public:
    void accept(Visitor &visitor) override{};
    void print(int indent = -1) const override;

    ReturnStmt(std::unique_ptr<ASTnode>& node):expr(std::move(node)){}
};

class BinaryOp : public ASTnode
{
    std::unique_ptr<ASTnode> left{};
    TOKEN token{};
    std::unique_ptr<ASTnode> right{};

public:
    void accept(Visitor &visitor) override{};
    void print(int indent = -1) const override;

    BinaryOp(std::unique_ptr<ASTnode>& left_, TOKEN token_, std::unique_ptr<ASTnode>& right_):
    left(std::move(left_)), token(token_), right(std::move(right_)){}
};

class UnaryOp : public ASTnode
{
public:
    TOKEN token{};
    std::unique_ptr<ASTnode> right{};
    void print(int indent = -1) const override;
    void accept(Visitor &visitor) override{};

    UnaryOp(TOKEN token_, std::unique_ptr<ASTnode>& node) : token(token_), right(std::move(node)){}
};

class NumberNode : public ASTnode
{
    TOKEN token{};
    std::string value{};

public:
    void accept(Visitor &visitor) override{};
    void print(int indent = -1) const override;
    NumberNode(TOKEN token_) : token(token_), value(token_.value){}
};

class Variable : public ASTnode
{
    TOKEN token{};
    std::string value{};

public:
    void accept(Visitor &visitor) override{};
    void print(int indent = -1) const override;
    Variable(TOKEN token_) : token(token_), value(token_.value){}
};

class Call : public ASTnode
{
    TOKEN token{};
    std::string value{};

public:
    void accept(Visitor &visitor) override{};
    void print(int indent = -1) const override;
    Call(TOKEN token_) : token(token_), value(token_.value){}
};

class Visitor
{
    virtual void visit(BinaryOp binaryop) =0;
    virtual void visit(UnaryOp unaryop) =0;
    virtual void visit(NumberNode num) =0;
};

class visitorPrinter: public Visitor
{
    void visit(BinaryOp binaryop) override;
    void visit(UnaryOp unaryop) override;
    void visit(NumberNode num) override;
};

class PARSER
{
    std::vector<TOKEN>& tokens;
    bool hasError{false};
    int current{};
    
    TOKEN advance();
    bool match(TokenType tokentype);
    bool match(const std::vector<TokenType>& tokentypes);
    TOKEN previous() const;
    TOKEN peek() const;
    TOKEN consume(TokenType type, const std::string& message);

    std::unique_ptr<ASTnode> equality();
    std::unique_ptr<ASTnode> comparison();
    std::unique_ptr<ASTnode> term();
    std::unique_ptr<ASTnode> factor();
    std::unique_ptr<ASTnode> unary();
    std::unique_ptr<ASTnode> primary();
    std::unique_ptr<ASTnode> varDeclare(TOKEN varType);
    std::unique_ptr<ASTnode> funcDeclare(TOKEN varType);

public:
    PARSER(std::vector<TOKEN>& tokens_) : tokens(tokens_) {}

    std::unique_ptr<ASTnode> parse_program();
    std::unique_ptr<ASTnode> parse_declaration();
    std::unique_ptr<ASTnode> parse_parameters();
    std::unique_ptr<ASTnode> parse_expression();
    std::unique_ptr<ASTnode> parse_ifStmt();
    std::unique_ptr<ASTnode> parse_whileStmt();

    void error(const TOKEN& token, const std::string& message);
    void synchronize();

};