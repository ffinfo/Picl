/* 
 * File:   ClusteringMain.cpp
 * Author: peter
 * 
 * Created on February 19, 2013, 1:35 PM
 */

#include "ClusteringMain.h"
#include "CalclulateNoise.h"
#include <iostream>
#include <fstream>
#include <stdio.h>

ClusteringMain::ClusteringMain() {
}

ClusteringMain::~ClusteringMain() {
}

int ClusteringMain::usage() {
    cerr << "Usage: <name program> clustering (options) <configfile>" << endl;
    cerr << "Options:" << endl;
    cerr << "   -ref      : Only do this ref, may used more then once" << endl;
    cerr << "   -transonly: Only do translocations" << endl;
    cerr << "   -transskip: Skip translocations" << endl;
    return 1;
}

int ClusteringMain::main(int argc, char *argv[]) {
    if (argc < 1) return this->usage();
    configFile = argv[argc-1];
    for (int t = 1; t < (argc - 1); t++) {
        if (strcmp(argv[t], "-config") == 0) {
            configFile = argv[t+1];
            t++;
        } else if (strcmp(argv[t], "-ref") == 0) {
            RefsSelect.push_back(argv[t+1]);
            t++;
        } else if (strcmp(argv[t], "-transonly") == 0) {
            config->TransSelect = "only";
        } else if (strcmp(argv[t], "-transskip") == 0) {
            config->TransSelect = "skip";
        }
    }
    if (config->TransSelect.compare("only") == 0) config->FilePrefix = "translocation";
    for (string refselect : RefsSelect) {
        if (config->FilePrefix.length() > 0) config->FilePrefix += "-";
        config->FilePrefix += refselect;
    }
    if (config->FilePrefix.length() > 0) config->FilePrefix += ".";
    
    config->LoadConfigFile(configFile);
    config->Log("Config loaded");
    config->Log("Initialization...");
    config->InitializationClustering();
    config->Log("Initialization done");
    Refs = ReadingRefs();
    config->Log("Refs loaded");
    
    int Sleeptime = 20000;
    int WritestatsWait = 100;
    int cycli = 0;
    
    for (ReferenceDataStorage* ref : Refs) {
        taskManager.AddRef(ref);
        config->clusterFile->AddRef(ref->Name);
    }
    config->Log("Tasks Loaded");
    
    config->Log("Starting Run...");
    
    
    
    while (not taskManager.isFinished()) {
        cycli++;
        taskManager.CheckTasks();
        taskManager.StartTasks();
        if (cycli >= WritestatsWait) {
            WriteStats();
            cycli = 0;
        }
        usleep(Sleeptime);
    }
    
    for (ReferenceDataStorage* Ref : Refs) {
        fstream file;
        string coverageFile = config->coverageDir + Ref->Name + ".cov";
        file.open(coverageFile, ios::trunc | ios::binary | ios::out);

        file.write((char*) & config->CoverageBinSize, sizeof(unsigned int));
        //Ref->Coverage->reserve(CoverageReads->size());
        for (CoverageFileRead read : *Ref->Coverage) {
            file.write((char*) & read, sizeof(CoverageFileRead));
        }
    }
    
    config->Log("Calclulate Noise");
    CalclulateNoise *calclulateNoise = new CalclulateNoise(config);
    calclulateNoise->DoBeforeRun();
    calclulateNoise->Run();
    
    //taskManager.CuttoffCalcTask->Run();
    taskManager.WriteTasksNotDone();
    WriteStats();
    config->Log("Run Finished");
    //ReadClusterFile();
        
    return 0;
}

