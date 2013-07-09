/* 
 * File:   SortSingleReads.cpp
 * Author: peter
 * 
 * Created on January 11, 2013, 12:23 PM
 */

#include <algorithm>
#include <iostream>
#include "SortSingleReads.h"

SortSingleReads::SortSingleReads(ReferenceDataStorage* Ref, int type, Config* config) : Task(config) {
    this->Ref = Ref;
    this->type = type;
    switch (type) {
        case TypeForward:
            this->TargetSingleReads = Ref->forwardreads;
            this->Bins = Ref->forwardreadsBins;
            TaskName = "Collect and sort Forward Reads for ref: " + Ref->Name;
            break;
        case TypeReverse:
            this->TargetSingleReads = Ref->reversereads;
            this->Bins = Ref->reversereadsBins;
            TaskName = "Collect and sort Reverse Reads for ref: " + Ref->Name;
            break;
        case TypeForwardLowQuality:
            this->TargetSingleReads = Ref->forwardreadsLowQuality;
            this->Bins = Ref->forwardreadsLowQualityBins;
            TaskName = "Collect and sort Forward LowQuality Reads for ref: " + Ref->Name;
            break;
        case TypeReverseLowQuality:
            this->TargetSingleReads = Ref->reversereadsLowQuality;
            this->Bins = Ref->reversereadsLowQualityBins;
            TaskName = "Collect and sort Reverse LowQuality Reads for ref: " + Ref->Name;
            break;
    }
    Ready = true;
}

SortSingleReads::~SortSingleReads() {
}

void SortSingleReads::InternalDoBeforeRun() {
    for (vector<Singleread>* singleReads : *Bins) {
        this->TargetSingleReads->reserve(singleReads->size());
    }
}

void SortSingleReads::InternalRun() {
    for (vector<Singleread>* singleReads : *Bins) {
        //this->TargetSingleReads->reserve(singleReads->size());
        this->TargetSingleReads->insert(this->TargetSingleReads->end(), singleReads->begin(), singleReads->end());
        //delete singleReads;
    }
    sort(TargetSingleReads->begin(), TargetSingleReads->end(), SortSinglereadOnID());
}

void SortSingleReads::InternalFinish() {
    for (vector<Singleread>* singleReads : *Bins) {
        delete singleReads;
    }
}