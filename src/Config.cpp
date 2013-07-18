/* 
 * File:   Config.cpp
 * Author: peter
 * 
 * Created on December 21, 2012, 10:11 AM
 */

#include <cstdlib>
#include <iostream>
#include <fstream>
#include "Config.h"
#include <sys/stat.h>
#include <sstream>
#include <regex>
#include <iterator>
#include <string>
#include "api/BamReader.h"
#include "api/BamAlignment.h"
#include "clustering/ReadNameConverter.h"

using namespace std;

Config::Config() {
}

Config::~Config() {
}

void Config::LoadConfigFile(string Configfile) {
    //Reading config file
    ifstream file;
    file.open(Configfile.c_str(), ios::in);
    if (!file.is_open()) {
        cerr << "[Warning] Config file could not be found" << endl;
        exit(1);
    }
    
    while (!file.eof()) {
        string line;
        getline(file,line);
        if (line.substr(0,1) == "#" || line.size() == 0) continue;
        int split = line.find('=');
        string name = line.substr(0, split);
        name = name.substr(name.find_first_not_of(' '),name.find_last_of(' '));
        string value = line.substr(split+1, line.size()-split);
        value = value.substr(value.find_first_not_of(' '),value.find_last_not_of(' '));
        if (name == "Workspace") { Workspace = value + "/"; }
        else if (name == "LibaryName") { LibaryName = value; }
        else if (name == "ForwardBam") { ForwardBam = value; }
        else if (name == "ForwardIndex") { ForwardIndex = value; }
        else if (name == "ReverseBam") { ReverseBam = value; }
        else if (name == "ReverseIndex") { ReverseIndex = value; }
        else if (name == "PairedBam") { PairedBam = value; }
        else if (name == "PairedIndex") { PairedIndex = value; }
        else if (name == "NameType") { 
            if (value == "Solid5") NameType = ReadNameConverter::Solid5;
            if (value == "Solid4") NameType = ReadNameConverter::Solid4;
            if (value == "Solid4-2") NameType = ReadNameConverter::Solid4_2;
            if (value == "Solid5-2") NameType = ReadNameConverter::Solid5_2;
            if (value == "Ilumnia") NameType = ReadNameConverter::Ilumnia;
            if (value == "Simulated") NameType = ReadNameConverter::Simulated;
        }
        else if (name == "LibType") {
            LibType = value;
            if (value == "MP") {
                ForwardOriInverted = true;
                ReverseOriInverted = true;
            } else if (value == "MP-inv") {
                ForwardOriInverted = false;
                ReverseOriInverted = false;
            } else if (value == "PE") {
                ForwardOriInverted = false;
                ReverseOriInverted = true;
            } else if (value == "PE-inv") {
                ForwardOriInverted = true;
                ReverseOriInverted = false;
            } else {
                cerr << "[Warning] Incorrect value at LibType" << endl;
                exit(1);
            }
        }
        else if (name == "RefPrefixSkip") {
            while (value.find(" ") < value.size()) {
                RefPrefixSkip.push_back(value.substr(0, value.find(" ")));
                value = value.substr(value.find(" ") + 1, value.size());
            }
            RefPrefixSkip.push_back(value);
        }
        else if (name == "ForwardOriInverted") {
            if (value == "true") ForwardOriInverted = true;
            else if (value == "false") ForwardOriInverted = false;
            else {
                cerr << "[Warning] Incorrect value at ForwardOriInverted" << endl;
                exit(1);
            }
        } else if (name == "ReverseOriInverted") {
            if (value == "true") ReverseOriInverted = true;
            else if (value == "false") ReverseOriInverted = false;
            else {
                cerr << "[Warning] Incorrect value at ReverseOriInverted" << endl;
                exit(1);
            }
        } else if (name == "NameTrim") { 
            while (value.find(" ") < value.size()) {
                NameTrim.push_back({value.substr(0, value.find(" ")), ""});
                value = value.substr(value.find(" ") + 1, value.size());
            }
            NameTrim.push_back({value, ""});
        }
        else if (name == "CoverageBinSize") { CoverageBinSize = atoi(value.c_str()); }
        else if (name == "maxX0") { maxX0 = atoi(value.c_str()); }
        else if (name == "maxX1") { maxX1 = atoi(value.c_str()); }
        else if (name == "maxXO") { maxXO = atoi(value.c_str()); }
        else if (name == "maxXM") { maxXM = atoi(value.c_str()); }
        else if (name == "qualityTreshold") { qualityTreshold = atoi(value.c_str()); }
        else if (name == "clusterMethod") { clusterMethod = atoi(value.c_str()); }
        else if (name == "secondaryClustering") { if (value == "false")  secondaryClustering = false; }
        else if (name == "minClearingPairs") { minClearingPairs = atoi(value.c_str()); }
        else if (name == "minClones") { minClones = atoi(value.c_str()); }
        else if (name == "InsertionCutoffPvalue") { InsertionCutoffPvalue = atof(value.c_str()); }
        else if (name == "DeletionCutoffPvalue") { DeletionCutoffPvalue = atof(value.c_str()); }
        else if (name == "MaxDistance") { MaxDistance = atoi(value.c_str()); }
        else if (name == "maxThreads") { maxThreads = atoi(value.c_str()); }
        else if (name == "maxReadThreads") { maxReadThreads = atoi(value.c_str()); }
        else if (name == "maxUsedRefs") { maxUsedRefs = atoi(value.c_str()); }
        else if (name == "readBinSize") { readBinSize = atoi(value.c_str()); }
        else if (name == "InsertionCutoff") { InsertionCutoff = atoi(value.c_str()); }
        else if (name == "DeletionCutoff") { DeletionCutoff = atoi(value.c_str()); }
        else if (name == "ClusterWindow") { ClusterWindow = atoi(value.c_str()); }
        else if (name == "OutputOnTerminal") { if (value == "true")  OutputOnTerminal = true; }
        else { cout << "[Warning] Config name '" << name << "' in configfile: not used" << endl; }
    }
    file.close();
    FilePrefix = LibaryName + "." + FilePrefix;
    logFile = Workspace + "/" + FilePrefix + "log";
}

