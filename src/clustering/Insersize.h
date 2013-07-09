/* 
 * File:   Insersize.h
 * Author: peter
 *
 * Created on January 17, 2013, 1:40 PM
 */

#ifndef INSERSIZESTATS_H
#define	INSERSIZESTATS_H

#include "../Task.h"
#include "../Structs.h"
#include "../Config.h"
#include "ReferenceDataStorage.h"


class Insersize: public Task {
public:
    Insersize(ReferenceDataStorage* Ref, Config* config);
    virtual ~Insersize();
    
    InsertsizeStats Stats;
protected:
    void InternalRun(void);
    void InternalFinish(void);
    void InternalDoWhilePauze(void);
private:
    //vector<CoverageFileRead>* CoverageReads;
    ReferenceDataStorage* Ref;
    vector<InsertsizePair>* THpairs;
    vector<Pair>* Normal;
    vector<Pair>* Deletions;
    vector<Pair>* Insertions;
};

#endif	/* INSERSIZESTATS_H */

