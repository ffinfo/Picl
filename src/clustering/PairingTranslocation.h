/* 
 * File:   PairingTranslocation.h
 * Author: peter
 *
 * Created on January 17, 2013, 7:15 PM
 */

#ifndef PAIRINGTRANSLOCATION_H
#define	PAIRINGTRANSLOCATION_H

#include "Pairing.h"
#include "TranslocationDataStorage.h"

class PairingTranslocation: public Pairing {
public:
    PairingTranslocation(TranslocationDataStorage* Trans, int type, Config* config);
    virtual ~PairingTranslocation();
    
    static const int TypeForwardReverse = 1;
    static const int TypeReverseForward = 2;
protected:
    void ProcesPair(ReadDetails& FirstRead, ReadDetails& SecondRead);
    void InternalFinish(void);
    void InternalDoWhilePauze(void);
private:
    int type;
    TranslocationDataStorage* Trans;
    vector<Pair>* THpairs;
    vector<Pair>* HTpairs;
    vector<Pair>* TTpairs;
    vector<Pair>* HHpairs;
};

#endif	/* PAIRINGTRANSLOCATION_H */

