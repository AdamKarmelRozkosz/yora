#include "editor.h"
#include <cctype> // required for iscntrl
#include <termios.h> // library required for working with terminal
#include <cstdlib> 
#include <unistd.h> // reqired for STDOUT_FILENO 
#include <iostream>
#include "keymap.h" 
#include <sys/ioctl.h> // required to get the size of the window 
#include <string> 
#include <fstream> // file manipulation
#include <ctime> // time 
#include <cstdarg> // required for va_list and printf like printing

struct EditorConfig E; // config variable

void die(const char *s){ //error handling
    write(STDOUT_FILENO, "\x1b[2J", 4); //x1b == 'esc' in ANSI 
    write(STDOUT_FILENO, "\x1b[H", 3); 

    perror(s); // seeks for the global variable errno to chech whether there was an error
    exit(1);
}

void DisableRawMode(){
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.termios_ori) == -1) // goes back to the previous terminal settings
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

int ReadMode(){
    int uread; //stands for user read
    char c;
    while ((uread=read(STDIN_FILENO, &c, 1)) !=1){ // reads the input from the user 
        if(uread == -1){
            die("read");
        }
    }
    if (c == '\x1b') { // checs whether it is an escape sequence 
        char seq[3];
        if (read(STDIN_FILENO, &seq[0], 1) != 1) return '\x1b';
        if (read(STDIN_FILENO, &seq[1], 1) != 1) return '\x1b';
        if (seq[0] == '[') {
        if (seq[1] >= '0' && seq[1] <= '9') {
        if (read(STDIN_FILENO, &seq[2], 1) != 1) return '\x1b';
        if (seq[2] == '~') {
            switch (seq[1]) {
                case '1': return HOME_KEY;
                case '3': return DEL_KEY;
                case '4': return END_KEY;
                case '5': return PAGE_UP;
                case '6': return PAGE_DOWN;
                case '7': return HOME_KEY;
                case '8': return END_KEY;
                }
            }
        } else {
            switch (seq[1]) {
                case 'A': return ARROW_UP;
                case 'B': return ARROW_DOWN;
                case 'C': return ARROW_RIGHT;
                case 'D': return ARROW_LEFT;
                case 'H': return HOME_KEY;
                case 'F': return END_KEY;
            }
        }
    } else if (seq[0] == 'O'){
        switch (seq[1]){
            case 'H': return HOME_KEY;
            case 'F': return END_KEY;
        }
    }
    return '\x1b'; // returns escape
  } else {
    return c;
  }
}
void EditorScroll(){
    if (E.cy < E.rowoff){ // vertical scrolling
        E.rowoff = E.cy;
    }
    if (E.cy >= E.rowoff + E.screenrows){
            E.rowoff = E.cy - E.screenrows + 1;
    }
    if (E.cx < E.coloff){ //horizontal scrolling 
        E.coloff = E.cx;
    }
    if (E.cx >= E.coloff + E.screencols){
        E.coloff = E.cx - E.screencols +1;
    }
}
int EditorRowCxToRxConverter(const std::string &row, int cx){ // converts to render index which helps with the rendering of tabs
    int rx = 0;
    for (int j = 0; j < cx; j++) {
        if (row[j] == '\t') {
            rx += (E.tabstop - 1) - (rx % E.tabstop);
        }
        rx++;
    }
    return rx;
}
void EditorRowInsertChar(std::string& row, int at, int c) { // inserts a character into a row string
    if (at < 0 || at > (int)row.length()) at = row.length();
    row.insert(at, 1, (char)c);
    E.dirty++; // check whether the file was modified
}


