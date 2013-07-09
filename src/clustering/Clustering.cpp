/* 
 * File:   Clustering.cpp
 * Author: peter
 * 
 * Created on January 17, 2013, 2:53 PM
 */

#include "Clustering.h"
#include "CuttoffCalc.h"
#include "../filter/ClusterFile.h"
#include <algorithm>
#include <iostream>
#include <fstream>

Clustering::Clustering(ReferenceDataStorage* Ref, int type, Config* config, CuttoffCalc* CuttoffCalcTask) : Task(config) {
    this->CuttoffCalcTask = CuttoffCalcTask;
    this->Ref = Ref;
    this->Ref1 = Ref;
    this->Ref2 = Ref;
    this->type = type;
    ref1 = Ref->Name;
    ref2 = Ref->Name;
    switch (type) {
        case Clustering::TypeDeletions:
            Pairs = Ref->Deletions;
            Ori = "TH";
            typeText = "deletion";
            break;
        case Clustering::TypeInsertions:
            Pairs = Ref->Insertions;
            Ori = "TH";
            typeText = "insertion";
            break;
        case Clustering::TypeEverted:
            Pairs = Ref->HTpairs;
            Ori = "HT";
            typeText = "everted";
            break;
        case Clustering::TypeInversionHH:
            Pairs = Ref->HHpairs;
            Ori = "HH";
            typeText = "inversion";
            break;
        case Clustering::TypeInversionTT:
            Pairs = Ref->TTpairs;
            Ori = "TT";
            typeText = "inversion";
            break;
    }
    TaskName = "Clustering " + typeText + "-" + Ori + " for ref: " + Ref->Name;
    ClusterFile = this->config->clusterDir + "ref." + Ref->Name + "." + typeText + "." + Ori + ".clusters";
    
    minClones = this->config->minClones;
    Ready = true;
}

Clustering::Clustering(TranslocationDataStorage* Trans, int type, Config* config, CuttoffCalc* CuttoffCalcTask) : Task(config) {
    this->CuttoffCalcTask = CuttoffCalcTask;
    this->config = config;
    this->type = type;
    this->Trans = Trans;
    this->Ref1 = Trans->Ref1;
    this->Ref2 = Trans->Ref2;
    typeText = "translocation";
    ref1 = Trans->Ref1->Name;
    ref2 = Trans->Ref2->Name;
    switch (type) {
        case Clustering::TypeTransTH:
            Pairs = Trans->THpairs;
            Ori = "TH";
            break;
        case Clustering::TypeTransHT:
            Pairs = Trans->HTpairs;
            Ori = "HT";
            break;
        case Clustering::TypeTransHH:
            Pairs = Trans->HHpairs;
            Ori = "HH";
            break;
        case Clustering::TypeTransTT:
            Pairs = Trans->TTpairs;
            Ori = "TT";
            break;
    }
    TaskName = "Clustering " + typeText + "-" + Ori + " for refs: " + ref1 + "-" + ref2;
    ClusterFile = config->clusterDir + "trans." + ref1 + "-" + ref2 + "." + Ori + ".clusters";
    minClones = config->minClones;
    Ready = true;
}

Clustering::~Clustering() {
}

