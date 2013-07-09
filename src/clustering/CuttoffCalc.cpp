/* 
 * File:   CuttoffCalc.cpp
 * Author: peter
 * 
 * Created on March 7, 2013, 3:02 PM
 */

#include "CuttoffCalc.h"
#include <iostream>
#include <fstream>
#include <math.h>

CuttoffCalc::CuttoffCalc(Config* config) : Task(config) {
    TaskName = "Cuttoff curve calculation";
    Ready = true;
}

CuttoffCalc::~CuttoffCalc() {
}

void CuttoffCalc::AddClusters(map<unsigned int,unsigned int> &cuttoffCurve) {
    for(map<unsigned int,unsigned int>::iterator it = cuttoffCurve.begin(); it != cuttoffCurve.end(); ++it) {
        this->cuttoffCurve[it->first] += it->second;
    }
}

void CuttoffCalc::InternalRun() {
    unsigned long int total = 0;
    //map<unsigned int,unsigned int> curve;
    for(map<unsigned int,unsigned int>::iterator it = cuttoffCurve.begin(); it != cuttoffCurve.end(); ++it) {
        total += it->second;
    }
    unsigned long int left = total;
    map<unsigned int,unsigned int> cumilative;
    for(map<unsigned int,unsigned int>::iterator it = cuttoffCurve.begin(); it != cuttoffCurve.end(); ++it) {
        cout << it->first << " - " << left << endl;
        cumilative[it->first] = left;
        left -= it->second;
    }
    
    double first_mx = 0, first_my = 0, first_mxy = 0, first_mx2 = 0;
    int first_count = 0;
    double second_mx = 0, second_my = 0, second_mxy = 0, second_mx2 = 0;
    int second_count = 0;
    
    for(map<unsigned int,unsigned int>::iterator it = cumilative.begin(); it != cumilative.end(); ++it) {
        int cutoff = it->first;
        double leftc = log10(double(it->second));
        if (cutoff < 5) {
            first_mx += double(cutoff);
            first_my += double(leftc);
            first_mxy += (double(cutoff) * double(leftc));
            first_mx2 += (double(cutoff) * double(cutoff));
            first_count++;
        }
        if (cutoff > 5 && cutoff <= 20) {
            second_mx += double(cutoff);
            second_my += double(leftc);
            second_mxy += (double(cutoff) * double(leftc));
            second_mx2 += (double(cutoff) * double(cutoff));
            second_count++;
        }
    }
    first_mx = first_mx / double(first_count);
    first_my = first_my / double(first_count);
    first_mxy = first_mxy / double(first_count);
    first_mx2 = first_mx2 / double(first_count);
    
    second_mx = second_mx / double(second_count);
    second_my = second_my / double(second_count);
    second_mxy = second_mxy / double(second_count);
    second_mx2 = second_mx2 / double(second_count);
    
    double first_b = (first_mxy - (first_my * first_mx)) / (first_mx2 - (first_mx * first_mx));
    double first_a = first_my - (first_b * first_mx);
    double second_b = (second_mxy - (second_my * second_mx)) / (second_mx2 - (second_mx * second_mx));
    double second_a = second_my - (second_b * second_mx);
    
    double threshold = (second_a - first_a) / (first_b - second_b);
    
    cout << "First a: " << first_a << "  b: " << first_b << endl;
    cout << "Second a: " << second_a << "  b: " << second_b << endl;
    cout << "Threshold: " << threshold << endl;
}

void CuttoffCalc::InternalFinish() {
    
}