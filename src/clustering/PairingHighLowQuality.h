/* 
 * File:   PairingHighLowQuality.h
 * Author: peter
 *
 * Created on January 17, 2013, 11:12 AM
 */

#ifndef PAIRINGHIGHLOWQUALITY_H
#define	PAIRINGHIGHLOWQUALITY_H

#include "Pairing.h"
#include "TranslocationDataStorage.h"

class PairingHighLowQuality: public Pairing {
public:
    PairingHighLowQuality(ReferenceDataStorage* Ref, int type, Config* config);
    PairingHighLowQuality(TranslocationDataStorage* Trans, int type, Config* config);
    virtual ~PairingHighLowQuality();
    
    static const int TypeHighQualityForward = 1;
    static const int TypeHighQualityReverse = 2;
    static const int TypeTransHighQualityForward1 = 3;
    static const int TypeTransHighQualityReverse1 = 4;
    static const int TypeTransHighQualityForward2 = 5;
    static const int TypeTransHighQualityReverse2 = 6;
protected:
    void ProcesPair(ReadDetails& HighQualityRead, ReadDetails& LowQualityRead);
    void InternalFinish(void);
    void InternalDoWhilePauze(void);
private:
    TranslocationDataStorage* Trans;
    int type;
    ReferenceDataStorage* Ref;
    vector<SingleLowQualityPair>* TargetPairs;
};

#endif	/* PAIRINGHIGHLOWQUALITY_H */

