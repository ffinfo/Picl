/* 
 * File:   Pairing.cpp
 * Author: peter
 * 
 * Created on January 15, 2013, 4:44 PM
 */

#include "Pairing.h"
#include <iostream>
#include <algorithm>

Pairing::Pairing(Config* config) : Task(config) {
}

Pairing::Pairing(ReferenceDataStorage* Ref, Config* config) : Task(config) {
    this->Ref = Ref;
    this->FirstReads = Ref->forwardreads;
    this->SecondReads = Ref->reversereads;
    this->THpairs = Ref->THpairs;
    this->HTpairs = Ref->HTpairs;
    this->TTpairs = Ref->TTpairs;
    this->HHpairs = Ref->HHpairs;
    Ref->Coverage->resize(ceil((double)Ref->Length / (double)config->CoverageBinSize));
    TaskName = "Pairing for ref: " + Ref->Name;
    Ready = true;
}

Pairing::~Pairing() {
}

void Pairing::InternalFinish() {
    THpairs->shrink_to_fit();
    HTpairs->shrink_to_fit();
    TTpairs->shrink_to_fit();
    HHpairs->shrink_to_fit();
    Ref->forwardreadsPaired =+ Paired;
    Ref->reversereadsPaired =+ Paired;
    Ref->Stats.Pair.HTpairs = HTpairs->size();
    Ref->Stats.Pair.THpairs = THpairs->size();
    Ref->Stats.Pair.HHpairs = HHpairs->size();
    Ref->Stats.Pair.TTpairs = TTpairs->size();
}

void Pairing::InternalRun() {
    if (FirstReads->size() <= 0 || SecondReads->size() <=0) {
        //Done = true;
        return;
    }
    
    vector<Singleread>::iterator FirstIt = FirstReads->begin();
    vector<Singleread>::iterator SecondIt = SecondReads->begin();
    //while (FirstIt != FirstReads->end() && SecondIt != SecondReads->end()) {
    while (true) {
        if (FirstIt == FirstReads->end() or SecondIt == SecondReads->end()) break;
        //First = &*FirstIt;
        //Second = &*SecondIt;
        
        if (FirstIt->ID == 0) {
            ++FirstIt;
            continue;
        }
        if (SecondIt->ID == 0) {
            ++SecondIt;
            continue;
        }
        if (FirstIt->ID == SecondIt->ID) {
            ProcesPair(FirstIt->Details, SecondIt->Details);
            
            FirstIt->ID = 0;
            SecondIt->ID = 0;
            ++FirstIt;
            ++SecondIt;
            ++Paired;
        } else if (FirstIt->ID > SecondIt->ID) {
            if (FirstIt != FirstReads->end()) ++FirstIt;
        } else {
            if (SecondIt != SecondReads->end()) ++SecondIt;
        }
    }
    
    return;
}

void Pairing::ProcesPair(ReadDetails& ForwardRead, ReadDetails& ReverseRead) {
    Pair pair;
    if (ForwardRead.Position < ReverseRead.Position) {
        pair.First = ForwardRead;
        pair.Second = ReverseRead;
    } else {
        pair.First = ReverseRead;
        pair.Second = ForwardRead;
    }
    
    int bin = floor((double)pair.First.Position / (double)config->CoverageBinSize);
    CoverageFileRead* coverageRead1 = &Ref->Coverage->at(bin);
    int bin2 = floor((double)pair.Second.Position / (double)config->CoverageBinSize);
    CoverageFileRead* coverageRead2 = &Ref->Coverage->at(bin2);
    
    if (pair.First.Strand == pair.Second.Strand) { // TH & HT
        if (pair.First.ForwardRead == pair.First.Strand) {
            /*if ((pair.Second.Position + pair.Second.Lenght - 1) < (pair.First.Position + pair.First.Lenght - 1) ||
                    (pair.First.Position + pair.First.Lenght - 1) > (pair.Second.Position + pair.Second.Lenght - 1)) {
                OverhangingPairs->push_back(pair);
            } else {*/
                InsertsizePair insertsizePair;
                insertsizePair.Details = pair;
                insertsizePair.Insertsize = pair.Second.Position - pair.First.Position + pair.Second.Lenght - 1;
                if (THpairs->size() >= THpairs->capacity()) this->PauseThread();
                THpairs->push_back(insertsizePair);
            //}
        } else {
            if (HTpairs->size() >= HTpairs->capacity()) this->PauseThread();
            HTpairs->push_back(pair);
            coverageRead1->Upstream.HT++;
            coverageRead2->Downstream.HT++;
        }
    } else { // HH & TT
        if (pair.First.ForwardRead == pair.First.Strand) {
            if (TTpairs->size() >= TTpairs->capacity()) this->PauseThread();
            TTpairs->push_back(pair);
            coverageRead1->Upstream.TT++;
            coverageRead2->Downstream.TT++;
        } else {
            if (HHpairs->size() >= HHpairs->capacity()) this->PauseThread();
            HHpairs->push_back(pair);
            coverageRead1->Upstream.HH++;
            coverageRead2->Downstream.HH++;
        }
    }
}

void Pairing::InternalDoWhilePauze() {
    if (THpairs->size() >= THpairs->capacity()) THpairs->reserve(10000);
    if (HTpairs->size() >= HTpairs->capacity()) HTpairs->reserve(10000);
    if (TTpairs->size() >= TTpairs->capacity()) TTpairs->reserve(10000);
    if (HHpairs->size() >= HHpairs->capacity()) HHpairs->reserve(10000);
}