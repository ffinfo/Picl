/* 
 * File:   PairedBamMakerMain.h
 * Author: peter
 *
 * Created on February 19, 2013, 2:12 PM
 */

#ifndef PAIREDBAMMAKERMAIN_H
#define	PAIREDBAMMAKERMAIN_H

#include <cstdlib>
#include <string>
#include <api/BamReader.h>
#include <api/BamWriter.h>
#include <api/BamAlignment.h>
#include <api/algorithms/Sort.h>
#include "../Config.h"
#include "../clustering/ReadNameConverter.h"
#include "../Structs.h"

using namespace std;

class PairedBamMakerMain {
public:
    PairedBamMakerMain();
    virtual ~PairedBamMakerMain();
    
    int main(int argc, char *argv[]);
    static int usage();
private:
    bool ReadAlignment(BamTools::BamReader &reader, BamTools::BamAlignment &alignment, bool first);
    bool WriteAlignment(BamTools::BamAlignment &alignment);
    void WritePair (BamTools::BamAlignment &FirstAlignment, BamTools::BamAlignment &SecondAlignment);
    string ConvertName(string Name);
    
    string FirstInputBamFile = "";
    string SecondInputBamFile = "";
    string OutputBamFile = "";
    bool AsumeSortedOnQueryName = false;
    bool OverwriteOutputfile = false;
    bool FirstOriInverted = false, SecondOriInverted = false, CorrectOriFirst = false, CorrectOriSecond = false;
        
    ReadNameConverter readNameConverter;
    BamTools::BamReader FirstBamReader;
    BamTools::BamReader SecondBamReader;
    BamTools::BamWriter OutputWriter;

    vector<BamTools::BamAlignment> ForwardUnpaired;
    vector<BamTools::BamAlignment> ReverseUnpaired;
    
    vector<string> addReadgroupToHeader;
};

#endif	/* PAIREDBAMMAKERMAIN_H */

