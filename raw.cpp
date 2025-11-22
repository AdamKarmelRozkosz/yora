#include "raw.h"
#include <termios.h>
#include <cstdlib>
#include <unistd.h>
#include <iostream>
#include "keymap.h"
termios termios_ori;

void die(const char *s){ //error handling
    perror(s); // seeks for the global variable errno to chech whether there was an error
    exit(1);
}
void DisableRawMode(){
      tcsetattr(STDIN_FILENO, TCSAFLUSH, &termios_ori);

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &termios_ori) == -1)
        die("tcsetattr");
}

void EnableRawMode(){
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &termios_ori) == -1)
        die("tcsetattr");

    tcgetattr(STDIN_FILENO, &termios_ori);
    atexit(DisableRawMode); // w use atexit from cstdlib to activate oru function our program stops.

    termios raw = termios_ori;
    tcgetattr(STDIN_FILENO, &raw);
    raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN); 
    // ECHO- Disables the echo mode
    // ICANON - Disables canonical mode
    // ISIG - Disables Ctrl-C and Ctrl-Z
    // IEXTEN - Fixes Ctrl-V and Ctrl-C by also fixes Ctrl-O on Mac 
    
    raw.c_iflag &= ~(ICRNL | IXON | BRKINT | INPCK | ISTRIP); 
    // ICRNL - Disables the Ctrl-S and Ctrl-Q
    // IXON - Disables carriage return
    // BRKINT - A break condidion will end the program
    // INPCK - Enables parity checking(kind of deprecated)
    // ISTRIP - Causes the 8th bit of of input to be set to 0

    raw.c_lflag &= ~(OPOST); 
    // OPOST - Disables output processing 

    raw.c_cflag |= ~(CS8);
    // CS8 - Sets the character size to 8 bit 

    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
        die("tcsetattr");
}
char ReadMode(){
    int uread;
    char c;
    while ((uread=read(STDIN_FILENO, &c, 1)) !=1){
        if(uread == -1){
            die("read");
        }
    }
    return c;
}
void Keypress(){
    char c = ReadMode();
    switch (c) {
    case CTRL_KEY('q'):
        exit(0);
        break;
    }
}
void clear(){
    write(STDOUT_FILENO, "\x1b[2J", 4);  
    write(STDOUT_FILENO, "\x1b[H", 3);
}
