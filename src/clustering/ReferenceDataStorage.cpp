/* 
 * File:   Reference.cpp
 * Author: Peter van 't Hof
 * 
 * Created on November 30, 2012, 2:23 PM
 */

#include "ReferenceDataStorage.h"
#include <algorithm>
#include <list>
#include <vector>
#include <sstream>
#include <ctime>
#include <boost/algorithm/string.hpp>
#include <math.h>

using namespace std;

ReferenceDataStorage::ReferenceDataStorage(int RefID, std::string RefName, long int Length, Config* config) {
    this->config = config;
    this->Length = Length;
    ID = RefID;
    Name = RefName;
    //InsertsizeMean = 0;
    forwardreads = new vector<Singleread>;
    forwardreadsLowQuality = new vector<Singleread>;
    reversereads = new vector<Singleread>;
    reversereadsLowQuality = new vector<Singleread>;
    
    forwardreadsBins = new vector<vector<Singleread>*>;
    forwardreadsLowQualityBins = new vector<vector<Singleread>*>;
    reversereadsBins = new vector<vector<Singleread>*>;
    reversereadsLowQualityBins = new vector<vector<Singleread>*>;
    
    HTpairs = new vector<Pair>;
    THpairs = new vector<InsertsizePair>;
    TTpairs = new vector<Pair>;
    HHpairs = new vector<Pair>;
    lowQualitypairs = new vector<LowQualityPair>;
    singleLowQualitypairs = new vector<SingleLowQualityPair>;
    Deletions = new vector<Pair>;
    Insertions = new vector<Pair>;
    OverlappingPairs = new vector<Pair>;
    OverhangingPairs = new vector<Pair>;
    Normal = new vector<Pair>;
    Coverage = new vector<CoverageFileRead>;
    
    /*
    //Reading tasks
    if (config->UsePairedBam) {
        TaskReadPaireds = new vector<ReadPairedReads*>;
        TaskCollectReads = new CollectReads(TaskReadPaireds, forwardreads, forwardreadsLowQuality, reversereads, reversereadsLowQuality);
        Tasks.push_back(TaskCollectReads);
        TotalTasks += 1;
        if (config->readBinSize > 0) {
            int bins = ceil((double)Length / (double)config->readBinSize);
            for (int it = 0; it < bins; it++) {
                int begin = it * config->readBinSize;
                int end = it * config->readBinSize + config->readBinSize - 1;
                if (end > Length) end = Length;
                ReadPairedReads* p = new ReadPairedReads(Name, begin, end, config);
                TaskReadPaireds->push_back(p);
                Tasks.push_back(p);
                TaskCollectReads->MustDoneBefore.push_back(p);
                TotalTasks += 1;
            }
        } else {
            ReadPairedReads* p = new ReadPairedReads(Name, 0, Length, config);
            TaskReadPaireds->push_back(p);
            Tasks.push_back(p);
            TaskCollectReads->MustDoneBefore.push_back(p);
            TotalTasks += 1;
        }
    } else {
        TaskReadForward = new ReadSingleReads(Name, forwardreads, forwardreadsLowQuality, true, config);
        Tasks.push_back(TaskReadForward);
        TaskReadReverse = new ReadSingleReads(Name, reversereads, reversereadsLowQuality, false, config);
        Tasks.push_back(TaskReadReverse);
        TotalTasks += 2;
    }
    
    // Sorting Tasks
    TaskSortForward = new SortSingleReads(forwardreads);
    Tasks.push_back(TaskSortForward);
    TaskSortReverse = new SortSingleReads(reversereads);
    Tasks.push_back(TaskSortReverse);
    TaskSortForwardLowquality = new SortSingleReads(forwardreadsLowQuality);
    Tasks.push_back(TaskSortForwardLowquality);
    TaskSortReverseLowquality = new SortSingleReads(reversereadsLowQuality);
    Tasks.push_back(TaskSortReverseLowquality);
    TotalTasks += 4;
    if (config->UsePairedBam) { // remove if ref is in the task
        TaskSortForward->MustDoneBefore.push_back(TaskCollectReads);
        TaskSortReverse->MustDoneBefore.push_back(TaskCollectReads);
        TaskSortForwardLowquality->MustDoneBefore.push_back(TaskCollectReads);
        TaskSortReverseLowquality->MustDoneBefore.push_back(TaskCollectReads);
    } else {
        TaskSortForward->MustDoneBefore.push_back(TaskReadForward);
        TaskSortForward->MustDoneBefore.push_back(TaskReadReverse);
        TaskSortReverse->MustDoneBefore.push_back(TaskReadForward);
        TaskSortReverse->MustDoneBefore.push_back(TaskReadReverse);
        TaskSortForwardLowquality->MustDoneBefore.push_back(TaskReadForward);
        TaskSortForwardLowquality->MustDoneBefore.push_back(TaskReadReverse);
        TaskSortReverseLowquality->MustDoneBefore.push_back(TaskReadForward);
        TaskSortReverseLowquality->MustDoneBefore.push_back(TaskReadReverse);
    }
    
    //Pairing Tasks
    TaskPairing = new Pairing(forwardreads, reversereads, THpairs, HTpairs, TTpairs, HHpairs);
    TaskPairing->MustDoneBefore.push_back(TaskSortForward);
    TaskPairing->MustDoneBefore.push_back(TaskSortReverse);
    Tasks.push_back(TaskPairing);
    TaskPairingForwardHighLowQuality = new PairingHighLowQuality(forwardreads, reversereadsLowQuality, singleLowQualitypairs);
    TaskPairingForwardHighLowQuality->MustDoneBefore.push_back(TaskSortForward);
    TaskPairingForwardHighLowQuality->MustDoneBefore.push_back(TaskSortReverseLowquality);
    Tasks.push_back(TaskPairingForwardHighLowQuality);
    TaskPairingReverseHighLowQuality = new PairingHighLowQuality(reversereads, forwardreadsLowQuality, singleLowQualitypairs);
    TaskPairingReverseHighLowQuality->MustDoneBefore.push_back(TaskSortForwardLowquality);
    TaskPairingReverseHighLowQuality->MustDoneBefore.push_back(TaskSortReverse);
    Tasks.push_back(TaskPairingReverseHighLowQuality);
    TaskPairingLowQuality = new PairingLowQuality(forwardreadsLowQuality, reversereadsLowQuality, lowQualitypairs);
    TaskPairingLowQuality->MustDoneBefore.push_back(TaskSortForwardLowquality);
    TaskPairingLowQuality->MustDoneBefore.push_back(TaskSortReverseLowquality);
    Tasks.push_back(TaskPairingLowQuality);
    TotalTasks += 4;
    
    //Clearing Tasks
    TaskClearForward = new ClearMemorySinglereads(forwardreads);
    TaskClearForward->MustDoneBefore.push_back(TaskPairing);
    TaskClearForward->MustDoneBefore.push_back(TaskPairingForwardHighLowQuality);
    Tasks.push_back(TaskClearForward);
    TaskClearReverse = new ClearMemorySinglereads(reversereads);
    TaskClearReverse->MustDoneBefore.push_back(TaskPairing);
    TaskClearReverse->MustDoneBefore.push_back(TaskPairingReverseHighLowQuality);
    Tasks.push_back(TaskClearReverse);
    TaskClearForwardLowQuality = new ClearMemorySinglereads(forwardreadsLowQuality);
    TaskClearForwardLowQuality->MustDoneBefore.push_back(TaskPairingLowQuality);
    TaskClearForwardLowQuality->MustDoneBefore.push_back(TaskPairingReverseHighLowQuality);
    Tasks.push_back(TaskClearForwardLowQuality);
    TaskClearReverseLowQuality = new ClearMemorySinglereads(reversereadsLowQuality);
    TaskClearReverseLowQuality->MustDoneBefore.push_back(TaskPairingLowQuality);
    TaskClearReverseLowQuality->MustDoneBefore.push_back(TaskPairingForwardHighLowQuality);
    Tasks.push_back(TaskClearReverseLowQuality);
    
    //Insersize task
    TaskInsersize = new Insersize(THpairs, Normal, Deletions, Insertions, Name, config);
    TaskInsersize->MustDoneBefore.push_back(TaskPairing);
    Tasks.push_back(TaskInsersize);
    TotalTasks += 1;
    
    //Clustering Tasks
    TaskClusterDeletions = new Clustering(Deletions, "TH", Name, Name, "deletion", config->clusterDir + "ref." + Name + ".deletions.clusters", config);
    TaskClusterDeletions->MustDoneBefore.push_back(TaskInsersize);
    Tasks.push_back(TaskClusterDeletions);
    TaskClusterInsertions = new Clustering(Insertions, "TH", Name, Name, "insertion", config->clusterDir + "ref." + Name + ".insertions.clusters", config);
    TaskClusterInsertions->MustDoneBefore.push_back(TaskInsersize);
    Tasks.push_back(TaskClusterInsertions);
    TaskClusterTTpairs = new Clustering(TTpairs, "TT", Name, Name, "inversion", config->clusterDir + "ref." + Name + ".TT.clusters", config);
    TaskClusterTTpairs->MustDoneBefore.push_back(TaskInsersize);
    Tasks.push_back(TaskClusterTTpairs);
    TaskClusterHHpairs = new Clustering(HHpairs, "HH", Name, Name, "inversion", config->clusterDir + "ref." + Name + ".HH.clusters", config);
    TaskClusterHHpairs->MustDoneBefore.push_back(TaskInsersize);
    Tasks.push_back(TaskClusterHHpairs);
    TaskClusterHTpairs = new Clustering(HTpairs, "HT", Name, Name, "duplication", config->clusterDir + "ref." + Name + ".HT.clusters", config);
    TaskClusterHTpairs->MustDoneBefore.push_back(TaskInsersize);
    Tasks.push_back(TaskClusterHTpairs);
    TotalTasks += 5;
    */
}

