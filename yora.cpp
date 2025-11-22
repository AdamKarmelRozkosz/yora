#include <unistd.h>
#include <termios.h>
#include <cstdlib>
#include <cctype>
#include <iostream>
#include "raw.h"
int main(){ 
    EnableRawMode();

    char c = '\0'; 
    if (read(STDIN_FILENO, &c, 1) == 1)
        die("tcsetattr");
    read(STDIN_FILENO, &c, 1);
    while (1){
        if (std::iscntrl(c)){ // checks whether a keypress is a controll character (control characters are non printable characters)
            std::cout << static_cast<int>(c) << "\r\n";
        } else {
            std::cout << static_cast<int>(c) << " ('" << c << "')\r\n";
        }
        if (c == 'q'){
            break;
        }
    }
    return 0;
}
