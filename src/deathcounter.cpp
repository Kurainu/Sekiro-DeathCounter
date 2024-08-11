#include "deathcounter.hpp"
#include "qeventloop.h"
#include <QDebug>
#include <QDateTime>
#include "utils.hpp"


static QString SEKIRO_PROCESSNAME = "sekiro.exe";

DeathCounter::DeathCounter(QWidget *parent): QMainWindow(parent), ui(new Ui::DeathCounterClass())
{
	ui->setupUi(this);

    ShouldStop = false;
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
{
	SetProcessHandle(NULL);
}

void DeathCounter::SetProcessHandle(HANDLE handle)
{
    SekiroProc = handle;
    ProcSnapshot = NULL;
    WaitHandle = NULL;
    OffsetPointer = NULL;
}

void CALLBACK WaitOrTimerCallback(_In_  PVOID lpParameter, _In_  BOOLEAN TimerOrWaitFired)
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

    if (ShouldStop == true) {
		timer->stop();
	    SetProcessHandle(NULL);
	    return;
	}

    if (SekiroProc != NULL) {
        DWORD64 DeathPointer = 0;
        if(!ReadProcessMemory(SekiroProc, (PBYTE*)OffsetPointer, &DeathPointer, sizeof(DWORD64), 0)) {
            qInfo() << "Could not read the Value Pointed by the Address of (Sekiro.exe Baseadress+0x03D5AAC0)";
        }

        DeathPointer += 0x90; // DeathPointer that points to the actual SaveGame DeathCount

        if (DeathPointer != 0x90) {
		    ui->label_sekiro_process_found->setText("Process Found");
		    ui->label_sekiro_process_found->setStyleSheet("color: green;");

            ReadProcessMemory(SekiroProc, (PBYTE*)DeathPointer, &DeathCount, sizeof(DeathCount), 0);
            qInfo() << QTime::currentTime() << ": " << DeathCount;
	        QString id = ui->combo_obs_tex_source->itemData(ui->combo_obs_tex_source->currentIndex(),Qt::UserRole).value<QString>();
	        SetSourceProperty(id, "text", QString::number(DeathCount));
        }
    }else {

        ui->label_sekiro_process_found->setText("Process not Found");
	    ui->label_sekiro_process_found->setStyleSheet("color: red;");

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

void DeathCounter::AddSources(CreatedEvent data)
{
	binfo("AddSource: %s - %d", data.SourceName.toStdString().c_str(), data.eventtype);
	ui->combo_obs_tex_source->addItem(data.SourceName,data.id);
}

void DeathCounter::RemoveSource(RemovedEvent data)
{
	int index = ui->combo_obs_tex_source->findText(data.SourceName, Qt::MatchContains);
	binfo("[DeathCounter::Remove Source]: %s", data.id.toStdString().c_str());
	ui->combo_obs_tex_source->removeItem(index);
}

void DeathCounter::RenameSources(RenamedEvent data)
{
	int index = ui->combo_obs_tex_source->findText(data.prev_name, Qt::MatchContains);

	binfo("[DeathCounter::Renamed Source]: %d", index);

	ui->combo_obs_tex_source->setItemText(index, ui->combo_obs_tex_source->itemText(index).replace(data.prev_name, data.new_name));
}

void DeathCounter::OBSFrontendExit() {
	ShouldStop = true;
}

void DeathCounter::SetSourceProperty(QString string, QString property, QString value)
{
	OBSSourceAutoRelease obs_source = obs_get_source_by_uuid(string.toStdString().c_str());
	if (!obs_source) { return; }
    if (obs_source_removed(obs_source)) {return;}
	    
    OBSDataAutoRelease source_data = obs_source_get_settings(obs_source);
	obs_data_set_string(source_data, property.toStdString().c_str(), value.toStdString().c_str());
	obs_source_update(obs_source, source_data);
}
