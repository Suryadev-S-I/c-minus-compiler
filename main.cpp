#include "parser.hpp"
//#include "parser.cpp"
//#include "lexer.cpp"
#include<fstream>


int main(int argc, char** argv) 
{
    //TODO: write the lexer, give it the argv[] values, and give output.
    std::string line{};
    std::string code{};

    std::string file_name{};
    (argc >= 2) ? file_name = argv[1] : file_name = "a.txt";

    std::ifstream file{file_name};

    while (std::getline(file, line))
    {
        code += line + "\n";
    }
    LEXER lexer{code};
    lexer.tokenize();
    PARSER parser{lexer.get_tokens()};
    auto expr { parser.parse_program() };
    expr->print();
}
