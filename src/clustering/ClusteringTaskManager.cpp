/* 
 * File:   ClusteringTaskManager.cpp
 * Author: peter
 * 
 * Created on February 19, 2013, 12:47 PM
 */

#include "ClusteringTaskManager.h"
#include <math.h>

ClusteringTaskManager::ClusteringTaskManager(vector<TranslocationDataStorage*>* Translocations, Config* config) : TaskManager(config) {
    this->config = config;
    this->Translocations = Translocations;
    
    CuttoffCalcTask = new CuttoffCalc(config);
    //AllTasks.push_back(CuttoffCalcTask);
}

ClusteringTaskManager::~ClusteringTaskManager() {
}

void ClusteringTaskManager::ResetClearTasks() {
    for (ClearMemorySinglereads* clearTask : ClearTasks) {
        clearTask->Reset();
    }
}

bool ClusteringTaskManager::InternalFinished() {
    if (Finished) {
        if (config->minClearingPairs > 1) {
            config->minClearingPairs = 1;
            config->Log("All tasks done, last clear memory");
            ResetClearTasks();
            Finished = false;
            return false;
        } else {
            return true;
        }
    }
    return false;
}

void ClusteringTaskManager::InternalCheckTasks() {
    ResetClearTasks();
}

void ClusteringTaskManager::AddRef(ReferenceDataStorage* ref) {
    RefTasks* refTasks = new RefTasks;
    refsTasks[ref] = refTasks;
    
    //Reading tasks
    if (config->UsePairedBam) {
        refTasks->TaskReadPaireds = new vector<ReadPairedReads*>;
    }
    else {
        refTasks->TaskReadForward = new vector<ReadSingleReads*>;
        refTasks->TaskReadReverse = new vector<ReadSingleReads*>;
    }
    long int BinSize = config->readBinSize;
    if (BinSize <= 0) BinSize = ref->Length;
    int bins = ceil((double)ref->Length / (double)BinSize);
    for (int it = 0; it < bins; it++) {
        int begin = it * BinSize;
        int end = it * BinSize + BinSize - 1;
        if (end > ref->Length) end = ref->Length;
        if (config->UsePairedBam) {
            ReadPairedReads* p = new ReadPairedReads(ref, begin, end, config);
            refTasks->TaskReadPaireds->push_back(p);
            refTasks->Tasks.push_back(p);
        } else {
            ReadSingleReads* p = new ReadSingleReads(ref, true, begin, end, config); // Forward
            refTasks->TaskReadForward->push_back(p);
            refTasks->Tasks.push_back(p);
            p = new ReadSingleReads(ref, false, begin, end, config); // Reverse
            refTasks->TaskReadReverse->push_back(p);
            refTasks->Tasks.push_back(p);
        }
    }
        
    // Sorting Tasks
    refTasks->TaskSortForward = new SortSingleReads(ref, SortSingleReads::TypeForward, config);
    refTasks->Tasks.push_back(refTasks->TaskSortForward);
    refTasks->TaskSortReverse = new SortSingleReads(ref, SortSingleReads::TypeReverse, config);
    refTasks->Tasks.push_back(refTasks->TaskSortReverse);
    refTasks->TaskSortForwardLowquality = new SortSingleReads(ref, SortSingleReads::TypeForwardLowQuality, config);
    refTasks->Tasks.push_back(refTasks->TaskSortForwardLowquality);
    refTasks->TaskSortReverseLowquality = new SortSingleReads(ref, SortSingleReads::TypeReverseLowQuality, config);
    refTasks->Tasks.push_back(refTasks->TaskSortReverseLowquality);
    if (config->UsePairedBam) {
        for (ReadPairedReads* readPairedReads : *refTasks->TaskReadPaireds) {
            refTasks->TaskSortForward->MustDoneBefore.push_back(readPairedReads);
            refTasks->TaskSortReverse->MustDoneBefore.push_back(readPairedReads);
            refTasks->TaskSortForwardLowquality->MustDoneBefore.push_back(readPairedReads);
            refTasks->TaskSortReverseLowquality->MustDoneBefore.push_back(readPairedReads);
        }
    } else {
        for (ReadSingleReads* readForward : *refTasks->TaskReadForward) {
            refTasks->TaskSortForward->MustDoneBefore.push_back(readForward);
            refTasks->TaskSortForwardLowquality->MustDoneBefore.push_back(readForward);
        }
        for (ReadSingleReads* readReverse : *refTasks->TaskReadReverse) {
            refTasks->TaskSortReverse->MustDoneBefore.push_back(readReverse);
            refTasks->TaskSortReverseLowquality->MustDoneBefore.push_back(readReverse);
        }
    }
    
    //Pairing Tasks
    refTasks->TaskPairing = new Pairing(ref, config);
    refTasks->TaskPairing->MustDoneBefore.push_back(refTasks->TaskSortForward);
    refTasks->TaskPairing->MustDoneBefore.push_back(refTasks->TaskSortReverse);
    refTasks->Tasks.push_back(refTasks->TaskPairing);
    refTasks->TaskPairingForwardHighLowQuality = new PairingHighLowQuality(ref, PairingHighLowQuality::TypeHighQualityForward, config);
    refTasks->TaskPairingForwardHighLowQuality->MustDoneBefore.push_back(refTasks->TaskSortForward);
    refTasks->TaskPairingForwardHighLowQuality->MustDoneBefore.push_back(refTasks->TaskSortReverseLowquality);
    refTasks->Tasks.push_back(refTasks->TaskPairingForwardHighLowQuality);
    refTasks->TaskPairingReverseHighLowQuality = new PairingHighLowQuality(ref, PairingHighLowQuality::TypeHighQualityReverse, config);
    refTasks->TaskPairingReverseHighLowQuality->MustDoneBefore.push_back(refTasks->TaskSortForwardLowquality);
    refTasks->TaskPairingReverseHighLowQuality->MustDoneBefore.push_back(refTasks->TaskSortReverse);
    refTasks->Tasks.push_back(refTasks->TaskPairingReverseHighLowQuality);
    refTasks->TaskPairingLowQuality = new PairingLowQuality(ref, config);
    refTasks->TaskPairingLowQuality->MustDoneBefore.push_back(refTasks->TaskSortForwardLowquality);
    refTasks->TaskPairingLowQuality->MustDoneBefore.push_back(refTasks->TaskSortReverseLowquality);
    refTasks->Tasks.push_back(refTasks->TaskPairingLowQuality);
    
    //Clearing Tasks
    refTasks->TaskClearForward = new ClearMemorySinglereads(ref, ClearMemorySinglereads::TypeForward, config);
    refTasks->TaskClearForward->MustDoneBefore.push_back(refTasks->TaskPairing);
    refTasks->TaskClearForward->MustDoneBefore.push_back(refTasks->TaskPairingForwardHighLowQuality);
    refTasks->Tasks.push_back(refTasks->TaskClearForward);
    ClearTasks.push_back(refTasks->TaskClearForward);
    refTasks->TaskClearReverse = new ClearMemorySinglereads(ref, ClearMemorySinglereads::TypeReverse, config);
    refTasks->TaskClearReverse->MustDoneBefore.push_back(refTasks->TaskPairing);
    refTasks->TaskClearReverse->MustDoneBefore.push_back(refTasks->TaskPairingReverseHighLowQuality);
    refTasks->Tasks.push_back(refTasks->TaskClearReverse);
    ClearTasks.push_back(refTasks->TaskClearReverse);
    refTasks->TaskClearForwardLowQuality = new ClearMemorySinglereads(ref, ClearMemorySinglereads::TypeForwardLowquality, config);
    refTasks->TaskClearForwardLowQuality->MustDoneBefore.push_back(refTasks->TaskPairingLowQuality);
    refTasks->TaskClearForwardLowQuality->MustDoneBefore.push_back(refTasks->TaskPairingReverseHighLowQuality);
    refTasks->Tasks.push_back(refTasks->TaskClearForwardLowQuality);
    ClearTasks.push_back(refTasks->TaskClearForwardLowQuality);
    refTasks->TaskClearReverseLowQuality = new ClearMemorySinglereads(ref, ClearMemorySinglereads::TypeReverseLowquality, config);
    refTasks->TaskClearReverseLowQuality->MustDoneBefore.push_back(refTasks->TaskPairingLowQuality);
    refTasks->TaskClearReverseLowQuality->MustDoneBefore.push_back(refTasks->TaskPairingForwardHighLowQuality);
    refTasks->Tasks.push_back(refTasks->TaskClearReverseLowQuality);
    ClearTasks.push_back(refTasks->TaskClearReverseLowQuality);
    
    //Insersize task
    refTasks->TaskInsersize = new Insersize(ref, config);
    refTasks->TaskInsersize->MustDoneBefore.push_back(refTasks->TaskPairing);
    refTasks->Tasks.push_back(refTasks->TaskInsersize);
    
    //Clustering Tasks
    if (not config->TransSelect.compare("only") == 0) {
        refTasks->TaskClusterDeletions = new Clustering(ref, Clustering::TypeDeletions, config, CuttoffCalcTask);
        refTasks->TaskClusterDeletions->MustDoneBefore.push_back(refTasks->TaskInsersize);
        refTasks->Tasks.push_back(refTasks->TaskClusterDeletions);
        CuttoffCalcTask->MustDoneBefore.push_back(refTasks->TaskClusterDeletions);
        refTasks->TaskClusterInsertions = new Clustering(ref, Clustering::TypeInsertions, config, CuttoffCalcTask);
        refTasks->TaskClusterInsertions->MustDoneBefore.push_back(refTasks->TaskInsersize);
        refTasks->Tasks.push_back(refTasks->TaskClusterInsertions);
        CuttoffCalcTask->MustDoneBefore.push_back(refTasks->TaskClusterInsertions);
        refTasks->TaskClusterTTpairs = new Clustering(ref, Clustering::TypeInversionTT, config, CuttoffCalcTask);
        refTasks->TaskClusterTTpairs->MustDoneBefore.push_back(refTasks->TaskInsersize);
        refTasks->Tasks.push_back(refTasks->TaskClusterTTpairs);
        CuttoffCalcTask->MustDoneBefore.push_back(refTasks->TaskClusterTTpairs);
        refTasks->TaskClusterHHpairs = new Clustering(ref, Clustering::TypeInversionHH, config, CuttoffCalcTask);
        refTasks->TaskClusterHHpairs->MustDoneBefore.push_back(refTasks->TaskInsersize);
        refTasks->Tasks.push_back(refTasks->TaskClusterHHpairs);
        CuttoffCalcTask->MustDoneBefore.push_back(refTasks->TaskClusterHHpairs);
        refTasks->TaskClusterHTpairs = new Clustering(ref, Clustering::TypeEverted, config, CuttoffCalcTask);
        refTasks->TaskClusterHTpairs->MustDoneBefore.push_back(refTasks->TaskInsersize);
        refTasks->Tasks.push_back(refTasks->TaskClusterHTpairs);
        CuttoffCalcTask->MustDoneBefore.push_back(refTasks->TaskClusterHTpairs);
    }
    ref->TotalTasks = refTasks->Tasks.size();
    AllTasks.reserve(refTasks->Tasks.size());
    AllTasks.insert(AllTasks.end(), refTasks->Tasks.begin(), refTasks->Tasks.end());
    
    for (map<ReferenceDataStorage*,RefTasks*>::iterator it = refsTasks.begin(); it != refsTasks.end(); it++) {
        if (it->first == ref) continue;
        AddTranslocation(ref, it->first);
    }
}

