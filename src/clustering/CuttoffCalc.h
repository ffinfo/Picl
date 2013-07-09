/* 
 * File:   CuttoffCalc.h
 * Author: peter
 *
 * Created on March 7, 2013, 3:02 PM
 */

#ifndef CUTTOFFCALC_H
#define	CUTTOFFCALC_H

#include <map>
#include "../Config.h"
#include "../Task.h"

class CuttoffCalc: public Task {
public:
    CuttoffCalc(Config* config);
    virtual ~CuttoffCalc();
    
    void AddClusters(map<unsigned int,unsigned int> &cuttoffCurve);
protected:
    void InternalRun();
    void InternalFinish();
private:
    Config* config;
    map<unsigned int,unsigned int> cuttoffCurve;
};

#endif	/* CUTTOFFCALC_H */

