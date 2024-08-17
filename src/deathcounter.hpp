#pragma once
#ifndef DEATHCOUNTER_HPP
#define DEATHCOUNTER_HPP

#include <QtWidgets/QMainWindow>
#include "ui_deathcounter.h"
#include <QTimer>
#include <windows.h>
#include <tlhelp32.h>
#include <Psapi.h>
#include "obs.hpp"
#include "plugin-support.h"
#include <obsevent.hpp>
#include "createdevent.hpp"
#include "renamedevent.hpp"
#include "removedevent.hpp"

#define binfo(format, ...) obs_log(LOG_INFO, format, ##__VA_ARGS__)
#define bwarning(format, ...) obs_log(LOG_WARNING, format, ##__VA_ARGS__)
#define berror(format, ...) obs_log(LOG_ERROR, format, ##__VA_ARGS__)
#define bdebug(format, ...) obs_log(LOG_DEBUG, "DEBUG: " format, ##__VA_ARGS__)

QT_BEGIN_NAMESPACE
namespace Ui {class DeathCounterClass;};
QT_END_NAMESPACE

class DeathCounter : public QMainWindow
{
    Q_OBJECT
public:
    DeathCounter(QWidget *parent = nullptr);
    ~DeathCounter();
    void SetProcessHandle(HANDLE handle);
    void SetData(obs_data_t *obsdata);
    obs_data_t* GetData();
    void SetSelectedScene();

private:
    Ui::DeathCounterClass *ui;
    QTimer* timer;
    HANDLE SekiroProc;
    HANDLE ProcSnapshot;
    HANDLE WaitHandle;
    DWORD64 OffsetPointer;
    DWORD DeathCount;
    bool ShouldStop = false;
    OBSDataAutoRelease data;
    void Timertick();
    void SaveSettings();

    void SetSourceProperty(QString string, QString property, QString value);


protected:
    virtual void closeEvent(QCloseEvent *event) override;

private slots:
    void Accepted();
    void Rejected();

public slots:
    void AddSources(CreatedEvent data);
    void RemoveSource(RemovedEvent data);
    void RenameSources(RenamedEvent data);
    void OBSFrontendExit();
};

#endif // DEATHCOUNTER_HPP
