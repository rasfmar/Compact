#include <bits/stdc++.h>
#include "CompactRelease/Compact.hpp"

int main(int argc, char* argv[]) {
    cmp_state* C = new cmp_state();

    if(argc < 2) {
        std::cerr << "Starting " << COMPACT_VERSION << " console! Enter empty line to exit\n";
        std::string line;
        do {
            std::cout << "- ";
            std::getline(std::cin, line);
            /*
            for(const auto& token : *(cmpL_lexer(C,line))) {
                std::cout << token->str() << '\n';
            }
            */
           cmp_do(C,line);
        } while(line != "" && C->ok);

        if(!(C->ok)) {
            std::cout << C->out->str();
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
            std::cout << C->out->str();
            delete C;
            return 1;
        }
    }

    std::cout << "\n\nProgram has terminated\n";

    delete C;
    return 0;
}