/* 
 * File:   SplitBamMain.cpp
 * Author: peter
 * 
 * Created on July 2, 2013, 4:41 PM
 */

#include "SplitBamMain.h"

SplitBamMain::SplitBamMain() {
}

SplitBamMain::~SplitBamMain() {
}

int SplitBamMain::usage() {
    cerr << "Usage: <name program> splitbam <options> -i <input bam file> -outputprefix <map/output prefix>" << endl;
    cerr << " Options: " << endl;
    cerr << "   -orientation <xx>: exist out of 1 char for each inputfile, must be 2 chars in total. " << endl;
    cerr << "            n = keep as in input file" << endl;
    cerr << "            i = use inverted for proper pair value" << endl;
    cerr << "            I = use inverted for proper pair value and invert in output file" << endl;
    cerr << "          Examples:" << endl;
    cerr << "            nn = used for Illumina matepair orientation (default)" << endl;
    cerr << "            ii = used for SOLID matepair orientation" << endl;
    cerr << "            II = used for SOLID matepair orientation, outputfile will be in same orientation as Illumina matepair" << endl;
    cerr << "            Ii = used for SOLID matepair orientation, outputfile will be in same orientation as Paired-end" << endl;
    cerr << "            ni = used for Paired-end orientation" << endl;
    cerr << "   -ori <xx>: Same as -orientation " << endl;
    return 1;
}

