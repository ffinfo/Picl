/* 
 * File:   PairingTranslocation.cpp
 * Author: peter
 * 
 * Created on January 17, 2013, 7:15 PM
 */

#include "PairingTranslocation.h"
#include <algorithm>

PairingTranslocation::PairingTranslocation(TranslocationDataStorage* Trans, int type, Config* config) : Pairing(config) {
    this->Trans = Trans;
    this->type = type;
    
    THpairs = new vector<Pair>;
    HTpairs = new vector<Pair>;
    TTpairs = new vector<Pair>;
    HHpairs = new vector<Pair>;
    
    switch (type) {
        case PairingTranslocation::TypeForwardReverse:
            FirstReads = Trans->Ref1->forwardreads;
            SecondReads = Trans->Ref2->reversereads;
            TaskName = "Pairing Forward-" + Trans->Ref1->Name + " to Reverse-" + Trans->Ref2->Name;
            break;
        case PairingTranslocation::TypeReverseForward:
            FirstReads = Trans->Ref1->reversereads;
            SecondReads = Trans->Ref2->forwardreads;
            TaskName = "Pairing Forward-" + Trans->Ref2->Name + " to Reverse-" + Trans->Ref1->Name;
            break;
    }
    Ready = true;
}

PairingTranslocation::~PairingTranslocation() {
}

void PairingTranslocation::InternalFinish() {
    THpairs->shrink_to_fit();
    HTpairs->shrink_to_fit();
    TTpairs->shrink_to_fit();
    HHpairs->shrink_to_fit();
    //CoverageFile coverageFile1 = CoverageFile(config->coverageDir + "/" + Trans->Ref1->Name + ".cov");
    //vector<CoverageFileRead> reads1 = coverageFile1.getAllReads();
    //CoverageFile coverageFile2 = CoverageFile(config->coverageDir + "/" + Trans->Ref2->Name + ".cov");
    //vector<CoverageFileRead> reads2 = coverageFile2.getAllReads();
    if (HHpairs->size() > 0) {
        Trans->HHpairs->reserve(HHpairs->size());
        Trans->HHpairs->insert(Trans->HHpairs->end(), HHpairs->begin(), HHpairs->end());
        for (Pair pair : *HHpairs) {
            Trans->Ref1->Coverage->at(floor((double)pair.First.Position / (double)config->CoverageBinSize)).Downstream.translocations++;
            Trans->Ref2->Coverage->at(floor((double)pair.Second.Position / (double)config->CoverageBinSize)).Downstream.translocations++;
        }
    }
    delete HHpairs;
    
    if (TTpairs->size() > 0) {
        Trans->TTpairs->reserve(TTpairs->size());
        Trans->TTpairs->insert(Trans->TTpairs->end(), TTpairs->begin(), TTpairs->end());
        for (Pair pair : *TTpairs) {
            Trans->Ref1->Coverage->at(floor((double)pair.First.Position / (double)config->CoverageBinSize)).Upstream.translocations++;
            Trans->Ref2->Coverage->at(floor((double)pair.Second.Position / (double)config->CoverageBinSize)).Upstream.translocations++;
        }
    }
    delete TTpairs;
    
    if (HTpairs->size() > 0) {
        Trans->HTpairs->reserve(HTpairs->size());
        Trans->HTpairs->insert(Trans->HTpairs->end(), HTpairs->begin(), HTpairs->end());
        for (Pair pair : *HTpairs) {
            Trans->Ref1->Coverage->at(floor((double)pair.First.Position / (double)config->CoverageBinSize)).Downstream.translocations++;
            Trans->Ref2->Coverage->at(floor((double)pair.Second.Position / (double)config->CoverageBinSize)).Upstream.translocations++;
        }
    }
    delete HTpairs;
    
    if (THpairs->size() > 0) {
        Trans->THpairs->reserve(THpairs->size());
        Trans->THpairs->insert(Trans->THpairs->end(), THpairs->begin(), THpairs->end());
        for (Pair pair : *THpairs) {
            Trans->Ref1->Coverage->at(floor((double)pair.First.Position / (double)config->CoverageBinSize)).Upstream.translocations++;
            Trans->Ref2->Coverage->at(floor((double)pair.Second.Position / (double)config->CoverageBinSize)).Downstream.translocations++;
        }
    }
    delete THpairs;
}

void PairingTranslocation::ProcesPair(ReadDetails& FirstRead, ReadDetails& SecondRead) {
    Pair pair;
    pair.First = FirstRead;
    pair.Second = SecondRead;
    if (pair.First.ForwardRead == pair.First.Strand) { // TH or TT
        if (pair.Second.ForwardRead == pair.Second.Strand) {
            if (TTpairs->size() >= TTpairs->capacity()) this->PauseThread();
            TTpairs->push_back(pair);
        } else {
            if (THpairs->size() >= THpairs->capacity()) this->PauseThread();
            THpairs->push_back(pair);
        }
    } else {
        if (pair.Second.ForwardRead == pair.Second.Strand) {
            if (HTpairs->size() >= HTpairs->capacity()) this->PauseThread();
            HTpairs->push_back(pair);
        } else {
            if (HHpairs->size() >= HHpairs->capacity()) this->PauseThread();
            HHpairs->push_back(pair);
        }
    }
}

void PairingTranslocation::InternalDoWhilePauze() {
    if (THpairs->size() >= THpairs->capacity()) THpairs->reserve(10000);
    if (HTpairs->size() >= HTpairs->capacity()) HTpairs->reserve(10000);
    if (TTpairs->size() >= TTpairs->capacity()) TTpairs->reserve(10000);
    if (HHpairs->size() >= HHpairs->capacity()) HHpairs->reserve(10000);
}