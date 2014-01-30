/* 
 * File:   ClusterFile.h
 * Author: peter
 *
 * Created on March 19, 2013, 4:30 PM
 */

#ifndef CLUSTERFILE_H
#define	CLUSTERFILE_H

#include "../Structs.h"
#include <map>
#include <vector>
#include <string>

using namespace std;

class ClusterFile {
public:
    struct Header {
        map<unsigned int,string> ID_to_Ref;
        map<string,unsigned int> Ref_to_ID;
        map<unsigned int,string> ID_to_Type;
        map<string,unsigned int> Type_to_ID;
        map<unsigned int,string> ID_to_Ori;
        map<string,unsigned int> Ori_to_ID;
        bool infile = false;
    };
    struct DataValue {
        unsigned int start1, end1, start2, end2;
        unsigned short int pairs, clone;
        float probability = 0;
        unsigned short int prob_size = 0;
        unsigned short int ov = 0;
        unsigned short int must = 0, against = 0;
        unsigned short int FirstConcordantOverlap = 0, SecondConcordantOverlap = 0;
        unsigned short int FirstNoise = 0, SecondNoise = 0;
    };
    struct DataKey {
        string Ref1, Ref2, Ori, Type;
        string getKey() {
            return Ref1 + ";" + Ref2 + ";" + Type + ";" + Ori;
        }
        vector<DataValue> Values;
        bool infile = false;
    };
    struct SortDataValueOnStart1 {
        bool operator()(const DataValue& t1, const DataValue& t2) { 
            if (t1.start1 < t2.start1) return true;
            else return false;
        }
    };
    struct ClusterFileRead {
        unsigned int ref1:16;
        unsigned int start1 = 0, end1 = 0;
        unsigned int ref2:16;
        unsigned int start2 = 0, end2 = 0;
        unsigned int ori:2;
        unsigned int type:6;
        int prob_size:16;
        float probability;
        unsigned int overlap:8;
        unsigned int pairs:16;
        unsigned int clone:16;
        unsigned short int concerdant1:16;
        unsigned short int concerdant2:16;
        unsigned short int noise1:16;
        unsigned short int noise2:16;
    };
    
    static const int OriTH = 0;
    static const int OriHT = 1;
    static const int OriHH = 2;
    static const int OriTT = 3;
    
    ClusterFile();
    ClusterFile(string filename);
    virtual ~ClusterFile();
    
    bool ReadFile(string filename, vector<string> &trim);
    bool ReadFile(string filename);
    bool ReadFile(string filename, vector<string> &trim, int Cuttoff, float prob_cutoff, float max_noise, float max_concordant, float min_concordant);
    bool ReadFile(string filename, int Cuttoff, float prob_cutoff, float max_noise, float max_concordant, float min_concordant);
    
    string PrintFile(unsigned short int MinAgainst, unsigned short int MinMust);
    string PrintFile(unsigned short int MinAgainst, unsigned short int MinMust, bool VCFoutput);
    bool OpenFile();
    bool CloseFile();
    bool WriteHeader();
    bool ReWriteFile();
    bool WriteCluster(DataKey &key, DataValue &value);
    bool AddCluster(Cluster &cluster, string &ref1, string &ref2, string &ori, string &type);
    bool AddRef(string ref);
    bool AddType(string type);
    bool AddOri(string ori);
    string GenerateKey(string &ref1, string &ref2, string &type, string &ori);
    void TrimRefNames(string trim);
    
    float max_noise= 0, max_concordant = 0, min_concordant = 0;
    string filename;
    
    //map<unsigned int,string> ClusterFileRefHeader;
    //map<unsigned int,string> ClusterFileTypeHeader;
    
    Header header;
    
    map<string,DataKey> Data;
    
private:
    fstream *FileStream;
};

#endif	/* CLUSTERFILE_H */

