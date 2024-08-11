#pragma once
#include <QString>

class RemovedEvent : public ObsEvent {
public:
	QString SourceName;
	QString id;
};