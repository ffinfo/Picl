/* 
 * File:   PairedBamMakerMain.cpp
 * Author: peter
 * 
 * Created on February 19, 2013, 2:12 PM
 */

#include "PairedBamMakerMain.h"
#include <api/SamProgram.h>

PairedBamMakerMain::PairedBamMakerMain() {
}

PairedBamMakerMain::~PairedBamMakerMain() {
}

int PairedBamMakerMain::usage() {
    cerr << "Usage: <name program> pairedbammaker <options> [-first (forward input BAM)] [-second (reverse input BAM)] <-output (output BAM)>" << endl;
    cerr << " Options: " << endl;
    cerr << "   -nametrim <text to trim>: trim read name and readgroup, can be used more then 1 time" << endl;
    cerr << "   -namereplace <text to trim> <replace for>: trim read name and readgroup and replace it for something else, can be used more then 1 time" << endl;
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
    cerr << "   -assumesorted : Assume sorted on queryname even if header say it's not" << endl;
    cerr << "   -overwriteoutputfile : Continue even if outputfile already consist, Warning, outputfile will be truncated" << endl;
    cerr << "   -addreadgrouptoheader : add readgroup to header" << endl;
    return 1;
}

struct ReadGroupSortStorage {
    BamTools::BamAlignment alignment;
    bool found = false;
    string RG;
};

