#include <unistd.h>
#include <termios.h>
#include "editor.h"

int main(int argc, char* argv[]){ 
    EditorConfig E;
    EnableRawMode(E);
    Editor(E);
    if (argc>=2){
        OpenEditor(E, argv[1]);
    }
    EditorSetStatusMessage(E,"HELP: Ctrl-S = save | Ctrl-Q = quit");
    while (true){
        ScreenRefresh(E);
        Keypress(E);
    }
    DisableRawMode(E);
    return 0;
}
