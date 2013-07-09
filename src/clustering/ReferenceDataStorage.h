/* 
 * File:   Reference.h
 * Author: Peter van 't Hof
 *
 * Created on November 30, 2012, 2:23 PM
 */

#ifndef REFERENCE_H
#define	REFERENCE_H

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include "../Config.h"
#include "../Structs.h"

using namespace std;

class ReferenceDataStorage {
public:
    ReferenceDataStorage();
    ReferenceDataStorage(int RefID, string RefName, long int Length, Config* config);
    virtual ~ReferenceDataStorage();
public:
    void ClearingTasks(void);
    
    Config* config;
    
    string Name;
    string LogFile;
    int ID;
    int ForwardID;
    int ReverseID;
    int Length;
    int TotalTasks = 0;
    int CompletedTasks = 0;
    int RunningTasks = 0;
    int RunningReadingTasks = 0;
    bool ReadingDone = false;
    bool ForwardReadingDone = false;
    bool ReverseReadingDone = false;
    bool ReadCollectedDone = false;
    bool SortingDone = false;
    bool PairingDone = false;
    bool ClearingDone = false;
    bool ClusteringDone = false;
    bool TranslocationUse = false;
    
    vector<Singleread>* forwardreads;
    vector<vector<Singleread>*>* forwardreadsBins;
    vector<Singleread>* forwardreadsLowQuality;
    vector<vector<Singleread>*>* forwardreadsLowQualityBins;
    vector<Singleread>* reversereads;
    vector<vector<Singleread>*>* reversereadsBins;
    vector<Singleread>* reversereadsLowQuality;
    vector<vector<Singleread>*>* reversereadsLowQualityBins;
    int long forwardreadsPaired = 0, forwardreadsLowQualityPaired = 0, reversereadsPaired = 0, reversereadsLowQualityPaired = 0; // For clearing memory
    vector<Pair>* HTpairs;
    vector<Pair>* Normal;
    vector<InsertsizePair>* THpairs;
    vector<CoverageFileRead>* Coverage;
    vector<Pair>* OverlappingPairs;
    vector<Pair>* OverhangingPairs;
    vector<Pair>* Deletions;
    vector<Pair>* Insertions;
    vector<Pair>* TTpairs;
    vector<Pair>* HHpairs;
    vector<LowQualityPair>* lowQualitypairs;
    vector<SingleLowQualityPair>* singleLowQualitypairs;
    
    RefStats Stats;
};

#endif	/* REFERENCE_H */
