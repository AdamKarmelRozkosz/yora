#ifndef EDITOR_H
#define EDITOR_H
#include <string>
#include <termios.h>
#include <vector>

struct EditorConfig {
    int screenrows = 0;
    int screencols = 0;
    int cy = 0;
    int cx = 0;
    int numrows = 0;
    int rowoff = 0;
    int coloff = 0;
    int tabstop = 8;
    int dirty = 0;
    int rx = 0;
    
    std::vector<std::string> rows;
    std::string filename;
    std::string statusmsg;
    time_t statusmsgtime = 0;
    std::string version = "0.0.1";
    struct termios termios_ori;
};
void die(const char *s);
void DisableRawMode(EditorConfig& E);
void EnableRawMode(EditorConfig& E);
int ReadMode();
void Keypress(EditorConfig& E);
void EditorScroll(EditorConfig& E);
void StatusBar(EditorConfig& E, std::string& ab);
void EditorSetStatusMessage(EditorConfig& E, const char *fmt, ...);
void DrawMessageBar(EditorConfig& E, std::string& ab);
void ScreenRefresh(EditorConfig& E);
int EditorRowCxToRxConverter(EditorConfig& E, const std::string& row, int cx);
int EditorRowRxToCxConverter(EditorConfig& E, const std::string& row, int rx);
void EditorRowInsertChar(EditorConfig& E, std::string& row, int at, int c);
void EditorInsertChar(EditorConfig& E, int c);
void EditorInesertNewline(EditorConfig& E);
void EditorDelchar(EditorConfig& E);
void DrawRows(EditorConfig& E);
int GetWindowSize(int *rows, int *cols);
void OpenEditor(EditorConfig& E,const std::string& filename);
std::string EditorPromt(EditorConfig& E, std::string prompt);
void EditorSave(EditorConfig& E);
void EditorFind(EditorConfig& E);
void Editor(EditorConfig& E);

#endif 
