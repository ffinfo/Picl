/* 
 * File:   Task.cpp
 * Author: peter
 * 
 * Created on January 10, 2013, 12:28 PM
 */

#include "Task.h"
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <stdexcept>

Task::Task(Config* config) {
    this->config = config;
}

Task::~Task() {
}

void Task::Run() {
    if (!IsReadyToRun()) return; 
    if (!IsDoBeforeDone()) return;
    Running  = true;
    try {
        InternalRun();
        Done = true;
    } catch (const std::out_of_range& oor) { // vector::at throws an out-of-range
        string what = oor.what();
        config->Log("[Warning] Out of Range error on task: " + TaskName + "  in:Run  error: " + what);
        errorCount++;
        if (errorCount >= 10) {
            Ready = false;
            Failed = true;
        }
        Running  = false;
    }
    return;
}

void Task::InternalRun() {
    
}

bool Task::IsReadyToFinish() {
    if (not Running or not Done) return false;
    for (Task* task : MustDoneBefore) {
        if (not task->IsDone() or task->IsRunning()) return false;
    }
    for (Task* task : CantRunTogether) {
        if (task->IsRunning()) return false;
    }
    return true;
}

bool Task::Finish() {
    try {
        if (not IsReadyToFinish()) return false;
        InternalFinish();
        config->Log("[Done] " + TaskName);
        if (thr != 0) {
            thr->join();
            delete thr;
        }
        Running = false;
        return true;
    } catch (const std::out_of_range& oor) { // vector::at throws an out-of-range
        string what = oor.what();
        config->Log("[Warning] Out of Range error on task: " + TaskName + "  in:Finish  error: " + what);
        errorCount++;
        if (errorCount >= 10) {
            Ready = false;
            Failed = true;
        }
        return false;
    }
}

void Task::InternalFinish() {
}

bool Task::AdditionalReady() {
    return true;
}

bool Task::isReadingTask() {
    return ReadingTask;
}

string Task::getName() {
    return TaskName;
}

thread* Task::RunAsThread() {
    if (IsReadyToRun()) {
        DoBeforeRun();
        thr = new thread(&Task::Run, this);
        usleep(100);
        return thr;
    } else {
        return 0;
    }
}

bool Task::IsDone() {
    return Done;
}

bool Task::IsRunning() {
    return Running;
}

void Task::SetRunning(bool Running) {
    this->Running = Running;
}

bool Task::IsReadyToRun() {
    for (Task* task : MustDoneBefore) {
        if (not task->IsDone() or task->IsRunning()) return false;
    }
    for (Task* task : CantRunTogether) {
        if (task->IsRunning()) return false;
    }
    if (Ready && !Done && !Running) {
        return AdditionalReady();
    } else {
        return false;
    }
}

void Task::DoBeforeRun(void) {
    try {
        InternalDoBeforeRun();
        DoBeforeDone = true;
    } catch (const std::out_of_range& oor) { // vector::at throws an out-of-range
        string what = oor.what();
        config->Log("[Warning] Out of Range error on task: " + TaskName + "  in:DoBeforeRun  error: " + what);
        errorCount++;
        if (errorCount >= 10) {
            Ready = false;
            Failed = true;
        }
    }
}

void Task::InternalDoBeforeRun(void) {
}

void Task::DoWhilePauze(void) {
    InternalDoWhilePauze();
    Pause = false;
}

void Task::InternalDoWhilePauze() {
}

void Task::PauseThread() {
    if (thr != 0) {
        Pause = true;
        while (Pause) {
            usleep(50);
        }
    } else {
        DoWhilePauze();
    }
}

bool Task::IsPaused() {
    return Pause;
}

bool Task::IsFailed() {
    return Failed;
}

bool Task::IsDoBeforeDone() {
    return DoBeforeDone;
}