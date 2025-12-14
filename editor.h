#ifndef EDITOR_H
#define EDITOR_H
#include <string>
#include <termios.h>
#include <vector>
struct EditorConfig{
    int screenrows;
    int screencols;
    int cy = 0;
    int cx = 0;
    int numrows;
    int rowoff;
    int coloff;
    int tabstop = 4;
    int dirty;
    std::vector<std::string> rows;
    std::string filename;
    time_t statusmsgtime = 0;
    std::string statusmsg;
    std::string version = "0.0.1";
    struct termios termios_ori;
};
extern struct EditorConfig E;

void die(const char *s);
void DisableRawMode();
void EnableRawMode();
int ReadMode();
void Keypress();
void EditorScroll();
void StatusBar(std::string& ab);
void EditorSetStatusMessage(const char *fmt, ...);
void DrawMessageBar(std::string& ab);
void ScreenRefresh();
int EditorRowCxToRxConverter(const std::string& row, int cx);
void EditorRowInsertChar(std::string& row, int at, int c);
void EditorInsertChar(int c);
void EditorInesertNewline();
void EditorDelchar();
void DrawRows();
int GetWindowSize(int *rows, int *cols);
void OpenEditor(const std::string& filename);
std::string EditorPromt(std::string prompt);
void EditorSave();
void Editor();

#endif 