void Config::writeConfigFile(string filename) {
    ofstream file;
    file.open(filename.c_str(), ios::trunc);
    file << getConfigAsText();
    file.close();
}

string Config::getConfigAsText() {
    ostringstream Buffer;
    Buffer << "#Config file for clustering" << endl;
    Buffer << "Workspace = " << Workspace << endl;
    Buffer << "LibaryName = " << LibaryName << endl;
    if (this->UsePairedBam) {
        Buffer << "PairedBam = " << PairedBam << endl;
        Buffer << "PairedIndex = " << PairedIndex << endl;
    } else {
        Buffer << "ForwardBam = " << ForwardBam << endl;
        Buffer << "ForwardIndex = " << ForwardIndex << endl;
        Buffer << "ReverseBam = " << ReverseBam << endl;
        Buffer << "ReverseIndex = " << ReverseIndex << endl;
    }
    if (NameType == ReadNameConverter::Solid5) Buffer << "NameType = Solid5" << endl;
    if (NameType == ReadNameConverter::Solid4) Buffer << "NameType = Solid4" << endl;
    if (NameType == ReadNameConverter::Solid4_2) Buffer << "NameType = Solid4-2" << endl;
    if (NameType == ReadNameConverter::Solid5_2) Buffer << "NameType = Solid5-2" << endl;
    if (NameType == ReadNameConverter::Ilumnia) Buffer << "NameType = Ilumnia" << endl;
    if (not LibType.empty()) Buffer << "LibType = " << LibType << endl;
    Buffer << "ForwardOriInverted = ";
    if (ForwardOriInverted) Buffer << "true" << endl; else Buffer << "false" << endl;
    Buffer << "ReverseOriInverted = ";
    if (ReverseOriInverted) Buffer << "true" << endl; else Buffer << "false" << endl;
    for (ReadNameConverter::ReplaceNameArg trim : NameTrim) Buffer << "NameTrim = " << trim.Replace << endl;
    
    Buffer << endl << "#Quality filter" << endl;
    Buffer << "qualityTreshold = " << qualityTreshold << endl;
    Buffer << "#BWA only" << endl;
    Buffer << "maxX0 = " << maxX0 << endl;
    Buffer << "maxX1 = " << maxX1 << endl;
    Buffer << "maxXO = " << maxXO << endl;
    Buffer << "maxXM = " << maxXM << endl;
    
    Buffer << endl << "#Insertsize" << endl;
    Buffer << "CoverageBinSize = " << CoverageBinSize << endl;
    Buffer << "InsertionCutoffPvalue = " << InsertionCutoffPvalue << endl;
    Buffer << "DeletionCutoffPvalue = " << DeletionCutoffPvalue << endl;
    Buffer << "MaxDistance = " << MaxDistance << endl;
    if (InsertionCutoff > 0) Buffer << "InsertionCutoff = " << InsertionCutoff << endl;
    if (DeletionCutoff > 0) Buffer << "DeletionCutoff = " << DeletionCutoff << endl;
    
    Buffer << endl << "#Cluster settings" << endl;
    Buffer << "minClones = " << minClones << endl;
    Buffer << "clusterMethod = " << clusterMethod << endl;
    if (ClusterWindow > 0) Buffer << "ClusterWindow = " << ClusterWindow << endl;
    Buffer << "secondaryClustering = ";
    if (secondaryClustering) Buffer << "true" << endl; else Buffer << "false" << endl;
    
    Buffer << endl << "#System settings" << endl;
    Buffer << "minClearingPairs = " << minClearingPairs << endl;
    Buffer << "maxThreads = " << maxThreads << endl;
    Buffer << "maxReadThreads = " << maxReadThreads << endl;
    Buffer << "readBinSize = " << readBinSize << endl;
    Buffer << "OutputOnTerminal = ";
    if (OutputOnTerminal) Buffer << "true" << endl; else Buffer << "false" << endl;
    
    return Buffer.str();
}

