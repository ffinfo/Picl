/* 
 * File:   PairingLowQuality.cpp
 * Author: peter
 * 
 * Created on January 17, 2013, 12:11 PM
 */

#include "PairingLowQuality.h"

PairingLowQuality::PairingLowQuality(ReferenceDataStorage* Ref, Config* config) : Pairing(config) {
    type = PairingLowQuality::TypeNormal;
    this->Ref = Ref;
    this->FirstReads = Ref->forwardreadsLowQuality;
    this->SecondReads = Ref->reversereadsLowQuality;
    TargetPairs = new vector<LowQualityPair>;
    TaskName = "Pairing LowQuality for ref: " + Ref->Name;
    Ready = true;
}

PairingLowQuality::PairingLowQuality(TranslocationDataStorage* Trans, int type, Config* config) : Pairing(config) {
    this->Trans = Trans;
    this->config = config;
    this->type = type;
    TargetPairs = new vector<LowQualityPair>;
    switch (type) {
        case PairingLowQuality::TypeTransFR:
            this->FirstReads = Trans->Ref1->forwardreadsLowQuality;
            this->SecondReads = Trans->Ref2->reversereadsLowQuality;
            TaskName = "Pairing LowQuality for: forward:" + Trans->Ref1->Name + "-reverse:" + Trans->Ref2->Name;
            break;
        case PairingLowQuality::TypeTransRF:
            this->FirstReads = Trans->Ref2->forwardreadsLowQuality;
            this->SecondReads = Trans->Ref1->reversereadsLowQuality;
            TaskName = "Pairing LowQuality for: forward:" + Trans->Ref2->Name + "-reverse:" + Trans->Ref1->Name;
            break;
    }
    Ready = true;
}

PairingLowQuality::~PairingLowQuality() {
}

void PairingLowQuality::InternalFinish() {
    switch (type) {
        case PairingLowQuality::TypeNormal:
            Ref->forwardreadsLowQualityPaired =+ Paired;
            Ref->reversereadsLowQualityPaired =+ Paired;
            break;
        case PairingLowQuality::TypeTransFR:
            break;
        case PairingLowQuality::TypeTransRF:
            break;
    }
    delete TargetPairs;
}

void PairingLowQuality::ProcesPair(ReadDetails& FirstRead, ReadDetails& SecondRead) {
    //not used yet
}