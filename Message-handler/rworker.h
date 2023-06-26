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

//��������������� �����, ����� �������� �������� ������.
//�������� ������ ��������� �� ����� ������
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
    HANDLE hUpdateEvt; //������� "������ ����������"
    HANDLE hStopEvt;   //������� "����� ������"

    HANDLE hMutex;  //������ ������

    //                                           __
    int number = 1; // ����� �������               |
    std::queue<Message> data_; // ���� ���������   |- ����� ������ ������
    int timer = 5000; // ������ �������� ����      |
    //                                           ~~
    HANDLE hThread; //���������� ������
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