void Clustering::InternalFinish() {
    PairsList.clear();
    PairsList.shrink_to_fit();
    switch (type) {
        case Clustering::TypeDeletions:
            Ref->Stats.Cluster.Deletions = ClustersFound;
            break;
        case Clustering::TypeInsertions:
            Ref->Stats.Cluster.Insertions = ClustersFound;
            break;
        case Clustering::TypeEverted:
            Ref->Stats.Cluster.HTclusters = ClustersFound;
            break;
        case Clustering::TypeInversionHH:
            Ref->Stats.Cluster.HHclusters = ClustersFound;
            break;
        case Clustering::TypeInversionTT:
            Ref->Stats.Cluster.TTclusters = ClustersFound;
            break;
        case Clustering::TypeTransTH:
            Trans->Stats.Cluster.THclusters = ClustersFound;
            break;
        case Clustering::TypeTransHT:
            Trans->Stats.Cluster.HTclusters = ClustersFound;
            break;
        case Clustering::TypeTransHH:
            Trans->Stats.Cluster.HHclusters = ClustersFound;
            break;
        case Clustering::TypeTransTT:
            Trans->Stats.Cluster.TTclusters = ClustersFound;
            break;
    }
    CuttoffCalcTask->AddClusters(cuttoffCurve);
    
    /*
    fstream file;
    file.open(config->BinClusterFile, ios::app | ios::binary | ios::out);
    for (Cluster cluster : *Clusters) {
        if (cluster.pairs.size() < 1) continue;
        ClusterFile::ClusterFileRead read;
        read.ref1 = config->ClusterFileRefHeader[ref1];
        read.start1 = cluster.s1;
        read.end1 = cluster.e1;
        read.ref2 = config->ClusterFileRefHeader[ref2];
        read.start2 = cluster.s2;
        read.end2 = cluster.e2;
        read.type = config->ClusterFileTypeHeader[typeText];
        if (Ori.compare("TH") == 0) read.ori = 0;
        else if (Ori.compare("HT") == 0) read.ori = 1;
        else if (Ori.compare("HH") == 0) read.ori = 2;
        else if (Ori.compare("TT") == 0) read.ori = 3;
        read.probability = cluster.probability;
        read.prob_size = cluster.prob_size;
        read.pairs = cluster.pairs.size();
        read.clone = cluster.clone;
        read.overlap = cluster.ov;
        file.write((char*) & read, sizeof(ClusterFile::ClusterFileRead));
    }
    file.close();
    */
     
    for (Cluster &cluster : *Clusters) {
        if (cluster.pairs.size() >= config->minClones) {
            config->clusterFile->AddCluster(cluster, ref1, ref2, Ori, typeText);
        }
    }
    config->clusterFile->CloseFile();
    
    delete Pairs;
    delete Clusters;
}

