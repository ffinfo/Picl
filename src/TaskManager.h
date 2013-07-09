/* 
 * File:   TaskMan.h
 * Author: peter
 *
 * Created on February 4, 2013, 6:56 PM
 */

#ifndef TASKMANAGER_H
#define	TASKMANAGER_H

#include <map>
#include "Config.h"
#include "Task.h"
//#include "ReferenceDataStorage.h"
//#include "ReadSingleReads.h"
//#include "ReadPairedReads.h"
//#include "SortSingleReads.h"
//#include "Pairing.h"
//#include "PairingHighLowQuality.h"
//#include "PairingLowQuality.h"
//#include "ClearMemorySinglereads.h"
//#include "Insersize.h"
//#include "Clustering.h"
//#include "PairingTranslocation.h"
//#include "TranslocationDataStorage.h"

class TaskManager {
public:
    TaskManager(Config* config);
    virtual ~TaskManager();
    
    //void AddRef(ReferenceDataStorage* ref);
    void CheckTasks();
    void StartTasks();
    bool isFinished();
    void WriteTasksNotDone();
        
    Config* config;
    
    unsigned int completed = 0;
    unsigned int RunningReadingTasks = 0;
    unsigned int notDone = 0;
    unsigned int totaltasks = 0;
    bool Finished = false;
protected:
    virtual bool InternalFinished();
    virtual void InternalCheckTasks();
    
    void WriteRunningTasks();
    
    vector<Task*> AllTasks;
    vector<Task*> RunningTasks;
private:
//    vector<ClearMemorySinglereads*> ClearTasks;
//    vector<TranslocationDataStorage*>* Translocations;
//    struct RefTasks {
//        vector<Task*> Tasks;
//        
//        vector<ReadSingleReads*>* TaskReadForward, * TaskReadReverse;
//        vector<ReadPairedReads*>* TaskReadPaireds;
//        SortSingleReads* TaskSortForward, * TaskSortReverse, * TaskSortForwardLowquality, * TaskSortReverseLowquality;
//        Pairing* TaskPairing;
//        PairingHighLowQuality* TaskPairingReverseHighLowQuality, * TaskPairingForwardHighLowQuality;
//        PairingLowQuality* TaskPairingLowQuality;
//        ClearMemorySinglereads* TaskClearForward, * TaskClearReverse, * TaskClearForwardLowQuality, * TaskClearReverseLowQuality;
//        Insersize* TaskInsersize;
//        Clustering* TaskClusterDeletions, * TaskClusterInsertions, * TaskClusterTTpairs, * TaskClusterHHpairs, * TaskClusterHTpairs;
//    };
//    map<ReferenceDataStorage*,RefTasks*> refsTasks;
//    struct TranslocationID {
//        ReferenceDataStorage* ref1;
//        ReferenceDataStorage* ref2;
//    };
//    struct TransTasks {
//        vector<Task*> Tasks;
//        
//        PairingTranslocation* PairingFR,* PairingRF;
//        PairingHighLowQuality* PairingHighLowF1R2,* PairingHighLowF2R1,* PairingHighLowR1F2,* PairingHighLowR2F1;
//        PairingLowQuality* PairingLowFR,* PairingLowRF;
//        Clustering* ClusteringHH,* ClusteringTT,* ClusteringTH,* ClusteringHT;
//    };
//    map<TranslocationDataStorage*, TransTasks*> TranlocationTasks;
};

#endif	/* TASKMANAGER_H */

