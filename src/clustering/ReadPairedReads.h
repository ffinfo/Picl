/* 
 * File:   ReadPairedReads.h
 * Author: peter
 *
 * Created on January 29, 2013, 4:08 PM
 */

#ifndef READPAIREDREADS_H
#define	READPAIREDREADS_H

#include <cstdlib>
#include <vector>
#include <thread>
#include <api/BamReader.h>
#include <api/BamAlignment.h>
#include "../Structs.h"
#include "ReferenceDataStorage.h"
#include "../Config.h"
#include "../Task.h"

class ReadPairedReads: public Task {
public:
    ReadPairedReads(ReferenceDataStorage* Ref, long int begin, long int end, Config* config);
    virtual ~ReadPairedReads();
    
    long int ForwardHighQualityCount = 0;
    long int ForwardLowQualityCount = 0;
    long int ReverseHighQualityCount = 0;
    long int ReverseLowQualityCount = 0;
    long int MateNotMapped = 0;
    long int NotPaired = 0;
    long int Duplicates = 0;
    
    long int begin;
    long int end;
    
    vector<Singleread>* ForwardHighQuality;
    vector<Singleread>* ForwardLowQuality;
    vector<Singleread>* ReverseHighQuality;
    vector<Singleread>* ReverseLowQuality;
protected:
    void InternalRun(void);
    void InternalFinish(void);
    void InternalDoWhilePauze(void);
private:
    BWA_TagData ReadBWA_TagData(BamTools::BamAlignment &alignment);
    
    ReferenceDataStorage* Ref;
};

#endif	/* READPAIREDREADS_H */

