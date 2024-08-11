#include "obseventhandler.hpp"
#include "utils.hpp"


obseventhandler::obseventhandler(QObject *parent) : QObject(parent)
{
	obs_frontend_add_event_callback(obseventhandler::FireEventFrontend, this);
	signal_handler_t *coreSignalHandler = obs_get_signal_handler();
	if (coreSignalHandler) {
		signal_handler_connect(coreSignalHandler, "source_create", OnCreated, this); // Connect Signal to be able to detect if Scenes are Created
		signal_handler_connect(coreSignalHandler, "source_rename", OnRenamed, this); // Connect Signal to be able for detecting Source object rename
		//signal_handler_connect(coreSignalHandler, "source_destroy", OnDestroyed, this); // Connect Signal to be able for detecting Source object destroy such as Scene objects
	} else {
		berror("[obseventhandler::obseventhandler] Not able to retrieve libobs signal handler!");
	}
}

obseventhandler::~obseventhandler()
{
	obs_frontend_remove_event_callback(obseventhandler::FireEventFrontend, this);

	signal_handler_t *coreSignalHandler = obs_get_signal_handler();
	if (coreSignalHandler) {
		signal_handler_disconnect(coreSignalHandler, "source_create", OnCreated, this); // Connect Signal to be able to detect if Scenes are Created
		signal_handler_disconnect(coreSignalHandler, "source_rename", OnRenamed, this); // Connect Signal to be able to detect if source are got renamed
	} else {
		berror("[obseventhandler::~obseventhandler] Not able to retrieve libobs signal handler!");
	}
}

void obseventhandler::FireEventFrontend(obs_frontend_event event, void *private_data)
{
	auto eventhandler = static_cast<obseventhandler *>(private_data);

	switch (event) {
	case OBS_FRONTEND_EVENT_FINISHED_LOADING: {
		eventhandler->FrontendLoaded = true;
		auto enumScenes = [](void *param, obs_source_t *source) {
			auto eventhandler =static_cast<obseventhandler *>(param);
			eventhandler->ConnectSceneSignal(source);
			obs_scene_enum_items(obs_scene_from_source(source), EnumSceneItems, param);
			return true;
		};
		obs_enum_scenes(enumScenes, private_data);
		break;
	} 
	case OBS_FRONTEND_EVENT_EXIT:
	{
		emit eventhandler->FrontendExit();
		break;
	}
	default:
		break;
	}
}

void obseventhandler::ConnectSceneSignal(obs_source_t *source)
{

	signal_handler_t *sh = obs_source_get_signal_handler(source);

	signal_handler_connect(sh, "item_add", OnCreated, this); // Connect Signal to be able to get the Source and the Scene object when adding
	signal_handler_connect(sh, "item_remove", OnDestroyed, this); // Connect Signal to be able to get the Source and the Scene object when removing
}

void obseventhandler::ConnectSceneSignal(calldata_t *data)
{
	obs_source_t* obs_source = utils::GetSourceCallData(data);

	ConnectSceneSignal(obs_source);
}


bool obseventhandler::EnumSceneItems(obs_scene_t *scene, obs_sceneitem_t *sceneItem, void *param)
{
	struct calldata params;
	uint8_t stack[128];

	calldata_init_fixed(&params, stack, sizeof(stack));
	calldata_set_ptr(&params, "item", sceneItem);
	calldata_set_ptr(&params, "scene", scene);
	//binfo("[obseventhandler::EnumSceneItems] Item: '%p' - scene: '%p'!", sceneItem, scene);
	OnCreated(param, &params);
	return true;
}

void obseventhandler::OnCreated(void *param, calldata_t *data)
{
	obseventhandler *eventhandler = static_cast<obseventhandler *>(param);
	if (!eventhandler->FrontendLoaded) { return; }

	obs_source_t    *obs_source    = utils::GetSourceCallData(data);
	obs_sceneitem_t *obs_sceneitem = utils::GetSceneitemCallData(data);
	obs_scene_t		*obs_scene	   = utils::GetSceneCallData(data);
	
	const char *sourcename = obs_source_get_name(obs_sceneitem_get_source(obs_sceneitem));
	const char *scenename = obs_source_get_name(obs_scene_get_source(obs_scene));
	QString id = QString::fromLatin1(obs_source_get_id(obs_sceneitem_get_source(obs_sceneitem)));
	
	CreatedEvent createdevent{};
	createdevent.eventtype  = ObsEvent::EventType::CREATED;
	createdevent.SourceName = QString("%1\\%2").arg(scenename, sourcename);
	createdevent.id = QString::fromLatin1(obs_source_get_uuid(obs_sceneitem_get_source(obs_sceneitem)));

	if (obs_source && obs_source_is_scene(obs_source)) {
		eventhandler->ConnectSceneSignal(obs_source);
	} else if (id.contains("text_gdiplus")) {
		emit eventhandler->OnCreateEvent(createdevent);
	}
}

void obseventhandler::OnRenamed(void *param, calldata_t *data)
{
	obseventhandler *eventhandler = static_cast<obseventhandler *>(param);

	QString prevn = calldata_string(data, "prev_name");
	QString newn = calldata_string(data, "new_name");

	obs_source_t *obs_source = utils::GetSourceCallData(data);

	if (!obs_source) {return;}
	if (!eventhandler->FrontendLoaded) {return;}

	RenamedEvent renamedevent{};
	renamedevent.eventtype = ObsEvent::EventType::RENAMED;
	renamedevent.new_name  = newn;
	renamedevent.prev_name = prevn;

	switch (obs_source_get_type(obs_source)) {
	case OBS_SOURCE_TYPE_INPUT:
		emit eventhandler->OnRenameEvent(renamedevent);
		break;
	case OBS_SOURCE_TYPE_SCENE:
		emit eventhandler->OnRenameEvent(renamedevent);
		break;
	default:
		break;
	}
}

void obseventhandler::OnDestroyed(void *param, calldata_t *data)
{
	auto eventhandler = static_cast<obseventhandler *>(param);

	binfo("[obseventhandler::OnDestroyed] called!");

	obs_source_t    *obs_source    = utils::GetSourceCallData(data);
	obs_sceneitem_t *obs_sceneitem = utils::GetSceneitemCallData(data);
	obs_scene_t     *obs_scene     = utils::GetSceneCallData(data);

	//if (obs_source_removed(obs_source)) {return;}
	if (!eventhandler->FrontendLoaded) {return;}


	const char *sourcename = obs_source_get_name(obs_sceneitem_get_source(obs_sceneitem));
	const char *scenename = obs_source_get_name(obs_scene_get_source(obs_scene));
	QString id = QString::fromLatin1(obs_source_get_uuid(obs_sceneitem_get_source(obs_sceneitem)));

	RemovedEvent removedevent{};
	removedevent.eventtype  = ObsEvent::EventType::REMOVED;
	removedevent.SourceName = QString("%1\\%2").arg(scenename, sourcename);
	removedevent.id			= id;

	switch (obs_source_get_type(obs_source)) {
	case OBS_SOURCE_TYPE_INPUT:
		emit eventhandler->OnRemoveEvent(removedevent);
		break;
	default:
		break;
	}

}