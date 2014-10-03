/**
 * (C) Copyright 2014.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the GNU Lesser General Public License
 * (LGPL) version 3.0 which accompanies this distribution, and is available at
 * http://www.gnu.org/licenses/lgpl-3.0.html
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * Contributors:
 *     whentp <tpsmsproject@gmail.com>
 */

#ifndef _THREAD_LINE_H_
#define _THREAD_LINE_H_

#include <pthread.h>
#include <vector>
#include <list>
using namespace std;

struct PoolNode {
    PoolNode *pre;
    PoolNode *next;
    size_t state;
    bool working;
    size_t *sth;
    PoolNode() {
        sth = NULL;
    }
    ~PoolNode() {
        if (sth) delete[] sth;
    }
};

typedef PoolNode *(*GenerateTaskFunc)(void);
typedef void (*StepFunc)(PoolNode *);

class ThreadLine;

struct ThreadParameters {
    ThreadLine *threadline;
    size_t level;
};

class ThreadLine {
private:
    // queue implementation.
    PoolNode *qhead = NULL;
    PoolNode *qtail = NULL;

    size_t pool_node_size;
    size_t *state_node_count;
    size_t state_count;
    void AddToQueue(PoolNode *node);
    void RemoveFromQueue(PoolNode *node);
    void AddTask(PoolNode *task);
    void SetCount();
    PoolNode *AssignOneTask(size_t state);

    size_t max_queue_size = 5;
    vector<vector<pthread_t *> > thread_pool;
    pthread_mutex_t *_readwrite;
    vector<pthread_cond_t *> thread_cond_pool;
    vector<ThreadParameters *> thread_params;

    // phread functions. the param should be ThreadParameters type.
    static void *TaskAddFunc(void *param);
    static void *TaskFinishFunc(void *param);
    static void *TaskStepFunc(void *param);
public:
    GenerateTaskFunc _generate_task_func;
    vector<StepFunc> _step_funcs;
    ThreadLine(size_t queue_size) {
        _generate_task_func = NULL;
        state_count = 0;
        pool_node_size = 0;
        max_queue_size = queue_size;
    }
    void AddStepFunc(StepFunc tmp) {
        _step_funcs.push_back(tmp);
    }
    void SetGenerateFunc(GenerateTaskFunc tmp) {
        _generate_task_func = tmp;
    }
    void Init(size_t steps, size_t *thread_counts);
    void Close();
};

#endif