//#include "parser.hpp"
#include "parser.cpp"
#include "lexer.cpp"
#include<fstream>


int main(int argc, char** argv) 
{
    //TODO: write the lexer, give it the argv[] values, and give output.
    std::string line{};
    std::string code{};
    std::ifstream file{"a.txt"};

    while (std::getline(file, line) && line !="exit")
    {
        code += line + "\n";
    }
    //std::cout << code;
    LEXER lexer{code};
    //lexer.tokenize(); //replace this with the bottom line
    lexer.tokenize();
    PARSER parser{lexer.get_tokens()};
    parser.expression();
}
