/* 
 * File:   ViewClustersMain.cpp
 * Author: peter
 * 
 * Created on March 18, 2013, 3:01 PM
 */

#include "ViewClustersMain.h"
#include "../filter/ClusterFile.h"
#include "../filter/ClusterFile.h"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <cstring>

ViewClustersMain::ViewClustersMain() {
}

ViewClustersMain::~ViewClustersMain() {
}

int ViewClustersMain::usage() {
    cerr << "Usage: <name program> viewclusters (options) <clusterfile (bpc)>" << endl;
    cerr << "Options:" << endl;
    cerr << "   -c <number> : min pairs" << endl;
    return 1;
}

int ViewClustersMain::main(int argc, char* argv[]) {
    if (argc < 2) return this->usage();
    int cuttoff = 0;
    for (int t = 1; t < (argc - 1); t++) {
        if (strcmp(argv[t], "-c") == 0) {
            cuttoff = atoi(argv[t+1]);
        }
        //cout << argv[t] << endl; 
    }
    string filename = argv[argc - 1];
    
    ClusterFile clusterFile;
    clusterFile.ReadFile(filename, cuttoff, 0, 0, 0, 0);
    
    cout << clusterFile.PrintFile(0, 0);
    exit (0);
    
    fstream file;
    file.open(filename, ios::binary | ios::in);
    
    //cout << "Ref header" << endl;
    map<unsigned int,string> ClusterFileRefHeader;
    unsigned int refcount;
    file.read((char*) & refcount, sizeof(unsigned int));
    for (unsigned int it = 0; it < refcount; it++) {
        unsigned int length;
        file.read((char*) & length, sizeof(unsigned int));
        char name[length];
        file.read((char*) & name, length);
        string nameS = name;
        nameS = nameS.substr(0, length);
        ClusterFileRefHeader[it+1] = nameS;
        //cout << (it+1) << " - " << nameS << endl;
    }
    
    //cout << "Type header" << endl;
    map<unsigned int,string> ClusterFileTypeHeader;
    unsigned int typecount;
    file.read((char*) & typecount, sizeof(unsigned int));
    for (unsigned int it = 0; it < typecount; it++) {
        unsigned int length;
        file.read((char*) & length, sizeof(unsigned int));
        char type[length];
        file.read((char*) & type, length);
        string typeS = type;
        typeS = typeS.substr(0, length);
        ClusterFileTypeHeader[it] = typeS;
        //cout << it << " - " << typeS << endl;
    }
    
    int t = 0;
    while (not file.eof()) {
        t++;
        ClusterFile::ClusterFileRead read;
        file.read((char*) & read, sizeof(ClusterFile::ClusterFileRead));
        if (read.pairs - read.clone < cuttoff) continue;
        cout << ClusterFileRefHeader[read.ref1] << "\t" << read.start1 << "\t" << read.end1 << "\t" ;
        cout << ClusterFileRefHeader[read.ref2] << "\t" << read.start2 << "\t" << read.end2 << "\t";
        switch (read.ori) {
            case 0:
                cout << "TH" << "\t";
                break;
            case 1:
                cout << "HT" << "\t";
                break;
            case 2:
                cout << "HH" << "\t";
                break;
            case 3:
                cout << "TT" << "\t";
                break;
        }
        cout << (read.pairs - read.clone) << "\t" << read.clone << "\t";
        cout << ClusterFileTypeHeader[read.type] << "\t";
        cout << read.overlap << "\t" << read.prob_size << "\t" << read.probability << endl;
    }
    
    file.close();
    
    return 0;
}