void EditorInsertChar(int c) { // helps insert character at the current cursor position
    if (E.cy == (int)E.rows.size()) {
        E.rows.push_back("");
    }
    
    EditorRowInsertChar(E.rows[E.cy], E.cx, c);
    
    E.cx++;
}
void EditorInsertNewLine() { // inserts newline and goes to the next row
    if (E.cx == 0) {
        E.rows.insert(E.rows.begin() + E.cy, "");
    } else {
        // spiting the line into two
        std::string current = E.rows[E.cy];
        E.rows[E.cy] = current.substr(0, E.cx);
        E.rows.insert(E.rows.begin() + E.cy + 1, current.substr(E.cx));
    }
    E.cy++;
    E.cx = 0;
    E.dirty++;
}
void EditorDelChar() { // deletes the characters from the file
    if (E.cy == (int)E.rows.size()) return;
    if (E.cx == 0 && E.cy == 0) return;

    if (E.cx > 0) {
        // delete in the middle of the line
        E.rows[E.cy].erase(E.cx - 1, 1);
        E.cx--;
    } else {
        // delete at start of line and join with the previous
        E.cx = E.rows[E.cy - 1].length();
        E.rows[E.cy - 1] += E.rows[E.cy];
        E.rows.erase(E.rows.begin() + E.cy);
        E.cy--;
    }
    E.dirty++;
}
void DrawRows(std::string& ab){ // function used for drawing the TUI 
    int y;
    for (y = 0; y < E.screenrows; y++){
        int filerow = y + E.rowoff;
        if (filerow < (int)E.rows.size()){ // draws text from the file whether there is something
            std::string line = E.rows[filerow];

            std::string renderline = "";
            int idx = 0; // manual tab rendering
            for (char c : line) {
                if (c == '\t') {
                    renderline += ' ';
                    idx++;
                    while (idx % E.tabstop != 0) {
                        renderline += ' ';
                        idx++;
                    }
                } else {
                    renderline += c;
                    idx++;
                }
            }
            // horizontal scroll truncation
            int len = renderline.length();
            int effective_len = len - E.coloff;
            if (effective_len < 0) effective_len = 0;
            if (effective_len > E.screencols) effective_len = E.screencols;

            if (effective_len > 0) {
                ab += renderline.substr(E.coloff, effective_len);
                }
            } else { // draws the welcome message only if the file is empty
                if (E.rows.empty() && y == E.screenrows / 3){
                    std::string Welcome = "Yora editor 夜空 -- Version " + E.version;
                    int WelcomeLen = Welcome.length();
                    if (WelcomeLen > E.screencols - WelcomeLen){
                        WelcomeLen = E.screencols;
                    }
                int padding = (E.screencols - WelcomeLen) / 2;
                if (padding){
                        ab += "~";
                        padding--;
                    }
                while (padding--){
                        ab += " ";
                    }
                    
                    ab += Welcome.substr(0,WelcomeLen);
                } else {
                    ab += "~"; // draws the tilde when there is a empty line 
                }
            }
            ab += "\x1b[K"; // clear line to right
            ab += "\r\n"; // newline
        }
    }
void StatusBar(std::string& ab){
    ab += "\x1b[7m"; // inverts colors 
                     //
    std::string status = E.filename.empty() ? "[No Name]" : E.filename;
    status += " - " + std::to_string(E.rows.size()) + " lines";

    if (E.dirty) status += " (modified)"; // displays whether the file was modified

    int len = status.length();
    if (len > E.screencols) len = E.screencols;
    ab += status.substr(0, len);

    std::string rstatus = std::to_string(E.cy + 1) + "/" + std::to_string(E.rows.size()); // line number aligned to the right
    
    while (len < E.screencols) {
        if (E.screencols - len == (int)rstatus.length()) {
            ab += rstatus;
            break;
        } else {
            ab += " ";
            len++;
        }
    }

    ab += "\x1b[m"; // resets colors
    ab += "\r\n";   
}
void EditorSetStatusMessage(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    char buf[80];
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    
    E.statusmsg = buf;
    E.statusmsgtime = time(NULL);}

