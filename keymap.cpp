#include "keymap.h"
#include "editor.h"
void EditorMoveCursor(EditorConfig& E,int key){
    std::string* row = (E.cy >= (int)E.rows.size()) ? nullptr : &E.rows[E.cy];
    int rx = E.cx;
    if (row){
        rx = EditorRowRxToCxConverter(E, *row, E.cx);
    }
    switch(key){
        case 'h':
        case ARROW_LEFT:
            if (E.cx != 0){
                E.cx--;
            } else if (E.cy > 0 ){
                E.cy--;
                E.cx = E.rows[E.cy].length();
            }
            break;
        case 'l':
        case ARROW_RIGHT:
            if (row && E.cx < (int)row -> length()){
                E.cx++;
            } else if (row && E.cx == (int)row->length()) {
                E.cy++;
                E.cx = 0;
            }
            break;
        case ARROW_UP:
        case 'k':
            if (E.cy != 0){
                E.cy--;
            }
            break;
        case 'j':
        case ARROW_DOWN:
            if (E.cy < (int)E.rows.size()){
                E.cy++;
            }
            break;
    }
    
    int rowlen = row ? row->length() : 0;
    
    if (E.cx > rowlen) {
        E.cx = rowlen;
    }
}
