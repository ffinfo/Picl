/* 
 * File:   ClusteringMain.h
 * Author: peter
 *
 * Created on February 19, 2013, 1:35 PM
 */

#ifndef CLUSTERINGMAIN_H
#define	CLUSTERINGMAIN_H

#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <string>
#include <thread>
#include <api/BamReader.h>
#include <api/BamAlignment.h>
#include <sys/stat.h>
#include <sstream>
#include <vector>
#include <chrono>
#include <time.h>
#include <unistd.h>
#include <array>
#include <list>
#include "ReferenceDataStorage.h"
#include "TranslocationDataStorage.h"
#include "ReadSingleReads.h"
#include "../Config.h"
#include "ClusteringTaskManager.h"

using namespace std;

class ClusteringMain {
public:
    ClusteringMain();
    virtual ~ClusteringMain();
    
    int main(int argc, char *argv[]);
    static int usage();
private:
    string configFile;
    vector<string> RefsSelect;
    vector<ReferenceDataStorage*> ReadingRefs (void);
    //void CreateClusterFileHeader();
    //void ReadClusterFile();
    int TransPairing (ReferenceDataStorage* ref1, ReferenceDataStorage* ref2);
    int TransPairingLowQuality (ReferenceDataStorage* ref1, ReferenceDataStorage* ref2);
    void WriteStats(void);
    
    Config* config = new Config();
    vector<ReferenceDataStorage*> Refs;
    vector<TranslocationDataStorage*>* Translocations = new vector<TranslocationDataStorage*>;
    ClusteringTaskManager taskManager = ClusteringTaskManager(Translocations, config);
};

#endif	/* CLUSTERINGMAIN_H */

