#ifndef KEYMAP_H
#define KEYMAP_H
#define CTRL_KEY(k) ((k) & 0x1f)
enum EditorKey {
  CTRL_H = 8,
  CTRL_Q = 17,
  CTRL_S = 19,
  BACKSPACE = 127,
  ARROW_LEFT = 1000,
  ARROW_RIGHT,
  ARROW_UP,
  ARROW_DOWN,
  DEL_KEY,
  HOME_KEY,
  END_KEY,
  PAGE_UP,
  PAGE_DOWN
};
void EditorMoveCursor(int key);
#endif 
