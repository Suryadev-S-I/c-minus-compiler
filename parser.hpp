//BinaryOp and UnaryOp Node arguments are references. learn move constructors(-> && reference also) and std::move 


#include "lexer.hpp"
#include <memory>

class Visitor;

class ASTnode
{
public:
    virtual ~ASTnode() = default;
    virtual void print()=0;
    ASTnode() = default;
    //virtual void get_v()=0;
    // virtual void print(int indent = 0) const = 0; //Pretty printing (indent is indentaion for the nodes in the tree)
    //virtual void accept(Visitor &visitor); // Visitor has functions to work with each derived class?
};

class VarDeclaration : public ASTnode
{
};
class FunDeclaration : public ASTnode
{
};
class Parameter : public ASTnode
{
};
class CompoundStmt : public ASTnode
{
};
class IfStmt : public ASTnode
{
};
class WhileStmt : public ASTnode
{
};
class ReturnStmt : public ASTnode
{
};
class BinaryOp : public ASTnode
{
public:
    std::unique_ptr<ASTnode> left{};
    TOKEN token{};
    std::unique_ptr<ASTnode> right{};
    void print() override {std::cout<<token.value;}
    BinaryOp(std::unique_ptr<ASTnode>& left_, TOKEN token_, std::unique_ptr<ASTnode>& right_):
    left(std::move(left_)), token(token_), right(std::move(right_)){}
};
class UnaryOp : public ASTnode
{
public:
    TOKEN token{};
    std::unique_ptr<ASTnode> right{};
    void print() override {std::cout<<token.value;}
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
public:
    TOKEN token{};
    std::string value{};
    void print() override {std::cout<<token.value;}
    NumberNode(TOKEN token_) : token(token_), value(token_.value){}
};

class Visitor
{
    void visitBinaryOp(BinaryOp binaryop);
    void visitNumber(NumberNode num);
};

class PARSER
{
    std::vector<TOKEN>& tokens;
    int current{};

    bool match(TokenType tokentype);
    bool match(const std::vector<TokenType>& tokentypes);
    TOKEN previous();
    std::unique_ptr<ASTnode> equality();
    std::unique_ptr<ASTnode> comparison();
    std::unique_ptr<ASTnode> term();
    std::unique_ptr<ASTnode> factor();
    std::unique_ptr<ASTnode> unary();
    std::unique_ptr<ASTnode> primary();

public:
    PARSER(std::vector<TOKEN>& tokens_) : tokens(tokens_) {}

    std::unique_ptr<ASTnode> parse_expression();

};