/* 
 * File:   PairingLowQuality.h
 * Author: peter
 *
 * Created on January 17, 2013, 12:11 PM
 */

#ifndef PAIRINGLOWQUALITY_H
#define	PAIRINGLOWQUALITY_H

#include "Pairing.h"
#include "TranslocationDataStorage.h"

class PairingLowQuality: public Pairing {
public:
    PairingLowQuality(ReferenceDataStorage* Ref, Config* config);
    PairingLowQuality(TranslocationDataStorage* Trans, int type, Config* config);
    virtual ~PairingLowQuality();
    
    static const int TypeNormal = 1;
    static const int TypeTransFR = 2;
    static const int TypeTransRF = 3;
protected:
    void ProcesPair(ReadDetails& FirstRead, ReadDetails& SecondRead);
    void InternalFinish(void);
private:
    int type;
    ReferenceDataStorage* Ref;
    TranslocationDataStorage* Trans;
    vector<LowQualityPair>* TargetPairs;
};

#endif	/* PAIRINGLOWQUALITY_H */

