#include "rworker.h"
using namespace std;

RWorker::RWorker()
{
    //Событие с ручным управлением, изначально активное
    hUpdateEvt = CreateEvent(NULL, TRUE, TRUE, NULL);
    //Событие с ручным управлением, изначально неактивное
    hStopEvt = CreateEvent(NULL, TRUE, TRUE, NULL);
    //Мютекс, пока свободный
    hMutex = CreateMutex(NULL, FALSE, NULL);

    hThread = INVALID_HANDLE_VALUE; //поток пока не запущен
}

RWorker::~RWorker()
{
    stopWork();  //если работали, останавливаемся
    CloseHandle(hUpdateEvt);
    CloseHandle(hStopEvt);
    CloseHandle(hMutex);
}

void RWorker::startWork()
{
    if (hThread != INVALID_HANDLE_VALUE) return; //уже запущен
    ResetEvent(hStopEvt); //перед началом работы сбрасываем флаг остановки
    hThread = CreateThread(NULL, 0, ThreadFunc, this, 0, NULL); //запускаемся в работу
}

void RWorker::stopWork()
{
    if (hThread == INVALID_HANDLE_VALUE) return; //не запущен
    SetEvent(hStopEvt); //взводим флаг остановки
    //ждем, когда поток заметит наш флаг и остановится
    if (WaitForSingleObject(hThread, 5000) != WAIT_OBJECT_0)
    {
        //прождали пять минут, не остановился, "приболел" видимо, пристрелим, чтобы не мучался
        TerminateThread(hThread, 1);
    }
    CloseHandle(hThread);
    hThread = INVALID_HANDLE_VALUE;
}

std::queue<Message> RWorker::GetQueue()
{
    RMutexLocker guard(hMutex);
    auto temp = data_;
    while (!data_.empty()) data_.pop();
    ResetEvent(hUpdateEvt);
    return temp;
}

bool RWorker::isStop()
{
    return WaitForSingleObject(hStopEvt, 0) == WAIT_OBJECT_0;
}

int RWorker::GetNumber() {
    RMutexLocker guard(hMutex);
    return number++;
}

void RWorker::SetTimer(int ttimer) {
    RMutexLocker guard(hMutex);
    timer = ttimer;
}

int RWorker::GetTime() {
    RMutexLocker guard(hMutex);
    return timer;
}

void RWorker::AddMessage(Message message) {
    RMutexLocker guard(hMutex);
    data_.push(message);
}

bool RWorker::Empty() {
    RMutexLocker guard(hMutex);
    return data_.empty();
}

DWORD WINAPI RWorker::ThreadFunc(LPVOID ptr)
{
    RWorker* _this = (RWorker*)ptr;
    if (!_this) return 1; //забыли нам дать объект, не будем работать

    int time = _this->timer;

    for (;;)
    {
        Sleep(time);

        if (!_this->Empty()) {
            auto temp_data = _this->GetQueue();
            while (!temp_data.empty()) {
                wofstream file("messages.txt", ios_base::app);
                struct tm time_push;
                localtime_s(&time_push, &temp_data.front().time);
                file << temp_data.front().number << " " << temp_data.front().line << " ";
                file << time_push.tm_hour << ":" << time_push.tm_min << ":" << time_push.tm_sec << std::endl;
                temp_data.pop();
            }
        }
        //поспим, глядя на флаг конца работы
        if (WaitForSingleObject(_this->hStopEvt, 1000) != WAIT_TIMEOUT)
            break;
        time = _this->GetTime();
    }
    return 0;
}