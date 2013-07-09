/* 
 * File:   PairingHighLowQuality.cpp
 * Author: peter
 * 
 * Created on January 17, 2013, 11:12 AM
 */

#include "PairingHighLowQuality.h"

PairingHighLowQuality::PairingHighLowQuality(ReferenceDataStorage* Ref, int type, Config* config) : Pairing(config) {
    this->Ref = Ref;
    this->type = type;
    TargetPairs = new vector<SingleLowQualityPair>;
    switch (type) {
        case PairingHighLowQuality::TypeHighQualityForward:
            this->FirstReads = Ref->forwardreads;
            this->SecondReads = Ref->reversereadsLowQuality;
            TaskName = "Pairing Forward High Quality to Reverse Low Quality for ref: " + Ref->Name;
            break;
        case PairingHighLowQuality::TypeHighQualityReverse:
            this->FirstReads = Ref->reversereads;
            this->SecondReads = Ref->forwardreadsLowQuality;
            TaskName = "Pairing Reverse High Quality to Forward Low Quality for ref: " + Ref->Name;
            break;
    }
    Ready = true;
}

PairingHighLowQuality::PairingHighLowQuality(TranslocationDataStorage* Trans, int type, Config* config) : Pairing(config) {
    this->Trans = Trans;
    this->type = type;
    this->config = config;
    TargetPairs = new vector<SingleLowQualityPair>;
    switch (type) {
        case PairingHighLowQuality::TypeTransHighQualityForward1:
            this->FirstReads = Trans->Ref1->forwardreads;
            this->SecondReads = Trans->Ref2->reversereadsLowQuality;
            TaskName = "Pairing Forward-" + Trans->Ref1->Name + " to ReverseLowQuality-" + Trans->Ref2->Name;
            break;
        case PairingHighLowQuality::TypeTransHighQualityReverse1:
            this->FirstReads = Trans->Ref1->reversereads;
            this->SecondReads = Trans->Ref2->forwardreadsLowQuality;
            TaskName = "Pairing ForwardLowQuality-" + Trans->Ref2->Name + " to Reverse-" + Trans->Ref1->Name;
            break;
        case PairingHighLowQuality::TypeTransHighQualityForward2:
            this->FirstReads = Trans->Ref2->forwardreads;
            this->SecondReads = Trans->Ref1->reversereadsLowQuality;
            TaskName = "Pairing Forward-" + Trans->Ref2->Name + " to ReverseLowQuality-" + Trans->Ref1->Name;
            break;
        case PairingHighLowQuality::TypeTransHighQualityReverse2:
            this->FirstReads = Trans->Ref2->reversereads;
            this->SecondReads = Trans->Ref1->forwardreadsLowQuality;
            TaskName = "Pairing ForwardLowQuality-" + Trans->Ref1->Name + " to Reverse-" + Trans->Ref2->Name;
            break;
    }
    Ready = true;
}

PairingHighLowQuality::~PairingHighLowQuality() {
}

void PairingHighLowQuality::InternalFinish() {
    delete TargetPairs;
}

void PairingHighLowQuality::InternalDoWhilePauze(void) {
}

void PairingHighLowQuality::ProcesPair(ReadDetails& HighQualityRead, ReadDetails& LowQualityRead) {
    //Not used yet
}