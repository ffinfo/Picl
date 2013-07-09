/* 
 * File:   MultisampleMain.h
 * Author: peter
 *
 * Created on May 3, 2013, 11:23 AM
 */

#ifndef MULTISAMPLEMAIN_H
#define	MULTISAMPLEMAIN_H

#include "../filter/ClusterFile.h"

using namespace std;

class MultisampleMain {
public:
    MultisampleMain();
    virtual ~MultisampleMain();
    
    int main(int argc, char *argv[]);
    static int usage();
private:
    vector<string> TrimRefNames;
    unsigned int cuttoff = 0;
    vector<ClusterFile*> clusterFiles;
    
    struct SampleDataValue {
        unsigned int sampleID;
        ClusterFile::DataValue * value;
    };
    struct ClusterDataValue {
        unsigned int start1, end1, start2, end2;
    };
    typedef pair<ClusterDataValue, vector<SampleDataValue>> MultiDataValue;
    struct DataKey : ClusterFile::DataKey {
        vector<MultiDataValue> Values;
    };
    struct SortOnS1 {
        bool operator()(const MultiDataValue& r1, const MultiDataValue& r2) { 
            if (r1.first.start1 < r2.first.start1) return true;
            else return false;
        }
    };
    map<string,DataKey> Data;
    
    double minOV = 0.5;
    float max_noise = 0, max_concordant = 0, min_concordant = 0;
};

#endif	/* MULTISAMPLEMAIN_H */

