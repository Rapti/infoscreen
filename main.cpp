#include "main.h"
#include <unistd.h>

using namespace std;

int main() {
    cout << "Hello World!" << endl;
//  char* lel = new char[256];
//  getcwd(lel, 255);
//  cout << lel << endl;
    Screen s;
    s.run();
    return 0;
}