/* 
 * File:   ViewCoverage.cpp
 * Author: peter
 * 
 * Created on March 29, 2013, 1:00 PM
 */

#include <fstream>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <algorithm>

#include "ViewCoverage.h"
#include "CoverageFile.h"

ViewCoverage::ViewCoverage() {
}

ViewCoverage::~ViewCoverage() {
}

int ViewCoverage::usage() {
    cerr << "Usage: <name program> viewcoverage (options) <coverage file (cov)>" << endl;
    cerr << "Options:" << endl;
    cerr << "   -region <start-end> : limit to selected region" << endl;
    return 1;
}


int ViewCoverage::main(int argc, char* argv[]) {
    if (argc < 2) return this->usage();
    unsigned int start = 0, end = 0;
    for (int t = 1; t < (argc - 1); t++) {
        if (strcmp(argv[t], "-region") == 0) {
            string value = argv[t+1];
            int split = value.find('-');
            start = atoi(value.substr(0, split).c_str());
            end = atoi(value.substr(split+1, value.length()).c_str());
            t++;
        }
    }
    string filename = argv[argc - 1];
    
    CoverageFile coverageFile = CoverageFile(filename);
    
    if (end == 0) end = coverageFile.getBinCount() * coverageFile.getBinsize() - 1;
    vector<CoverageFileRead> reads = coverageFile.getRegion(start, end);
    
    int t = coverageFile.getBin(start);
    for (CoverageFileRead read : reads) {
        unsigned int position = t * coverageFile.getBinsize();
        if (start != end and position > end) break;
        cout << position << "\t";
        cout << read.Upstream.concortdantPairs << "\t" << read.Upstream.deletions << "\t";
        cout << read.Upstream.insertions << "\t" << read.Upstream.HT << "\t" << read.Upstream.HH << "\t" << read.Upstream.TT << "\t";
        cout << read.Upstream.translocations << "\t";
        cout << read.Downstream.concortdantPairs << "\t" << read.Downstream.deletions << "\t";
        cout << read.Downstream.insertions << "\t" << read.Downstream.HT << "\t" << read.Downstream.HH << "\t" << read.Downstream.TT << "\t";
        cout << read.Downstream.translocations << endl;
        t++;
    }
    return 0;
}