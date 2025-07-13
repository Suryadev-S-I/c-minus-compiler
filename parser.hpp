//BinaryOp and UnaryOp Node arguments are references. learn move constructors(-> && reference also) and std::move 


#include "lexer.hpp"
#include <memory>

class Visitor;

class ASTnode
{
public:
    virtual ~ASTnode() = default;
    virtual void print() const =0;
    
    //virtual void get_v()=0;
    // virtual void print(int indent = 0) const = 0; //Pretty printing (indent is indentaion for the nodes in the tree)
    virtual void accept(Visitor &visitor) =0; // Visitor has functions to work with each derived class?
};

class VarDeclaration : public ASTnode
{   
    TOKEN var_type{};
    TOKEN var_name{};
    std::unique_ptr<ASTnode> expr{};

    public:
    void print() const override;
    void accept(Visitor &visitor) override{}
    VarDeclaration(TOKEN type, TOKEN name, std::unique_ptr<ASTnode>&node)
    :var_type(type), var_name(name), expr(std::move(node)){}
};
class FuncDeclaration : public ASTnode
{
    //TO-ADD: Parameters, body of the function
    TOKEN func_type{};
    TOKEN func_name{};
    std::unique_ptr<ASTnode> expr{};

    public:
    void print() const override;
    void accept(Visitor &visitor) override{}
    
    FuncDeclaration(TOKEN type, TOKEN name,std::unique_ptr<ASTnode>&node)
    :func_type(type), func_name(name), expr(std::move(node)){}
};
class Parameter : public ASTnode
{
};
class CompoundStmt : public ASTnode
{
    public:
    std::vector<std::unique_ptr<ASTnode>> statements{};
    //std::vector<int> a{};
    void print() const override;
    void accept(Visitor &visitor) override{}
    //void add_statement(){}
};
class IfStmt : public ASTnode
{
    std::unique_ptr<ASTnode> condition{};
    std::unique_ptr<ASTnode> thenBranch{};
    std::unique_ptr<ASTnode> elseBranch{};

    public:
    void print() const override;
    void accept(Visitor &visitor) override{}

    IfStmt(std::unique_ptr<ASTnode>& condition_,std::unique_ptr<ASTnode>& then_,std::unique_ptr<ASTnode>& else_)
    :condition(std::move(condition_)), thenBranch(std::move(then_)), elseBranch(std::move(else_)){}
};
class WhileStmt : public ASTnode
{
};
class ReturnStmt : public ASTnode
{
    std::unique_ptr<ASTnode> expr{};
    public:
    void accept(Visitor &visitor) override{};
    void print() const override;

    ReturnStmt(std::unique_ptr<ASTnode>& node):expr(std::move(node)){}
};
class BinaryOp : public ASTnode
{
    std::unique_ptr<ASTnode> left{};
    TOKEN token{};
    std::unique_ptr<ASTnode> right{};

public:
    void accept(Visitor &visitor) override{};
    void print() const override;

    BinaryOp(std::unique_ptr<ASTnode>& left_, TOKEN token_, std::unique_ptr<ASTnode>& right_):
    left(std::move(left_)), token(token_), right(std::move(right_)){}
};
class UnaryOp : public ASTnode
{
public:
    TOKEN token{};
    std::unique_ptr<ASTnode> right{};
    void print() const override;
    void accept(Visitor &visitor) override{};

    UnaryOp(TOKEN token_, std::unique_ptr<ASTnode>& node) : token(token_), right(std::move(node)){}
};
class Variable : public ASTnode
{
};
class Call : public ASTnode
{
};
class NumberNode : public ASTnode
{
    TOKEN token{};
    std::string value{};

public:
    void accept(Visitor &visitor) override{};
    void print() const override;
    NumberNode(TOKEN token_) : token(token_), value(token_.value){}
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
    std::unique_ptr<ASTnode> parse_expression();
    std::unique_ptr<ASTnode> parse_ifStmt();

    void error(const TOKEN& token, const std::string& message);
    void synchronize();

};