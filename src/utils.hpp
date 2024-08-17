#pragma once
#include <QJsonObject>
#include <QString>
#include "callback/calldata.h"
#include "obs.h"
#include "plugin-support.h"

#define binfo(format, ...) obs_log(LOG_INFO, format, ##__VA_ARGS__)
#define bwarning(format, ...) obs_log(LOG_WARNING, format, ##__VA_ARGS__)
#define berror(format, ...) obs_log(LOG_ERROR, format, ##__VA_ARGS__)
#define bdebug(format, ...) obs_log(LOG_DEBUG, "DEBUG: " format, ##__VA_ARGS__)

class utils
{
public:
	static QJsonValue GetNestedJsonValue(QJsonObject object,QString path);
	static obs_source_t* GetSourceCallData(calldata_t* data);
	static obs_scene_t *GetSceneCallData(calldata_t *data);
	static obs_sceneitem_t *GetSceneitemCallData(calldata_t *data);
	static char *GetConfigPath(const char*file);
};

