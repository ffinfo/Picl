/* 
 * File:   ReadPairedReads.cpp
 * Author: peter
 * 
 * Created on January 29, 2013, 4:08 PM
 */

#include "ReadPairedReads.h"

ReadPairedReads::ReadPairedReads(ReferenceDataStorage* Ref, long int begin, long int end, Config* config) : Task(config) {
    this->Ref = Ref;
    this->ForwardHighQuality = new vector<Singleread>;
    this->ForwardLowQuality = new vector<Singleread>;
    this->ReverseHighQuality = new vector<Singleread>;
    this->ReverseLowQuality = new vector<Singleread>;
    Ref->forwardreadsBins->push_back(ForwardHighQuality);
    Ref->forwardreadsLowQualityBins->push_back(ForwardLowQuality);
    Ref->reversereadsBins->push_back(ReverseHighQuality);
    Ref->reversereadsLowQualityBins->push_back(ReverseLowQuality);

    this->begin = begin;
    this->end = end;
    
    ostringstream logBuffer;
    logBuffer << "Reading Bin for ref: " << Ref->Name << "   Bin: " << begin << "-" << end;
    TaskName = logBuffer.str();
    
    ReadingTask = true;
    Ready = true;
}

ReadPairedReads::~ReadPairedReads() {
}

void ReadPairedReads::InternalFinish() {
    ForwardLowQuality->shrink_to_fit();
    ForwardHighQuality->shrink_to_fit();
    ReverseLowQuality->shrink_to_fit();
    ReverseHighQuality->shrink_to_fit();
    Ref->Stats.Read.Forward += ForwardHighQualityCount;
    Ref->Stats.Read.ForwardLowQuality += ForwardLowQualityCount;
    Ref->Stats.Read.Reverse += ReverseHighQualityCount;
    Ref->Stats.Read.ReverseLowQuality += ReverseLowQualityCount;
    Ref->CompletedTasks++;
}

void ReadPairedReads::InternalRun() {
    int qualityTreshold = config->qualityTreshold;
    
    BamTools::BamAlignment alignment;
    BamTools::BamReader BamReader;
    BamReader.Open(config->PairedBam);
    if (config->PairedIndex.empty()) {
        BamReader.LocateIndex();
    } else {
        BamReader.OpenIndex(config->PairedIndex);
    }
    bool ForwardOriInverted = config->ForwardOriInverted;
    bool ReverseOriInverted = config->ReverseOriInverted;
    
    BamReader.SetRegion(BamReader.GetReferenceID(Ref->Name), begin, BamReader.GetReferenceID(Ref->Name), end);
    
    while (BamReader.GetNextAlignment(alignment)) {
        if (!alignment.IsPaired()) {
            NotPaired++;
            continue;
        }
        if (!alignment.IsMateMapped()) {
            MateNotMapped++;
            continue;
        }
        if (!alignment.IsPrimaryAlignment()) {
            continue;
        }
        if (config ->SkipDuplicates && alignment.IsDuplicate()) {
            Duplicates++;
            continue;
        }
        Singleread read;
        string RG;
        if (alignment.GetTag("RG", RG)) {
            read.ID = config->readNameConverter.ConvertNameToInt(alignment.Name, RG);
        } else {
            read.ID = config->readNameConverter.ConvertNameToInt(alignment.Name);
        }
        
        if (config->NameType == 0) read.ID_string = alignment.Name;
        
        read.Details.Lenght = alignment.Length;
        read.Details.Position = alignment.Position;
        BWA_TagData tagdata = ReadBWA_TagData(alignment);
        if (alignment.IsFirstMate()) {
            read.Details.ForwardRead = true;
            if (ForwardOriInverted) read.Details.Strand = alignment.IsReverseStrand();
            else read.Details.Strand = !alignment.IsReverseStrand();
            if (alignment.MapQuality < qualityTreshold || tagdata.X0 > config->maxX0 || tagdata.X1 > config->maxX1 || tagdata.XO > config->maxXO || tagdata.XM > config->maxXM) {
                if (ForwardLowQuality->size() >= ForwardLowQuality->capacity()) this->PauseThread();
                ForwardLowQuality->push_back(read);
            } else {
                if (ForwardHighQuality->size() >= ForwardHighQuality->capacity()) this->PauseThread();
                ForwardHighQuality->push_back(read);
            }
        } else {
            read.Details.ForwardRead = false;
            if (ReverseOriInverted) read.Details.Strand = alignment.IsReverseStrand();
            else read.Details.Strand = !alignment.IsReverseStrand();
            if (alignment.MapQuality < qualityTreshold || tagdata.X0 > config->maxX0 || tagdata.X1 > config->maxX1 || tagdata.XO > config->maxXO || tagdata.XM > config->maxXM) {
                if (ReverseLowQuality->size() >= ReverseLowQuality->capacity()) this->PauseThread();
                ReverseLowQuality->push_back(read);
            } else {
                if (ReverseHighQuality->size() >= ReverseHighQuality->capacity()) this->PauseThread();
                ReverseHighQuality->push_back(read);
            }
        }
    }
    ForwardHighQualityCount = ForwardHighQuality->size();
    ReverseHighQualityCount = ReverseHighQuality->size();
    ForwardLowQualityCount = ForwardLowQuality->size();
    ReverseLowQualityCount = ReverseLowQuality->size();
    return;
}

void ReadPairedReads::InternalDoWhilePauze() {
    if (ForwardLowQuality->size() >= ForwardLowQuality->capacity()) ForwardLowQuality->reserve(10000);
    if (ForwardHighQuality->size() >= ForwardHighQuality->capacity()) ForwardHighQuality->reserve(10000);
    if (ReverseLowQuality->size() >= ReverseLowQuality->capacity()) ReverseLowQuality->reserve(10000);
    if (ReverseHighQuality->size() >= ReverseHighQuality->capacity()) ReverseHighQuality->reserve(10000);
}

BWA_TagData ReadPairedReads::ReadBWA_TagData(BamTools::BamAlignment &alignment) {
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