/*
void ClusteringMain::CreateClusterFileHeader() {
    fstream file;
    file.open(config->BinClusterFile, ios::trunc | ios::binary | ios::out);

    unsigned int refcount = Refs.size();
    //cout << refcount << endl;
    file.write((char*) & refcount, sizeof(unsigned int));
    
    unsigned int refIt = 0;
    for (ReferenceDataStorage* Ref : Refs) {
        refIt++;
        config->ClusterFileRefHeader[Ref->Name] = refIt;
        unsigned int length = Ref->Name.size();
        file.write((char*) & length, sizeof(unsigned int));
        file.write(Ref->Name.c_str(), length);
    }
    
    vector<string> Types;
    Types.push_back("undefined");
    Types.push_back("deletion");
    Types.push_back("insertion");
    Types.push_back("inversion");
    Types.push_back("everted");
    Types.push_back("translocation");
    unsigned int typeIt = 0;
    unsigned int typecount = Types.size();
    file.write((char*) & typecount, sizeof(unsigned int));
    for (string type : Types) {
        unsigned int length = type.size();
        file.write((char*) & length, sizeof(unsigned int));
        file.write(type.c_str(), length);
        config->ClusterFileTypeHeader[type] = typeIt;
        typeIt++;
    }
    file.close();
}
*/

/*
void ClusteringMain::ReadClusterFile() {
    fstream file;
    file.open(config->BinClusterFile, ios::binary | ios::in);
    
    cout << "Ref header" << endl;
    map<unsigned int,string> ClusterFileRefHeader;
    unsigned int refcount;
    file.read((char*) & refcount, sizeof(unsigned int));
    //cout << refcount << endl;
    for (unsigned int it = 0; it < refcount; it++) {
        unsigned int length;
        file.read((char*) & length, sizeof(unsigned int));
        //cout << length << endl;
        char name[length];
        file.read((char*) & name, length);
        string nameS = name;
        nameS = nameS.substr(0, length);
        ClusterFileRefHeader[it+1] = nameS;
        cout << (it+1) << " - " << nameS << endl;
    }
    
    cout << "Type header" << endl;
    map<unsigned int,string> ClusterFileTypeHeader;
    unsigned int typecount;
    file.read((char*) & typecount, sizeof(unsigned int));
    //cout << typecount << endl;
    for (unsigned int it = 0; it < typecount; it++) {
        unsigned int length;
        file.read((char*) & length, sizeof(unsigned int));
        //cout << length << endl;
        char type[length];
        file.read((char*) & type, length);
        string typeS = type;
        typeS = typeS.substr(0, length);
        ClusterFileTypeHeader[it] = typeS;
        cout << it << " - " << typeS << endl;
    }
    
    int t = 0;
    while (not file.eof()) {
        t++;
        ClusterFileRead read;
        file.read((char*) & read, sizeof(ClusterFileRead));
        cout << ClusterFileRefHeader[read.ref1] << "-" << read.start1 << "-" << read.end1 << "-" << ClusterFileTypeHeader[read.type] << endl;
        if (t > 100) break;
    }
    
    file.close();
}*/

