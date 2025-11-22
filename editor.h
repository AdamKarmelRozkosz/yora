#ifndef EDITOR_H
#define EDITOR_H

void die(const char *s);
void DisableRawMode();
void EnableRawMode();
char ReadMode();
void Keypress();
void ScreenRefresh();
void DrawRows();
int GetWindowSize(int *rows, int *cols);
void Editor();

#endif 
