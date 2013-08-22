/* 
 * File:   Structs.h
 * Author: peter
 *
 * Created on November 30, 2012, 2:21 PM
 */

#ifndef STRUCTS_H
#define	STRUCTS_H

#include <string>
#include <vector>

using namespace std;

//Data structs
struct ReadDetails {
    unsigned int Position : 30;
    bool Strand : 1;
    bool ForwardRead : 1;
    int Lenght : 8;
};
struct Singleread {
    string ID_string;
    long int ID;
    ReadDetails Details;
};
struct LowQualityPair {
    ReadDetails Forward;
    ReadDetails Reverse;
};
struct SingleLowQualityPair {
    ReadDetails HighQuality;
    ReadDetails LowQuality;
};
struct Pair {
    ReadDetails First;
    ReadDetails Second;
};
struct Cluster {
    unsigned int s1 = 0, e1 = 0, s2 = 0, e2 = 0;
    unsigned short int ov = 0;
    unsigned int clone = 0;
    int prob_size = 0;
    float probability = 0;
    vector<unsigned int> pairs;
};
//struct ClusterFileRead {
//    unsigned int ref1:16;
//    unsigned int start1 = 0, end1 = 0;
//    unsigned int ref2:16;
//    unsigned int start2 = 0, end2 = 0;
//    unsigned int ori:2;
//    unsigned int type:6;
//    int prob_size:16;
//    float probability;
//    unsigned int overlap:8;
//    unsigned int pairs:16;
//    unsigned int clone:16;
//    unsigned int concerdant1:16;
//    unsigned int concerdant2:16;
//    unsigned int noise1:16;
//    unsigned int noise2:16;
//};

struct CoverageFileRead {
    struct Details {
        //unsigned short int maxInsertSize = 0;
        //unsigned short int medianInsertSize = 0;
        //unsigned short int minInsterSize = 0;
        unsigned short int concortdantPairs = 0;
        unsigned short int deletions = 0;
        unsigned short int insertions = 0;
        unsigned short int HT = 0;
        unsigned short int HH = 0;
        unsigned short int TT = 0;
        unsigned short int translocations = 0;
    };
    Details Upstream, Downstream;
};
/*struct CoverageRead {
    CoverageFileRead FileRead;
    unsigned int totalUpStream;
    unsigned int totalInsertsizeUpStream;
    unsigned int totalDownStream;
    unsigned int totalInsertsizeDownStream;
};*/
struct InsertsizePair {
    unsigned int Insertsize;
    Pair Details;
};
struct BWA_TagData {
    uint32_t X0;
    uint32_t X1;
    uint32_t XO;
    uint32_t XM;
    uint32_t XG; // not used in 123sv
};

//Stats structs
struct ReadStats {
    unsigned long int Forward = 0;
    unsigned long int Reverse = 0;
    unsigned long int ForwardLowQuality = 0;
    unsigned long int ReverseLowQuality = 0;
};
struct PairStats {
    unsigned long int THpairs = 0; // Devides in Normal, Deletions and Insertions
    unsigned long int HTpairs = 0;
    unsigned long int HHpairs = 0;
    unsigned long int TTpairs = 0;
    unsigned long int LowQuality = 0;
    unsigned long int SingleLowQuality = 0;
};
struct InsertsizeStats {
    long unsigned int max_curve = 0;
    long unsigned int CurveTotal = 0;
    vector<long unsigned int> curve;
    unsigned int Median = 0;
    unsigned int DeletionCutoff = 0;
    unsigned int InsertionCutoff = 0;
    unsigned long int Normal = 0;
    unsigned long int Deletions = 0;
    unsigned long int Insertions = 0;
};
struct ClusterStats {
    unsigned long int HTclusters = 0;
    unsigned long int HHclusters = 0;
    unsigned long int TTclusters = 0;
    unsigned long int Deletions = 0;
    unsigned long int Insertions = 0;
    unsigned long int Translocations = 0;
};
struct RefStats {
    ReadStats Read;
    PairStats Pair;
    InsertsizeStats Insertsize;
    ClusterStats Cluster;
};
struct TransClusterStats {
    unsigned long int THclusters = 0;
    unsigned long int HTclusters = 0;
    unsigned long int HHclusters = 0;
    unsigned long int TTclusters = 0;
};
struct TransStats {
    PairStats Pair;
    TransClusterStats Cluster;
};

//sort structs
struct SortSinglereadOnID {
    bool operator()(const Singleread& t1, const Singleread& t2)
    { 
        if (t1.ID == t2.ID) {
            if (t1.ID_string.compare(t2.ID_string) > 0) {
                return true;
            } else {
                return false;
            }
        } else if (t1.ID > t2.ID) {
                return true;
        } else {
                return false;
        } 
    }
};
struct SortClustersOnS1 {
    bool operator()(const Cluster& t1, const Cluster& t2)
    { 
        if (t1.s1 < t2.s1) {
                return true;
        } else {
                return false;
        } 
    }
};
struct SortClustersOnS2 {
    bool operator()(const Cluster& t1, const Cluster& t2)
    { 
        if (t1.s2 < t2.s2) {
                return true;
        } else {
                return false;
        } 
    }
};
struct SortClustersOnClustersize {
    bool operator()(const Cluster& t1, const Cluster& t2)
    { 
        if (t1.pairs.size() > t2.pairs.size()) {
                return true;
        } else if (t1.pairs.size() < t2.pairs.size()){
                return false;
        } else if (t1.s1 < t2.s1) {
                return true;
        } else {
                return false;
        }
    }
};
struct SortOnInsertsize {
    bool operator()(const InsertsizePair& r1, const InsertsizePair& r2)
    { 
        if (r1.Insertsize < r2.Insertsize) {
                return true;
        } else {
                return false;
        } 
    }
};
struct SortPairsOnFirstPosition {
    bool operator()(const Pair& r1, const Pair& r2)
    { 
        if (r1.First.Position < r2.First.Position) {
                return true;
        } else {
                return false;
        } 
    }
};

struct PairedBin {
    unsigned long int start, stop;
    string Ref, FilePaired, FileNotPaired;
};

#endif	/* STRUCTS_H */

