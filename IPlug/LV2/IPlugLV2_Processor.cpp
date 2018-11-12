#include "IPlugLV2.h"


static int d_lastBufferSize = 0; // TODO: do something with me
static int d_lastSampleRate = 0; // TODO: do something with me

typedef std::map<const String, String> StringMap;


IPlugLV2Processor:IPlugLV2Processor(IPlugInstanceInfo instanceInfo, IPlugConfig c)
: IPlugAPIBase(c, kAPILV2)
, IPlugProcessor<PLUG_SAMPLE_DST>(c, kAPILV2)
{
}

IPlugLV2Processor::~IPlugLV2Processor()
{
}

static LV2_Handle lv2_instantiate(const LV2_Descriptor*, double sampleRate, const char*, const LV2_Feature* const* features)
{
  const LV2_Options_Option* options = nullptr;
  const LV2_URID_Map* uridMap = nullptr;
  const LV2_Worker_Schedule* worker = nullptr;

  for (int i=0; features[i] != nullptr; ++i)
  {
    if (std::strcmp(features[i]->URI, LV2_OPTIONS__options) == 0)
      options = (const LV2_Options_Option*)features[i]->data;
    else if (std::strcmp(features[i]->URI, LV2_URID__map) == 0)
      uridMap = (const LV2_URID_Map*)features[i]->data;
    else if (std::strcmp(features[i]->URI, LV2_WORKER__schedule) == 0)
      worker = (const LV2_Worker_Schedule*)features[i]->data;
  }

  if (options == nullptr)
  {
    DBGMSG("Options feature missing, cannot continue!");
    return nullptr;
  }

  if (uridMap == nullptr)
  {
    DBGMSG("URID Map feature missing, cannot continue!");
    return nullptr;
  }

// #if PLUGIN_WANT_STATE
//   if (worker == nullptr)
//   {
//     DBGMSG("Worker feature missing, cannot continue!");
//     return nullptr;
//   }
// #endif

  d_lastBufferSize = 0;
  bool usingNominal = false;

  for (int i=0; options[i].key != 0; ++i)
  {
    if (options[i].key == uridMap->map(uridMap->handle, LV2_BUF_SIZE__nominalBlockLength))
    {
      if (options[i].type == uridMap->map(uridMap->handle, LV2_ATOM__Int))
      {
        d_lastBufferSize = *(const int*)options[i].value;
        usingNominal = true;
      }
      else
      {
        DBGMSG("Host provides nominalBlockLength but has wrong value type");
      }
      break;
    }

    if (options[i].key == uridMap->map(uridMap->handle, LV2_BUF_SIZE__maxBlockLength))
    {
      if (options[i].type == uridMap->map(uridMap->handle, LV2_ATOM__Int))
        d_lastBufferSize = *(const int*)options[i].value;
      else
        DBGMSG("Host provides maxBlockLength but has wrong value type");

      // no break, continue in case host supports nominalBlockLength
    }
  }

  if (d_lastBufferSize == 0)
  {
    DBGMSG("Host does not provide nominalBlockLength or maxBlockLength options");
    d_lastBufferSize = 2048;
  }

  d_lastSampleRate = sampleRate;

  return new IPlugLV2Processor(sampleRate, uridMap, worker, usingNominal);
}

#define instancePtr ((PluginLv2*)instance)

static void lv2_connect_port(LV2_Handle instance, uint32_t port, void* dataLocation)
{
  instancePtr->lv2_connect_port(port, dataLocation);
}

static void lv2_activate(LV2_Handle instance)
{
  instancePtr->lv2_activate();
}

static void lv2_run(LV2_Handle instance, uint32_t sampleCount)
{
  instancePtr->lv2_run(sampleCount);
}

static void lv2_deactivate(LV2_Handle instance)
{
  instancePtr->lv2_deactivate();
}

static void lv2_cleanup(LV2_Handle instance)
{
  delete instancePtr;
}

static uint32_t lv2_get_options(LV2_Handle instance, LV2_Options_Option* options)
{
  return instancePtr->lv2_get_options(options);
}

static uint32_t lv2_set_options(LV2_Handle instance, const LV2_Options_Option* options)
{
  return instancePtr->lv2_set_options(options);
}

// #if PLUGIN_WANT_PROGRAMS
// static const LV2_Program_Descriptor* lv2_get_program(LV2_Handle instance, uint32_t index)
// {
//   return instancePtr->lv2_get_program(index);
// }
//
// static void lv2_select_program(LV2_Handle instance, uint32_t bank, uint32_t program)
// {
//   instancePtr->lv2_select_program(bank, program);
// }
// #endif

// #if PLUGIN_WANT_STATE
// static LV2_State_Status lv2_save(LV2_Handle instance, LV2_State_Store_Function store, LV2_State_Handle handle, uint32_t, const LV2_Feature* const*)
// {
//   return instancePtr->lv2_save(store, handle);
// }
//
// static LV2_State_Status lv2_restore(LV2_Handle instance, LV2_State_Retrieve_Function retrieve, LV2_State_Handle handle, uint32_t, const LV2_Feature* const*)
// {
//   return instancePtr->lv2_restore(retrieve, handle);
// }
//
// LV2_Worker_Status lv2_work(LV2_Handle instance, LV2_Worker_Respond_Function, LV2_Worker_Respond_Handle, uint32_t, const void* data)
// {
//   return instancePtr->lv2_work(data);
// }
// #endif

// #if PLUGIN_WANT_DIRECT_ACCESS
// static void* lv2_get_instance_pointer(LV2_Handle instance)
// {
//   return instancePtr->lv2_get_instance_pointer();
// }
// #endif

static const void* lv2_extension_data(const char* uri)
{
  static const LV2_Options_Interface options = { lv2_get_options, lv2_set_options };

  if (std::strcmp(uri, LV2_OPTIONS__interface) == 0)
    return &options;

// #if PLUGIN_WANT_PROGRAMS
//   static const LV2_Programs_Interface programs = { lv2_get_program, lv2_select_program };
//
//   if (std::strcmp(uri, LV2_PROGRAMS__Interface) == 0)
//     return &programs;
// #endif
//
// #if PLUGIN_WANT_STATE
//   static const LV2_State_Interface state = { lv2_save, lv2_restore };
//   static const LV2_Worker_Interface worker = { lv2_work, nullptr, nullptr };
//
//   if (std::strcmp(uri, LV2_STATE__interface) == 0)
//     return &state;
//   if (std::strcmp(uri, LV2_WORKER__interface) == 0)
//     return &worker;
// #endif

// #if PLUGIN_WANT_DIRECT_ACCESS
//   struct LV2_DirectAccess_Interface {
//     void* (*get_instance_pointer)(LV2_Handle handle);
//   };
//
//   static const LV2_DirectAccess_Interface directaccess = { lv2_get_instance_pointer };
//
//   if (std::strcmp(uri, PLUGIN_LV2_STATE_PREFIX "direct-access") == 0)
//     return &directaccess;
// #endif

  return nullptr;
// #endif
}

#undef instancePtr

static const LV2_Descriptor sLv2Descriptor = {
  LV2_PLUGIN_URI,
  lv2_instantiate,
  lv2_connect_port,
  lv2_activate,
  lv2_run,
  lv2_deactivate,
  lv2_cleanup,
  lv2_extension_data
};

extern "C" {
  EXPORT const LV2_Descriptor* lv2_descriptor(uint32_t index)
  {
    return (index == 0) ? &sLv2Descriptor : nullptr;
  }
}