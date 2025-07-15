#include "lexer.hpp"

#define nPrint //change to print to d isplay token list

namespace HashMap
{
    std::unordered_map<std::string, TokenType> map{
        {"if", IF},
        {"else", ELSE},
        {"return", RETURN},
        {"while", WHILE},
        {"int", INT},
        {"void", VOID},
        {"input", INPUT},
        {"output", OUTPUT}};
}

std::vector<TOKEN>& LEXER::get_tokens()
{
    return LEXER::tokens;
}

void LEXER::tokenize()
{
    bool had_error{false};
    while (current < source.length())
    {
        start = current;
        column++;
        char chr = source[current++];
        TokenType type{};
        switch (chr)
        {
        case '(':
            add_token(LEFT_PAREN);
            break;
        case '{':
            add_token(LEFT_BRACE);
            break;
        case ')':
            add_token(RIGHT_PAREN);
            break;
        case '}':
            add_token(RIGHT_BRACE);
            break;
        case ',':
            add_token(COMMA);
            break;
        case '-':
            add_token(MINUS);
            break;
        case '+':
            add_token(PLUS);
            break;
        case ';':
            add_token(SEMICOLON);
            break;
        case '*':
            add_token(STAR);
            break;
        case '!':
            if(peek_char() == '='){ add_token(NOT_EQUAL); }
            break;
        case '=':
            add_token((peek_char() == '=') ? EQUAL_EQUAL : EQUAL);
            break;
        case '<':
            add_token((peek_char() == '=') ? LESS_EQUAL : LESS);
            break;
        case '>':
            add_token((peek_char() == '=') ? GREATER_EQUAL : GREATER);
            break;
        case '/':
            (peek_char() == '/') ? handle_comments() : add_token(SLASH);
        case ' ':
        case '\r':
        case '\t':
        case '\n':
            skip_whitespace();
            break;
        default:
            if (chr >= '0' && chr <= '9')
            {
                read_number();
            }
            else if (is_alpha(chr))
            {
                read_identifier();   
            }
            else
            {
                std::string text {source.substr(start, current-start)};
                std::cout << "Error: Unexpected \"" << text << "\" at line " << line << ", column " << column << ".\n";
                hasError = true;    
            }
        }
        //std::cout << LEXER::tokens.front().value;
    }
    if (hasError == true){ std::exit(1); }
    
    #ifdef Print
    //Temporary?
    for (int i{}; i< tokens.size(); i++){
        tokens[i].print();
    }
    #endif
    //return LEXER::tokens;
}



char LEXER::peek_char()
{
    if (current == source.length())
        return '\0';
    else
        return source[current];
}

bool LEXER::is_alpha(char chr)
{
    if ((chr >= 'a' && chr <= 'z') || (chr >= 'A' && chr <= 'Z') || chr == '_') { return true; }
    else { return false; }
}

void LEXER::skip_whitespace()
{
    current--;
    column--;
    char c{peek_char()};
    //std::cout<<"\na\n";
    do
    {
        if (c == '\n')
        {
            line++;
            column = -1;
            //return;
        }
        current++;
        column++;
        c = peek_char();
    }while (c == ' ' || c == '\t' || c == '\r' || c == '\n');
    //current--;
    //column--;
}

void LEXER::read_number()
{
    while(isdigit(peek_char())) { current++; column++; }
    add_token(INTEGER);
}

void LEXER::read_identifier()
{
    while (is_alpha(peek_char())) { current++; column++;}
    std::string token{source.substr(start, current - start)};
    if (HashMap::map.find(token) != HashMap::map.end())
    {
        add_token(HashMap::map[token]);
    }
    else
    {
        int emptyCharCount{};
        bool func_name { false };
        do
        {
            if (source[current] == '(')
            {
                current -= emptyCharCount;
                column -= emptyCharCount;
                add_token(FUNC_NAME);
                current +=emptyCharCount;
                column +=emptyCharCount;
                func_name = true;
                
            }
            emptyCharCount++;
            column++;
            current++;
        } while (source[current] == ' '); 
        current--;
        column--;
        emptyCharCount--;
        if (!func_name)
        {
            current -= emptyCharCount;
            column -= emptyCharCount;
            add_token(VAR_NAME);
        }
    }
}

void LEXER::handle_comments()
{
    while (peek_char() != '\n' && current != source.length()) {current++; column++;}
}

void LEXER::add_token(TokenType type)
{
    std::string text {source.substr(start, current-start)};
    LEXER::tokens.emplace_back(type, text, line, column-(current-start)+1);
}
