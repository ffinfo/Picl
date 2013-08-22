/* 
 * File:   SplitBamMain.h
 * Author: peter
 *
 * Created on July 2, 2013, 4:41 PM
 */

#include <cstdlib>
#include <string>
#include <api/BamReader.h>
#include <api/BamWriter.h>
#include <api/BamAlignment.h>
#include "../Structs.h"


#ifndef SPLITBAMMAIN_H
#define	SPLITBAMMAIN_H

class SplitBamMain {
public:
    struct qValues {
        unsigned long int all = 0;
        unsigned long int q30plus = 0;
        unsigned long int unique = 0;
    };
    
    SplitBamMain();
    virtual ~SplitBamMain();
    
    void qualitycheck(BamTools::BamAlignment &alignment, qValues &values);
    
    int main(int argc, char* argv[]);
    int usage();
    
    int splitOnOri();
    int splitOnQuality();
private:
    bool FirstOriInverted = false, SecondOriInverted = false, CorrectOriFirst = false, CorrectOriSecond = false;
    string Inputfile, Outputprefix;
    
    string Type = "ori";
    
    BamTools::BamReader BamReader;
};

#endif	/* SPLITBAMMAIN_H */

