/* 
 * File:   Clustering.h
 * Author: peter
 *
 * Created on January 17, 2013, 2:53 PM
 */

#ifndef CLUSTERING_H
#define	CLUSTERING_H

#include "../Task.h"
#include "../Structs.h"
#include "../Config.h"
#include "ReferenceDataStorage.h"
#include "TranslocationDataStorage.h"
#include "Insersize.h"
#include "CuttoffCalc.h"
#include <map>

class Clustering: public Task {
public:
    Clustering(ReferenceDataStorage* Ref, int type, Config* config, CuttoffCalc* CuttoffCalcTask);
    Clustering(TranslocationDataStorage* Trans, int type, Config* config, CuttoffCalc* CuttoffCalcTask);
    virtual ~Clustering();
    
    long int ClustersFound = 0;
    static const int TypeDeletions = 1;
    static const int TypeInsertions = 2;
    static const int TypeEverted = 3;
    static const int TypeInversionHH = 4;
    static const int TypeInversionTT = 5;
    static const int TypeTransTH = 6;
    static const int TypeTransHT = 7;
    static const int TypeTransHH = 8;
    static const int TypeTransTT = 9;
protected:
    void InternalRun(void);
    void InternalFinish(void);
    void InternalDoWhilePauze(void);
private:
    CuttoffCalc* CuttoffCalcTask;
    int type;
    map<unsigned int,unsigned int> cuttoffCurve;
    ReferenceDataStorage* Ref, * Ref1, * Ref2;
    TranslocationDataStorage* Trans;
    void Clustering123sv();
    void SecondaryClustering();
    
    vector<Cluster>* Clusters;
    vector<Pair>* Pairs;
    vector<int> PairsList;
    string Ori, ref1, ref2, typeText;
    string ClusterFile;
    
    int secondaryClustering = 0;
    unsigned int ClusterWindow;
    unsigned int SearchWindow;
    unsigned int minClones;
    int pairsleft;
};

#endif	/* CLUSTERING_H */

