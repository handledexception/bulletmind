struct bm_source_info create_signalgen_source_info()
{
	struct bm_source_info aja_source_info = {};
	aja_source_info.id = kUIPropCaptureModule.id;
	aja_source_info.type = OBS_SOURCE_TYPE_INPUT;
	aja_source_info.output_flags = OBS_SOURCE_ASYNC_VIDEO |
				       OBS_SOURCE_AUDIO |
				       OBS_SOURCE_DO_NOT_DUPLICATE;
	aja_source_info.get_name = aja_source_get_name;
	aja_source_info.create = aja_source_create;
	aja_source_info.destroy = aja_source_destroy;
	aja_source_info.update = aja_source_update;
	aja_source_info.show = aja_source_show;
	aja_source_info.hide = aja_source_hide;
	aja_source_info.activate = aja_source_activate;
	aja_source_info.deactivate = aja_source_deactivate;
	aja_source_info.get_properties = aja_source_get_properties;
	aja_source_info.get_defaults = aja_source_get_defaults;
	aja_source_info.save = aja_source_save;
	aja_source_info.icon_type = OBS_ICON_TYPE_CAMERA;

	return aja_source_info;
}