int SplitBamMain::main(int argc, char* argv[]) {
    if (argc < 3) return this->usage();
    for (int t = 1; t < (argc); t++) {
        if (strcmp(argv[t], "-i") == 0) {
            Inputfile = argv[t+1];
            t++;
        } else if (strcmp(argv[t], "-outputprefix") == 0) {
            Outputprefix = argv[t+1];
            t++;
        } else if (strcmp(argv[t], "-orientation") == 0 or strcmp(argv[t], "-ori") == 0) {
            string Ori = argv[t+1];
            string First = Ori.substr(0,1);
            string Second = Ori.substr(1,2);
            if (First.compare("i") == 0) FirstOriInverted = true;
            if (Second.compare("i") == 0) SecondOriInverted = true;
            if (First.compare("I") == 0) {
                FirstOriInverted = true;
                CorrectOriFirst = true;
            }
            if (Second.compare("I") == 0) {
                SecondOriInverted = true;
                CorrectOriSecond = true;
            }
            t++;
        }
    }
    
    if (not BamReader.Open(Inputfile)) {
        cerr << "Could not open bam file" << endl;
        return 1;
    }
    
    if (Outputprefix.empty()) {
        Outputprefix = Inputfile.substr(0, Inputfile.find_last_of(".bam") - 3);
    }
    
    //BamReader.SetRegion(BamReader.GetReferenceID("chr1"),40000000, BamReader.GetReferenceID("chr1"), 50000000);
    
    OutputWriter_TH.Open(Outputprefix + "_TH.bam", BamReader.GetHeader(), BamReader.GetReferenceData()); // Normal
    OutputWriter_TT.Open(Outputprefix + "_TT.bam", BamReader.GetHeader(), BamReader.GetReferenceData());
    OutputWriter_HH.Open(Outputprefix + "_HH.bam", BamReader.GetHeader(), BamReader.GetReferenceData());
    OutputWriter_HT.Open(Outputprefix + "_HT.bam", BamReader.GetHeader(), BamReader.GetReferenceData());
    OutputWriter_Translocation.Open(Outputprefix + "_Translocation.bam", BamReader.GetHeader(), BamReader.GetReferenceData());
    OutputWriter_Singletons_T.Open(Outputprefix + "_Singletons_T.bam", BamReader.GetHeader(), BamReader.GetReferenceData());
    OutputWriter_Singletons_H.Open(Outputprefix + "_Singletons_H.bam", BamReader.GetHeader(), BamReader.GetReferenceData());
    OutputWriter_Unmapped.Open(Outputprefix + "_Unmapped.bam", BamReader.GetHeader(), BamReader.GetReferenceData());
    
    BamTools::BamAlignment Alignment;
    while (BamReader.GetNextAlignment(Alignment)) {
        if (not Alignment.IsMapped() and not Alignment.IsMateMapped()) { 
            OutputWriter_Unmapped.SaveAlignment(Alignment);
            continue;
        }
        BamTools::BamAlignment Alignment_temp = Alignment;
        if (FirstOriInverted and Alignment.IsFirstMate() and Alignment.IsMapped()) Alignment_temp.SetIsReverseStrand(not Alignment.IsReverseStrand());
        if (FirstOriInverted and Alignment.IsSecondMate() and Alignment.IsMateMapped()) Alignment_temp.SetIsMateReverseStrand(not Alignment.IsMateReverseStrand());
        if (SecondOriInverted and Alignment.IsSecondMate() and Alignment.IsMapped()) Alignment_temp.SetIsReverseStrand(not Alignment.IsReverseStrand());
        if (SecondOriInverted and Alignment.IsFirstMate() and Alignment.IsMateMapped()) Alignment_temp.SetIsMateReverseStrand(not Alignment.IsMateReverseStrand());
        
        if (not Alignment_temp.IsMapped()) {
            if ((Alignment_temp.IsSecondMate() and not Alignment_temp.IsMateReverseStrand()) or (Alignment_temp.IsFirstMate() and Alignment_temp.IsMateReverseStrand())) {
                OutputWriter_Singletons_T.SaveAlignment(Alignment);
            } else {
                OutputWriter_Singletons_H.SaveAlignment(Alignment);
            }
        } else if (not Alignment_temp.IsMateMapped()) {
            if ((Alignment_temp.IsSecondMate() and Alignment_temp.IsReverseStrand()) or (Alignment_temp.IsFirstMate() and not Alignment_temp.IsReverseStrand())) {
                OutputWriter_Singletons_T.SaveAlignment(Alignment);
            } else {
                OutputWriter_Singletons_H.SaveAlignment(Alignment);
            }
        } else { // Both reads are mapped
            if (Alignment_temp.RefID == Alignment_temp.MateRefID) {
                string O1, O2;
                if (Alignment_temp.IsFirstMate()) {
                    if (not Alignment_temp.IsReverseStrand()) O1 = "T";
                    else O1 = "H";
                    if (not Alignment_temp.IsMateReverseStrand()) O2 = "H";
                    else O2 = "T";
                }
                if (Alignment_temp.IsSecondMate()) {
                    if (not Alignment_temp.IsReverseStrand()) O2 = "H";
                    else O2 = "T";
                    if (not Alignment_temp.IsMateReverseStrand()) O1 = "T";
                    else O1 = "H";
                }
                // ori check
                if (O1.compare(O2) == 0) { // TT & HH
                    if (O1.compare("H") == 0) OutputWriter_HH.SaveAlignment(Alignment);
                    else OutputWriter_TT.SaveAlignment(Alignment);
                } else { // TH & HT
                    string ori;
                    if (Alignment_temp.Position < Alignment_temp.MatePosition) {
                        if (Alignment_temp.IsFirstMate() == not Alignment_temp.IsReverseStrand()) {
                            ori = "TH";
                        } else {
                            ori = "HT";
                        }
                        //ori = O1 + O2;
                        /*if (O1.compare("H") == 0) {
                            OutputWriter_TH.SaveAlignment(Alignment);
                        }
                        else {
                            OutputWriter_HT.SaveAlignment(Alignment);
                        }*/
                    } else {
                        if (Alignment_temp.IsSecondMate() == not Alignment_temp.IsMateReverseStrand()) {
                            ori = "TH";
                        } else {
                            ori = "HT";
                        }
                        //ori = O2 + O1;
                        /*if (O1.compare("T") == 0) {
                            OutputWriter_TH.SaveAlignment(Alignment);
                        }
                        else {
                            OutputWriter_HT.SaveAlignment(Alignment);
                        }*/
                    }
                    if (ori.compare("TH")) {
                        OutputWriter_TH.SaveAlignment(Alignment);
                    }
                    else {
                        OutputWriter_HT.SaveAlignment(Alignment);
                    }
                }
            } else { // Translocations
                OutputWriter_Translocation.SaveAlignment(Alignment);
            }
        }
    }
    BamReader.Close();
        
    OutputWriter_TH.Close();
    BamReader.Open(Outputprefix + "_TH.bam");
    BamReader.CreateIndex(BamTools::BamIndex::STANDARD);
    BamReader.Close();
    OutputWriter_TT.Close();
    BamReader.Open(Outputprefix + "_TT.bam");
    BamReader.CreateIndex(BamTools::BamIndex::STANDARD);
    BamReader.Close();
    OutputWriter_HH.Close();
    BamReader.Open(Outputprefix + "_HH.bam");
    BamReader.CreateIndex(BamTools::BamIndex::STANDARD);
    BamReader.Close();
    OutputWriter_HT.Close();
    BamReader.Open(Outputprefix + "_HT.bam");
    BamReader.CreateIndex(BamTools::BamIndex::STANDARD);
    BamReader.Close();
    OutputWriter_Translocation.Close();
    BamReader.Open(Outputprefix + "_Translocation.bam");
    BamReader.CreateIndex(BamTools::BamIndex::STANDARD);
    BamReader.Close();
    OutputWriter_Singletons_T.Close();
    BamReader.Open(Outputprefix + "_Singletons_T.bam");
    BamReader.CreateIndex(BamTools::BamIndex::STANDARD);
    BamReader.Close();
    OutputWriter_Singletons_H.Close();
    BamReader.Open(Outputprefix + "_Singletons_H.bam");
    BamReader.CreateIndex(BamTools::BamIndex::STANDARD);
    BamReader.Close();
    OutputWriter_Unmapped.Close();
    BamReader.Open(Outputprefix + "_Unmapped.bam");
    BamReader.CreateIndex(BamTools::BamIndex::STANDARD);
    BamReader.Close();
    
    return 0;
}