void Clustering::InternalRun() {
    Clusters = new vector<Cluster>;
    if (Pairs->empty()) return;
    
    InsertsizeStats insertsizeStats = config->GetInsertsizeStats(ref1);
    
    ClusterWindow = insertsizeStats.DeletionCutoff * 1;
    if (config->ClusterWindow > 0) ClusterWindow = config->ClusterWindow;
    SearchWindow = ClusterWindow * 2;
    
    pairsleft = Pairs->size();
    
    sort(Pairs->begin(), Pairs->end(), SortPairsOnFirstPosition());
    
    Clustering123sv(); // need to have a IF when more algoritmes come
    
    sort(Clusters->begin(), Clusters->end(), SortClustersOnS1());
    
    // Secondary Clustering
    if (config->secondaryClustering) {
        SecondaryClustering();
        sort(Clusters->begin(), Clusters->end(), SortClustersOnS1());
    }
    
    for (Cluster &cluster : *Clusters) {
        int total_comparison = 0;
        int total_overlaps = 0;
        map<int, double> p_vals;
        sort(cluster.pairs.begin(), cluster.pairs.end());
        for (unsigned int pairIt : cluster.pairs) {
            bool clone = false;
            for (unsigned int pairIt2 : cluster.pairs) {
                if (pairIt == pairIt2) continue;
                // Clone finding
                if ((not clone) && Pairs->at(pairIt).First.Position == Pairs->at(pairIt2).First.Position && Pairs->at(pairIt).Second.Position == Pairs->at(pairIt2).Second.Position) {
                    cluster.clone++;
                    clone = true;
                }
                // Overlap in cluster
                total_comparison++;
                if (Pairs->at(pairIt).Second.Position > Pairs->at(pairIt2).First.Position) total_overlaps++;
            }
            //cout << "Clusterwindow " << ClusterWindow << endl;
            for (int dist = (int)ClusterWindow * -1; dist <= (int)ClusterWindow; dist++) {
                int size = dist;
//                if (Ori.at(0) == 'T') size += cluster.e1 - Pairs->at(pairIt).First.Position; 
//                else size += Pairs->at(pairIt).First.Position - Pairs->at(pairIt).First.Lenght - cluster.s1;
//                if (Ori.at(1) == 'T') size += cluster.e2 - Pairs->at(pairIt).Second.Position; 
//                else size += Pairs->at(pairIt).Second.Position - Pairs->at(pairIt).Second.Lenght - cluster.s2;
                
                size += cluster.e1 - Pairs->at(pairIt).First.Position;
                size += Pairs->at(pairIt).Second.Position + Pairs->at(pairIt).Second.Lenght - cluster.e2;
                //size += Pairs->at(pairIt).Second.Position + Pairs->at(pairIt).Second.Lenght - cluster.s2;
                
                if (size >= 0 && size < insertsizeStats.curve.size()) {
//                    if (dist > 2500) {
//                        cout << dist << " - " << ((double)insertsizeStats.curve[dist] / (double)insertsizeStats.CurveTotal);
//                        cout << endl;
//                    }
                    p_vals[dist] += ((double)insertsizeStats.curve[size] / (double)insertsizeStats.CurveTotal) / ((double)insertsizeStats.max_curve / (double)insertsizeStats.CurveTotal);
                }
                //cout << dist << " - " << size << " - " << p_vals[dist] << endl;
            }
        }
        
        double max_p = 0;
        int prob_size;
        for (map<int, double>::iterator it = p_vals.begin(); it != p_vals.end(); it++) {
            if (it->second > max_p) {
                max_p = it->second;
                prob_size = it->first;
            }
        }
        cluster.prob_size = prob_size;
        cluster.probability = max_p / cluster.pairs.size();
        cluster.ov = 100 * total_overlaps / total_comparison;
        
//        int binBegin = floor((double)cluster.s1 / (double)config->CoverageBinSize) - 1;
//        int binEnd = floor((double)cluster.e1 / (double)config->CoverageBinSize) - 1;
//        for (int t = binBegin; t <= binEnd; t++) {
//            if (Ori.substr(0,1).compare("T") == 0) cluster.ConcordantPairsUpstream += Ref1->Coverage->at(t).Upstream.concortdantPairs;
//            else cluster.ConcordantPairsUpstream += Ref1->Coverage->at(t).Downstream.concortdantPairs;
//        }
//        
//        binBegin = floor((double)cluster.s2 / (double)config->CoverageBinSize);
//        binEnd = floor((double)cluster.e2 / (double)config->CoverageBinSize);
//        for (int t = binBegin; t <= binEnd; t++) {
//            if (Ori.substr(1,2).compare("H") == 0) cluster.ConcordantPairsDownstream += Ref2->Coverage->at(t).Downstream.concortdantPairs;
//            else cluster.ConcordantPairsDownstream += Ref2->Coverage->at(t).Upstream.concortdantPairs;
//        }
    }
        
    // Reporting to cluster file
    ofstream file;
    long int count = 0, count2 = 0, count3 = 0;
    for (Cluster &cluster : *Clusters) {
        if ((cluster.pairs.size() - cluster.clone) >= minClones) {
            cuttoffCurve[cluster.pairs.size() - cluster.clone]++;
            if (!file.is_open()) file.open(ClusterFile, ios::trunc);
            count++;
            //double ovRel = 1 / (double)cluster.ov;
            
//            float Con1, Con2;
//            if (cluster.ConcordantPairsUpstream > 0) Con1 = (float)(cluster.pairs.size() - cluster.clone) / cluster.ConcordantPairsUpstream;
//            else Con1 = cluster.pairs.size() - cluster.clone;
//            if (cluster.ConcordantPairsDownstream > 0) Con2 = (float)(cluster.pairs.size() - cluster.clone) / cluster.ConcordantPairsDownstream;
//            else Con2 = cluster.pairs.size() - cluster.clone;
            
            file << ref1 << "\t" << cluster.s1 << "\t" << cluster.e1 
            << "\t" << ref2 << "\t" << cluster.s2 << "\t" << cluster.e2 
            << "\t" << Ori << "\t" << (cluster.pairs.size() - cluster.clone)
            << "\t" << cluster.clone << "\t" << typeText
            << "\t" << cluster.ov << "\t" << cluster.prob_size << "\t" << cluster.probability
//            << "\t" << cluster.ConcordantPairsUpstream << "\t" << cluster.ConcordantPairsDownstream
//            << "\t" << ((Con1 + Con2) / 2);
            << endl;
        }
        if (cluster.pairs.size() > 1) count2++;
        if (cluster.pairs.size() == 0) count3++;
    }
    
    ClustersFound = count + count2;
    
    file.close();
    
    return;
}

void Clustering::InternalDoWhilePauze() {
    if (Clusters->size() >= Clusters->capacity()) Clusters->reserve(10000);
    if (Pairs->size() >= Pairs->capacity()) Pairs->reserve(10000);
    if (PairsList.size() >= PairsList.capacity()) PairsList.reserve(10000);
}

