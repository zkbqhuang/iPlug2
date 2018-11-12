
static LV2UI_Handle lv2ui_instantiate(const LV2UI_Descriptor*, const char* uri, const char* bundlePath,
                   LV2UI_Write_Function writeFunction, LV2UI_Controller controller, LV2UI_Widget* widget, const LV2_Feature* const* features)
{
  if (uri == nullptr || std::strcmp(uri, PLUGIN_URI) != 0)
  {
    DBGMSG("Invalid plugin URI");
    return nullptr;
  }

  const LV2_Options_Option* options = nullptr;
  const LV2_URID_Map*    uridMap = nullptr;
  const LV2UI_Resize*   uiResize = nullptr;
  const LV2UI_Touch*    uiTouch = nullptr;
  void* parentId = nullptr;
  void* instance = nullptr;

#if PLUGIN_WANT_DIRECT_ACCESS
  struct LV2_DirectAccess_Interface {
    void* (*get_instance_pointer)(LV2_Handle handle);
  };
  const LV2_Extension_Data_Feature* extData = nullptr;
#endif

  for (int i=0; features[i] != nullptr; ++i)
  {
    if (std::strcmp(features[i]->URI, LV2_OPTIONS__options) == 0)
      options = (const LV2_Options_Option*)features[i]->data;
    else if (std::strcmp(features[i]->URI, LV2_URID__map) == 0)
      uridMap = (const LV2_URID_Map*)features[i]->data;
    else if (std::strcmp(features[i]->URI, LV2_UI__resize) == 0)
      uiResize = (const LV2UI_Resize*)features[i]->data;
    else if (std::strcmp(features[i]->URI, LV2_UI__parent) == 0)
      parentId = features[i]->data;
#if PLUGIN_WANT_DIRECT_ACCESS
    else if (std::strcmp(features[i]->URI, LV2_DATA_ACCESS_URI) == 0)
      extData = (const LV2_Extension_Data_Feature*)features[i]->data;
    else if (std::strcmp(features[i]->URI, LV2_INSTANCE_ACCESS_URI) == 0)
      instance = features[i]->data;
#endif
  }

  if (options == nullptr && parentId == nullptr)
  {
    DBGMSG("Options feature missing (needed for show-interface), cannot continue!");
    return nullptr;
  }

  if (uridMap == nullptr)
  {
    DBGMSG("URID Map feature missing, cannot continue!");
    return nullptr;
  }

  if (parentId == nullptr)
  {
    DBGMSG("Parent Window Id missing, host should be using ui:showInterface...");
  }

#if PLUGIN_WANT_DIRECT_ACCESS
  if (extData == nullptr || instance == nullptr)
  {
    DBGMSG("Data or instance access missing, cannot continue!");
    return nullptr;
  }

  if (const LV2_DirectAccess_Interface* const directAccess = (const LV2_DirectAccess_Interface*)extData->data_access(PLUGIN_LV2_STATE_PREFIX "direct-access"))
    instance = directAccess->get_instance_pointer(instance);
  else
    instance = nullptr;

  if (instance == nullptr)
  {
    DBGMSG("Failed to get direct access, cannot continue!");
    return nullptr;
  }
#endif

  const intptr_t winId((intptr_t)parentId);

  if (options != nullptr)
  {
    const LV2_URID uridSampleRate(uridMap->map(uridMap->handle, LV2_PARAMETERS__sampleRate));

    for (int i=0; options[i].key != 0; ++i)
    {
      if (options[i].key == uridSampleRate)
      {
        if (options[i].type == uridMap->map(uridMap->handle, LV2_ATOM__Float))
          d_lastUiSampleRate = *(const float*)options[i].value;
        else
          DBGMSG("Host provides UI sample-rate but has wrong value type");

        break;
      }
    }
  }

  if (d_lastUiSampleRate < 1.0)
  {
    DBGMSG("WARNING: this host does not send sample-rate information for LV2 UIs, using 44100 as fallback (this could be wrong)");
    d_lastUiSampleRate = 44100.0;
  }

  return new UiLv2(bundlePath, winId, options, uridMap, uiResize, uiTouch, controller, writeFunction, widget, instance);
}

#define uiPtr ((UiLv2*)ui)

static void lv2ui_cleanup(LV2UI_Handle ui)
{
  delete uiPtr;
}

static void lv2ui_port_event(LV2UI_Handle ui, uint32_t portIndex, uint32_t bufferSize, uint32_t format, const void* buffer)
{
  uiPtr->lv2ui_port_event(portIndex, bufferSize, format, buffer);
}

static int lv2ui_idle(LV2UI_Handle ui)
{
  return uiPtr->lv2ui_idle();
}

static int lv2ui_show(LV2UI_Handle ui)
{
  return uiPtr->lv2ui_show();
}

static int lv2ui_hide(LV2UI_Handle ui)
{
  return uiPtr->lv2ui_hide();
}

static int lv2ui_resize(LV2UI_Handle ui, int width, int height)
{
  SAFE_ASSERT_RETURN(ui != nullptr, 1);
  SAFE_ASSERT_RETURN(width > 0, 1);
  SAFE_ASSERT_RETURN(height > 0, 1);

  return 1; // This needs more testing
  //return uiPtr->lv2ui_resize(width, height);
}

static uint32_t lv2_get_options(LV2UI_Handle ui, LV2_Options_Option* options)
{
  return uiPtr->lv2_get_options(options);
}

static uint32_t lv2_set_options(LV2UI_Handle ui, const LV2_Options_Option* options)
{
  return uiPtr->lv2_set_options(options);
}

#if PLUGIN_WANT_PROGRAMS
static void lv2ui_select_program(LV2UI_Handle ui, uint32_t bank, uint32_t program)
{
  uiPtr->lv2ui_select_program(bank, program);
}
#endif

static const void* lv2ui_extension_data(const char* uri)
{
  static const LV2_Options_Interface options = { lv2_get_options, lv2_set_options };
  static const LV2UI_Idle_Interface uiIdle = { lv2ui_idle };
  static const LV2UI_Show_Interface uiShow = { lv2ui_show, lv2ui_hide };
  static const LV2UI_Resize     uiResz = { nullptr, lv2ui_resize };

  if (std::strcmp(uri, LV2_OPTIONS__interface) == 0)
    return &options;
  if (std::strcmp(uri, LV2_UI__idleInterface) == 0)
    return &uiIdle;
  if (std::strcmp(uri, LV2_UI__showInterface) == 0)
    return &uiShow;
  if (std::strcmp(uri, LV2_UI__resize) == 0)
    return &uiResz;

#if PLUGIN_WANT_PROGRAMS
  static const LV2_Programs_UI_Interface uiPrograms = { lv2ui_select_program };

  if (std::strcmp(uri, LV2_PROGRAMS__UIInterface) == 0)
    return &uiPrograms;
#endif

  return nullptr;
}

#undef instancePtr

static const LV2UI_Descriptor sLv2UiDescriptor = {
  LV2_UI_URI,
  lv2ui_instantiate,
  lv2ui_cleanup,
  lv2ui_port_event,
  lv2ui_extension_data
};

extern "C"
{
  EXPORT const LV2UI_Descriptor* lv2ui_descriptor(uint32_t index)
  {
    USE_NAMESPACE_DISTRHO
    return (index == 0) ? &sLv2UiDescriptor : nullptr;
  }
}
