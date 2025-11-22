#include "editor.h"
#include <termios.h> // library required for working with terminal
#include <cstdlib> 
#include <unistd.h> 
#include <iostream>
#include "keymap.h"
#include <sys/ioctl.h> // required to get the size of the window 

struct EditorConfig {
    int screenrows;
    int screencols;
    struct termios termios_ori;
};

struct EditorConfig E;
void die(const char *s){ //error handling
    write(STDOUT_FILENO, "\x1b[2J", 4); //x1b == 'esc' in ANSI 
    write(STDOUT_FILENO, "\x1b[H", 3); 

    perror(s); // seeks for the global variable errno to chech whether there was an error
    exit(1);
}

void DisableRawMode(){
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.termios_ori) == -1)
        die("tcsetattr");
}

void EnableRawMode(){
    if (tcgetattr(STDIN_FILENO, &E.termios_ori) == -1)
        die("tcsetattr");

    atexit(DisableRawMode); // w use atexit from cstdlib to activate oru function our program stops.

    struct termios raw = E.termios_ori;
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

    raw.c_oflag &= ~(OPOST); 
    // OPOST - Diables output processing 

    raw.c_cflag |= (CS8);
    // CS8 - Sets the character size to 8 bit 

    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;

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
        write(STDOUT_FILENO, "\x1b[2J", 4); // Escape sequence that errases entire screen
        write(STDOUT_FILENO, "\x1b[H", 3); // Escape sequence that moves cursor to the home position
        exit(0);
        break;
    }
}

void DrawRows(){
    int y;
    for (y = 0; y < E.screencols; y++){
        write(STDOUT_FILENO, "~", 3);
        if (y < E.screencols -1){
            write(STDOUT_FILENO,"\r\n",2);
        }
    }
}

void ScreenRefresh(){
    write(STDOUT_FILENO, "\x1b[2J", 4);  
    write(STDOUT_FILENO, "\x1b[H", 3);

    DrawRows();
    write(STDOUT_FILENO, "\x1b[H", 3);
}
int GetWindowSize(int *rows, int *cols){
    struct winsize winsize;
    
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &winsize) == -1 || winsize.ws_col == 0){
        return -1;
    } else {
        *cols = winsize.ws_col;
        *rows = winsize.ws_row;
        return 0;
    }
}
void Editor(){
    if (GetWindowSize(&E.screenrows, &E.screencols) == -1){
        die("GetWindowSize");
    }
}
