/*
Plugin Name
Copyright (C) <Year> <Developer> <Email Address>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program. If not, see <https://www.gnu.org/licenses/>
*/

#include <obs-module.h>
#include <obs-frontend-api.h>
#include <plugin-support.h>
#include <deathcounter.hpp>
#include <QMainWindow>

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE(PLUGIN_NAME, "en-US")

DeathCounter *_deathcounter = nullptr;


bool obs_module_load(void)
{
	obs_log(LOG_INFO, "plugin loaded successfully (version %s)", PLUGIN_VERSION);
	const auto menu_action = static_cast<QAction *>(obs_frontend_add_tools_menu_qaction(obs_module_text("Sekiro DeathCounter")));
	obs_frontend_push_ui_translation(obs_module_get_string);
	const auto main_window = static_cast<QMainWindow *>(obs_frontend_get_main_window());
	obs_frontend_pop_ui_translation();
	_deathcounter = new DeathCounter(main_window);
	const auto menu_cb = [] {
		if (!_deathcounter->isVisible())
			_deathcounter->setVisible(true);
		else
			_deathcounter->setVisible(false);
	};

	QAction::connect(menu_action, &QAction::triggered, menu_cb);	
	return true;
}

void obs_module_unload(void)
{
	obs_log(LOG_INFO, "plugin unloaded");
}
