/* 
 * File:   CompareMain.h
 * Author: peter
 *
 * Created on April 8, 2013, 12:33 PM
 */

#ifndef COMPAREMAIN_H
#define	COMPAREMAIN_H

#include <map>
#include "../Structs.h"

using namespace std;

class CompareMain {
public:
    CompareMain();
    virtual ~CompareMain();
    
    int main(int argc, char *argv[]);
    static int usage();
private:
    struct SV_record {
        unsigned int start, end;
        bool overlap = false;
    };
    map<string, vector<SV_record>> RefSVs;
    vector<string> InputLines;
    int minsize = 0;
};

#endif	/* COMPAREMAIN_H */

