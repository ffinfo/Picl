/* 
 * File:   ClearMemorySinglereads.h
 * Author: peter
 *
 * Created on January 17, 2013, 1:07 PM
 */

#ifndef CLEARMEMORYSINGLEREADS_H
#define	CLEARMEMORYSINGLEREADS_H

#include "../Structs.h"
#include "../Task.h"
#include "../Config.h"
#include "ReferenceDataStorage.h"

class ClearMemorySinglereads: public Task {
public:
    ClearMemorySinglereads(ReferenceDataStorage* Ref, int type, Config* config);
    virtual ~ClearMemorySinglereads();
    
    void Reset(void);
    
    long int Cleared = 0;
    static const int TypeForward = 1;
    static const int TypeReverse = 2;
    static const int TypeForwardLowquality = 3;
    static const int TypeReverseLowquality = 4;
protected:
    void InternalRun(void);
    void InternalFinish(void);
private:
    int type;
    ReferenceDataStorage* Ref;
    vector<Singleread>* SingleReads;
};

#endif	/* CLEARMEMORYSINGLEREADS_H */

