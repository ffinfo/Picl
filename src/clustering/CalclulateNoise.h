/* 
 * File:   CalclulateNoise.h
 * Author: peter
 *
 * Created on May 7, 2013, 1:00 PM
 */

#ifndef CALCLULATENOISE_H
#define	CALCLULATENOISE_H

#include "../Task.h"

class CalclulateNoise : public Task {
public:
    CalclulateNoise(Config* config);
    virtual ~CalclulateNoise();
protected:
    void InternalRun(void);
    void InternalFinish(void);
    void InternalDoWhilePauze(void);
private:

};

#endif	/* CALCLULATENOISE_H */

