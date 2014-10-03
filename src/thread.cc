///// should be named production_line

#include "thread.h"
#include <iostream>

using namespace std;

void ThreadLine::Init(size_t steps, size_t *thread_counts) {
    state_count = steps;
    if (_step_funcs.size() + 1 != state_count) {
        throw "please check step functions.";
    }
    if (!_generate_task_func) {
        throw "please set _generate_task_func.";
    }
    _readwrite = new pthread_mutex_t;
    pthread_mutex_init(_readwrite, NULL);

    state_node_count = new size_t[steps + 1]; // in case an extra state.

    for (size_t i = 0; i < steps; ++i) {
        pthread_cond_t *new_cond = new pthread_cond_t;
        pthread_cond_init(new_cond, NULL);
        thread_cond_pool.push_back(new_cond);
        state_node_count[i] = 0;
    }

    for (size_t i = 0; i < steps; ++i) {
        vector<pthread_t *> step_thread_pool;

        ThreadParameters *tmp_parameters = new ThreadParameters();
        tmp_parameters->threadline = this;
        tmp_parameters->level = i;
        thread_params.push_back(tmp_parameters);

        for (size_t j = 0; j < thread_counts[i]; ++j) {
            int err;
            pthread_t *new_tid = new pthread_t;
            if (i == 0) {
                //cout << "Create TaskAddFunc." << endl;
                err = pthread_create(new_tid, NULL, TaskAddFunc, (void *)tmp_parameters);
            } else if (i == steps - 1) {
                //cout << "Create TaskFinishFunc." << endl;
                err = pthread_create(new_tid, NULL, TaskFinishFunc, (void *)tmp_parameters);
            } else {
                //cout << "Create TaskStepFunc." << endl;
                err = pthread_create(new_tid, NULL, TaskStepFunc, (void *)tmp_parameters);
            }
            if (!err) {
                step_thread_pool.push_back(new_tid);
            } else {
                //cout << "can't create thread" << endl;
            }
        }
        thread_pool.push_back(step_thread_pool);
    }
}

void ThreadLine::Close() {
    for (size_t i = 0; i < thread_pool.size(); ++i) {
        for (size_t j = 0; j < thread_pool.at(i).size(); ++j) {
            pthread_join(*(thread_pool.at(i).at(j)), NULL);
            delete thread_pool.at(i).at(j);
        }
    }

    for (size_t i = 0; i < thread_cond_pool.size(); ++i) {
        pthread_cond_destroy(thread_cond_pool.at(i));
        delete thread_cond_pool.at(i);
        delete thread_params.at(i);
    }

    pthread_mutex_destroy(_readwrite);
    cout << "close end." << endl;
}

void *ThreadLine::TaskAddFunc(void *param) {
    ThreadLine *tmp_this = ((ThreadParameters *)param)->threadline;
    for (;;) {
        while (tmp_this->pool_node_size < tmp_this->max_queue_size) {
            PoolNode *task = tmp_this->_generate_task_func();
            if (!task || !(task->sth)) {
                break;
            }
            pthread_mutex_lock(tmp_this->_readwrite);
            while (tmp_this->pool_node_size >= tmp_this->max_queue_size) {
                pthread_cond_wait(tmp_this->thread_cond_pool.at(0), tmp_this->_readwrite);
            }
            tmp_this->AddTask(task);
            tmp_this->SetCount();
            pthread_mutex_unlock(tmp_this->_readwrite);
            pthread_cond_signal(tmp_this->thread_cond_pool.at(1));
        }
    }
    return NULL;
}

void *ThreadLine::TaskFinishFunc(void *param) {
    ThreadLine *tmp_this = ((ThreadParameters *)param)->threadline;
    size_t level = ((ThreadParameters *)param)->level;
    size_t lower_level = level - 1;
    for (;;) {
        pthread_mutex_lock(tmp_this->_readwrite);
        while (tmp_this->state_node_count[lower_level] == 0) {
            pthread_cond_wait(tmp_this->thread_cond_pool.at(level), tmp_this->_readwrite);
        }
        PoolNode *tmp = tmp_this->AssignOneTask(lower_level);
        tmp_this->SetCount();
        pthread_mutex_unlock(tmp_this->_readwrite);

        tmp_this->_step_funcs.at(level - 1)(tmp);

        tmp->working = false;

        pthread_mutex_lock(tmp_this->_readwrite);
        if (tmp->state >= tmp_this->state_count - 1) {
            tmp_this->RemoveFromQueue(tmp);
            delete tmp;
            tmp_this->SetCount();
        }
        pthread_mutex_unlock(tmp_this->_readwrite);

        pthread_cond_signal(tmp_this->thread_cond_pool.at(0));
    }
    return NULL;
}

void *ThreadLine::TaskStepFunc(void *param) {
    ThreadLine *tmp_this = ((ThreadParameters *)param)->threadline;
    size_t level = ((ThreadParameters *)param)->level;
    size_t lower_level = level - 1;
    for (;;) {
        pthread_mutex_lock(tmp_this->_readwrite);
        while (tmp_this->state_node_count[lower_level] == 0) {
            pthread_cond_wait(tmp_this->thread_cond_pool.at(level), tmp_this->_readwrite);
        }
        PoolNode *tmp = tmp_this->AssignOneTask(lower_level);
        tmp_this->SetCount();
        pthread_mutex_unlock(tmp_this->_readwrite);

        tmp_this->_step_funcs.at(level - 1)(tmp);

        pthread_mutex_lock(tmp_this->_readwrite);
        tmp->working = false;
        tmp_this->SetCount();
        pthread_mutex_unlock(tmp_this->_readwrite);
        pthread_cond_signal(tmp_this->thread_cond_pool.at(level + 1));
    }
    return NULL;
}

void ThreadLine::AddTask(PoolNode *task) {
    AddToQueue(task);
}

void ThreadLine::AddToQueue(PoolNode *node) {
    node->next = NULL;
    node->pre = NULL;
    if (qhead == NULL && qtail == NULL) {
        qhead = qtail = node;
    } else {
        node->pre = qtail;
        qtail->next = node;
        qtail = node;
    }
    ++pool_node_size;
}

void ThreadLine::RemoveFromQueue(PoolNode *node) {
    if (node->pre == NULL && node->next == NULL) {
        qhead = qtail = NULL;
    } else if (node->pre == NULL && node->next != NULL) {
        qhead = node->next;
        qhead->pre = NULL;
    } else if (node->pre != NULL && node->next == NULL) {
        qtail = node->pre;
        qtail->next = NULL;
    } else {
        node->pre->next = node->next;
        node->next->pre = node->pre;
    }
    --pool_node_size;
}

void ThreadLine::SetCount() {
    PoolNode *node = qhead;
    for (size_t i = 0; i < state_count; ++i) {
        state_node_count[i] = 0;
    }
    size_t total = 0;
    while (node) {
        ++total;
        if (node->working == false) {
            state_node_count[node->state] ++;
        }
        node = node->next;
    }
    pool_node_size = total;
}

PoolNode *ThreadLine::AssignOneTask(size_t state) {
    PoolNode *node = qhead;
    while (node) {
        if (node->state == state && node->working == false) {
            node->working = true;
            (node->state) ++;
            // move the selected node to the end.
            if (node->next) {
                RemoveFromQueue(node);
                AddToQueue(node);
            }
            return node;
        }
        node = node->next;
    }
    return NULL;
}
