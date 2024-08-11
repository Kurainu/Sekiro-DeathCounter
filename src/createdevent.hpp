#pragma once
#include <QString>

class CreatedEvent : public ObsEvent {
public:
	QString SourceName;
	QString id;
};