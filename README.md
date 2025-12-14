# Yora 夜空 ligthweight TUI text editor
## Compatibility
This editor currently works under unix based operating systems.
## Instalation
### Prerequisites 
You need a C/C++ compiler like clang or gcc and make to build this app.  
### Build
1. Clone the repo: 
```
git clone https://github.com/AdamKarmelRozkosz/yora.git
```
2. Built the program:
```
sudo make install 
```  
Note: You might want to change your compiler in makefile. The default one is clang++  
  
## Usage 
To open the editor simply type yora in your terminal, you can also specify the file you want to open
```
yora [filename]
```
### Keybindings 
| Key | Action |
| -------------- | --------------- |
| Ctrl-Q | Quit the editor |
| Ctrl-S | Save the changes |
| Arrow keys | Move around the editor |
