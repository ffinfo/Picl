/* 
 * File:   MultisampleMain.cpp
 * Author: peter
 * 
 * Created on May 3, 2013, 11:23 AM
 */

#include "MultisampleMain.h"
#include <cstdlib>
#include <stdio.h>
#include <iostream>
#include <cstring>
#include <algorithm>

using namespace std;

MultisampleMain::MultisampleMain() {
}

MultisampleMain::~MultisampleMain() {
}

int MultisampleMain::usage() {
    cerr << "Usage: <name program> multisample (options) <clusterfiles (bpc)>" << endl;
    cerr << "Options:" << endl;
    cerr << "         -minov       : Min overlap" << endl;
    cerr << "         -trimrefname : Trim for the refname, needed if samples have not the same refname, like 1 and chr1" << endl;
    cerr << "         -c           : Min pairs for single cluster" << endl;
    cerr << "   -maxnoise <number> : max noise ratio, at 1 noise = sv-coverage" << endl;
    cerr << "   -minconcordant <number> : needed for concordent and noise ratio's" << endl;
    cerr << "   -maxconcordant <number> : needed for concordent and noise ratio's" << endl;
    return 1;
}

int MultisampleMain::main(int argc, char* argv[]) {
    if (argc < 2) return this->usage();
    
    for (int t = 1; t < (argc); t++) {
        if (strcmp(argv[t], "-c") == 0 or strcmp(argv[t], "--cuttoff") == 0) {
            cuttoff = atoi(argv[t+1]);
            t++;
        } else if (strcmp(argv[t], "-minov") == 0) {
            minOV = atof(argv[t+1]);
            cerr << "minOV = " << minOV << endl;
            t++;
        } else if (strcmp(argv[t], "-trimrefname") == 0) {
            TrimRefNames.push_back(argv[t+1]);
            cerr << "TrimRefNames = " << argv[t+1] << endl;
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
        } else {
            cerr << "Reading: " << argv[t] << endl;
            ClusterFile *clusterFile = new ClusterFile();
            clusterFile->ReadFile(argv[t], TrimRefNames);
            clusterFiles.push_back(clusterFile);
            cerr << "Done Reading: " << argv[t] << "  ID: " << (clusterFiles.size() - 1) << endl;
        }
    }
    cerr << "Read all files" << endl;
        
    int sampleID = -1;
    for (ClusterFile *clusterFile : clusterFiles) {
        sampleID++;
        cerr << "Starting clustering on: " << clusterFile->filename << endl;
        for (map<string,ClusterFile::DataKey>::iterator it = clusterFile->Data.begin(); it != clusterFile->Data.end(); it++) {
            for (ClusterFile::DataValue &sampleValue : it->second.Values) {
                MultiDataValue * overlapValue = 0;
                double ov1 = 0, ov2 = 0;
                for (MultiDataValue &clusterValue : Data[it->first].Values) {
                    if (clusterValue.first.end1 < sampleValue.start1) continue;
                    if (clusterValue.first.start1 > sampleValue.end1) break;
                    if (clusterValue.first.end2 < sampleValue.start2) continue;
                    if (clusterValue.first.start2 > sampleValue.end2) continue;
                    unsigned int overhang1 = 0;
                    if (sampleValue.start1 < clusterValue.first.start1) overhang1 += (clusterValue.first.start1 - sampleValue.start1);
                    if (sampleValue.end1 > clusterValue.first.end1) overhang1 += (sampleValue.end1 = clusterValue.first.end1);
                    double tempov1 = double(sampleValue.end1 - sampleValue.start1 - overhang1) / double(sampleValue.end1 - sampleValue.start1);
                    unsigned int overhang2 = 0;
                    if (sampleValue.start2 < clusterValue.first.start2) overhang2 += (clusterValue.first.start2 - sampleValue.start2);
                    if (sampleValue.end2 > clusterValue.first.end2) overhang2 += (sampleValue.end2 = clusterValue.first.end2);
                    double tempov2 = double(sampleValue.end2 - sampleValue.start2 - overhang2) / double(sampleValue.end2 - sampleValue.start2);
                    //if (tempov1 < minOV or tempov2 < minOV) continue;
                    if (tempov1 + tempov2 > ov1 + ov2) {
                        overlapValue = &clusterValue;
                        ov1 = tempov1;
                        ov2 = tempov2;
                    }
                }
                if (overlapValue == 0) {
                    MultiDataValue tempValue;
                    tempValue.first.start1 = sampleValue.start1;
                    tempValue.first.end1 = sampleValue.end1;
                    tempValue.first.start2 = sampleValue.start2;
                    tempValue.first.end2 = sampleValue.end2;
                    SampleDataValue tempSampleDataValue;
                    tempSampleDataValue.sampleID = sampleID;
                    tempSampleDataValue.value = &sampleValue;
                    tempValue.second.push_back(tempSampleDataValue);
                    if (Data[it->first].Values.empty()) {
                        Data[it->first].Ref1 = it->second.Ref1;
                        Data[it->first].Ref2 = it->second.Ref2;
                        Data[it->first].Ori = it->second.Ori;
                        Data[it->first].Type = it->second.Type;
                    }
                    Data[it->first].Values.push_back(tempValue);
                } else {
                    if (overlapValue->first.start1 > sampleValue.start1) overlapValue->first.start1 = sampleValue.start1;
                    if (overlapValue->first.end1 < sampleValue.end1) overlapValue->first.end1 = sampleValue.end1;
                    if (overlapValue->first.start2 > sampleValue.start2) overlapValue->first.start2 = sampleValue.start2;
                    if (overlapValue->first.end2 < sampleValue.end2) overlapValue->first.end2 = sampleValue.end2;
                    SampleDataValue tempSampleDataValue;
                    tempSampleDataValue.sampleID = sampleID;
                    tempSampleDataValue.value = &sampleValue;
                    overlapValue->second.push_back(tempSampleDataValue);
                }
            }
            sort(Data[it->first].Values.begin(), Data[it->first].Values.end(), SortOnS1());
        }
    }
    cerr << "First clustering done" << endl;
    
    // Here comes secondary clustering
    
    for (map<string,DataKey>::iterator it = Data.begin(); it != Data.end(); it++) {
        //cerr << it->second.Ref1 << endl;
        for (MultiDataValue value : it->second.Values) {
            vector<unsigned int> samples;
            unsigned int FirstConcordantOverlap = 0, SecondConcordantOverlap = 0;
            unsigned int FirstNoise = 0, SecondNoise = 0;
            unsigned int pairs = 0;
            for (SampleDataValue sampleDataValue : value.second) {
                samples.push_back(sampleDataValue.sampleID);
                FirstConcordantOverlap += sampleDataValue.value->FirstConcordantOverlap;
                SecondConcordantOverlap += sampleDataValue.value->SecondConcordantOverlap;
                FirstNoise += sampleDataValue.value->FirstNoise;
                SecondNoise += sampleDataValue.value->SecondNoise;
                pairs += sampleDataValue.value->pairs;
            }
            FirstConcordantOverlap = round((double)FirstConcordantOverlap / (double)value.second.size());
            SecondConcordantOverlap = round((double)SecondConcordantOverlap / (double)value.second.size());
            FirstNoise = round((double)FirstNoise / (double)value.second.size());
            SecondNoise = round((double)SecondNoise / (double)value.second.size());
            pairs = round((double)pairs / (double)value.second.size());
            samples.resize(distance(samples.begin(),unique(samples.begin(), samples.end())));
            
            if (pairs < cuttoff) continue;
            
            if (FirstConcordantOverlap == 0) FirstConcordantOverlap++;
            if (SecondConcordantOverlap == 0) SecondConcordantOverlap++;
            float relativeConcordant = ((float)pairs / (float)FirstConcordantOverlap + (float)pairs / (float)SecondConcordantOverlap) / 2;
            if (max_concordant > 0 and relativeConcordant > max_concordant) continue;
            if (min_concordant > 0 and relativeConcordant < min_concordant) continue;
            
            if (FirstNoise > pairs) FirstNoise -= pairs;
            else FirstNoise = 0;
            if (SecondNoise > pairs) SecondNoise -= pairs;
            else SecondNoise = 0;
            float relativeNoise = ((float)FirstNoise + (float)SecondNoise) / ((float)pairs * 2);
            if (max_noise > 0 and relativeNoise > max_noise) continue;
            
            cout << samples.size()<< "\t" << value.second.size() << "\t";
            cout << it->second.Ref1 << "\t" << value.first.start1 << "\t" << value.first.end1 << "\t";
            cout << it->second.Ref2 << "\t" << value.first.start2 << "\t" << value.first.end2 << "\t";
            cout << pairs << "\t" << it->second.Ori << "\t" << it->second.Type << "\t";
            cout << FirstConcordantOverlap << "-" << SecondConcordantOverlap << "\t" << relativeConcordant << "\t";
            cout << FirstNoise << "-" << SecondNoise << "\t" << relativeNoise << "\t";
            for (unsigned int sample : samples) cout << sample << ",";
            cout << endl;
        }
    }
    return 0;
}