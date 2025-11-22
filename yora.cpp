#include <unistd.h>
#include <termios.h>
#include <cstdlib>
#include <cctype>
#include <iostream>
#include "raw.h"
#include "keymap.h"

int main(){ 
    EnableRawMode();
    
    while (true){
        clear();
        Keypress();
    }
   /* char c = '\0'; 
    if (read(STDIN_FILENO, &c, 1) == 1)
        die("tcsetattr");
    read(STDIN_FILENO, &c, 1);
    while (true){
        if (std::iscntrl(c)){ // checks whether a keypress is a controll character (control characters are non printable characters)
            std::cout << static_cast<int>(c) << "\r\n";
        } else {
            std::cout << static_cast<int>(c) << " ('" << c << "')\r\n";
        }
        if (c == CTRL_KEY(k)){
            break;
        }
    } */
    return 0;
}