void ClusteringMain::WriteStats() {
    ofstream file;
    file.open(config->StatsFile, ios::trunc);
    file << "RefName\tF-Reads\tR-Reads\tFL-Reads\tRL-Reads\t";
    file << "THpairs\tHTpairs\tHHpairs\tTTpairs\t";
    file << "iCutoff\tMedian\tdCutoff\t";
    file << "Normal\tInsert\tDel\t";
    file << "C-Del\tC-Insert\tC-HT\tC-HH\tC-TT\t";
    file << "F-Upair\tR-Upair\tFL-Upair\tRL-Upair";
    file << endl;
    
    RefStats Total;
    int F = 0, R = 0, FL = 0, RL = 0;
    
    for (ReferenceDataStorage* Ref : Refs) {
        file << Ref->Name << "\t" << Ref->Stats.Read.Forward << "\t" << Ref->Stats.Read.Reverse << "\t";
        file << Ref->Stats.Read.ForwardLowQuality << "\t" << Ref->Stats.Read.ReverseLowQuality << "\t";
        file << Ref->Stats.Pair.THpairs << "\t" << Ref->Stats.Pair.HTpairs << "\t";
        file << Ref->Stats.Pair.HHpairs << "\t" << Ref->Stats.Pair.TTpairs << "\t";
        file << Ref->Stats.Insertsize.InsertionCutoff << "\t" << Ref->Stats.Insertsize.Median << "\t";
        file << Ref->Stats.Insertsize.DeletionCutoff << "\t";
        file << Ref->Stats.Insertsize.Normal << "\t" << Ref->Stats.Insertsize.Insertions << "\t" << Ref->Stats.Insertsize.Deletions << "\t";
        file << Ref->Stats.Cluster.Deletions << "\t" << Ref->Stats.Cluster.Insertions << "\t";
        file << Ref->Stats.Cluster.HTclusters << "\t" << Ref->Stats.Cluster.HHclusters << "\t" << Ref->Stats.Cluster.TTclusters << "\t";
        file << Ref->forwardreads->size() << "\t" << Ref->reversereads->size() << "\t";
        file << Ref->forwardreadsLowQuality->size() << "\t" << Ref->reversereadsLowQuality->size();
        file << endl;
        
        Total.Read.Forward += Ref->Stats.Read.Forward;
        Total.Read.ForwardLowQuality += Ref->Stats.Read.ForwardLowQuality;
        Total.Read.Reverse += Ref->Stats.Read.Reverse;
        Total.Read.ReverseLowQuality += Ref->Stats.Read.ReverseLowQuality;
        
        Total.Pair.HHpairs += Ref->Stats.Pair.HHpairs;
        Total.Pair.HTpairs += Ref->Stats.Pair.HTpairs;
        Total.Pair.LowQuality += Ref->Stats.Pair.LowQuality;
        Total.Pair.SingleLowQuality += Ref->Stats.Pair.SingleLowQuality;
        Total.Pair.THpairs += Ref->Stats.Pair.THpairs;
        Total.Pair.TTpairs += Ref->Stats.Pair.TTpairs;
        
        Total.Cluster.Deletions += Ref->Stats.Cluster.Deletions;
        Total.Cluster.HHclusters += Ref->Stats.Cluster.HHclusters;
        Total.Cluster.HTclusters += Ref->Stats.Cluster.HTclusters;
        Total.Cluster.Insertions += Ref->Stats.Cluster.Insertions;
        Total.Cluster.TTclusters += Ref->Stats.Cluster.TTclusters;
        Total.Cluster.Translocations += Ref->Stats.Cluster.Translocations;
        
        Total.Insertsize.Deletions += Ref->Stats.Insertsize.Deletions;
        Total.Insertsize.Insertions += Ref->Stats.Insertsize.Insertions;
        Total.Insertsize.Normal += Ref->Stats.Insertsize.Normal;
        Total.Insertsize.DeletionCutoff += Ref->Stats.Insertsize.DeletionCutoff;
        Total.Insertsize.InsertionCutoff += Ref->Stats.Insertsize.InsertionCutoff;
        Total.Insertsize.Median += Ref->Stats.Insertsize.Median;
        
        F += Ref->forwardreads->size();
        R += Ref->reversereads->size();
        FL += Ref->forwardreadsLowQuality->size();
        RL += Ref->reversereadsLowQuality->size();
    }
    
    Total.Insertsize.DeletionCutoff = round((double)Total.Insertsize.DeletionCutoff / (double)Refs.size());
    Total.Insertsize.InsertionCutoff = round((double)Total.Insertsize.InsertionCutoff / (double)Refs.size());
    Total.Insertsize.Median = round((double)Total.Insertsize.Median / (double)Refs.size());
    
    file << "Total\t" << Total.Read.Forward << "\t" << Total.Read.Reverse << "\t";
    file << Total.Read.ForwardLowQuality << "\t" << Total.Read.ReverseLowQuality << "\t";
    file << Total.Pair.THpairs << "\t" << Total.Pair.HTpairs << "\t";
    file << Total.Pair.HHpairs << "\t" << Total.Pair.TTpairs << "\t";
    file << Total.Insertsize.InsertionCutoff << "\t" << Total.Insertsize.Median << "\t";
    file << Total.Insertsize.DeletionCutoff << "\t";
    file << Total.Insertsize.Normal << "\t" << Total.Insertsize.Insertions << "\t" << Total.Insertsize.Deletions << "\t";
    file << Total.Cluster.Deletions << "\t" << Total.Cluster.Insertions << "\t";
    file << Total.Cluster.HTclusters << "\t" << Total.Cluster.HHclusters << "\t" << Total.Cluster.TTclusters << "\t";
    file << F << "\t" << R << "\t" << FL << "\t" << RL;
    file << endl;
    
    file.close();
}

