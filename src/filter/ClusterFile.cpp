/* 
 * File:   ClusterFile.cpp
 * Author: peter
 * 
 * Created on March 19, 2013, 4:30 PM
 */

#include "ClusterFile.h"
#include "../clustering/ReadNameConverter.h"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <algorithm>
#include <sstream>

ClusterFile::ClusterFile() {
    FileStream = new fstream;
    AddOri("TH");
    AddOri("HT");
    AddOri("HH");
    AddOri("TT");
    AddType("undefined");
    AddType("deletion");
    AddType("insertion");
    AddType("inversion");
    AddType("everted");
    AddType("translocation");
}

ClusterFile::~ClusterFile() {
}

ClusterFile::ClusterFile(string filename) : ClusterFile() {
    this->filename = filename;
}

bool ClusterFile::ReadFile(string filename) {
    return ReadFile(filename, 0, 0, 0, 0, 0);
}

bool ClusterFile::ReadFile(string filename, int Cuttoff, float prob_cutoff, float max_noise, float max_concordant, float min_concordant) {
    vector<string> temp;
    return ReadFile(filename, temp, Cuttoff, prob_cutoff, max_noise, max_concordant, min_concordant);
}

bool ClusterFile::ReadFile(string filename, vector<string> &trim) {
    return ReadFile(filename, trim, 0, 0, 0, 0, 0);
}

bool ClusterFile::ReadFile(string filename, vector<string> &trim, int Cuttoff, float prob_cutoff, float max_noise, float max_concordant, float min_concordant) {
    this->filename = filename;
    this->max_noise = max_noise;
    this->max_concordant = max_concordant;
    this->min_concordant = min_concordant;
    header.Type_to_ID.clear();
    header.ID_to_Type.clear();
    header.Ref_to_ID.clear();
    header.ID_to_Ref.clear();
    fstream file;
    file.open(filename, ios::binary | ios::in);
    
    //Ref header
    ReadNameConverter readNameConverter;
    for (string trimname : trim) readNameConverter.NameReplace->push_back({trimname, ""});
    unsigned int refcount;
    file.read((char*) & refcount, sizeof(unsigned int));
    for (unsigned int it = 0; it < refcount; it++) {
        unsigned int length;
        file.read((char*) & length, sizeof(unsigned int));
        char name[length];
        file.read((char*) & name, length);
        string nameS = name;
        nameS = nameS.substr(0, length);
        readNameConverter.TrimName(nameS);
        AddRef(nameS);
        //ClusterFileRefHeader[it+1] = nameS;
        //cout << (it+1) << " - " << nameS << endl;
    }
    
    //Type header
    unsigned int typecount;
    file.read((char*) & typecount, sizeof(unsigned int));
    for (unsigned int it = 0; it < typecount; it++) {
        unsigned int length;
        file.read((char*) & length, sizeof(unsigned int));
        char type[length];
        file.read((char*) & type, length);
        string typeS = type;
        typeS = typeS.substr(0, length);
        AddType(typeS);
        //ClusterFileTypeHeader[it] = typeS;
        //cout << it << " - " << typeS << endl;
    }
    
    int t = 0;
    while (not file.eof()) {
        t++;
        ClusterFileRead read;
        file.read((char*) & read, sizeof(ClusterFileRead));
        if (read.pairs - read.clone < Cuttoff) continue;
        if (read.probability < prob_cutoff) continue;
        if (read.end2 == 0) continue;
        /*string Ref1 = ClusterFileRefHeader[read.ref1];
        string Ref2 = ClusterFileRefHeader[read.ref2];
        string Type = ClusterFileTypeHeader[read.type];
        string Ori;
        switch (read.ori) {
            case OriTH:
                Ori = "TH";
                break;
            case OriHT:
                Ori = "HT";
                break;
            case OriHH:
                Ori = "HH";
                break;
            case OriTT:
                Ori = "TT";
                break;
        }
        string key = Ref1 + ";" + Ref2 + ";" + Type + ";" + Ori;*/
        string key = GenerateKey(header.ID_to_Ref[read.ref1], header.ID_to_Ref[read.ref2], header.ID_to_Type[read.type], header.ID_to_Ori[read.ori]);
        if (Data.find(key) == Data.end()) {
            Data[key].Ref1 = header.ID_to_Ref[read.ref1];
            Data[key].Ref2 = header.ID_to_Ref[read.ref2];
            Data[key].Type = header.ID_to_Type[read.type];
            Data[key].Ori = header.ID_to_Ori[read.ori];
        }
        
        DataValue value;
        value.start1 = read.start1;
        value.end1 = read.end1;
        value.start2 = read.start2;
        value.end2 = read.end2;
        value.pairs = read.pairs;
        value.clone = read.clone;
        value.ov = read.overlap;
        value.prob_size = read.prob_size;
        value.probability = read.probability;
        value.FirstConcordantOverlap = read.concerdant1;
        value.SecondConcordantOverlap = read.concerdant2;
        value.FirstNoise = read.noise1;
        value.SecondNoise = read.noise2;
        Data[key].Values.push_back(value);
    }
    file.close();
    
    for(map<string,DataKey>::iterator it = Data.begin(); it != Data.end(); it++) {
        sort(it->second.Values.begin(), it->second.Values.end(), SortDataValueOnStart1());
    }
    return true;
}

