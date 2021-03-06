//
//  Task.h
//  Project Unknown
//
//  Created by Robert Bu on 12/13/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_Task_h
#define Project_Unknown_Task_h

#include "mist/Platform.h"
#include "mist/SSignal.h"
#include "mist/Event.h"
#include "mist/Util.h"

#include "mist/Ptr.h"
#include <list>

namespace mist {
    
    enum TaskNotificationType {
        TN_Started,
        TN_Finished,
        TN_Canceled,
        TN_Progress,
    };
    
    class Task;
    typedef SharedPtr<Task> TaskPtr;
    
    class TaskNotificationEventArgs {
    public:
        TaskNotificationEventArgs(TaskNotificationType type, Task* task):
        mTask(task),
        mType(type) {
        }
        
        TaskNotificationType notificationType() const {
            return mType;
        }
        
        Task* task() const {
            return mTask;
        }
                
    private:
        Task* mTask;
        TaskNotificationType mType;
    };
    
    class TaskManager;
    
    enum TaskState {
        TS_Idle,
        TS_Running,
        TS_Canceling,
        TS_Finished,
        TS_Preparing,
    };
    
    class Task {
    public:
        Task(const MistString& name);
        virtual ~Task();
        
        const MistString& getName() const;
        
        void run();
        void cancel();
        void reset();
        
        virtual void onTaskRun() = 0;
        virtual void onTaskCancel() = 0;
        
        void   reportProgress(float progress);
        float  getProgress() const;
        
        int32 getPriority() const;
        
        TaskState state() const;
        
        TaskManager& getOwner() const;
        
    private:
        friend class TaskManager;
        
        MistString mName;
        
        int32 mPriority;
        float mProgress;
        TaskState mState;
        
        bool mPeriodical;
        TaskManager* mOwner;
    };
    
    inline void WaitForTaskFinish(const TaskPtr& task) {
        while(task->state() == TS_Preparing ||
              task->state() == TS_Running) {
            
        }
    }
    
    class TaskManager {
        friend class Task;
        
        struct TaskInfo;
        
    public:
        typedef Array<TaskInfo>  TaskList;
        typedef Array<TaskInfo>  PendingTaskList;
        typedef Event<TaskNotificationEventArgs> NotificationEvent;
        
    public:
        TaskManager(bool multithreaded=false);
        ~TaskManager();
        
        // start a task, if the task finished, it would be removed from the task list
        void start(const TaskPtr& task, int32 priority, bool periodical=false);
        
        // request to cancel all tasks
        void cancelAll();

    public:
        // add a task to the task list
        // all task would run during the next update cycle unless manually started
        void addTask(const TaskPtr& task, int32 priority, bool periodical=false);
        void removeTask(const TaskPtr& task);
        
        // run a added task manually
        void runTask(const MistString& name);
        
        void onUpdate(float dt);
        
    private:
        void removeTask(Task* task);
        void taskFinished(Task* task);
        void taskStarted(Task* task);
        void taskCanceld(Task* task);
        void taskProgress(Task* task);
        
    public:
        TaskPtr taskByName(const MistString& name) const;
        
        size_t count() const;
        
        // todo
        bool isMultiThreaded() const;
        
        const TaskList& allTasks() const;
        
        NotificationEvent& onTaskNotification();
        
        static TaskManager& DefaultManager(bool multiThreaded=false);
        
    public:
        /**
         * Static Functions
         **/
        
        // start managed in defaultManager
        static void StartTask(const TaskPtr& task, int32 priority, bool periodical=false);
        
    private:
        bool mMultiThread;
        
        TaskList mTasks;
        PendingTaskList mPendingTask;
        
        NotificationEvent mNoficationEvent;
    };
    
} // namespace ukn

#endif
