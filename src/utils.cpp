#include "utils.hpp"
#include "callback/calldata.h"

obs_source_t* utils::GetSourceCallData(calldata_t* data)
{
    void *ptr = nullptr;
    calldata_get_ptr(data, "source", &ptr);
    obs_source_t *source = static_cast<obs_source_t *>(ptr);
    return source;
}


obs_scene_t *utils::GetSceneCallData(calldata_t *data)
{
    void *ptr = nullptr;
    calldata_get_ptr(data, "scene", &ptr);
    obs_scene_t *scene = static_cast<obs_scene_t *>(ptr);

    return scene;
}

obs_sceneitem_t *utils::GetSceneitemCallData(calldata_t *data)
{
    void *ptr = nullptr;
    calldata_get_ptr(data, "item", &ptr);
    obs_sceneitem_t *scene_item = static_cast<obs_sceneitem_t *>(ptr);
    return scene_item;
}
