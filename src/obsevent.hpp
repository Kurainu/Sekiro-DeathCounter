#pragma once

class ObsEvent {
public:
	enum class EventType {UNKNOWN, CREATED, RENAMED, REMOVED};
	EventType eventtype = EventType::UNKNOWN;
};