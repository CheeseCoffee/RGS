#include <iostream>
#include <thread>
#include <grid/CellData.h>
#include <grid/TestConstructor.h>
#include "utilities/Parallel.h"
#include "Solver.h"

int main(int argc, char* argv[]) {
    //std::ofstream out("log.txt");
    //std::streambuf *coutbuf = std::cout.rdbuf(); //save old buf
    //std::cout.rdbuf(out.rdbuf()); //redirect std::cout to out.txt!

    Parallel::init();

    std::chrono::time_point<std::chrono::steady_clock, std::chrono::nanoseconds> startTime;
    if (Parallel::isMaster()) {
        startTime = std::chrono::steady_clock::now();
    }

    // Print off a hello world message
    std::cout << "Hello world from processor " << Parallel::getProcessorName()
              << ", rank " << Parallel::getRank()
              << " out of " << Parallel::getSize() << " processors." << std::endl;

    // Create config
    Config::getInstance()->init();
    if (Parallel::isMaster() == true) {
        std::cout << "Config:" << std::endl << *Config::getInstance() << std::endl;
    }

    try {
        Solver solver{new TestConstructor()};
        solver.init();
        solver.run();
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
        Parallel::abort();
    }

    if (Parallel::isMaster()) {
        auto now = std::chrono::steady_clock::now();
        auto wholeTime = std::chrono::duration_cast<std::chrono::seconds>(now - startTime).count();
        std::cout << "It took - " << wholeTime << " seconds" << std::endl;
    }

    Parallel::finalize();
}
