/* 
 * File:   TranslocationDataStorage.h
 * Author: peter
 *
 * Created on February 12, 2013, 1:04 PM
 */

#ifndef TRANSLOCATIONDATASTORAGE_H
#define	TRANSLOCATIONDATASTORAGE_H

#include "../Config.h"
#include "../Structs.h"
#include "ReferenceDataStorage.h"

class TranslocationDataStorage {
public:
    TranslocationDataStorage(ReferenceDataStorage* Ref1, ReferenceDataStorage* Ref2);
    virtual ~TranslocationDataStorage();
    
    ReferenceDataStorage* Ref1;
    ReferenceDataStorage* Ref2;
    
    vector<Pair>* THpairs,* HTpairs,* HHpairs,* TTpairs;
    vector<SingleLowQualityPair>* singleLowQualityPairs;
    vector<LowQualityPair>* lowQualityPairs;
    
    TransStats Stats;
private:

};

#endif	/* TRANSLOCATIONDATASTORAGE_H */

