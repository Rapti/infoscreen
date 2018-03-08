#include "main.h"
#include <unistd.h>

using namespace std;

int main() {
    Screen s;
    s.run();
    return 0;
}


std::string exec(const char* cmd) {
    char buffer[128];
    std::string result = "";
    std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
    if(!pipe) {std::cout << "Error opening pipe" << std::endl; return "";}
    if (!pipe) throw std::runtime_error("popen() failed!");
    while (!feof(pipe.get())) {
        if (fgets(buffer, 128, pipe.get()) != NULL)
            result += buffer;
    }
    return result;
}