/* 
 * File:   Insersize.cpp
 * Author: peter
 * 
 * Created on January 17, 2013, 1:40 PM
 */

#include <algorithm>
#include "Insersize.h"

Insersize::Insersize(ReferenceDataStorage* Ref, Config* config) : Task(config) {
    this->Ref = Ref;
    this->THpairs = Ref->THpairs;
    this->Normal = Ref->Normal;
    this->Deletions = Ref->Deletions;
    this->Insertions = Ref->Insertions;
    
    TaskName = "Insertsize for ref: " + Ref->Name;
    Ready = true;
}

Insersize::~Insersize() {
}

void Insersize::InternalFinish() {
    Insertions->shrink_to_fit();
    Deletions->shrink_to_fit();
    Stats.curve.shrink_to_fit();
    Ref->Stats.Insertsize = Stats;
    config->AddInsertsizeStats(Ref->Name, Stats);
    
    fstream file;
    string coverageFile = config->coverageDir + Ref->Name + ".cov";
    file.open(coverageFile, ios::trunc | ios::binary | ios::out);
    
    file.write((char*) & config->CoverageBinSize, sizeof(unsigned int));
    //Ref->Coverage->reserve(CoverageReads->size());
    for (CoverageFileRead read : *Ref->Coverage) {
        file.write((char*) & read, sizeof(CoverageFileRead));
    }
    
    delete THpairs;
    if (config->TransSelect.compare("only") == 0) {
        delete Ref->HHpairs;
        delete Ref->TTpairs;
        delete Ref->HTpairs;
        delete Ref->Deletions;
        delete Ref->Insertions;
    }
    //delete CoverageReads;
}

