/* 
 * File:   Config.h
 * Author: peter
 *
 * Created on December 21, 2012, 10:11 AM
 */

#ifndef CONFIG_H
#define	CONFIG_H

#include <cstdlib>
#include <string>
#include <map>
#include "Structs.h"
#include "clustering/ReadNameConverter.h"
#include "filter/ClusterFile.h"

using namespace std;

class Config {
public:
    Config();
    virtual ~Config();
public:
    void LoadConfigFile(string Configfile);
    void InitializationClustering();
    void Log(string Text);
    string getTimeStamp();
    string getConfigAsText();
    void writeConfigFile(string filename);
    long int ConvertNameToInt(string Name);
    void AddInsertsizeStats(string RefName, InsertsizeStats &is);
    InsertsizeStats GetInsertsizeStats(string RefName);
    
    unsigned int NameType = 0;
    unsigned int maxThreads = 1, maxUsedRefs = 1, maxReadThreads = 1, readBinSize = 20000000;
    bool ForwardOriInverted = false, ReverseOriInverted = true;
    bool UsePairedBam;
    string Workspace, logFile, clusterDir, insertsizeDir, StatsFile, RunningTasksFile, TempDir, BinClusterFile, coverageDir, LibaryName, LibType;
    string ForwardBam, ReverseBam, ForwardIndex, ReverseIndex, PairedBam, PairedIndex; //bam files
    unsigned int maxX0 = 1, maxX1 = 0, maxXO = 0, maxXM = 10; // BWA filter
    unsigned int qualityTreshold = 30; // quality filter
    unsigned int clusterMethod = 0;
    bool secondaryClustering = false;
    unsigned int minClearingPairs = 50000;
    unsigned int minClones = 2;
    unsigned int InsertionCutoff = 0, DeletionCutoff = 0, ClusterWindow = 0;
    double InsertionCutoffPvalue = 0.005;
    double DeletionCutoffPvalue = 0.995;
    unsigned int MaxDistance = 10000;
    unsigned int CoverageBinSize = 50;
    bool OutputOnTerminal = false;
    bool SkipDuplicates = true;
    vector<string> RefPrefixSkip;
    vector<ReadNameConverter::ReplaceNameArg> NameTrim;
    ReadNameConverter readNameConverter = ReadNameConverter(&NameType, &NameTrim);
    
    ClusterFile *clusterFile;
    
    map<string,unsigned int> ClusterFileRefHeader;
    map<string,unsigned int> ClusterFileTypeHeader;
    
    string TransSelect = "all";
    string FilePrefix = "";
private:
    map<string,InsertsizeStats> insertsizeStats;
};

#endif	/* CONFIG_H */