void ClusteringTaskManager::AddTranslocation(ReferenceDataStorage* ref1, ReferenceDataStorage* ref2) {
    TranslocationDataStorage* data = new TranslocationDataStorage(ref1, ref2);
    Translocations->push_back(data);
    TransTasks* tasks = new TransTasks;
    TranlocationTasks[data] = tasks;
    
    tasks->PairingFR = new PairingTranslocation(data, PairingTranslocation::TypeForwardReverse, config);
    tasks->PairingFR->MustDoneBefore.push_back(refsTasks[data->Ref1]->TaskInsersize);
    tasks->PairingFR->MustDoneBefore.push_back(refsTasks[data->Ref2]->TaskInsersize);
    tasks->PairingFR->CantRunTogether.push_back(refsTasks[data->Ref1]->TaskClearForward);
    refsTasks[data->Ref1]->TaskClearForward->CantRunTogether.push_back(tasks->PairingFR);
    tasks->PairingFR->CantRunTogether.push_back(refsTasks[data->Ref2]->TaskClearReverse);
    refsTasks[data->Ref2]->TaskClearReverse->CantRunTogether.push_back(tasks->PairingFR);
    tasks->Tasks.push_back(tasks->PairingFR);
    
    tasks->PairingRF = new PairingTranslocation(data, PairingTranslocation::TypeReverseForward, config);
    tasks->PairingRF->MustDoneBefore.push_back(refsTasks[data->Ref1]->TaskInsersize);
    tasks->PairingRF->MustDoneBefore.push_back(refsTasks[data->Ref2]->TaskInsersize);
    tasks->PairingRF->CantRunTogether.push_back(refsTasks[data->Ref2]->TaskClearForward);
    refsTasks[data->Ref2]->TaskClearForward->CantRunTogether.push_back(tasks->PairingRF);
    tasks->PairingRF->CantRunTogether.push_back(refsTasks[data->Ref1]->TaskClearReverse);
    refsTasks[data->Ref1]->TaskClearReverse->CantRunTogether.push_back(tasks->PairingRF);
    tasks->Tasks.push_back(tasks->PairingRF);
    
    tasks->PairingHighLowF1R2 = new PairingHighLowQuality(data, PairingHighLowQuality::TypeTransHighQualityForward1, config);
    tasks->PairingHighLowF1R2->MustDoneBefore.push_back(refsTasks[data->Ref1]->TaskPairing);
    tasks->PairingHighLowF1R2->MustDoneBefore.push_back(refsTasks[data->Ref2]->TaskPairing);
    tasks->PairingHighLowF1R2->CantRunTogether.push_back(refsTasks[data->Ref1]->TaskClearForward);
    refsTasks[data->Ref1]->TaskClearForward->CantRunTogether.push_back(tasks->PairingHighLowF1R2);
    tasks->PairingHighLowF1R2->CantRunTogether.push_back(refsTasks[data->Ref2]->TaskClearReverseLowQuality);
    refsTasks[data->Ref2]->TaskClearReverseLowQuality->CantRunTogether.push_back(tasks->PairingHighLowF1R2);
    tasks->Tasks.push_back(tasks->PairingHighLowF1R2);
    
    tasks->PairingHighLowF2R1 = new PairingHighLowQuality(data, PairingHighLowQuality::TypeTransHighQualityForward2, config);
    tasks->PairingHighLowF2R1->MustDoneBefore.push_back(refsTasks[data->Ref1]->TaskPairing);
    tasks->PairingHighLowF2R1->MustDoneBefore.push_back(refsTasks[data->Ref2]->TaskPairing);
    tasks->PairingHighLowF2R1->CantRunTogether.push_back(refsTasks[data->Ref2]->TaskClearForward);
    refsTasks[data->Ref2]->TaskClearForward->CantRunTogether.push_back(tasks->PairingHighLowF2R1);
    tasks->PairingHighLowF2R1->CantRunTogether.push_back(refsTasks[data->Ref1]->TaskClearReverseLowQuality);
    refsTasks[data->Ref1]->TaskClearReverseLowQuality->CantRunTogether.push_back(tasks->PairingHighLowF2R1);
    tasks->Tasks.push_back(tasks->PairingHighLowF2R1);
    
    tasks->PairingHighLowR1F2 = new PairingHighLowQuality(data, PairingHighLowQuality::TypeTransHighQualityReverse1, config);
    tasks->PairingHighLowR1F2->MustDoneBefore.push_back(refsTasks[data->Ref1]->TaskPairing);
    tasks->PairingHighLowR1F2->MustDoneBefore.push_back(refsTasks[data->Ref2]->TaskPairing);
    tasks->PairingHighLowR1F2->CantRunTogether.push_back(refsTasks[data->Ref1]->TaskClearReverse);
    refsTasks[data->Ref1]->TaskClearReverse->CantRunTogether.push_back(tasks->PairingHighLowR1F2);
    tasks->PairingHighLowR1F2->CantRunTogether.push_back(refsTasks[data->Ref2]->TaskClearForwardLowQuality);
    refsTasks[data->Ref2]->TaskClearForwardLowQuality->CantRunTogether.push_back(tasks->PairingHighLowR1F2);
    tasks->Tasks.push_back(tasks->PairingHighLowR1F2);
    
    tasks->PairingHighLowR2F1 = new PairingHighLowQuality(data, PairingHighLowQuality::TypeTransHighQualityReverse2, config);
    tasks->PairingHighLowR2F1->MustDoneBefore.push_back(refsTasks[data->Ref1]->TaskPairing);
    tasks->PairingHighLowR2F1->MustDoneBefore.push_back(refsTasks[data->Ref2]->TaskPairing);
    tasks->PairingHighLowR2F1->CantRunTogether.push_back(refsTasks[data->Ref2]->TaskClearReverse);
    refsTasks[data->Ref2]->TaskClearReverse->CantRunTogether.push_back(tasks->PairingHighLowR2F1);
    tasks->PairingHighLowR2F1->CantRunTogether.push_back(refsTasks[data->Ref1]->TaskClearForwardLowQuality);
    refsTasks[data->Ref1]->TaskClearForwardLowQuality->CantRunTogether.push_back(tasks->PairingHighLowR2F1);
    tasks->Tasks.push_back(tasks->PairingHighLowR2F1);
    
    tasks->PairingLowFR = new PairingLowQuality(data, PairingLowQuality::TypeTransFR, config);
    tasks->PairingLowFR->MustDoneBefore.push_back(refsTasks[data->Ref1]->TaskPairing);
    tasks->PairingLowFR->MustDoneBefore.push_back(refsTasks[data->Ref2]->TaskPairing);
    tasks->PairingLowFR->CantRunTogether.push_back(refsTasks[data->Ref1]->TaskClearForwardLowQuality);
    refsTasks[data->Ref1]->TaskClearForwardLowQuality->CantRunTogether.push_back(tasks->PairingLowFR);
    tasks->PairingLowFR->CantRunTogether.push_back(refsTasks[data->Ref2]->TaskClearReverseLowQuality);
    refsTasks[data->Ref2]->TaskClearReverseLowQuality->CantRunTogether.push_back(tasks->PairingLowFR);
    tasks->Tasks.push_back(tasks->PairingLowFR);
    
    tasks->PairingLowRF = new PairingLowQuality(data, PairingLowQuality::TypeTransRF, config);
    tasks->PairingLowRF->MustDoneBefore.push_back(refsTasks[data->Ref1]->TaskPairing);
    tasks->PairingLowRF->MustDoneBefore.push_back(refsTasks[data->Ref2]->TaskPairing);
    tasks->PairingLowRF->CantRunTogether.push_back(refsTasks[data->Ref2]->TaskClearForwardLowQuality);
    refsTasks[data->Ref2]->TaskClearForwardLowQuality->CantRunTogether.push_back(tasks->PairingLowRF);
    tasks->PairingLowRF->CantRunTogether.push_back(refsTasks[data->Ref1]->TaskClearReverseLowQuality);
    refsTasks[data->Ref1]->TaskClearReverseLowQuality->CantRunTogether.push_back(tasks->PairingLowRF);
    tasks->Tasks.push_back(tasks->PairingLowRF);
    
    tasks->ClusteringHH = new Clustering(data, Clustering::TypeTransHH, config, CuttoffCalcTask);
    tasks->ClusteringHH->MustDoneBefore.push_back(tasks->PairingFR);
    tasks->ClusteringHH->MustDoneBefore.push_back(tasks->PairingRF);
    tasks->Tasks.push_back(tasks->ClusteringHH);
    CuttoffCalcTask->MustDoneBefore.push_back(tasks->ClusteringHH);
    
    tasks->ClusteringTT = new Clustering(data, Clustering::TypeTransTT, config, CuttoffCalcTask);
    tasks->ClusteringTT->MustDoneBefore.push_back(tasks->PairingFR);
    tasks->ClusteringTT->MustDoneBefore.push_back(tasks->PairingRF);
    tasks->Tasks.push_back(tasks->ClusteringTT);
    CuttoffCalcTask->MustDoneBefore.push_back(tasks->ClusteringTT);
    
    tasks->ClusteringTH = new Clustering(data, Clustering::TypeTransTH, config, CuttoffCalcTask);
    tasks->ClusteringTH->MustDoneBefore.push_back(tasks->PairingFR);
    tasks->ClusteringTH->MustDoneBefore.push_back(tasks->PairingRF);
    tasks->Tasks.push_back(tasks->ClusteringTH);
    CuttoffCalcTask->MustDoneBefore.push_back(tasks->ClusteringTH);
    
    tasks->ClusteringHT = new Clustering(data, Clustering::TypeTransHT, config, CuttoffCalcTask);
    tasks->ClusteringHT->MustDoneBefore.push_back(tasks->PairingFR);
    tasks->ClusteringHT->MustDoneBefore.push_back(tasks->PairingRF);
    tasks->Tasks.push_back(tasks->ClusteringHT);
    CuttoffCalcTask->MustDoneBefore.push_back(tasks->ClusteringHT);
    
    AllTasks.reserve(tasks->Tasks.size());
    AllTasks.insert(AllTasks.end(), tasks->Tasks.begin(), tasks->Tasks.end());
}