#pragma once
#include <QString>

class RenamedEvent : public ObsEvent {
public:
	QString prev_name;
	QString new_name;
};