#include "Game.h"

using namespace std;

int main() {
    return Game::start();
}

/* Notes n' shit:
    * If there is a runtime_error the program must halt. Continuing could lead to memory leaks.
*/