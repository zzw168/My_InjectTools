#ifndef THREAD_H
#define THREAD_H

#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

class Thread
{
public:
    Thread();
    virtual ~Thread();

    enum State
    {
        Stoped,     ///<ֹͣ״̬��������δ��������������ֹͣ
        Running,    ///<����״̬
        Paused      ///<��ͣ״̬
    };

    State state() const;

    void start();
    void stop();
    void pause();
    void resume();

protected:
    virtual void process() = 0;

private:
    void run();

private:
    std::thread* _thread;
    std::mutex _mutex;
    std::condition_variable _condition;
    std::atomic_bool _pauseFlag;   ///<��ͣ��ʶ
    std::atomic_bool _stopFlag;   ///<ֹͣ��ʶ
    State _state;
};

#endif // THREAD_H
