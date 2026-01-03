#include <unistd.h>
#include <termios.h>
#include "editor.h"
#include "path.h"

int main(int argc, char* argv[]){ 
    EditorConfig E;
    EnableRawMode(E);
    Editor(E);
    ParseArgs(E, argc, argv);
    EditorSetStatusMessage(E,"HELP: Ctrl-S = save | Ctrl-Q = quit | Ctrl-F = search");
    while (true){
        ScreenRefresh(E);
        Keypress(E);
    }
    DisableRawMode(E);
    return 0;
}