void DrawMessageBar(std::string& ab) {
    ab += "\x1b[K"; //clears the line
    int msglen = E.statusmsg.length();
    if (msglen > E.screencols) msglen = E.screencols;
    if (msglen && (time(NULL) - E.statusmsgtime < 5)) { // displays the message only if it is younger than 5 sec
        ab += E.statusmsg.substr(0, msglen);
    }
}
void ScreenRefresh(){ // main function that combines all the visual functions
    EditorScroll(); // lets the user scroll
    std::string ab; // string buffer
    
    ab += "\x1b[?25l"; // hide cursor
    ab += "\x1b[H"; // move cursor "HOME"

    DrawRows(ab);
    StatusBar(ab);
    DrawMessageBar(ab);

    int rx = E.cx; // function that calculates the cursor position relative to tabs
    if (E.cy < (int)E.rows.size()) {
        rx = EditorRowCxToRxConverter(E.rows[E.cy], E.cx);
    }
    // move cursor comand
    ab += "\x1b[" + std::to_string((E.cy -E.rowoff) + 1) + ";" + std::to_string((rx - E.coloff) + 1) + "H";
    ab += "\x1b[?25h"; // show cursor 

    write(STDOUT_FILENO,ab.c_str(),ab.length());
}
int GetWindowSize(int *rows, int *cols){ // gets the windows size 
    struct winsize winsize;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &winsize) == -1 || winsize.ws_col == 0){
        return -1;
    } else {
        *cols = winsize.ws_col;
        *rows = winsize.ws_row;
        return 0;
    }
}
void OpenEditor(const std::string& filename){ // function that alows us to open the file with an editor 
    E.filename = filename;
    std::ifstream file(filename);
    
    if (!file.is_open()){
        return;
    }

    std::string line;
    while (std::getline(file,line)){ // line reading
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        E.rows.push_back(line);
    }
}
std::string EditorPrompt(std::string prompt) {
    std::string buf = "";

    while (true) {
        // Display the prompt and the current buffer content
        EditorSetStatusMessage(prompt.c_str(), buf.c_str());
        ScreenRefresh();

        int c = ReadMode();

        // Confirm with Enter
        if (c == '\r') {
            if (buf.length() != 0) {
                EditorSetStatusMessage(""); // Clear message
                return buf;
            }
        } 
        // Cancel with Escape
        else if (c == '\x1b') {
            EditorSetStatusMessage("");
            return ""; 
        } 
        // Handle Backspace
        else if (c == BACKSPACE || c == CTRL_KEY('h') || c == DEL_KEY) {
            if (!buf.empty()) buf.pop_back();
        } 
        // Append typed characters
        else if (!iscntrl(c) && c < 128) {
            buf += (char)c;
        }
    }
}

void EditorSave(){  // option to save the file 
    if (E.filename.empty()) {
        E.filename = EditorPrompt("Save as: %s (ESC to cancel)");
        if (E.filename.empty()) {
            EditorSetStatusMessage("Save aborted");
            return;
        }
    }

    std::ofstream file(E.filename);
    if (file.is_open()) {
        for (const auto& row : E.rows) {
            file << row << "\n";
        }
        E.dirty = 0; // clears the dirty buffer 
        EditorSetStatusMessage("Saved file: %s (%d lines)", E.filename.c_str(), (int)E.rows.size()); // prints out the message of the saving
    } else {
        EditorSetStatusMessage("Can't save! I/O error: %s", E.filename.c_str());
    }}

void Keypress(){
    static int quit_times = 3;
    int c = ReadMode();
    switch (c) {
    case '\r': // enter key 
        EditorInsertNewLine();
        break;
    case CTRL_KEY('s'):
        EditorSave();
        break;
    case CTRL_KEY('q'):
        if (E.dirty && quit_times > 0) { // checks whether there are no unsaved changes and desnt let the user close the file if it is not
            EditorSetStatusMessage("WARNING!!! File has unsaved changes. "
                                   "Press Ctrl-Q %d more times to quit.", quit_times);
            quit_times--;
            return; 
        }
        write(STDOUT_FILENO, "\x1b[2J", 4); // Escape sequence that errases entire screen
        write(STDOUT_FILENO, "\x1b[H", 3); // Escape sequence that moves cursor to the home position
        exit(0);
        break;
    // case 'h':
    case ARROW_LEFT:
    // case 'l':
    case ARROW_RIGHT:
    // case 'k':
    case ARROW_UP:
    // case 'j':
    case ARROW_DOWN:
        EditorMoveCursor(c);
        break;
    case PAGE_UP:
    case PAGE_DOWN:
        if (c == PAGE_UP){
            E.cy = E.rowoff;
            break;
        } else if (c == PAGE_DOWN){
            E.cy = E.rowoff + E.screenrows -1;
            break;
        }
    case HOME_KEY:
        E.cx = 0;
        break;
    case END_KEY:
        E.cx = E.rows[E.cy].length();
        break;
    case BACKSPACE:
    case DEL_KEY:
        EditorDelChar();
        break;
    case '\x1b':
        break;
    default:
        if(!iscntrl(c) || c == '\t'){ // allows user to type in the editor
            EditorInsertChar(c);
        }
        break;
    }
    if (c != CTRL_KEY('q') && c != '\x1b') { 
        quit_times = 3;
    }
}
void Editor(){ // initializing the editor
    E.cx = 0;
    E.cy = 0;
    E.numrows = 0;
    E.rowoff = 0;
    E.coloff = 0;
    E.dirty = 0;
    if (GetWindowSize(&E.screenrows, &E.screencols) == -1){
        die("GetWindowSize");
    }
    E.screenrows -= 2;
}
