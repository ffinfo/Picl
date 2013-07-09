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
    SplitBamMain();
    virtual ~SplitBamMain();
    
    int main(int argc, char* argv[]);
    int usage();
private:
    bool FirstOriInverted = false, SecondOriInverted = false, CorrectOriFirst = false, CorrectOriSecond = false;
    string Inputfile, Outputprefix;
    
    BamTools::BamReader BamReader;
    BamTools::BamWriter OutputWriter_TH, OutputWriter_TT, OutputWriter_HH, OutputWriter_HT;
    BamTools::BamWriter OutputWriter_Translocation;
    BamTools::BamWriter OutputWriter_Singletons_T, OutputWriter_Singletons_H, OutputWriter_Unmapped;
};

#endif	/* SPLITBAMMAIN_H */

