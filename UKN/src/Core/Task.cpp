//
//  Task.cpp
//  Project Unknown
//
//  Created by Robert Bu on 12/13/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "UKN/Task.h"
#include "UKN/Common.h"
#include "UKN/Logger.h"

namespace ukn {
    
    Task::Task(const ukn_string& name):
    mName(name),
    mProgress(0),
    mState(TS_Idle),
    mOwner(0),
    mPeriodical(false),
    mPriority(0) {
        
    }
    
    Task::~Task() {
        
    }
    
    const ukn_string& Task::getName() const {
        return mName;
    }

    int32 Task::getPriority() const {
        return mPriority;
    }
    
    void Task::run() {
        ukn_assert(mState != TS_Running && mState != TS_Preparing);
        
        mState = TS_Running;
        mProgress = 0.0;
        
        if(mOwner)
            mOwner->taskStarted(this);
        
        try {
            onTaskRun();
        } catch(std::exception& exp) {
            log_error(format_string("SoraAbstractTask::run: %s", exp.what()));
        }
        
        mState = TS_Finished;
        mProgress = 1.0f;
        
        if(mOwner)
            mOwner->taskFinished(this);
    }
    
    void Task::cancel() {
        if(mState == TS_Running || mState == TS_Preparing) {
            mOwner->taskCanceld(this);
        }
        
        onTaskCancel();
        mState = TS_Canceling;
    }
    
    void Task::reset() {
        if(mState == TS_Running || mState == TS_Preparing)
            mOwner->taskCanceld(this);
        
        mState = TS_Idle;
        mProgress = 0.0f;
    }
    
    void Task::reportProgress(float progress) {
        mProgress = progress;
        
        if(mOwner)
            mOwner->taskProgress(this);
    }
    
    float Task::getProgress() const {
        return mProgress;
    }
    
    TaskState Task::state() const {
        return mState;
    }
    
    struct TaskManager::TaskInfo {
        TaskPtr task;
        int priority;
        
        TaskInfo() { }
        
        TaskInfo(const TaskPtr& ptr, int p):
        task(ptr),
        priority(p) {
       
        };
        
        bool operator < (const TaskInfo& rhs) const {
            return this->priority < rhs.priority;
        }
        
        bool operator > (const TaskInfo& rhs) const {
            return this->priority > rhs.priority;
        }
        
        bool operator != (const TaskInfo& rhs) const {
            return this->priority != rhs.priority;
        }
        
        operator TaskPtr() {
            return task;
        }
    };
    
    TaskManager& Task::getOwner() const {
        return *mOwner;
    }
    
    TaskManager::TaskManager(bool multithreaded):
    mMultiThread(multithreaded) {
        mMultiThread = multithreaded;
    }
    
    TaskManager::~TaskManager() {
        cancelAll();
    }
    
    void TaskManager::start(const TaskPtr& task, int32 priority, bool periodical) {
        if(!task)
            return;
        
        ukn_assert(task->state() != TS_Running && task->state() != TS_Preparing);
        
        task->mPeriodical = periodical;
        
        // avoid add a task already in the task list
        bool isInTaskList = false;
        TaskList::const_iterator it = mTasks.begin();
        TaskList::const_iterator end = mTasks.end();
        for(; it != end; ++it) {
            if(it->task == task) {
                isInTaskList = true;
            }
        }
        if(!isInTaskList) {
            mTasks.push_back(TaskInfo(task, 0));
        }
        
        task->mPriority = priority;
        task->mOwner = this;
        
        task->run();
    }
    
    void TaskManager::cancelAll() {
        TaskList::iterator it = mTasks.begin();
        TaskList::iterator end = mTasks.end();
        
        for(; it != end; ++it) 
            it->task->cancel();
    }

    void TaskManager::addTask(const TaskPtr& task, int32 priority, bool periodical) {
        task->mPeriodical = periodical;
        mPendingTask.insertSorted(TaskInfo(task, 0));
    }
    
    void TaskManager::removeTask(const TaskPtr& task) {
        TaskList::iterator it = mTasks.begin();
        TaskList::iterator end = mTasks.end();
        for(; it != end; ++it) {
            if(it->task == task) {
                mTasks.erase(it);
                break;
            }
        }
    }
    
    void TaskManager::runTask(const ukn_string& name) {
        TaskPtr task = taskByName(name);
        if(task) {
            start(task, task->mPriority);
        } else {
            if(!mPendingTask.empty()) {
                // otherwise find it in the pending task list
                PendingTaskList::iterator it = mPendingTask.begin();
                PendingTaskList::iterator end = mPendingTask.end();
                for(; it != end; ++it) {
                    if(it->task->getName() == name) {
                        start(*it, it->task->mPriority);
                        
                        // remove from pending task list
                        mPendingTask.erase(it);
                    }
                }
            }
        }
    }
    
    void TaskManager::onUpdate(float dt) {
        if(!mPendingTask.empty()) {
            // otherwise find it in the pending task list
            PendingTaskList::const_iterator it = mPendingTask.begin();
            PendingTaskList::const_iterator end = mPendingTask.end();
            for(; it != end; ++it) {
                start(it->task, it->priority);
            }
            
            mPendingTask.clear();
        }
        
        if(!mTasks.empty()) {
            TaskList::const_iterator it = mTasks.begin();
            TaskList::const_iterator end = mTasks.end();
            
            for(; it != end; ++it) {
                Task* task = it->task.get();
                if(task->state() == TS_Finished &&
                   task->mPeriodical) {
                    task->run();
                }
            }
        }
    }
    
    void TaskManager::taskFinished(Task* task) {
        TaskNotification notification(TN_Finished, task);
        
        mNoficationEvent.getEvent()(notification);
        
        // remove the task from the task list
        if(task->mPeriodical == false)
            removeTask(task);
    }
    
    void TaskManager::taskStarted(Task* task) {
        TaskNotification notification(TN_Started, task);
        
        mNoficationEvent.getEvent()(notification);
    }
    
    void TaskManager::taskCanceld(Task* task) {
        TaskNotification notification(TN_Canceled, task);
        
        mNoficationEvent.getEvent()(notification);
    }
    
    void TaskManager::taskProgress(Task* task) {
        TaskNotification notification(TN_Progress, task);
        
        mNoficationEvent.getEvent()(notification);
    }
    
    TaskPtr TaskManager::taskByName(const ukn_string& name) const {
        TaskList::const_iterator it = mTasks.begin();
        TaskList::const_iterator end = mTasks.end();
        
        for(; it != end; ++it)
            if(it->task->getName() == name)
                return it->task;
        
		return TaskPtr();
    }
    
    size_t TaskManager::count() const {
        return mTasks.size();
    }
    
    const TaskManager::TaskList& TaskManager::allTasks() const {
        return mTasks;
    }
    
    TaskManager& TaskManager::DefaultManager(bool multiThreaded) {
        if(!multiThreaded) {
			static TaskManager mTaskManager(false);
            return mTaskManager;
		}
        else {
			static TaskManager mMultiThreadedTaskManager(true);
            return mMultiThreadedTaskManager;
		}
    }
    
    void TaskManager::StartTask(const TaskPtr& task, int32 priority, bool periodical) {
        ukn_assert(task);
        
        DefaultManager(false).start(task, priority, periodical);
    }
    
} // namespace ukn
