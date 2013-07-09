/* 
 * File:   ClearMemorySinglereads.cpp
 * Author: peter
 * 
 * Created on January 17, 2013, 1:07 PM
 */

#include "ClearMemorySinglereads.h"
#include <algorithm>
#include <iostream>

ClearMemorySinglereads::ClearMemorySinglereads(ReferenceDataStorage* Ref, int type, Config* config) : Task(config) {
    this->Ref = Ref;
    this->type = type;
    switch (type) {
        case ClearMemorySinglereads::TypeForward:
            TaskName = "Clearing Forward Reads for ref: " + Ref->Name;
            SingleReads = Ref->forwardreads;
            break;
        case ClearMemorySinglereads::TypeReverse:
            TaskName = "Clearing Reverse Reads for ref: " + Ref->Name;
            SingleReads = Ref->reversereads;
            break;
        case ClearMemorySinglereads::TypeForwardLowquality:
            TaskName = "Clearing Forward Lowquality Reads for ref: " + Ref->Name;
            SingleReads = Ref->forwardreadsLowQuality;
            break;
        case ClearMemorySinglereads::TypeReverseLowquality:
            TaskName = "Clearing Reverse Lowquality Reads for ref: " + Ref->Name;
            SingleReads = Ref->reversereadsLowQuality;
            break;
    }
    Ready = true;
}

ClearMemorySinglereads::~ClearMemorySinglereads() {
}

void ClearMemorySinglereads::Reset() {
    if (not Running) {
        switch (type) {
            case ClearMemorySinglereads::TypeForward:
                if (Ref->forwardreadsPaired < config->minClearingPairs) return;
                break;
            case ClearMemorySinglereads::TypeReverse:
                if (Ref->reversereadsPaired < config->minClearingPairs) return;
                break;
            case ClearMemorySinglereads::TypeForwardLowquality:
                if (Ref->forwardreadsLowQualityPaired < config->minClearingPairs) return;
                break;
            case ClearMemorySinglereads::TypeReverseLowquality:
                if (Ref->reversereadsLowQualityPaired < config->minClearingPairs) return;
                break;
        }
        Running = false;
        Done = false;
    }
}

void ClearMemorySinglereads::InternalFinish() {
    SingleReads->shrink_to_fit();
    switch (type) {
        case ClearMemorySinglereads::TypeForward:
            Ref->forwardreadsPaired = 0;
            break;
        case ClearMemorySinglereads::TypeReverse:
            Ref->reversereadsPaired = 0;
            break;
        case ClearMemorySinglereads::TypeForwardLowquality:
            Ref->forwardreadsLowQualityPaired = 0;
            break;
        case ClearMemorySinglereads::TypeReverseLowquality:
            Ref->reversereadsLowQualityPaired = 0;
            break;
    }
}

void ClearMemorySinglereads::InternalRun() {
    if (SingleReads->size() <= 0) {
        //Done = true;
        return;
    }
    
    long int Total = SingleReads->size();
    
    vector<Singleread>::iterator ReadsIt;
    sort(SingleReads->begin(), SingleReads->end(), SortSinglereadOnID());
    for (ReadsIt = SingleReads->end(); ReadsIt != SingleReads->begin(); ReadsIt--) {
        if (ReadsIt->ID != 0){
            ReadsIt++;
            break;
        }
    }
    if (ReadsIt->ID <= 0) {
        SingleReads->erase(ReadsIt, SingleReads->end());
    }
    
    Cleared = Total - SingleReads->size();
    return;
}