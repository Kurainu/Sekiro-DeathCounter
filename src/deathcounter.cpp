#include "deathcounter.hpp"
#include "qeventloop.h"
#include <QDebug>
#include <QDateTime>

static QString SEKIRO_PROCESSNAME = "sekiro.exe";

DeathCounter::DeathCounter(QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &DeathCounter::Timertick);
    timer->start(5000);

    SekiroProc = NULL;
    ProcSnapshot = NULL;
    WaitHandle = NULL;
    OffsetPointer = NULL;
    DeathCount = 0;
}

DeathCounter::~DeathCounter() 
{}

void DeathCounter::SetProcessHandle(HANDLE handle)
{
    SekiroProc = handle;
    ProcSnapshot = NULL;
    WaitHandle = NULL;
    OffsetPointer = NULL;
}

void CALLBACK WaitOrTimerCallback(
    _In_  PVOID lpParameter,
    _In_  BOOLEAN TimerOrWaitFired)
{
    Q_UNUSED(TimerOrWaitFired);
    DeathCounter *deathcounter = reinterpret_cast<DeathCounter*>(lpParameter);
    deathcounter->SetProcessHandle(NULL);
}

DWORD64 GetBaseAddress(HANDLE hProcess) {
    if (hProcess == NULL)
        return 0;

    HMODULE lphModule[1024];
    DWORD lpcbNeeded(0);

    if (!EnumProcessModules(hProcess, lphModule, sizeof(lphModule), &lpcbNeeded))
        return 0;

    TCHAR szModName[MAX_PATH];
    if (!GetModuleFileNameEx(hProcess, lphModule[0], szModName, sizeof(szModName) / sizeof(TCHAR)))
        return 0;

    GetModuleFileNameEx(hProcess, lphModule[0], szModName, sizeof(szModName) / sizeof(TCHAR));
    qInfo() << QString().fromWCharArray(szModName) << ": " << QString().setNum(reinterpret_cast<DWORD64>(lphModule[0]),16);

    return reinterpret_cast<DWORD64>(lphModule[0]);
}

void DeathCounter::Timertick(){
    if (SekiroProc != NULL) {
        DWORD64 DeathPointer = 0;
        if(!ReadProcessMemory(SekiroProc, (PBYTE*)OffsetPointer, &DeathPointer, sizeof(DWORD64), 0)) {
            qInfo() << "Could not read the Value Pointed by the Address of (Sekiro.exe Baseadress+0x03D5AAC0)";
        }

        DeathPointer += 0x90; // DeathPointer that points to the actual SaveGame DeathCount

        if (DeathPointer != 0x90) {
            ReadProcessMemory(SekiroProc, (PBYTE*)DeathPointer, &DeathCount, sizeof(DeathCount), 0);
            qInfo() << QTime::currentTime() << ": " << DeathCount;
	        binfo("DeathCount: %d", DeathCount);
        }
    }else {

       ProcSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

        if(ProcSnapshot != NULL)
        {
            PROCESSENTRY32  process;
            process.dwSize = sizeof(PROCESSENTRY32);

            Process32First(ProcSnapshot, &process);
            do
            {
                if (QString().fromWCharArray(process.szExeFile) == SEKIRO_PROCESSNAME) {
                    SetProcessHandle(OpenProcess(PROCESS_VM_READ | SYNCHRONIZE | PROCESS_QUERY_INFORMATION , FALSE, process.th32ProcessID));
                    OffsetPointer = GetBaseAddress(SekiroProc)+ 0x03D5AAC0; // This is the Sekiro.exe BaseAdress + the Offset
                    qInfo() << "After get Base Address"<< QString().setNum(OffsetPointer,16);

                    RegisterWaitForSingleObject(&WaitHandle, SekiroProc, &WaitOrTimerCallback, reinterpret_cast<void*>(this), INFINITE, WT_EXECUTEONLYONCE);
                    break;
                }
            } while(Process32Next(ProcSnapshot, &process));

            CloseHandle(ProcSnapshot);
        }
    }
}


