/* 
 * File:   Task.h
 * Author: peter
 *
 * Created on January 10, 2013, 12:28 PM
 */

#ifndef TASK_H
#define	TASK_H

#include <string>
#include <vector>
#include <thread>
#include <sstream>
#include "Config.h"

using namespace std;

class Task {
public:
    Task(Config* config);
    virtual ~Task();
    
    void Run(void);
    thread* RunAsThread(void);
    virtual void DoBeforeRun(void);
    void DoWhilePauze(void);
    bool IsPaused(void);
    bool IsDone(void);
    bool IsDoBeforeDone(void);
    bool IsRunning(void);
    bool IsReadyToRun(void);
    bool IsFailed(void);
    bool IsReadyToFinish(void);
    void SetRunning(bool Running);
    bool Finish(void);
    string getName();
    bool isReadingTask();
    
    vector<Task*> MustDoneBefore;
    vector<Task*> CantRunTogether;
protected:
    Config* config;
    virtual bool AdditionalReady();
    virtual void InternalFinish(void);
    virtual void InternalRun(void);
    virtual void InternalDoWhilePauze(void);
    virtual void InternalDoBeforeRun(void);
    void PauseThread(void);
    
    string TaskName = "Empty";
    bool ReadingTask = false;
    bool Ready = false;
    bool Done = false;
    bool DoBeforeDone = false;
    bool Running = false;
    bool Pause = false;
    bool Failed = false;
    
    unsigned int errorCount = 0;
    
    thread* thr = 0;
    void Log(string Text);
};

#endif	/* TASK_H */