ReferenceDataStorage::~ReferenceDataStorage() {
}

void ReferenceDataStorage::ClearingTasks() {
    /*
    RunningTasks = 0;
    RunningReadingTasks = 0;
    //Reading
    if (!ReadingDone && config->UsePairedBam) {
        ReadingDone = true;
        for (ReadPairedReads* TaskReadPaired : *TaskReadPaireds) {
            if (TaskReadPaired->IsDone()) {
                if (TaskReadPaired->IsRunning()) {
                    Stats.Read.Forward += TaskReadPaired->ForwardHighQualityCount;
                    Stats.Read.ForwardLowQuality += TaskReadPaired->ForwardLowQualityCount;
                    Stats.Read.Reverse += TaskReadPaired->ReverseHighQualityCount;
                    Stats.Read.ReverseLowQuality += TaskReadPaired->ReverseLowQualityCount;
                    TaskReadPaired->SetRunning(false);
                    CompletedTasks++;
                    ostringstream logBuffer;
                    logBuffer << "Reading Bin done for ref: " << Name << "   Bin: " << TaskReadPaired->begin << "-" << TaskReadPaired->end;
                    config->Log(logBuffer.str());
                }
            } else {
                ReadingDone = false;
                if (TaskReadPaired->IsRunning()) {
                    RunningTasks++;
                    RunningReadingTasks++;
                }
            }
        }
        if (ReadingDone) {
            config->Log("Reading done for ref: " + Name);
        }
    } else if (!ReadingDone && !config->UsePairedBam) {
        if (TaskReadForward->IsDone() && TaskReadForward->IsRunning()) {
            Stats.Read.Forward = TaskReadForward->HighQualityCount;
            Stats.Read.ForwardLowQuality = TaskReadForward->LowQualityCount;
            TaskReadForward->SetRunning(false);
            ForwardReadingDone = true;
            CompletedTasks++;
            config->Log("Reading forward done for ref: " + Name);
        } else if (TaskReadForward->IsRunning()) { 
            RunningTasks++;
            RunningReadingTasks++;
        }
        if (TaskReadReverse->IsDone() && TaskReadReverse->IsRunning()) {
            Stats.Read.Reverse = TaskReadReverse->HighQualityCount;
            Stats.Read.ReverseLowQuality = TaskReadReverse->LowQualityCount;
            TaskReadReverse->SetRunning(false);
            ReverseReadingDone = true;
            CompletedTasks++;
            config->Log("Reading reverse done for ref: " + Name);
        } else if (TaskReadReverse->IsRunning()) {
            RunningTasks++;
            RunningReadingTasks++;
        }
        if (TaskReadReverse->IsDone() && TaskReadForward->IsDone()) {
            ReadingDone = true;
            ReadCollectedDone = true;
        }
    }
    
    //Collect reads
    if (TaskCollectReads->IsDone() && TaskCollectReads->IsRunning()) {
        TaskCollectReads->SetRunning(false);
        ReadCollectedDone = true;
        CompletedTasks++;
        config->Log("Reads Collected for ref: " + Name);
    } else if (TaskCollectReads->IsRunning()) RunningTasks++;
    
    //Sorting
    if (TaskSortForward->IsDone() && TaskSortForward->IsRunning()) {
        TaskSortForward->SetRunning(false);
        CompletedTasks++;
        config->Log("Sorting forward done for ref: " + Name);
    } else if (TaskSortForward->IsRunning()) RunningTasks++;
    if (TaskSortReverse->IsDone() && TaskSortReverse->IsRunning()) {
        TaskSortReverse->SetRunning(false);
        CompletedTasks++;
        config->Log("Sorting reverse done for ref: " + Name);
    } else if (TaskSortReverse->IsRunning()) RunningTasks++;
    if (TaskSortForwardLowquality->IsDone() && TaskSortForwardLowquality->IsRunning()) {
        TaskSortForwardLowquality->SetRunning(false);
        CompletedTasks++;
        config->Log("Sorting low quality forward done for ref: " + Name);
    } else if (TaskSortForwardLowquality->IsRunning()) RunningTasks++;
    if (TaskSortReverseLowquality->IsDone() && TaskSortReverseLowquality->IsRunning()) {
        TaskSortReverseLowquality->SetRunning(false);
        CompletedTasks++;
        config->Log("Sorting low quality reverse done for ref: " + Name);
    } else if (TaskSortReverseLowquality->IsRunning()) RunningTasks++;
    if (TaskSortForward->IsDone() && TaskSortReverse->IsDone() && TaskSortForwardLowquality->IsDone() && TaskSortReverseLowquality->IsDone()) SortingDone = true;
    
    //Pairing
    if (TaskPairing->IsDone() && TaskPairing->IsRunning()) {
        forwardreadsPaired =+ TaskPairing->Paired;
        reversereadsPaired =+ TaskPairing->Paired;
        Stats.Pair = TaskPairing->GetStats();
        TaskPairing->SetRunning(false);
        CompletedTasks++;
        config->Log("Pairing done for ref: " + Name);
    } else if (TaskPairing->IsRunning()) RunningTasks++;
    if (TaskPairingForwardHighLowQuality->IsDone() && TaskPairingForwardHighLowQuality->IsRunning()) {
        forwardreadsPaired =+ TaskPairingForwardHighLowQuality->Paired;
        reversereadsLowQualityPaired =+ TaskPairingForwardHighLowQuality->Paired;
        TaskPairingForwardHighLowQuality->SetRunning(false);
        CompletedTasks++;
        config->Log("Pairing Forward High Quality to Reverse Low Quality done for ref: " + Name);
    } else if (TaskPairingForwardHighLowQuality->IsRunning()) RunningTasks++;
    if (TaskPairingReverseHighLowQuality->IsDone() && TaskPairingReverseHighLowQuality->IsRunning()) {
        forwardreadsLowQualityPaired =+ TaskPairingReverseHighLowQuality->Paired;
        reversereadsPaired =+ TaskPairingReverseHighLowQuality->Paired;
        TaskPairingReverseHighLowQuality->SetRunning(false);
        CompletedTasks++;
        config->Log("Pairing Reverse High Quality to Forward Low Quality done for ref: " + Name);
    } else if (TaskPairingReverseHighLowQuality->IsRunning()) RunningTasks++;
    if (TaskPairingLowQuality->IsDone() && TaskPairingLowQuality->IsRunning()) {
        forwardreadsLowQualityPaired =+ TaskPairingLowQuality->Paired;
        reversereadsLowQualityPaired =+ TaskPairingLowQuality->Paired;
        TaskPairingLowQuality->SetRunning(false);
        CompletedTasks++;
        config->Log("Pairing LowQuality done for ref: " + Name);
    } else if (TaskPairingLowQuality->IsRunning()) RunningTasks++;
    if (TaskPairing->IsDone() && TaskPairingForwardHighLowQuality->IsDone() && TaskPairingReverseHighLowQuality->IsDone() && TaskPairingLowQuality->IsDone()) PairingDone = true;
    
    //Clearing memory
    if (TaskClearForward->IsDone() && TaskClearForward->IsRunning()) {
        forwardreadsPaired -= TaskClearForward->Cleared;
        forwardreadsPaired = 0;
        TaskClearForward->Reset();
        config->Log("Clearing Forward done for ref: " + Name);
    } else if (TaskClearForward->IsRunning()) RunningTasks++;
    if (TaskClearReverse->IsDone() && TaskClearReverse->IsRunning()) {
        reversereadsPaired -= TaskClearReverse->Cleared;
        reversereadsPaired = 0;
        TaskClearReverse->Reset();
        config->Log("Clearing Reverse done for ref: " + Name);
    } else if (TaskClearReverse->IsRunning()) RunningTasks++;
    if (TaskClearForwardLowQuality->IsDone() && TaskClearForwardLowQuality->IsRunning()) {
        forwardreadsLowQualityPaired -= TaskClearForwardLowQuality->Cleared;
        forwardreadsLowQualityPaired = 0;
        TaskClearForwardLowQuality->Reset();
        config->Log("Clearing LowQuality Forward done for ref: " + Name);
    } else if (TaskClearForwardLowQuality->IsRunning()) RunningTasks++;
    if (TaskClearReverseLowQuality->IsDone() && TaskClearReverseLowQuality->IsRunning()) {
        reversereadsLowQualityPaired -= TaskClearReverseLowQuality->Cleared;
        reversereadsLowQualityPaired = 0;
        TaskClearReverseLowQuality->Reset();
        config->Log("Clearing LowQuality Reverse done for ref: " + Name);
    } else if (TaskClearReverseLowQuality->IsRunning()) RunningTasks++;
    if (!TaskClearForward->IsRunning() && !TaskClearReverse->IsRunning() && !TaskClearForwardLowQuality->IsRunning() && !TaskClearReverseLowQuality->IsRunning()) {
        ClearingDone = true;
    } 
    
    //Insertsize
    if (TaskInsersize->IsDone() && TaskInsersize->IsRunning()) {
        Stats.Insertsize = TaskInsersize->Stats;
        TaskInsersize->SetRunning(false);
        CompletedTasks++;
        config->Log("Insertsize done for ref: " + Name);
    } else if (TaskInsersize->IsRunning()) RunningTasks++;
    
    //Clustering
    if (TaskClusterDeletions->IsDone() && TaskClusterDeletions->IsRunning()) {
        Stats.Cluster.Deletions = TaskClusterDeletions->ClustersFound;
        TaskClusterDeletions->SetRunning(false);
        CompletedTasks++;
        config->Log("Clustering Deletions done for ref: " + Name);
    } else if (TaskClusterDeletions->IsRunning()) RunningTasks++;
    if (TaskClusterInsertions->IsDone() && TaskClusterInsertions->IsRunning()) {
        Stats.Cluster.Insertions = TaskClusterInsertions->ClustersFound;
        TaskClusterInsertions->SetRunning(false);
        CompletedTasks++;
        config->Log("Clustering Insertions done for ref: " + Name);
    } else if (TaskClusterInsertions->IsRunning()) RunningTasks++;
    if (TaskClusterTTpairs->IsDone() && TaskClusterTTpairs->IsRunning()) {
        Stats.Cluster.TTclusters = TaskClusterTTpairs->ClustersFound;
        TaskClusterTTpairs->SetRunning(false);
        CompletedTasks++;
        config->Log("Clustering TTpairs done for ref: " + Name);
    } else if (TaskClusterTTpairs->IsRunning()) RunningTasks++;
    if (TaskClusterHHpairs->IsDone() && TaskClusterHHpairs->IsRunning()) {
        Stats.Cluster.HHclusters = TaskClusterHHpairs->ClustersFound;
        TaskClusterHHpairs->SetRunning(false);
        CompletedTasks++;
        config->Log("Clustering HHpairs done for ref: " + Name);
    } else if (TaskClusterHHpairs->IsRunning()) RunningTasks++;
    if (TaskClusterHTpairs->IsDone() && TaskClusterHTpairs->IsRunning()) {
        Stats.Cluster.HTclusters = TaskClusterHTpairs->ClustersFound;
        TaskClusterHTpairs->SetRunning(false);
        CompletedTasks++;
        config->Log("Clustering HTpairs done for ref: " + Name);
    } else if (TaskClusterHTpairs->IsRunning()) RunningTasks++;
    if (TaskClusterDeletions->IsDone() && TaskClusterInsertions->IsDone() && 
            TaskClusterTTpairs->IsDone() && TaskClusterHHpairs->IsDone() && TaskClusterHTpairs->IsDone()) {
        ClusteringDone = true;
    }
    */
}