void Config::InitializationClustering() {
    struct stat st;
    if(stat(Workspace.c_str(),&st) == 0 and st.st_mode and S_IFDIR != 0) Log("[Warning] Workspace directory already present");
    else if (mkdir(Workspace.c_str(), 0755) != 0) {
        Log("[Error] Could not create workspace directory: " + Workspace);
        exit(1);
    }
    RunningTasksFile = Workspace + "/" + FilePrefix + "running.tasks";
    StatsFile = Workspace + "/" + FilePrefix + "stats";
    BinClusterFile = Workspace + "/" + FilePrefix + "bpc";
    clusterFile = new ClusterFile(BinClusterFile);
    clusterDir = Workspace + "/clusters/";
    if(stat(clusterDir.c_str(),&st) == 0 and st.st_mode and S_IFDIR != 0) Log("[Warning] Cluster directory already present");
    else if (mkdir(clusterDir.c_str(), 0755) != 0) {
        Log("[Error] Could not create cluster directory: " + clusterDir);
        exit(1);
    }
    insertsizeDir = Workspace + "/insertsize/";
    if(stat(insertsizeDir.c_str(),&st) == 0 and st.st_mode and S_IFDIR != 0) Log("[Warning] Insertsize directory already present");
    else if (mkdir(insertsizeDir.c_str(), 0755) != 0) {
        Log("[Error] Could not create insertsize directory: " + insertsizeDir);
        exit(1);
    }
    coverageDir = Workspace + "/coverage/";
    if(stat(coverageDir.c_str(),&st) == 0 and st.st_mode and S_IFDIR != 0) Log("[Warning] Coverage directory already present");
    else if (mkdir(coverageDir.c_str(), 0755) != 0) {
        Log("[Error] Could not create coverage directory: " + coverageDir);
        exit(1);
    }
    
    if (!ForwardBam.empty() && !ReverseBam.empty() && PairedBam.empty()) {
        UsePairedBam = false;
    } else if (ForwardBam.empty() && ReverseBam.empty() && !PairedBam.empty()) {
        UsePairedBam = true;
    } else {
        Log("[Error] No correct bam file(s)");
        exit(1);
    }
    
    BamTools::BamAlignment alignment;
    BamTools::BamReader BamReader;
    
    if (UsePairedBam) {
        BamReader.Open(PairedBam);
        if (not BamReader.IsOpen()) {
            Log("[Error] Could not open paired bam");
            exit(1);
        }
        if (PairedIndex.empty()) {
            if (not BamReader.LocateIndex(BamTools::BamIndex::STANDARD)) {
                PairedIndex = PairedBam.substr(0,PairedBam.find_last_of(".bam")-3) + ".bai";
                BamReader.OpenIndex(PairedIndex);
            }
            if (not BamReader.HasIndex()) {
                Log("[Error] No index for bamfile");
                exit(1);
            }
        }
        BamTools::SamHeader header = BamReader.GetHeader();
        for (BamTools::SamReadGroupIterator it = header.ReadGroups.Begin(); it != header.ReadGroups.End(); it++) {
            BamTools::SamReadGroup* readgroup = &*it;
            readNameConverter.TrimName(readgroup->ID);
            readNameConverter.AddReadGroup(readgroup->ID);
        }
        long int count = 0;
        while (BamReader.GetNextAlignment(alignment)) {
            string RG;
            if (alignment.GetTag("RG", RG)) {
                if (not NameTrim.empty()) readNameConverter.TrimName(RG);
                if (readNameConverter.AddReadGroup(RG)) {
                    Log("[Warning] Readgroup '" + RG + "' found in reads but not in header");
                    count = 0;
                }
            }
            count++;
            if (count > 10000) break;
        }
        BamReader.Close();
    } else {
        BamReader.Open(ForwardBam);
        if (not BamReader.IsOpen()) {
            Log("[Error] Could not open first/forward bam");
            exit(1);
        }
        if (ForwardIndex.empty()) {
            if (not BamReader.LocateIndex(BamTools::BamIndex::STANDARD)) {
                ForwardIndex = ForwardBam.substr(0,ForwardBam.find_last_of(".bam")-3) + ".bai";
                BamReader.OpenIndex(ForwardIndex);
            }
            if (not BamReader.HasIndex()) {
                Log("[Error] No index for forward bamfile");
                exit(1);
            }
        }
        BamTools::SamHeader forwardheader = BamReader.GetHeader();
        for (BamTools::SamReadGroupIterator it = forwardheader.ReadGroups.Begin(); it != forwardheader.ReadGroups.End(); it++) {
            BamTools::SamReadGroup* readgroup = &*it;
            readNameConverter.TrimName(readgroup->ID);
            readNameConverter.AddReadGroup(readgroup->ID);
        }
        long int count = 0;
        while (BamReader.GetNextAlignment(alignment)) {
            string RG;
            if (alignment.GetTag("RG", RG)) {
                if (!NameTrim.empty()) readNameConverter.TrimName(RG);
                if (readNameConverter.AddReadGroup(RG)) {
                    Log("[Warning] Readgroup '" + RG + "' found in forward reads but not in header");
                    count = 0;
                }
            }
            count++;
            if (count > 10000) break;
        }
        BamReader.Close();
        BamReader.Open(ReverseBam);
        if (not BamReader.IsOpen()) {
            Log("[Error] Could not open second/reverse bam");
            exit(1);
        }
        if (ReverseIndex.empty()) {
            if (not BamReader.LocateIndex(BamTools::BamIndex::STANDARD)) {
                ReverseIndex = ReverseBam.substr(0,ReverseBam.find_last_of(".bam")-3) + ".bai";
                BamReader.OpenIndex(ReverseIndex);
            }
            if (not BamReader.HasIndex()) {
                Log("[Error] No index for reverse bamfile");
                exit(1);
            }
        }
        BamTools::SamHeader reverseheader = BamReader.GetHeader();
        for (BamTools::SamReadGroupIterator it = reverseheader.ReadGroups.Begin(); it != reverseheader.ReadGroups.End(); it++) {
            BamTools::SamReadGroup* readgroup = &*it;
            readNameConverter.TrimName(readgroup->ID);
            if (readNameConverter.AddReadGroup(readgroup->ID)) {
                Log("[Warning] Readgroup '" + readgroup->ID + "' found in reverse but not in forward");
            }
        }
        count = 0;
        while (BamReader.GetNextAlignment(alignment)) {
            string RG;
            if (alignment.GetTag("RG", RG)) {
                if (!NameTrim.empty()) readNameConverter.TrimName(RG);
                if (readNameConverter.AddReadGroup(RG)) {
                    Log("[Warning] Readgroup '" + RG + "' found in reverse reads but not in header");
                    count = 0;
                } 
            }
            count++;
            if (count > 10000) break;
        }
        BamReader.Close();
    }
    
    for(map<string, int>::iterator it = readNameConverter.ReadGroups.begin(); it!=readNameConverter.ReadGroups.end(); ++it) {
        ostringstream logBuffer;
        logBuffer << "Readgroup found: " << it->second << " - " << it->first;
        Log(logBuffer.str());
    }
    
    writeConfigFile(Workspace + FilePrefix + "config");
}

void Config::Log(string Text) {
    ofstream file;
    file.open(logFile, ios::app);
    if (OutputOnTerminal) cout << getTimeStamp() << Text << endl;
    file << getTimeStamp() << Text << endl;
    file.close();
}

string Config::getTimeStamp() {
    time_t now = time(0);
    string TimeStamp = ctime(&now);
    TimeStamp.resize(TimeStamp.length()-1);
    TimeStamp = "[" + TimeStamp + "] ";
    return TimeStamp;
}

void Config::AddInsertsizeStats(string RefName, InsertsizeStats &is) {
    insertsizeStats[RefName] = is;
    return;
}

InsertsizeStats Config::GetInsertsizeStats(string RefName) {
    return insertsizeStats[RefName];
}