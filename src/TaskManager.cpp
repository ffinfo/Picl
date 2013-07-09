/* 
 * File:   TaskMan.cpp
 * Author: peter
 * 
 * Created on February 4, 2013, 6:56 PM
 */

#include "TaskManager.h"
#include <math.h>
#include <fstream>
#include <sstream>

TaskManager::TaskManager(Config* config) {
    //this->Translocations = Translocations;
    this->config = config;
}

TaskManager::~TaskManager() {
}

bool TaskManager::isFinished() {
    if (Finished) {
        return InternalFinished();
    }
    return false;
}

bool TaskManager::InternalFinished() {
    return true;
}

void TaskManager::InternalCheckTasks() {
}

void TaskManager::CheckTasks() {
    RunningReadingTasks = 0;
    completed = 0;
    notDone = 0;
    InternalCheckTasks();
    RunningTasks.clear();
    bool change = false;
    for (Task* task : AllTasks) {
        if (task->Finish()) change = true;
        if (not task->IsDone()) notDone++;
        if (task->IsDone() && not task->IsRunning()) completed++;
        if (task->IsRunning()) {
            RunningTasks.push_back(task);
            if (task->isReadingTask()) RunningReadingTasks++;
            if (task->IsPaused()) task->DoWhilePauze();
        }
    }
    if (change) WriteRunningTasks();
}

void TaskManager::StartTasks() {
    for (Task* task : AllTasks) {
        if (RunningTasks.size() >= config->maxThreads) break;
        if (task->IsReadyToRun()) {
            if (config->maxThreads > 1) {
                if (task->isReadingTask()) {
                        if (RunningReadingTasks >= config->maxReadThreads) continue;
                        else RunningReadingTasks++;
                }
                RunningTasks.push_back(task);
                WriteRunningTasks();
                task->RunAsThread();
            }
            else { //non treaded
                RunningTasks.clear();
                RunningTasks.push_back(task);
                WriteRunningTasks();
                task->DoBeforeRun();
                task->Run();
                task->Finish();
            }
        }
    }
    if (RunningTasks.size() == 0) Finished = true;
}

void TaskManager::WriteTasksNotDone() {
    ofstream file;
    file.open(config->Workspace + "/" + config->FilePrefix + "tasks.notdone", ios::trunc);
    for (Task* task : AllTasks) {
        if (!task->IsDone()) file << task->getName() << endl;
    }
    file.close();
    file.open(config->Workspace + "/" + config->FilePrefix + "tasks.failed", ios::trunc);
    for (Task* task : AllTasks) {
        if (task->IsFailed()) file << task->getName() << endl;
    }
    file.close();
}

void TaskManager::WriteRunningTasks() {
    ofstream file;
    file.open(config->RunningTasksFile, ios::trunc);
    if (RunningTasks.size() > 0) {
        for (Task* task : RunningTasks) {
            file << task->getName() << endl;
        }
    }
    file.close();
}