void Insersize::InternalRun() {
    if (THpairs->size() <= 0) {
        //Done = true;
        return;
    }
    //int bins = ceil((double)Ref->Length / (double)config->CoverageBinSize);
    //cout << bins << endl;
    //CoverageReads->resize(bins);
    
    double InsertionCutoffPvalue = config->InsertionCutoffPvalue;
    double DeletionCutoffPvalue = config->DeletionCutoffPvalue;
    //long unsigned int totalreads = 0;
    long unsigned int cumulative = 0;
    //long unsigned int insertsizeCurve[config->MaxDistance];
    
    /*for (InsertsizePair insertsizePair : *THpairs) { // Coverage track
        //Upstream
        int bin = floor((double)insertsizePair.Details.First.Position / (double)config->CoverageBinSize);
        CoverageFileRead* coverageRead = &Ref->Coverage->at(bin);
        coverageRead->totalUpStream++;
        coverageRead->totalInsertsizeUpStream += insertsizePair.Insertsize;
        if (insertsizePair.Insertsize > coverageRead->FileRead.Upstream.maxInsertSize) 
            coverageRead->FileRead.Upstream.maxInsertSize = insertsizePair.Insertsize;
        if (insertsizePair.Insertsize < coverageRead->FileRead.Upstream.minInsterSize or coverageRead->FileRead.Upstream.minInsterSize == 0) 
            coverageRead->FileRead.Upstream.minInsterSize = insertsizePair.Insertsize;
        //Downstream
        bin = floor((double)insertsizePair.Details.Second.Position / (double)config->CoverageBinSize);
        coverageRead = &CoverageReads->at(bin);
        coverageRead->totalDownStream++;
        coverageRead->totalInsertsizeDownStream += insertsizePair.Insertsize;
        if (insertsizePair.Insertsize > coverageRead->FileRead.Downstream.maxInsertSize) 
            coverageRead->FileRead.Downstream.maxInsertSize = insertsizePair.Insertsize;
        if (insertsizePair.Insertsize < coverageRead->FileRead.Downstream.minInsterSize or coverageRead->FileRead.Downstream.minInsterSize == 0) 
            coverageRead->FileRead.Downstream.minInsterSize = insertsizePair.Insertsize;
    }
    
    for (CoverageRead &read : *CoverageReads) {
        read.FileRead.Upstream.medianInsertSize = round((double)read.totalInsertsizeUpStream / (double)read.totalUpStream);
        read.FileRead.Downstream.medianInsertSize = round((double)read.totalInsertsizeDownStream / (double)read.totalDownStream);
    }*/
    
    sort(THpairs->begin(), THpairs->end(), SortOnInsertsize());
    
    //vector<InsertsizePair>::iterator THpairsIt = THpairs->end() - 1;
    //long int it = 0;
        
    for (InsertsizePair insertsizePair : *THpairs) {
        Stats.CurveTotal++;
        if (insertsizePair.Insertsize > config->MaxDistance) break;
    }
    
    for (unsigned int it = 0; it < config->MaxDistance; it++) {
        if (Stats.curve.size() >= Stats.curve.capacity()) this->PauseThread();
        Stats.curve.push_back(0);
    }
    
    unsigned long int count = 1;
    unsigned int insertsize = 0;
    for (InsertsizePair insertsizePair : *THpairs) {
        if (insertsizePair.Insertsize > config->MaxDistance) break;
        ++Stats.curve[insertsizePair.Insertsize - 1];
        
        if (insertsize == insertsizePair.Insertsize) {
            count++;
        } else {
            cumulative += count;
            double pValue = (double)cumulative / (double)Stats.CurveTotal;
            
            if (pValue <= InsertionCutoffPvalue) {
                Stats.InsertionCutoff = insertsize;
            } else if (pValue >= DeletionCutoffPvalue && Stats.DeletionCutoff == 0) {
                Stats.DeletionCutoff = insertsize;
            } else if (pValue >= 0.5 && Stats.Median == 0) {
                Stats.Median = insertsize;
            }
            
            insertsize = insertsizePair.Insertsize;
            count = 1;
        }
    }
    
    if (config->DeletionCutoff > 0) Stats.DeletionCutoff = config->DeletionCutoff;
    if (config->InsertionCutoff > 0) Stats.InsertionCutoff = config->InsertionCutoff;
    
    ofstream file;
    file.open(config->insertsizeDir + Ref->Name + ".insertsize", ios::trunc);
    for (unsigned int it = 0; it < config->MaxDistance; it++) {
        if (Stats.curve[it] > Stats.max_curve) Stats.max_curve = Stats.curve[it];
        file << (it + 1) << "\t" << Stats.curve[it] << endl;
    }
    file.close();
    
    //THpairsIt = THpairs->begin();
    for (InsertsizePair insertsizePair : *THpairs) {
        int bin = floor((double)insertsizePair.Details.First.Position / (double)config->CoverageBinSize);
        CoverageFileRead* coverageRead1 = &Ref->Coverage->at(bin);
        int bin2 = floor((double)insertsizePair.Details.Second.Position / (double)config->CoverageBinSize);
        CoverageFileRead* coverageRead2 = &Ref->Coverage->at(bin2);
        if (insertsizePair.Insertsize <= Stats.InsertionCutoff) {
            //if (THpairsIt->Insertsize >= (THpairsIt->Details.First.Lenght + THpairsIt->Details.Second.Lenght)) {
                if (Insertions->size() >= Insertions->capacity()) this->PauseThread();
                Insertions->push_back(insertsizePair.Details);
                coverageRead1->Upstream.insertions++;
                coverageRead2->Downstream.insertions++;
            //} else {
                //OverlappingPairs->push_back(THpairsIt->Details);
            //}
        } else if (insertsizePair.Insertsize >= Stats.DeletionCutoff) {
            if (Deletions->size() >= Deletions->capacity()) this->PauseThread();
            Deletions->push_back(insertsizePair.Details);
            coverageRead1->Upstream.deletions++;
            coverageRead2->Downstream.deletions++;
        } else {
            //Normal->push_back(THpairsIt->Details); // Only needed for hetro/homozygozis, not build in yet
            coverageRead1->Upstream.concortdantPairs++;
            coverageRead2->Downstream.concortdantPairs++;
            Stats.Normal++;
        }
        //++THpairsIt;
    }
    
    Stats.Deletions = Deletions->size();
    Stats.Insertions = Insertions->size();
    
    return;
}

void Insersize::InternalDoWhilePauze() {
    if (Insertions->size() >= Insertions->capacity()) Insertions->reserve(10000);
    if (Deletions->size() >= Deletions->capacity()) Deletions->reserve(10000);
    if (Stats.curve.size() >= Stats.curve.capacity()) Stats.curve.reserve(10000);
}