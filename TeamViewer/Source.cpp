#include <iostream>
#include "windowsHeaders.h"
#include "cursor.h"
#include "keyboard.h"
#include <thread>

int main() {
    std::thread cursorThread(listenToCursor);
    std::thread keyboardThread(listenToKeyboard);
    
    cursorThread.join();
    keyboardThread.join();
    system("pause");
    return 0;
}
