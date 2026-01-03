#include "path.h"
#include "editor.h"
#include <string>
#include <iostream>
void ParseArgs(EditorConfig& E,int argc, char* argv[]) {
    if (argc < 2) return; 

    std::string arg = argv[1];
    std::string targetFile = "";


    if (arg == "-mambo") {
        targetFile = "src/assets/mambo";
        E.readonly = true;
    } 
    else if (arg == "-t") {
        targetFile = "src/assets/tutor";
    } 
    else if (arg == "-v" || arg == "--version") {
        DisableRawMode(E);
        std::cout << "Yora Editor -- Version " << E.version << " \n";
        exit(0);
    }
    else {
        targetFile = arg;
    }
    if (!targetFile.empty()) {
        OpenEditor(E, targetFile);
    }
}
