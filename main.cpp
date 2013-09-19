/* 
 * File:   main.cpp
 * Author: Peter van 't Hof
 *
 * Created on November 29, 2012, 11:55 AM
 */
 
#include <cstdlib>
#include <iostream>
#include "src/clustering/ClusteringMain.h"
#include "src/pairedbammaker/PairedBamMakerMain.h"
#include "src/viewclusters/ViewClustersMain.h"
#include "src/viewcoverage/ViewCoverage.h"
#include "src/filter/FilterMain.h"
#include "src/compare/CompareMain.h"
#include "src/generateconfig/GenerateConfigMain.h"
#include "src/multisample/MultisampleMain.h"
#include "src/splitbam/SplitBamMain.h"

using namespace std;

static int usage() {
    cerr << "Usage: <name program> <command> <args>" << endl;
    cerr << "Commands: clustering" << endl;
    cerr << "          filter" << endl;
    cerr << "          viewclusters " << endl;
    cerr << "          viewcoverage" << endl;
    cerr << "          pairedbammaker" << endl;
    cerr << "          generateconfig" << endl;
    cerr << "          multisample" << endl;
    cerr << "          splitbam" << endl;
    return 1;
}

int main(int argc, char *argv[]) {
    if (argc < 2) return usage();
    if (strcmp(argv[1], "clustering") == 0) {
        ClusteringMain clusteringMain;
        return clusteringMain.main(argc-1, argv+1);
    } else if (strcmp(argv[1], "pairedbammaker") == 0) {
        PairedBamMakerMain pairedBamMakerMain;
        return pairedBamMakerMain.main(argc-1, argv+1);
    } else if (strcmp(argv[1], "viewclusters") == 0) {
        ViewClustersMain viewClustersMain;
        return viewClustersMain.main(argc-1, argv+1);
    } else if (strcmp(argv[1], "viewcoverage") == 0) {
        ViewCoverage viewCoverage;
        return viewCoverage.main(argc-1, argv+1);
    } else if (strcmp(argv[1], "filter") == 0) {
        FilterMain filterMain;
        return filterMain.main(argc-1, argv+1);
    } else if (strcmp(argv[1], "generateconfig") == 0) {
        GenerateConfigMain generateConfigMain;
        return generateConfigMain.main(argc-1, argv+1);
    } else if (strcmp(argv[1], "multisample") == 0) {
        MultisampleMain multisampleMain;
        return multisampleMain.main(argc-1, argv+1);
    } else if (strcmp(argv[1], "compare") == 0) {
        CompareMain compareMain;
        return compareMain.main(argc-1, argv+1);
    } else if (strcmp(argv[1], "splitbam") == 0) {
        SplitBamMain splitBamMain;
        return splitBamMain.main(argc-1, argv+1);
    } else return usage();
}