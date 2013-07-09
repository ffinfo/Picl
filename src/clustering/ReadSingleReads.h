/* 
 * File:   ReadSingleReads.h
 * Author: peter
 *
 * Created on January 8, 2013, 1:45 PM
 */

#ifndef READSINGLEREADS_H
#define	READSINGLEREADS_H

#include <cstdlib>
#include <vector>
#include <thread>
#include <api/BamAlignment.h>
#include "ReferenceDataStorage.h"
#include "../Structs.h"
#include "../Config.h"
#include "../Task.h"

using namespace std;

class ReadSingleReads: public Task {
public:
    ReadSingleReads(ReferenceDataStorage* Ref, bool forward, long int begin, long int end, Config* config);
    virtual ~ReadSingleReads();
    
    long int HighQualityCount = 0;
    long int LowQualityCount = 0;
    vector<Singleread>* HighQuality;
    vector<Singleread>* LowQuality;
protected:
    void InternalRun(void);
    void InternalFinish(void);
    void InternalDoWhilePauze(void);
private:
    BWA_TagData ReadBWA_TagData(BamTools::BamAlignment &alignment);
    
    long int begin, end;
    ReferenceDataStorage* Ref;
    bool forward;
};

#endif	/* READSINGLEREADS_H */

