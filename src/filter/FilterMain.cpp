/* 
 * File:   FilterMain.cpp
 * Author: peter
 * 
 * Created on March 19, 2013, 3:54 PM
 */

#include "FilterMain.h"
#include "../viewcoverage/CoverageFile.h"
#include <cstdlib>
#include <stdio.h>
#include <iostream>
#include <cstring>
#include <algorithm>

using namespace std;

FilterMain::FilterMain() {
}

FilterMain::~FilterMain() {
}

int FilterMain::usage() {
    cerr << "Usage: <name program> filter (options) <clusterfile (bpc)>" << endl;
    cerr << "Options:" << endl;
    cerr << "   -c <number> : manual cuttoff, if not set program try to do this automatic" << endl;
    cerr << "   --cuttoff <number> : same as -c" << endl;
    cerr << "   -p <number> : cuttoff on " << endl;
    cerr << "   -i <clusterfile (bpc)> : Input file, if no -i defined last argument used as input file" << endl;
    cerr << "   -a <clusterfile (bpc)> : Filter clusters that have overlap with this dataset, this options can used more then once" << endl;
    cerr << "   -mina <number> : Minimal overlaps in against needed for filtering, default is 1" << endl;
    cerr << "   -m <clusterfile (bpc)> : Only show clusters that have overlap with this dataset, this options can used more then once" << endl;
    cerr << "   -minm <number> : Minimal overlaps in must needed for NOT filtering, default is number of -m options" << endl;
    //cerr << "   -coveragedir <directory of coverage files> : needed for concordent and noise ratio's" << endl;
    cerr << "   -maxnoise <number> : max noise ratio, at 1 noise = sv-coverage" << endl;
    cerr << "   -minconcordant <number> : needed for concordent and noise ratio's" << endl;
    cerr << "   -maxconcordant <number> : needed for concordent and noise ratio's" << endl;
    return 1;
}

int FilterMain::main(int argc, char* argv[]) {
    if (argc < 2) {
        this->usage();
        exit(1);
    }
    
    InputFile = argv[argc-1];
    for (int t = 1; t < (argc - 1); t++) {
        if (strcmp(argv[t], "-c") == 0 or strcmp(argv[t], "--cuttoff") == 0) {
            cuttoff = atoi(argv[t+1]);
            t++;
        } else if (strcmp(argv[t], "-i") == 0) {
            InputFile = argv[t+1];
            t++;
        } else if (strcmp(argv[t], "-p") == 0) {
            prob_cutoff = atof(argv[t+1]);
            t++;
        } else if (strcmp(argv[t], "-a") == 0) {
            Against.push_back(argv[t+1]);
            t++;
        } else if (strcmp(argv[t], "-m") == 0) {
            Must.push_back(argv[t+1]);
            t++;
        } else if (strcmp(argv[t], "-mina") == 0) {
            MinAgainst = atoi(argv[t+1]);
            t++;
        } else if (strcmp(argv[t], "-minm") == 0) {
            MinMustOption = atoi(argv[t+1]);
            t++;
        } else if (strcmp(argv[t], "-coveragedir") == 0) {
            CoverageDir = argv[t+1];
            t++;
        } else if (strcmp(argv[t], "-maxnoise") == 0) {
            max_noise = atof(argv[t+1]);
            t++;
        } else if (strcmp(argv[t], "-maxconcordant") == 0) {
            max_concordant = atof(argv[t+1]);
            t++;
        } else if (strcmp(argv[t], "-minconcordant") == 0) {
            min_concordant = atof(argv[t+1]);
            t++;
        }
    }
    if (MinMustOption != 0) MinMust = MinMustOption;
    else MinMust = Must.size();
    
    Input.ReadFile(InputFile, cuttoff, prob_cutoff, max_noise, max_concordant, min_concordant);
    
    for (string file : Against) {
        ClusterFile AgainstFile;
        AgainstFile.ReadFile(file);
        for(map<string,ClusterFile::DataKey>::iterator InputIt = Input.Data.begin(); InputIt != Input.Data.end(); InputIt++) {
            for (ClusterFile::DataValue &value : InputIt->second.Values) {
                if (HasOverlap(value, AgainstFile.Data[InputIt->second.getKey()])) {
                    value.against++;
                    continue;
                }
            }
        }
    }
    
    for (string file : Must) {
        ClusterFile MustFile;
        MustFile.ReadFile(file);
        for(map<string,ClusterFile::DataKey>::iterator InputIt = Input.Data.begin(); InputIt != Input.Data.end(); InputIt++) {
            for (ClusterFile::DataValue &value : InputIt->second.Values) {
                if (HasOverlap(value, MustFile.Data[InputIt->second.getKey()])) {
                    value.must++;
                    break;
                }
            }
        }
    }
    
    /*if (CoverageDir.compare("") != 0) for(map<string,ClusterFile::DataKey>::iterator InputIt = Input.Data.begin(); InputIt != Input.Data.end(); InputIt++) {
        CoverageFile coverageFileRef1 = CoverageFile(CoverageDir + "/" + InputIt->second.Ref1 + ".cov");
        CoverageFile coverageFileRef2 = CoverageFile(CoverageDir + "/" + InputIt->second.Ref2 + ".cov");
        bool upstream1 = InputIt->second.Ori.substr(0,1).compare("T") == 0;
        bool upstream2 = InputIt->second.Ori.substr(1,2).compare("H") == 0;
        for (ClusterFile::DataValue &value : InputIt->second.Values) {
            for (CoverageFileRead read : coverageFileRef1.getRegion(value.start1, value.end1)) {
                if (upstream1) {
                    value.FirstConcordantOverlap += read.Upstream.concortdantPairs;
                    value.FirstNoise += read.Upstream.HH + read.Upstream.HT + read.Upstream.TT + read.Upstream.translocations;
                    value.FirstNoise += read.Upstream.deletions + read.Upstream.insertions;
                }
                else {
                    value.FirstConcordantOverlap += read.Downstream.concortdantPairs;
                    value.FirstNoise += read.Downstream.HH + read.Downstream.HT + read.Downstream.TT + read.Upstream.translocations;
                    value.FirstNoise += read.Downstream.deletions + read.Downstream.insertions;
                }
            }
            for (CoverageFileRead read : coverageFileRef2.getRegion(value.start2, value.end2)) {
                if (upstream2) {
                    value.SecondConcordantOverlap += read.Upstream.concortdantPairs;
                    value.SecondNoise += read.Upstream.HH + read.Upstream.HT + read.Upstream.TT + read.Upstream.translocations;
                    value.SecondNoise += read.Upstream.deletions + read.Upstream.insertions;
                }
                else {
                    value.SecondConcordantOverlap += read.Downstream.concortdantPairs;
                    value.SecondNoise += read.Downstream.HH + read.Downstream.HT + read.Downstream.TT + read.Downstream.translocations;
                    value.SecondNoise += read.Downstream.deletions + read.Downstream.insertions;
                }
            }
        }
    }*/
    
    cout << Input.PrintFile(MinAgainst, MinMust);
    
    return 0;
}

bool FilterMain::HasOverlap(ClusterFile::DataValue &InputValue, ClusterFile::DataKey &OverlapSet) {
    //sort(OverlapSet.Values.begin(), OverlapSet.Values.end(), ClusterFile::SortDataValueOnStart1());
    for (ClusterFile::DataValue value : OverlapSet.Values) {
        if (value.end1 < InputValue.start1) continue;
        if (value.start1 > InputValue.end1) return false;
        if (value.end2 >= InputValue.start2 && value.start2 <= InputValue.end2) return true;
    }
    return false;
}