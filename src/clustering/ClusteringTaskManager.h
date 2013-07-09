/* 
 * File:   ClusteringTaskManager.h
 * Author: peter
 *
 * Created on February 19, 2013, 12:47 PM
 */

#ifndef CLUSTERINGTASKMANAGER_H
#define	CLUSTERINGTASKMANAGER_H

#include "../TaskManager.h"
#include "ReferenceDataStorage.h"
#include "ReadSingleReads.h"
#include "ReadPairedReads.h"
#include "SortSingleReads.h"
#include "Pairing.h"
#include "PairingHighLowQuality.h"
#include "PairingLowQuality.h"
#include "ClearMemorySinglereads.h"
#include "Insersize.h"
#include "Clustering.h"
#include "PairingTranslocation.h"
#include "TranslocationDataStorage.h"
#include "CuttoffCalc.h"

class ClusteringTaskManager: public TaskManager {
public:
    ClusteringTaskManager(vector<TranslocationDataStorage*>* Translocations, Config* config);
    virtual ~ClusteringTaskManager();
    
    void AddRef(ReferenceDataStorage* ref);
    CuttoffCalc* CuttoffCalcTask;
protected:
    bool InternalFinished();
    void InternalCheckTasks();
private:
    void AddTranslocation(ReferenceDataStorage* ref1, ReferenceDataStorage* ref2);
    void ResetClearTasks();
    
    vector<ClearMemorySinglereads*> ClearTasks;
    vector<TranslocationDataStorage*>* Translocations;
    struct RefTasks {
        vector<Task*> Tasks;
        
        vector<ReadSingleReads*>* TaskReadForward, * TaskReadReverse;
        vector<ReadPairedReads*>* TaskReadPaireds;
        SortSingleReads* TaskSortForward, * TaskSortReverse, * TaskSortForwardLowquality, * TaskSortReverseLowquality;
        Pairing* TaskPairing;
        PairingHighLowQuality* TaskPairingReverseHighLowQuality, * TaskPairingForwardHighLowQuality;
        PairingLowQuality* TaskPairingLowQuality;
        ClearMemorySinglereads* TaskClearForward, * TaskClearReverse, * TaskClearForwardLowQuality, * TaskClearReverseLowQuality;
        Insersize* TaskInsersize;
        Clustering* TaskClusterDeletions, * TaskClusterInsertions, * TaskClusterTTpairs, * TaskClusterHHpairs, * TaskClusterHTpairs;
    };
    map<ReferenceDataStorage*,RefTasks*> refsTasks;
    struct TranslocationID {
        ReferenceDataStorage* ref1;
        ReferenceDataStorage* ref2;
    };
    struct TransTasks {
        vector<Task*> Tasks;
        
        PairingTranslocation* PairingFR,* PairingRF;
        PairingHighLowQuality* PairingHighLowF1R2,* PairingHighLowF2R1,* PairingHighLowR1F2,* PairingHighLowR2F1;
        PairingLowQuality* PairingLowFR,* PairingLowRF;
        Clustering* ClusteringHH,* ClusteringTT,* ClusteringTH,* ClusteringHT;
    };
    map<TranslocationDataStorage*, TransTasks*> TranlocationTasks;
};

#endif	/* CLUSTERINGTASKMANAGER_H */

