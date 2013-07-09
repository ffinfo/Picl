/* 
 * File:   CalclulateNoise.cpp
 * Author: peter
 * 
 * Created on May 7, 2013, 1:00 PM
 */

#include "CalclulateNoise.h"
#include <iostream>
#include "../filter/ClusterFile.h"
#include "../viewcoverage/CoverageFile.h"

CalclulateNoise::CalclulateNoise(Config* config) : Task(config) {
    this->TaskName = "CalclulateNoise";
    this->Ready = true;
}

CalclulateNoise::~CalclulateNoise() {
}

void CalclulateNoise::InternalRun() {
    for(map<string,ClusterFile::DataKey>::iterator it = config->clusterFile->Data.begin(); it != config->clusterFile->Data.end(); it++) {
        bool upstream1 = it->second.Ori.substr(0,1).compare("T") == 0;
        bool upstream2 = it->second.Ori.substr(1,2).compare("H") == 0;
        CoverageFile coverageFileRef1 = CoverageFile(config->coverageDir + "/" + it->second.Ref1 + ".cov");
        CoverageFile coverageFileRef2 = CoverageFile(config->coverageDir + "/" + it->second.Ref2 + ".cov");
        for (ClusterFile::DataValue &value : it->second.Values) {
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
    }
    //cout << config->clusterFile->PrintFile(0, 0);
    config->clusterFile->ReWriteFile();
}

void CalclulateNoise::InternalDoWhilePauze() {
    
}

void CalclulateNoise::InternalFinish() {
    
}