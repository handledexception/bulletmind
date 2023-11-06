#include "game/app.h"
#include "core/logger.h"
#include "core/utils.h"
#include "platform/platform.h"

#ifdef BM_WINDOWS
#include <Windows.h>
#define WIN32_LEAN_AND_MEAN
#endif

#define APP_NAME "Bulletmind"
#define APP_VER_MAJ 1
#define APP_VER_MIN 0
#define APP_VER_REV 0
#define APP_ASSETS_TOML_PATH "assets/assets.toml"
#define VIEW_WIDTH 1280
#define VIEW_HEIGHT 800

int main(int argc, char** argv)
{
	logger_init("bulletmind.log");
	logger(LOG_INFO, "%s %d.%d.%d (built %s %s)", APP_NAME, APP_VER_MAJ,
	       APP_VER_MIN, APP_VER_REV, BM_BUILD_DATE, BM_BUILD_TIME);
	logger(LOG_INFO, "Platform: %s", os_platform_string());
	logger(LOG_INFO, "Architecture: %s (%s)", os_architecture_string(),
	       os_bits_string());
	logger(LOG_INFO, "Compiler: %s", os_compiler_string());
	logger(LOG_INFO, "Pointer size: %zu bytes", sizeof(intptr_t));

	struct application app;
	memset(&app, 0, sizeof(struct application));
	u32 app_ver = pack_version(APP_VER_MAJ, APP_VER_MIN, APP_VER_REV);
	ENSURE_OK(app_init(&app, APP_NAME, app_ver, VIEW_WIDTH, VIEW_HEIGHT,
			   APP_ASSETS_TOML_PATH));
	app_refresh(&app);
	app_shutdown(&app);

	logger_shutdown();
	
	ENSURE_OK(mem_report_leaks());

	return RESULT_OK;
}
