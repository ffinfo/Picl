/* 
 * File:   SortSingleReads.h
 * Author: peter
 *
 * Created on January 11, 2013, 12:23 PM
 */

#ifndef SORTSINGLEREADS_H
#define	SORTSINGLEREADS_H

#include <vector>
#include "../Structs.h"
#include "../Config.h"
#include "../Task.h"
#include "ReferenceDataStorage.h"

class SortSingleReads: public Task {
public:
    SortSingleReads(ReferenceDataStorage* Ref, int type, Config* config);
    virtual ~SortSingleReads();
    void InternalDoBeforeRun(void);
    
    static const int TypeForward = 1;
    static const int TypeReverse = 2;
    static const int TypeForwardLowQuality = 3;
    static const int TypeReverseLowQuality = 4;
protected:
    void InternalRun(void);
    void InternalFinish(void);
private:
    int type;
    ReferenceDataStorage* Ref;
    vector<vector<Singleread>*>* Bins;
    vector<Singleread>* TargetSingleReads;
};

#endif	/* SORTSINGLEREADS_H */