bool ClusterFile::OpenFile() {
    FileStream->open(filename, ios::app | ios::binary | ios::out);
    if (FileStream->is_open()) return true;
    else {
        cerr << "Could not open cluster file" << endl;
        return false;
    }
}

bool ClusterFile::CloseFile() {
    if (FileStream->is_open()) {
        FileStream->close();
        if (FileStream->is_open()) return false;
        else return true;
    } else return false;
}

bool ClusterFile::WriteHeader() {
    FileStream->open(filename, ios::trunc | ios::binary | ios::out);
    if (not FileStream->is_open()) {
        cerr << "Could not open cluster file" << endl;
        return false;
    }
    
    //Ref header
    unsigned int refcount = header.ID_to_Ref.size();
    FileStream->write((char*) & refcount, sizeof(unsigned int));
    for(map<unsigned int,string>::iterator it = header.ID_to_Ref.begin(); it != header.ID_to_Ref.end(); it++) {
        unsigned int length = it->second.size();
        FileStream->write((char*) & length, sizeof(unsigned int));
        FileStream->write(it->second.c_str(), length);
    }
    
    //Type header
    unsigned int typecount = header.ID_to_Type.size();
    FileStream->write((char*) & typecount, sizeof(unsigned int));
    for(map<unsigned int,string>::iterator it = header.ID_to_Type.begin(); it != header.ID_to_Type.end(); it++) {
        unsigned int length = it->second.size();
        FileStream->write((char*) & length, sizeof(unsigned int));
        FileStream->write(it->second.c_str(), length);
    }
    FileStream->close();
    
    header.infile = true;
    return true;
}

bool ClusterFile::ReWriteFile() {
    if (not this->WriteHeader()) return false;
    // Writing clusters
    OpenFile();
    for(map<string,DataKey>::iterator it = Data.begin(); it != Data.end(); it++) {
        for (DataValue &value : it->second.Values) {
            WriteCluster(it->second, value);
        }
    }
    
    CloseFile();
    return true;
}

bool ClusterFile::WriteCluster(DataKey &key, DataValue &value) {
    if (not FileStream->is_open()) {
        if (not OpenFile()) return false;
        if (not FileStream->is_open()) return false;
    }
    ClusterFileRead read;
    read.ref1 = header.Ref_to_ID[key.Ref1];
    read.start1 = value.start1;
    read.end1 = value.end1;
    read.ref2 = header.Ref_to_ID[key.Ref2];
    read.start2 = value.start2;
    read.end2 = value.end2;
    read.ori = header.Ori_to_ID[key.Ori];
    read.type = header.Type_to_ID[key.Type];
    read.prob_size = value.prob_size;
    read.probability = value.probability;
    read.overlap = value.ov;
    read.pairs = value.pairs;
    read.clone = value.clone;
    read.concerdant1 = value.FirstConcordantOverlap;
    read.concerdant2 = value.SecondConcordantOverlap;
    read.noise1 = value.FirstNoise;
    read.noise2 = value.SecondNoise;
    
    FileStream->write((char*) & read, sizeof(ClusterFile::ClusterFileRead));
    return true;
}

bool ClusterFile::AddCluster(Cluster &cluster, string &ref1, string &ref2, string &ori, string &type) {
    string key = GenerateKey(ref1, ref2, type, ori);
    if (Data.find(key) == Data.end()) {
        this->AddRef(ref1);
        Data[key].Ref1 = ref1;
        this->AddRef(ref2);
        Data[key].Ref2 = ref2;
        this->AddType(type);
        Data[key].Type = type;
        this->AddOri(ori);
        Data[key].Ori = ori;
    }
    DataValue value;
    value.start1 = cluster.s1;
    value.end1 = cluster.e1;
    value.start2 = cluster.s2;
    value.end2 = cluster.e2;
    value.ov = cluster.ov;
    value.clone = cluster.clone;
    value.prob_size = cluster.prob_size;
    value.probability = cluster.probability;
    value.pairs = cluster.pairs.size();
    Data[key].Values.push_back(value);
    
    if (header.infile) return WriteCluster(Data[key], value);
    else return this->ReWriteFile();
}

