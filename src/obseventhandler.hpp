#pragma once
#include <QObject>
#include <obs.hpp>
#include <obs-frontend-api.h>
#include "obsevent.hpp"
#include "plugin-support.h"
#include "createdevent.hpp"
#include "renamedevent.hpp"
#include "removedevent.hpp"

#define binfo(format, ...) obs_log(LOG_INFO, format, ##__VA_ARGS__)
#define bwarning(format, ...) obs_log(LOG_WARNING, format, ##__VA_ARGS__)
#define berror(format, ...) obs_log(LOG_ERROR, format, ##__VA_ARGS__)
#define bdebug(format, ...) obs_log(LOG_DEBUG, "DEBUG: " format, ##__VA_ARGS__)

class obseventhandler : public QObject {
	Q_OBJECT
public:
	obseventhandler(QObject *parent = nullptr);
	~obseventhandler();

private:
	void ConnectSceneSignal(obs_source_t *source);
	void ConnectSceneSignal(calldata_t *data);
	bool FrontendLoaded = false;
	static void FireEventFrontend(obs_frontend_event event, void *private_data);
	static bool EnumSceneItems(obs_scene_t *, obs_sceneitem_t *sceneItem, void *param);
	static void OnCreated(void *param, calldata_t *data);
	static void OnRenamed(void *param, calldata_t *data);
	static void OnDestroyed(void *param, calldata_t *data);

signals:
	void OnCreateEvent(CreatedEvent event);
	void OnRemoveEvent(RemovedEvent event);
	void OnRenameEvent(RenamedEvent event);
	void FrontendExit();
};