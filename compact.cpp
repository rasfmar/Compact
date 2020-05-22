#include <bits/stdc++.h>
#include "CompactRelease/Compact.hpp"

int main(int argc, char* argv[]) {
    cmp_state* C = new cmp_state();

    if(argc < 2) {
        std::cout << "Starting " << COMPACT_VERSION << " console! Enter empty line to exit\n";
        std::string line;
        do {
            std::cout << "\033[1m\033[37m$ ";
            std::getline(std::cin, line);
            /*
            for(const auto& token : *(cmpL_lexer(C,line))) {
                std::cout << token->str() << '\n';
            }
            */
            std::cout << "\033[0m\033[36m";
            cmp_do(C, line);
        } while(line != "" && C->ok);

        if(!(C->ok)) {
            std::cerr << "\033[31m" << C->out->str() << "\033[0m";
            delete C;
            return 1;
        }
    } else {
        std::string filename(argv[1]);
        std::ifstream file(filename);

        std::string line;
        while(std::getline(file,line) && C->ok)
            if(line.size() > 0)
                cmp_do(C,line);

        if(!(C->ok)) {
            std::cerr << "\033[31m" << C->out->str() << "\033[0m";
            delete C;
            return 1;
        }
    }

    std::cout << "\n\nProgram has terminated\n";

    delete C;
    return 0;
}