vector<ReferenceDataStorage*> ClusteringMain::ReadingRefs () {
    vector<ReferenceDataStorage*> Refs;
    
    if (config->UsePairedBam) {
        BamTools::BamReader bamReader;
        bamReader.Open(config->PairedBam);
        bamReader.OpenIndex(config->PairedIndex);
        for (BamTools::RefData ref : bamReader.GetReferenceData()) {
            if (RefsSelect.size() > 0) {
                bool found = false;
                for (string refselect : RefsSelect) {
                    if (refselect.compare(ref.RefName) == 0) found = true;
                }
                if (not found) continue;
            }
            bool skip = false;
            for (string prefix : config->RefPrefixSkip) {
                if (ref.RefName.find(prefix) == 0) skip = true;
            }
            if (skip) continue;
            ReferenceDataStorage* temp = new ReferenceDataStorage(Refs.size(), ref.RefName, ref.RefLength, config);
            temp->Length = ref.RefLength;
            mkdir((config->Workspace + "/Refs/" + ref.RefName).c_str(), 0755);
            Refs.push_back(temp);
        }
    } else {
        BamTools::BamReader ForwardBamReader;
        ForwardBamReader.Open(config->ForwardBam);
        ForwardBamReader.OpenIndex(config->ForwardIndex);

        BamTools::BamReader  ReverseBamReader;
        ReverseBamReader.Open(config->ReverseBam);
        ReverseBamReader.OpenIndex(config->ReverseIndex);

        for (BamTools::RefData ref : ForwardBamReader.GetReferenceData()) {
            if (RefsSelect.size() > 0) {
                bool found = false;
                for (string refselect : RefsSelect) {
                    if (refselect.compare(ref.RefName) == 0) found = true;
                }
                if (not found) continue;
            }
            bool skip = false;
            for (string prefix : config->RefPrefixSkip) {
                if (ref.RefName.find(prefix) == 0) skip = true;
            }
            if (skip) continue;
            ReferenceDataStorage* temp = new ReferenceDataStorage(Refs.size(), ref.RefName, ref.RefLength, config);
            temp->Length = ref.RefLength;
            temp->ForwardID = ForwardBamReader.GetReferenceID(ref.RefName);
            temp->ReverseID = ReverseBamReader.GetReferenceID(ref.RefName);
            mkdir((config->Workspace + "/Refs/" + ref.RefName).c_str(), 0755);
            Refs.push_back(temp);
        }

        for (BamTools::RefData ref : ReverseBamReader.GetReferenceData()) {
            if (RefsSelect.size() > 0) {
                bool found = false;
                for (string refselect : RefsSelect) {
                    if (refselect.compare(ref.RefName) == 0) found = true;
                }
                if (not found) continue;
            }
            bool skip = false;
            for (string prefix : config->RefPrefixSkip) {
                if (ref.RefName.find(prefix) == 0) skip = true;
            }
            if (skip) continue;
            bool found = false;
            for (ReferenceDataStorage* ref2 : Refs) {
                if (ref.RefName == ref2->Name) {
                    ref2->ReverseID = ReverseBamReader.GetReferenceID(ref.RefName);
                    if (ref2->Length < ref.RefLength) {
                        ref2->Length = ref.RefLength;
                    }
                    found = true;
                }
            }
            if (!found) {
                ReferenceDataStorage* temp = new ReferenceDataStorage(Refs.size(), ref.RefName, ref.RefLength, config);
                temp->Length = ref.RefLength;
                temp->ReverseID = ReverseBamReader.GetReferenceID(ref.RefName);
                Refs.push_back(temp);
            }
        }
    }
    struct SortRefsOnLength {
        bool operator()(const ReferenceDataStorage* r1, const ReferenceDataStorage* r2)
        { 
            if (r1->Length > r2->Length) {
                return true;
            } else {
                return false;
            } 
        }
    };
    sort(Refs.begin(), Refs.end(), SortRefsOnLength());
    
    return Refs;
}