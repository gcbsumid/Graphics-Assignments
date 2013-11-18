#ifdef WIN32
#include <windows.h>
#endif

#include <memory>
#include <stdexcept>
#include <iostream>
#include <string>

#include "Engine.hpp"

using namespace std;

int main(int argc, char* argv[]) {
    unique_ptr<Engine> engine(nullptr);

    try {
        engine = unique_ptr<Engine> (new Engine());
    } catch (exception& err) {
        cerr << "Setup Error: " << err.what() << endl;
        return EXIT_FAILURE;
    }

    try {
        if (engine) {
            engine->Run();
        }
    } catch (exception& err) {
        cerr << "Runtime Error: " << err.what() << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}