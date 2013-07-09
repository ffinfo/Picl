/* 
 * File:   CompareMain.cpp
 * Author: peter
 * 
 * Created on April 8, 2013, 12:33 PM
 */

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <cstring>
#include <algorithm>
#include <sstream>
#include "CompareMain.h"

CompareMain::CompareMain() {
}

CompareMain::~CompareMain() {
}

int CompareMain::usage() {
    cerr << "Usage: <name program> compare (options) <output filter>" << endl;
    cerr << "Options:" << endl;
    cerr << "   -i <file> : input file, no need when using pipe from filter" << endl;
    cerr << "   -minsize <number> : minimal size of deletions" << endl;
    cerr << "   -NA18507 <file> : Ref file" << endl;
    return 1;
}

int CompareMain::main(int argc, char* argv[]) {
    if (argc < 1) return this->usage();
    string NA18507FileName;
    string InputFileName;
    for (int t = 1; t < (argc - 1); t++) {
        if (strcmp(argv[t], "-NA18507") == 0) {
            NA18507FileName = argv[t+1];
        } else if (strcmp(argv[t], "-i") == 0) {
            InputFileName = argv[t+1];
        } else if (strcmp(argv[t], "-minsize") == 0) {
            minsize = atoi(argv[t+1]);
        }
    }
    
    while (!cin.eof()) {
        string input;
        getline (cin,input);
        if (input.empty()) break;
        if (input.find("deletion") == input.npos) continue;
        InputLines.push_back(input);
    }
    
    fstream RefFile;
    RefFile.open(NA18507FileName, ios::in);
    int toSmall = 0, use = 0;
    while (!RefFile.eof()) {
        string line, buf;
        getline (RefFile,line);
        if (line.empty()) break;
        stringstream ss(line);
        vector<string> tokens;
        while (ss >> buf) tokens.push_back(buf);
        
        SV_record record;
        string chr = string(tokens[4]).substr(string(tokens[4]).find("chr") + 3, string(tokens[4]).size());
        record.start = atoi(string(tokens[5]).c_str());
        record.end = atoi(string(tokens[6]).c_str());
        if (record.end-record.start > minsize) {
            RefSVs[chr].push_back(record);
            use++;
        }
        else toSmall++;
    }
    cout << "Useing : " << use << " / " << toSmall+use << endl;
    
    unsigned int overlap = 0;
    for (string line : InputLines) {
        string buf;
        stringstream ss(line);
        vector<string> tokens;
        while (ss >> buf) tokens.push_back(buf);
        string chr = tokens[0];
        unsigned int start = atoi(tokens[2].c_str());
        unsigned int end = atoi(tokens[4].c_str());
        for (SV_record &record : RefSVs[chr]) {
            if (start < record.start && start + 250 > record.start && end > record.end && end - 250 < record.end) {
                record.overlap = true;
                overlap++;
                break;
            }
        }
        //cout << chr << "\t" << start << "\t" << end << endl;
    }
    
    int refOverlap = 0;
    for (map<string, vector<SV_record>>::iterator it = RefSVs.begin(); it != RefSVs.end(); it++) {
        for (SV_record record : it->second) {
            if (record.overlap) {
                refOverlap++;
                if (record.end - record.start > 1000) cout << it->first << "\t" << record.start << "\t" << record.end << endl;
            }
        }
    }
    
    cout << "Input: " << overlap << " / " << InputLines.size() << endl;
    cout << "Ref: " << refOverlap << " / " << use << endl;
    
    return 0;
}