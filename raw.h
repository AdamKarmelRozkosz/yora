#ifndef RAW_H
#define RAW_H

void die(const char *s);
void DisableRawMode();
void EnableRawMode();
char ReadMode();
void Keypress();
void clear();

#endif 
