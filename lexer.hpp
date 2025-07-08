#ifndef LEXER_H
#define LEXER_H

#include<vector>
#include<string>
#include<iostream>
#include<unordered_map>

enum TokenType {
    // Keywords
    INT,
    VOID,
    IF,
    ELSE,
    WHILE,
    RETURN,
    INPUT,
    OUTPUT,

    LEFT_PAREN,
    RIGHT_PAREN,
    LEFT_BRACE,
    RIGHT_BRACE,
    COMMA,
    MINUS,
    PLUS,
    SEMICOLON,
    SLASH,
    STAR,
    NOT_EQUAL,
    EQUAL,
    EQUAL_EQUAL,
    GREATER,
    GREATER_EQUAL,
    LESS,
    LESS_EQUAL,

    // Literals & Identifiers
    INTEGER,
    VAR_NAME,
    FUNC_NAME
};

class TOKEN{
    public:
    TokenType type{};
    std::string value{};
    int line{};
    int column{};
    
    public:
        TOKEN(TokenType type_, std::string value_, int line_, int column_):type(type_), value(value_), line(line_), column(column_){}

        TOKEN()=default;

        void print(){
            std::cout << "TOKEN: " << value << " at line " << line << " column " << column << "\n";
        }
};
 
class LEXER{
    bool hasError{false};
    std::vector<TOKEN> tokens{};
    const std::string source{};

    int start{}, current{}, line{1}, column{};
    char peek_char();
    void skip_whitespace();
    void read_number();
    void read_identifier();
    void handle_comments();
    void add_token(TokenType type);
    bool is_alpha(char chr);
    
    public:
        void tokenize();
        std::vector<TOKEN>& get_tokens();
        LEXER(std::string& source):source(source){};
        
};

#endif