string ClusterFile::GenerateKey(string &ref1, string &ref2, string &type, string &ori) {
    return ref1 + ";" + ref2 + ";" + type + ";" + ori;
}

string ClusterFile::PrintFile(unsigned short int MinAgainst, unsigned short int MinMust) {
    stringstream buffer;
    for(map<string,DataKey>::iterator it = Data.begin(); it != Data.end(); it++) {
        for (DataValue value : it->second.Values) {
            if (value.FirstConcordantOverlap == 0) value.FirstConcordantOverlap++;
            if (value.SecondConcordantOverlap == 0) value.SecondConcordantOverlap++;
            float relativeConcordant = ((float)value.pairs / (float)value.FirstConcordantOverlap + (float)value.pairs / (float)value.SecondConcordantOverlap) / 2;
            if (max_concordant > 0 and relativeConcordant > max_concordant) continue;
            if (min_concordant > 0 and relativeConcordant < min_concordant) continue;
            
            if (value.FirstNoise > value.pairs) value.FirstNoise -= value.pairs;
            else value.FirstNoise = 0;
            if (value.SecondNoise > value.pairs) value.SecondNoise -= value.pairs;
            else value.SecondNoise = 0;
            float relativeNoise = ((float)value.FirstNoise + (float)value.SecondNoise) / ((float)value.pairs * 2);
            if (max_noise > 0 and relativeNoise > max_noise) continue;
            
            if ((value.against >= MinAgainst && MinAgainst != 0) || (value.must < MinMust && MinMust != 0)) continue;
            
            buffer << it->second.Ref1 << "\t" << value.start1 << "\t" << value.end1 << "\t";
            buffer << it->second.Ref2 << "\t" << value.start2 << "\t" << value.end2 << "\t";
            buffer << it->second.Ori << "\t" << (value.pairs - value.clone) << "\t" << value.clone << "\t" << it->second.Type << "\t";
            buffer << value.ov << "\t" << value.prob_size << "\t" << value.probability << "\t";
            buffer << value.against << "-" << value.must << "\t";
            buffer << value.FirstConcordantOverlap << "-" << value.SecondConcordantOverlap << "\t";
            buffer << relativeConcordant << "\t";
            buffer << value.FirstNoise << "-" << value.SecondNoise << "\t";
            buffer << relativeNoise;
            buffer << endl;
        }
    }
    return buffer.str();
}

void ClusterFile::TrimRefNames(string trim) {
    ReadNameConverter readNameConverter;
    readNameConverter.NameReplace->push_back({"",trim});
    for(map<string,DataKey>::iterator it = Data.begin(); it != Data.end(); it++) {
        if (readNameConverter.TrimName(it->second.Ref1) or readNameConverter.TrimName(it->second.Ref2)) {
            Data[GenerateKey(it->second.Ref1, it->second.Ref2, it->second.Type, it->second.Ori)] = it->second;
            Data.erase(it);
        }
        //it->first = GenerateKey(it->second.Ref1, it->second.Ref2, it->second.Type, it->second.Ori);
    }
}

bool ClusterFile::AddRef(string ref) {
    if (header.Ref_to_ID.find(ref) != header.Ref_to_ID.end()) return false;
    unsigned int id = header.Ref_to_ID.size();
    header.Ref_to_ID[ref] = id;
    header.ID_to_Ref[id] = ref;
    header.infile = false;
    return true;
}

bool ClusterFile::AddType(string type) {
    if (header.Type_to_ID.find(type) != header.Type_to_ID.end()) return false;
    unsigned int id = header.Type_to_ID.size();
    header.Type_to_ID[type] = id;
    header.ID_to_Type[id] = type;
    header.infile = false;
    return true;
}

bool ClusterFile::AddOri(string ori) {
    if (header.Ori_to_ID.find(ori) != header.Ori_to_ID.end()) return false;
    unsigned int id = header.Ori_to_ID.size();
    header.Ori_to_ID[ori] = id;
    header.ID_to_Ori[id] = ori;
    header.infile = false;
    return true;
}