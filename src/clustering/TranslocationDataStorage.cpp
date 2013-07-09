/* 
 * File:   TranslocationDataStorage.cpp
 * Author: peter
 * 
 * Created on February 12, 2013, 1:04 PM
 */

#include "TranslocationDataStorage.h"

TranslocationDataStorage::TranslocationDataStorage(ReferenceDataStorage* Ref1, ReferenceDataStorage* Ref2) {
    if (Ref1->Name.compare(Ref2->Name) < 0) {
        this->Ref1 = Ref1;
        this->Ref2 = Ref2;
    } else {
        this->Ref1 = Ref2;
        this->Ref2 = Ref1;
    }
     
    THpairs = new vector<Pair>;
    HTpairs = new vector<Pair>;
    HHpairs = new vector<Pair>;
    TTpairs = new vector<Pair>;
    singleLowQualityPairs = new vector<SingleLowQualityPair>;
    lowQualityPairs = new vector<LowQualityPair>;
}

TranslocationDataStorage::~TranslocationDataStorage() {
}

