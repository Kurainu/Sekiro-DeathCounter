#pragma once
#ifndef DEATHCOUNTER_HPP
#define DEATHCOUNTER_HPP

#include <QtWidgets/QWidget>
#include "ui_deathcounter.h"
#include <QTimer>
#include <windows.h>
#include <tlhelp32.h>
#include <Psapi.h>
#include "obs.h"
#include "plugin-support.h"

#define binfo(format, ...) obs_log(LOG_INFO, format, ##__VA_ARGS__)
#define bwarning(format, ...) obs_log(LOG_WARNING, format, ##__VA_ARGS__)
#define berror(format, ...) obs_log(LOG_ERROR, format, ##__VA_ARGS__)
#define bdebug(format, ...) obs_log(LOG_DEBUG, "DEBUG: " format, ##__VA_ARGS__)

class DeathCounter : public QWidget
{
    Q_OBJECT
public:
    DeathCounter(QWidget *parent = nullptr);
    ~DeathCounter();
    void SetProcessHandle(HANDLE handle);

private:
    Ui::DeathCounterClass ui;
    QTimer* timer;
    HANDLE SekiroProc;
    HANDLE ProcSnapshot;
    HANDLE WaitHandle;
    DWORD64 OffsetPointer;
    DWORD DeathCount;
    void Timertick();

signals:
};

#endif // DEATHCOUNTER_HPP
