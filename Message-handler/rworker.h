#pragma once

#define workerH
#include <windows.h>
#include <queue>
#include <string>
#include <ctime>
#include <fstream>

struct Message {
    int number = 0;
    std::wstring line = { 0 };
    time_t time = std::time(NULL);
};

//Вспомогательный класс, очень помогает защищать данные.
//Защищает данные буквально до самой смерти
class RMutexLocker
{
    HANDLE hMutex;
public:
    RMutexLocker(HANDLE mutex)
        :hMutex(mutex)
    {
        WaitForSingleObject(hMutex, INFINITE);
    }
    ~RMutexLocker()
    {
        ReleaseMutex(hMutex);
    }
};

class RWorker
{
    HANDLE hUpdateEvt; //событие "данные обновились"
    HANDLE hStopEvt;   //событие "конец работы"

    HANDLE hMutex;  //защита данных

    //                                           __
    int number = 1; // номер запроса               |
    std::queue<Message> data_; // база сообщений   |- очень важные данные
    int timer = 5000; // таймер проверки базы      |
    //                                           ~~
    HANDLE hThread; //дескриптор потока
public:
    RWorker();
    ~RWorker();

    std::queue<Message> GetQueue();
    bool isStop();
    int GetNumber();
    void SetTimer(int ttimer);
    int GetTime();
    bool Empty();
    void AddMessage(Message message);

    void startWork();
    void stopWork();    

private:
    static DWORD WINAPI ThreadFunc(LPVOID ptr);
};
