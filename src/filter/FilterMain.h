/* 
 * File:   FilterMain.h
 * Author: peter
 *
 * Created on March 19, 2013, 3:54 PM
 */

#ifndef FILTERMAIN_H
#define	FILTERMAIN_H

#include "../Structs.h"
#include "ClusterFile.h"

using namespace std;

class FilterMain {
public:
    FilterMain();
    virtual ~FilterMain();
    
    int main(int argc, char *argv[]);
    static int usage();
private:
    bool HasOverlap(ClusterFile::DataValue &InputValue, ClusterFile::DataKey &OverlapSet);
    int cuttoff = 0;
    float prob_cutoff = 0, max_noise = 0, min_concordant = 0, max_concordant = 0;
    unsigned short int MinAgainst = 1;
    unsigned short int MinMust;
    unsigned short int MinMustOption = 0;
    string InputFile;
    string CoverageDir = "";
    
    ClusterFile Input;
    
    vector<string> Against;
    vector<string> Must;
};

#endif	/* FILTERMAIN_H */

