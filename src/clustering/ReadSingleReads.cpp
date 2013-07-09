/* 
 * File:   ReadSingleReads.cpp
 * Author: peter
 * 
 * Created on January 8, 2013, 1:45 PM
 */

#include <api/BamReader.h>
#include <sstream>
#include "ReadSingleReads.h"

ReadSingleReads::ReadSingleReads(ReferenceDataStorage* Ref, bool forward, long int begin, long int end, Config* config) : Task(config) {
    this->Ref = Ref;
    this->begin = begin;
    this->end = end;
    this->forward = forward;
    HighQuality = new vector<Singleread>;
    LowQuality = new vector<Singleread>;
    ostringstream NameBuffer;
    if (forward) {
        Ref->forwardreadsBins->push_back(HighQuality);
        Ref->forwardreadsLowQualityBins->push_back(LowQuality);
        NameBuffer << "Reading forward Bin for ref: " << Ref->Name << "   Bin: " << begin << "-" << end;
    } else {
        Ref->reversereadsBins->push_back(HighQuality);
        Ref->reversereadsLowQualityBins->push_back(LowQuality);
        NameBuffer << "Reading reverse Bin for ref: " << Ref->Name << "   Bin: " << begin << "-" << end;
    }
    TaskName = NameBuffer.str();
    ReadingTask = true;
    Ready = true;
}

ReadSingleReads::~ReadSingleReads() {
}

void ReadSingleReads::InternalFinish() {
    LowQuality->shrink_to_fit();
    HighQuality->shrink_to_fit();
    if (forward) {
        Ref->Stats.Read.Forward = HighQualityCount;
        Ref->Stats.Read.ForwardLowQuality = LowQualityCount;
        Ref->ForwardReadingDone = true;
    } else {
        Ref->Stats.Read.Reverse = HighQualityCount;
        Ref->Stats.Read.ReverseLowQuality = LowQualityCount;
        Ref->ReverseReadingDone = true;
    }
    Ref->CompletedTasks++;
}

void ReadSingleReads::InternalRun() {
    bool OriInverted;
    uint32_t maxX0 = config->maxX0, maxX1 = config->maxX1, maxXO = config->maxXO, maxXM = config->maxXM, qualityTreshold = config->qualityTreshold;
    BamTools::BamAlignment alignment;
    BamTools::BamReader BamReader;
    if (forward) {
        BamReader.Open(config->ForwardBam);
        BamReader.OpenIndex(config->ForwardIndex);
        OriInverted = config->ForwardOriInverted;
    } else {
        BamReader.Open(config->ReverseBam);
        BamReader.OpenIndex(config->ReverseIndex);
        OriInverted = config->ReverseOriInverted;
    }
    BamReader.SetRegion(BamReader.GetReferenceID(Ref->Name), begin, BamReader.GetReferenceID(Ref->Name), end);
    
    while (BamReader.GetNextAlignment(alignment)) {        
        Singleread read;
        string RG;
        if (alignment.GetTag("RG", RG)) {
            read.ID = config->readNameConverter.ConvertNameToInt(alignment.Name, RG);
        } else {
            read.ID = config->readNameConverter.ConvertNameToInt(alignment.Name);
        }
        read.Details.Lenght = alignment.Length;
        read.Details.Position = alignment.Position;
        if (OriInverted) read.Details.Strand = alignment.IsReverseStrand(); 
        else read.Details.Strand = !alignment.IsReverseStrand();
        if (forward) read.Details.ForwardRead = true; 
            else read.Details.ForwardRead = false;
        
        BWA_TagData tagdata = ReadBWA_TagData(alignment);
        if (alignment.MapQuality < qualityTreshold || tagdata.X0 > maxX0 || tagdata.X1 > maxX1 || tagdata.XO > maxXO || tagdata.XM > maxXM) {
            if (LowQuality->size() >= LowQuality->capacity()) this->PauseThread();
            LowQuality->push_back(read);
        } else {
            if (HighQuality->size() >= HighQuality->capacity()) this->PauseThread();
            HighQuality->push_back(read);
        }
    }
    HighQualityCount = HighQuality->size();
    LowQualityCount = LowQuality->size();
    
    return;
}

void ReadSingleReads::InternalDoWhilePauze() {
    if (LowQuality->size() >= LowQuality->capacity()) LowQuality->reserve(10000);
    if (HighQuality->size() >= HighQuality->capacity()) HighQuality->reserve(10000);
}

BWA_TagData ReadSingleReads::ReadBWA_TagData(BamTools::BamAlignment &alignment) {
    BWA_TagData tagdata;
    
    if (!alignment.GetTag("X0", tagdata.X0)) {
        tagdata.X0 = 1;
    }
    if (!alignment.GetTag("X1", tagdata.X1)) {
        tagdata.X1 = 0;
    }
    if (!alignment.GetTag("XG", tagdata.XG)) {
        tagdata.XG = 0;
    }
    if (!alignment.GetTag("XM", tagdata.XM)) {
        tagdata.XM = 0;
    }
    if (!alignment.GetTag("XO", tagdata.XO)) {
        tagdata.XO = 0;
    }
    
    return tagdata;
}