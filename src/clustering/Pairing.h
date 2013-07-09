/* 
 * File:   Pairing.h
 * Author: peter
 *
 * Created on January 15, 2013, 4:44 PM
 */

#ifndef PAIRING_H
#define	PAIRING_H

#include "../Structs.h"
#include "../Task.h"
#include "../Config.h"
#include "ReferenceDataStorage.h"
#include <vector>

class Pairing: public Task {
public:
    Pairing(ReferenceDataStorage* Ref, Config* config);
    virtual ~Pairing();
    
    long int Paired = 0;
protected:
    Pairing(Config* config);
    virtual void ProcesPair(ReadDetails& FirstRead, ReadDetails& SecondRead);
    void InternalRun(void);
    virtual void InternalDoWhilePauze(void);
    virtual void InternalFinish(void);
    
    vector<Singleread>* FirstReads;
    vector<Singleread>* SecondReads;
    
    Singleread* First;
    Singleread* Second;
private:
    ReferenceDataStorage* Ref;
    PairStats Stats;
    vector<InsertsizePair>* THpairs;
    vector<Pair>* HTpairs;
    vector<Pair>* TTpairs;
    vector<Pair>* HHpairs;
};

#endif	/* PAIRING_H */

