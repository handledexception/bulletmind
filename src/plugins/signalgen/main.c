// #include "aja-card-manager.hpp"
// #include <obs-module.h>

// OBS_DECLARE_MODULE()
// OBS_MODULE_USE_DEFAULT_LOCALE("aja", "en-US")

// MODULE_EXPORT const char *obs_module_description(void)
// {
// 	return "aja";
// }

#include "core/types.h"

struct bm_source_info {

};

extern struct bm_source_info create_signalgen_source_info();
struct bm_source_info signalgen_source_info;

// extern struct obs_output_info create_aja_output_info();
// struct obs_output_info aja_output_info;

bool bm_plugin_load(void)
{
	signalgen_source_info = create_signalgen_source_info();
	bm_register_source_plugin(&signalgen_source_info);

	// aja_output_info = create_aja_output_info();
	// obs_register_output(&aja_output_info);

	return true;
}

void bm_plugin_unload(void)
{
}
