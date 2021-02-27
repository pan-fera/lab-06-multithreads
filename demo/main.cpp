#include "MultithreadsSHA.hpp"

int main(int argc, char** argv) {
    if (argc == 1){
        MultithreadsSHA a;
        a.start();
    }else if (argc == 2) {
        MultithreadsSHA a ( boost::lexical_cast<unsigned>(argv[1]));
        a.start();
    }else if (argc == 3) {
        MultithreadsSHA a (boost::lexical_cast<unsigned>(argv[1]), argv[2]);
        a.start();
    }
    return 0;
}