int PairedBamMakerMain::main(int argc, char* argv[]) {
    if (argc < 3) return this->usage();
    for (int t = 1; t < (argc); t++) {
        if (strcmp(argv[t], "-first") == 0 or strcmp(argv[t], "--cuttoff") == 0) {
            FirstInputBamFile = argv[t+1];
            t++;
        } else if (strcmp(argv[t], "-second") == 0) {
            SecondInputBamFile = argv[t+1];
            t++;
        } else if (strcmp(argv[t], "-output") == 0) {
            OutputBamFile = argv[t+1];
            t++;
        } else if (strcmp(argv[t], "-nametrim") == 0) {
            readNameConverter.NameReplace->push_back({argv[t+1], ""});
            t++;
        } else if (strcmp(argv[t], "-namereplace") == 0) {
            readNameConverter.NameReplace->push_back({argv[t+1], argv[t+2]});
            t += 2;
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
        } else if (strcmp(argv[t], "-addreadgrouptoheader") == 0) {
            addReadgroupToHeader.push_back(argv[t+1]);
            t++;
        } else if (strcmp(argv[t], "-assumesorted") == 0) {
            AsumeSortedOnQueryName = true;
        } else if (strcmp(argv[t], "-overwriteoutputfile") == 0) {
            OverwriteOutputfile = true;
        }
    }
    if (FirstInputBamFile.empty() or SecondInputBamFile.empty() or OutputBamFile.empty()) {
        if (FirstInputBamFile.empty()) cerr << "[Error] First bam file is missing" << endl;
        if (SecondInputBamFile.empty()) cerr << "[Error] Second bam file is missing" << endl;
        if (OutputBamFile.empty()) cerr << "[Error] Output bam file is missing" << endl;
        return usage();
    } else {
        fstream outputfile;
        outputfile.open(OutputBamFile, ios::in);
        if (outputfile.is_open()) {
            if (OverwriteOutputfile) cerr << "[Warning] Outputfile already exsist, file will be truncated" << endl;
            else {
                cerr << "[Error] Outputfile already exsist" << endl;
                exit(1);
            }
        }
        outputfile.close();
    }
    
    cout << "FirstInputBamFile: " << FirstInputBamFile << endl;
    cout << "SecondInputBamFile: " << SecondInputBamFile << endl;
    cout << "OutputBamFile: " << OutputBamFile << endl;
    cout << "FirstOriInverted: " << FirstOriInverted << endl;
    cout << "SecondOriInverted: " << SecondOriInverted << endl;
    cout << "CorrectFirst: " << CorrectOriFirst << endl;
    cout << "CorrectSecond: " << CorrectOriSecond << endl;
    for (ReadNameConverter::ReplaceNameArg trim : *readNameConverter.NameReplace) {
        cout << "Trim: " << trim.Trim;
        if (not trim.Replace.empty()) cout << " Will be replaced for " << trim.Replace;
        cout << endl;
    }
    
    BamTools::RefVector Refs;
    BamTools::SamHeader Header;
    Header.SortOrder = "unsorted";
    Header.Comments.push_back("Paired by Picl");
    Header.Version = "1.4";
    
    FirstBamReader.Open(FirstInputBamFile);
    if (FirstBamReader.IsOpen()) {
        for (BamTools::RefData Ref : FirstBamReader.GetReferenceData()) {
            Refs.push_back(Ref);
        }
        BamTools::SamHeader FirstHeader = FirstBamReader.GetHeader();
        if (FirstHeader.SortOrder.compare("queryname") != 0) {
            if (AsumeSortedOnQueryName) cerr << "[Warning] First bamfile is not sorted on queryname, ignoring because '-assumesorted' option" << endl;
            else {
                cerr << "[Error] First bamfile is not sorted on queryname" << endl;
                exit(1);
            }
        }
        for (BamTools::SamReadGroupIterator it = FirstHeader.ReadGroups.Begin(); it != FirstHeader.ReadGroups.End(); it++) {
            BamTools::SamReadGroup* readgroup = &*it;
            readNameConverter.TrimName(readgroup->ID);
            if (not Header.ReadGroups.Contains(*readgroup)) Header.ReadGroups.Add(*readgroup);
        }
        for (BamTools::SamProgramIterator programIt = FirstHeader.Programs.Begin(); programIt != FirstHeader.Programs.End(); programIt++) {
            if (not Header.Programs.Contains(*programIt)) Header.Programs.Add(*programIt);
        }
        Header.Comments.insert(Header.Comments.end(), FirstHeader.Comments.begin(), FirstHeader.Comments.end());
    } else {
        cerr << "[Error] FirstInputBamFile could not be opened. Location of file: " << FirstInputBamFile << endl;
        exit(1);
    }
    SecondBamReader.Open(SecondInputBamFile);
    if (SecondBamReader.IsOpen()) {
        BamTools::SamHeader SecondHeader = SecondBamReader.GetHeader();
        if (SecondHeader.SortOrder.compare("queryname") != 0) {
            if (AsumeSortedOnQueryName) cerr << "[Warning] Second bamfile is not sorted on queryname, ignoring because '-assumesorted' option" << endl;
            else {
                cerr << "[Error] Second bamfile is not sorted on queryname" << endl;
                exit(1);
            }
        }
        for (BamTools::SamReadGroupIterator it = SecondHeader.ReadGroups.Begin(); it != SecondHeader.ReadGroups.End(); it++) {
            BamTools::SamReadGroup* readgroup = &*it;
            readNameConverter.TrimName(readgroup->ID);
            if (not Header.ReadGroups.Contains(*readgroup)) Header.ReadGroups.Add(*readgroup);
        }
        for (BamTools::SamProgramIterator programIt = SecondHeader.Programs.Begin(); programIt != SecondHeader.Programs.End(); programIt++) {
            if (not Header.Programs.Contains(*programIt)) Header.Programs.Add(*programIt);
        }
        Header.Comments.insert(Header.Comments.end(), SecondHeader.Comments.begin(), SecondHeader.Comments.end());
    } else {
        cerr << "[Error] SecondInputBamFile could not be opened. Location of file: " << SecondInputBamFile << endl;
        exit(1);
    }
    
    for (string readgroup : addReadgroupToHeader) {
        if (not Header.Programs.Contains(readgroup)) Header.ReadGroups.Add(readgroup);
    }
    
    BamTools::SamProgram program;
    program.ID = "picl";
    program.Name = "Picl";
    program.CommandLine += "picl ";
    for (int t = 0; t < argc; t++) program.CommandLine += (string)argv[t] + " ";
    Header.Programs.Add(program);
    
    OutputWriter.Open(OutputBamFile, Header, Refs);
    
    BamTools::BamAlignment FirstAlignment;
    BamTools::BamAlignment SecondAlignment;
    bool FirstFound = ReadAlignment(FirstBamReader, FirstAlignment, true);
    if (FirstFound && CorrectOriFirst && FirstOriInverted) FirstAlignment.SetIsReverseStrand(not FirstAlignment.IsReverseStrand());
    bool SecondFound = ReadAlignment(SecondBamReader, SecondAlignment, false);
    if (SecondFound && CorrectOriSecond && SecondOriInverted) SecondAlignment.SetIsReverseStrand(not SecondAlignment.IsReverseStrand());
    
    string firstRG, secondRG;
    while (FirstFound and SecondFound) {
        int compareRG = 0;
        if (FirstAlignment.GetTag("RG", firstRG) && SecondAlignment.GetTag("RG", secondRG)) {
            compareRG = firstRG.compare(secondRG);
        }
        int compare = FirstAlignment.Name.compare(SecondAlignment.Name);
        if (compare == 0) {
            if (compareRG == 0) {
                WritePair(FirstAlignment, SecondAlignment);

                FirstFound = ReadAlignment(FirstBamReader, FirstAlignment, true);
                if (FirstFound && CorrectOriFirst && FirstOriInverted) FirstAlignment.SetIsReverseStrand(not FirstAlignment.IsReverseStrand());
                SecondFound = ReadAlignment(SecondBamReader, SecondAlignment, false);
                if (SecondFound && CorrectOriSecond && SecondOriInverted) SecondAlignment.SetIsReverseStrand(not SecondAlignment.IsReverseStrand());
            } else {
                vector<ReadGroupSortStorage> FirstAlignmentNotPaired, SecondAlignmentNotPaired;
                string FirstName = FirstAlignment.Name;
                string SecondName = SecondAlignment.Name;
                while (FirstFound && FirstAlignment.Name.compare(FirstName) == 0) {
                    ReadGroupSortStorage read;
                    read.alignment = FirstAlignment;
                    if (FirstAlignment.GetTag("RG", read.RG)) {
                        
                    } else {
                        cerr << "ReadGroup was there but now it's gone!!!" << endl; // This should never be possible
                        exit(1);
                    }
                    FirstAlignmentNotPaired.push_back(read);
                    FirstFound = ReadAlignment(FirstBamReader, FirstAlignment, true);
                    if (FirstFound && CorrectOriFirst && FirstOriInverted) FirstAlignment.SetIsReverseStrand(not FirstAlignment.IsReverseStrand());
                }
                while (SecondFound && SecondAlignment.Name.compare(SecondName) == 0) {
                    ReadGroupSortStorage read;
                    read.alignment = SecondAlignment;
                    if (SecondAlignment.GetTag("RG", read.RG)) {
                        
                    } else {
                        cerr << "ReadGroup was there but now it's gone!!!" << endl; // This should never be possible
                        exit(1);
                    }
                    SecondAlignmentNotPaired.push_back(read);
                    SecondFound = ReadAlignment(SecondBamReader, SecondAlignment, false);
                    if (SecondFound && CorrectOriSecond && SecondOriInverted) SecondAlignment.SetIsReverseStrand(not SecondAlignment.IsReverseStrand());
                }
                for (ReadGroupSortStorage &First : FirstAlignmentNotPaired) {
                    for (ReadGroupSortStorage &Second : SecondAlignmentNotPaired) {
                        if (First.RG.compare(Second.RG) == 0) {
                            WritePair(First.alignment, Second.alignment);
                            First.found = true;
                            Second.found = true;
                        }
                    }
                }
                for (ReadGroupSortStorage &First : FirstAlignmentNotPaired) {
                    if (not First.found) {
                        //First.alignment.SetIsFirstMate(true);
                        First.alignment.SetIsPaired(false);
                        First.alignment.Name = First.alignment.Name + "/1";
                        WriteAlignment(First.alignment);
                    }
                }
                for (ReadGroupSortStorage &Second : SecondAlignmentNotPaired) {
                    if (not Second.found) {
                        //Second.alignment.SetIsSecondMate(true);
                        Second.alignment.SetIsPaired(false);
                        Second.alignment.Name = Second.alignment.Name + "/2";
                        WriteAlignment(Second.alignment);
                    }
                }
            }
        } else if (compare < 0) {
            //FirstAlignment.SetIsFirstMate(true);
            FirstAlignment.SetIsPaired(false);
            FirstAlignment.Name = FirstAlignment.Name + "/1";
            WriteAlignment(FirstAlignment);
            FirstFound = ReadAlignment(FirstBamReader, FirstAlignment, true);
            if (FirstFound && CorrectOriFirst && FirstOriInverted) FirstAlignment.SetIsReverseStrand(not FirstAlignment.IsReverseStrand());
        } else {
            //SecondAlignment.SetIsSecondMate(true);
            SecondAlignment.SetIsPaired(false);
            SecondAlignment.Name = SecondAlignment.Name + "/2";
            WriteAlignment(SecondAlignment);
            SecondFound = ReadAlignment(SecondBamReader, SecondAlignment, false);
            if (SecondFound && CorrectOriSecond && SecondOriInverted) SecondAlignment.SetIsReverseStrand(not SecondAlignment.IsReverseStrand());
        }
    }
    while (FirstFound and not SecondFound) {
        FirstAlignment.SetIsPaired(false);
        FirstAlignment.Name = FirstAlignment.Name + "/1";
        WriteAlignment(FirstAlignment);
        FirstFound = ReadAlignment(FirstBamReader, FirstAlignment, true);
        if (FirstFound && CorrectOriFirst && FirstOriInverted) FirstAlignment.SetIsReverseStrand(not FirstAlignment.IsReverseStrand());
    }
    while (not FirstFound and SecondFound) {
        SecondAlignment.SetIsPaired(false);
        SecondAlignment.Name = SecondAlignment.Name + "/2";
        WriteAlignment(SecondAlignment);
        SecondFound = ReadAlignment(SecondBamReader, SecondAlignment, false);
        if (SecondFound && CorrectOriSecond && SecondOriInverted) SecondAlignment.SetIsReverseStrand(not SecondAlignment.IsReverseStrand());
    }
    
    OutputWriter.Close();
    
    return 0;
}