void Clustering::Clustering123sv() {
    for (int it = 0; it < pairsleft; it++) {
        if (PairsList.size() >= PairsList.capacity()) this->PauseThread();
        PairsList.push_back(it);
    }
    
    while (PairsList.size() > 1) {
        Cluster tempCluster;
        vector<int>::iterator it = PairsList.begin();
        while (*it < 0 && it != PairsList.end()) {
            it++;
        }
        if (it == PairsList.end()) break;
        if (it.base() == 0 or *it < 0) continue;
        
        tempCluster.s1 = Pairs->at(*it).First.Position + 1;
        tempCluster.e1 = Pairs->at(*it).First.Position + Pairs->at(0).First.Lenght;
        tempCluster.s2 = Pairs->at(*it).Second.Position + 1;
        tempCluster.e2 = Pairs->at(*it).Second.Position + Pairs->at(0).Second.Lenght;
        tempCluster.pairs.push_back(*it);
        unsigned int pairs = 1;
        *it = -1;
        for (it++; it != PairsList.end(); it++) {
            if (it.base() == 0 or *it < 0) continue;
            if (Pairs->at(*it).First.Position > (tempCluster.s1 + SearchWindow)) break;
            unsigned int s1New, e1New, s2New, e2New;
            //unsigned int s1 = Pairs->at(*it).First.Position + 1;
            unsigned int e1 = Pairs->at(*it).First.Position + Pairs->at(*it).First.Lenght;
            unsigned int s2 = Pairs->at(*it).Second.Position + 1;
            unsigned int e2 = Pairs->at(*it).Second.Position + Pairs->at(*it).Second.Lenght;
            s1New = tempCluster.s1;
            if (e1 > tempCluster.e1) {
                e1New = e1;
            } else {
                e1New = tempCluster.e1;
            }
            if (s2 < tempCluster.s2) {
                s2New = s2;
            } else {
                s2New = tempCluster.s2;
            }
            if (e2 > tempCluster.e2) {
                e2New = e2;
            } else {
                e2New = tempCluster.e2;
            }
            if (e1New - s1New <= ClusterWindow && e2New - s2New <= ClusterWindow) {
                tempCluster.pairs.push_back(*it);
                tempCluster.s1 = s1New;
                tempCluster.e1 = e1New;
                tempCluster.s2 = s2New;
                tempCluster.e2 = e2New;
                *it = -1;
                pairs++;
            }
        }
        if (pairs >= minClones) {
            if (Clusters->size() >= Clusters->capacity()) this->PauseThread();
            Clusters->push_back(tempCluster);
        }
    }
}

void Clustering::SecondaryClustering() {
    for (unsigned long int it=0;it < Clusters->size();++it) {
        if (Clusters->at(it).pairs.size() == 0) continue;
        for (unsigned long int it2 = it+1; it2 < Clusters->size();++it2) {
            if (Clusters->at(it2).e1 > (Clusters->at(it).s1 + SearchWindow)) break;
            if (Clusters->at(it2).pairs.size() == 0) continue;
            unsigned int s1New, e1New, s2New, e2New;
            s1New = Clusters->at(it).s1;
            if (Clusters->at(it2).e1 > Clusters->at(it).e1) {
                e1New = Clusters->at(it2).e1;
            } else {
                e1New = Clusters->at(it).e1;
            }
            if (Clusters->at(it2).s2 < Clusters->at(it).s2) {
                s2New = Clusters->at(it2).s2;
            } else {
                s2New = Clusters->at(it).s2;
            }
            if (Clusters->at(it2).e2 > Clusters->at(it).e2) {
                e2New = Clusters->at(it2).e2;
            } else {
                e2New = Clusters->at(it).e2;
            }
            if (e1New - s1New <= ClusterWindow && e2New - s2New <= ClusterWindow) {
                // Merging clusters
                secondaryClustering++;
                Clusters->at(it).s1 = s1New;
                Clusters->at(it).e1 = e1New;
                Clusters->at(it).s2 = s2New;
                Clusters->at(it).e2 = e2New;
                for (int t : Clusters->at(it2).pairs) {
                    Clusters->at(it).pairs.push_back(t);
                }
                Clusters->at(it2).pairs.clear();
            }
        }
    }
}