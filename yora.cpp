#include <unistd.h>
#include <termios.h>
#include "editor.h"

int main(int argc, char* argv[]){ 
    EnableRawMode();
    Editor();
    if (argc>=2){
        OpenEditor(argv[1]);
    }
    EditorSetStatusMessage("HELP: Ctrl-S = save | Ctrl-Q = quit");
    while (true){
        ScreenRefresh();
        Keypress();
    }
    return 0;
}