bool PairedBamMakerMain::ReadAlignment(BamTools::BamReader& reader, BamTools::BamAlignment& alignment, bool first) {
    bool oke = reader.GetNextAlignment(alignment);
    readNameConverter.TrimName(alignment.Name);
    //if (first) alignment.SetIsFirstMate(true);
    //else alignment.SetIsSecondMate(true);
    string RG;
    if (alignment.GetTag("RG", RG)) {
        readNameConverter.TrimName(RG);
        alignment.EditTag("RG", "Z", RG);
    }
    return oke;
}

bool PairedBamMakerMain::WriteAlignment(BamTools::BamAlignment &alignment) {
    string RG;
    if (alignment.GetTag("RG", RG)) {
        unsigned int lanefind = RG.find("lane");
        if (lanefind < RG.length()) {
            alignment.Name = RG.substr(lanefind+4, 1) + "." + alignment.Name;
        }
    }
    return OutputWriter.SaveAlignment(alignment);
}

void PairedBamMakerMain::WritePair(BamTools::BamAlignment &FirstAlignment, BamTools::BamAlignment &SecondAlignment) {
    FirstAlignment.MatePosition = SecondAlignment.Position;
    FirstAlignment.MateRefID = SecondAlignment.RefID;
    FirstAlignment.SetIsMateMapped(SecondAlignment.IsMapped());
    if (SecondAlignment.IsReverseStrand()) FirstAlignment.SetIsMateReverseStrand(true);
    FirstAlignment.SetIsPaired(true);
    FirstAlignment.SetIsFirstMate(true);

    SecondAlignment.MatePosition = FirstAlignment.Position;
    SecondAlignment.MateRefID = FirstAlignment.RefID;
    SecondAlignment.SetIsMateMapped(FirstAlignment.IsMapped());
    if (FirstAlignment.IsReverseStrand()) SecondAlignment.SetIsMateReverseStrand(true);
    SecondAlignment.SetIsPaired(true);
    SecondAlignment.SetIsSecondMate(true);

    if (FirstAlignment.IsMapped() && FirstAlignment.RefID == SecondAlignment.RefID) {
        //FirstAlignment.InsertSize = SecondAlignment.Position - FirstAlignment.Position + FirstAlignment.Length;
        //SecondAlignment.InsertSize = FirstAlignment.Position - SecondAlignment.Position + SecondAlignment.Length;
        int32_t InsertSize;
        if (FirstAlignment.Position < SecondAlignment.Position) InsertSize = SecondAlignment.Position + SecondAlignment.Length - FirstAlignment.Position;
        else InsertSize = -1 * (FirstAlignment.Position + FirstAlignment.Length - SecondAlignment.Position);
        FirstAlignment.InsertSize = InsertSize;
        SecondAlignment.InsertSize = -1 * InsertSize;
        bool FirstOri = not FirstAlignment.IsReverseStrand();
        if (FirstOriInverted and not CorrectOriFirst) FirstOri = not FirstOri;
        bool SecondOri = not SecondAlignment.IsReverseStrand();
        if (SecondOriInverted and not CorrectOriSecond) SecondOri = not SecondOri;
        if ((FirstOri && SecondOri 
                && FirstAlignment.Position < SecondAlignment.Position
                ) || 
                (not FirstOri && not SecondOri 
                && FirstAlignment.Position > SecondAlignment.Position
                )) {
            FirstAlignment.SetIsProperPair(true);
            SecondAlignment.SetIsProperPair(true);
        }
    }

    WriteAlignment(FirstAlignment);
    WriteAlignment(SecondAlignment);
}