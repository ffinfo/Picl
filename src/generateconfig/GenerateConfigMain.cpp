/* 
 * File:   GenerateConfigMain.cpp
 * Author: peter
 * 
 * Created on April 25, 2013, 3:11 PM
 */

#include "GenerateConfigMain.h"
#include "../Config.h"

GenerateConfigMain::GenerateConfigMain() {
}

GenerateConfigMain::~GenerateConfigMain() {
}

int GenerateConfigMain::usage() {
    cerr << "Usage: <name program> generateconfig <targetfile>" << endl;
    return 1;
}

int GenerateConfigMain::main(int argc, char* argv[]) {
    if (argc < 2) return this->usage();
    string configFile = argv[argc-1];
    Config config;
    cout << "Workspace: ";
    cin >> config.Workspace;
    cout << "Library Name: ";
    cin >> config.LibaryName;
    cout << "Use Paired Bam (Y/y/N/n): ";
    char UsePaired;
    cin >> UsePaired;
    if (UsePaired == 'Y' or UsePaired == 'y') {
        config.UsePairedBam = true;
        cout << "Paired BamFile: ";
        cin >> config.PairedBam;
        cout << "Paired BamIndex: ";
        cin >> config.PairedIndex;
    } else {
        config.UsePairedBam = false;
        cout << "First/Forward BamFile: ";
        cin >> config.ForwardBam;
        cout << "First/Forward BamIndex: ";
        cin >> config.ForwardIndex;
        cout << "Second/Reverse BamFile: ";
        cin >> config.ReverseBam;
        cout << "Second/Reverse BamIndex: ";
        cin >> config.ReverseIndex;
    }
    cout << "Choose a library type for orientation reads:" << endl;
    cout << "1: Paired-end:" << endl;
    cout << "2: MatePair Solid:" << endl;
    cout << "3: MatePair Ilumnia:" << endl;
    unsigned short libtype;
    cin >> libtype;
    switch (libtype) {
        case 1: 
            config.ForwardOriInverted = false;
            config.ForwardOriInverted = true;
            break;
        case 2: 
            config.ForwardOriInverted = true;
            config.ForwardOriInverted = true;
            break;
        case 3: 
            config.ForwardOriInverted = false;
            config.ForwardOriInverted = false;
            break;
        default:
            cerr << "Non existing libtype" << endl;
            exit(1);
    }
    
    config.writeConfigFile(configFile);
    
    return 0;
}