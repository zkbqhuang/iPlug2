#ifndef _IPLUGAPI_
#define _IPLUGAPI_

/* IPlugLV2Processor based on DISTRHOPluginLV2.cpp */

/*
 * DISTRHO Plugin Framework (DPF)
 * Copyright (C) 2012-2016 Filipe Coelho <falktx@falktx.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any purpose with
 * or without fee is hereby granted, provided that the above copyright notice and this
 * permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD
 * TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN
 * NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
 * IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */



#include "lv2/atom.h"
#include "lv2/atom-util.h"
#include "lv2/buf-size.h"
#include "lv2/data-access.h"
#include "lv2/instance-access.h"
#include "lv2/midi.h"
#include "lv2/options.h"
#include "lv2/parameters.h"
#include "lv2/state.h"
#include "lv2/time.h"
#include "lv2/urid.h"
#include "lv2/worker.h"
#include "lv2/lv2_programs.h"

#include "IPlugAPIBase.h"
#include "IPlugProcessor.h"

/** Used to pass various instance info to the API class, where needed */
struct IPlugInstanceInfo {};

class IPlugLV2Processor : public IPlugAPIBase
                        , public IPlugProcessor<PLUG_SAMPLE_DST>
{
public:
  IPlugLV2Processor(IPlugInstanceInfo instanceInfo, IPlugConfig c);
  virtual ~IPlugLV2Processor();

  //IPlugProcessor
  bool SendMidiMsg(const IMidiMsg& msg) override { return false; } //TODO: SendMidiMsg

  void lv2_activate();
  void lv2_deactivate();
  void lv2_connect_port(const uint32_t port, void* const dataLocation);
  void lv2_run(const uint32_t sampleCount);
  uint32_t lv2_get_options(LV2_Options_Option* const /*options*/);
  uint32_t lv2_set_options(const LV2_Options_Option* const options);
//   #if PLUGIN_WANT_PROGRAMS
//   const LV2_Program_Descriptor* lv2_get_program(const uint32_t index);
//   void lv2_select_program(const uint32_t bank, const uint32_t program);
//   #endif

//   #if PLUGIN_WANT_STATE
//   LV2_State_Status lv2_save(const LV2_State_Store_Function store, const LV2_State_Handle handle);
//   LV2_State_Status lv2_restore(const LV2_State_Retrieve_Function retrieve, const LV2_State_Handle handle);
//   LV2_Worker_Status lv2_work(const void* const data);
//   #endif
//
//   #if PLUGIN_WANT_DIRECT_ACCESS
//   void* lv2_get_instance_pointer();
//   #endif
private:
  PluginExporter fPlugin;
  const bool fUsingNominal; // if false use maxBlockLength

  // LV2 ports
#if PLUGIN_NUM_INPUTS > 0
  const float* fPortAudioIns[PLUGIN_NUM_INPUTS];
#else
  const float** fPortAudioIns;
#endif
#if PLUGIN_NUM_OUTPUTS > 0
  float* fPortAudioOuts[PLUGIN_NUM_OUTPUTS];
#else
  float** fPortAudioOuts;
#endif
  float** fPortControls;
#if LV2_USE_EVENTS_IN
  LV2_Atom_Sequence* fPortEventsIn;
#endif
#if LV2_USE_EVENTS_OUT
  LV2_Atom_Sequence* fPortEventsOut;
#endif
#if PLUGIN_WANT_LATENCY
  float* fPortLatency;
#endif

  // Temporary data
  float* fLastControlValues;
  double fSampleRate;
#if PLUGIN_WANT_MIDI_INPUT
  MidiEvent fMidiEvents[kMaxMidiEvents];
#endif
#if PLUGIN_WANT_TIMEPOS
  TimePosition fTimePosition;

  struct Lv2PositionData {
    int64_t bar;
    float  barBeat;
    uint32_t beatUnit;
    float  beatsPerBar;
    float  beatsPerMinute;
    int64_t frame;
    double  speed;
    double  ticksPerBeat;

    Lv2PositionData()
      : bar(-1),
       barBeat(-1.0f),
       beatUnit(0),
       beatsPerBar(0.0f),
       beatsPerMinute(0.0f),
       frame(-1),
       speed(0.0),
       ticksPerBeat(-1.0) {}

  } fLastPositionData;
#endif

  // LV2 URIDs
  struct URIDs {
    LV2_URID atomBlank;
    LV2_URID atomObject;
    LV2_URID atomDouble;
    LV2_URID atomFloat;
    LV2_URID atomInt;
    LV2_URID atomLong;
    LV2_URID atomSequence;
    LV2_URID atomString;
    LV2_URID distrhoState;
    LV2_URID midiEvent;
    LV2_URID timePosition;
    LV2_URID timeBar;
    LV2_URID timeBarBeat;
    LV2_URID timeBeatUnit;
    LV2_URID timeBeatsPerBar;
    LV2_URID timeBeatsPerMinute;
    LV2_URID timeTicksPerBeat;
    LV2_URID timeFrame;
    LV2_URID timeSpeed;

    URIDs(const LV2_URID_Map* const uridMap)
      : atomBlank(uridMap->map(uridMap->handle, LV2_ATOM__Blank)),
       atomObject(uridMap->map(uridMap->handle, LV2_ATOM__Object)),
       atomDouble(uridMap->map(uridMap->handle, LV2_ATOM__Double)),
       atomFloat(uridMap->map(uridMap->handle, LV2_ATOM__Float)),
       atomInt(uridMap->map(uridMap->handle, LV2_ATOM__Int)),
       atomLong(uridMap->map(uridMap->handle, LV2_ATOM__Long)),
       atomSequence(uridMap->map(uridMap->handle, LV2_ATOM__Sequence)),
       atomString(uridMap->map(uridMap->handle, LV2_ATOM__String)),
       distrhoState(uridMap->map(uridMap->handle, PLUGIN_LV2_STATE_PREFIX "KeyValueState")),
       midiEvent(uridMap->map(uridMap->handle, LV2_MIDI__MidiEvent)),
       timePosition(uridMap->map(uridMap->handle, LV2_TIME__Position)),
       timeBar(uridMap->map(uridMap->handle, LV2_TIME__bar)),
       timeBarBeat(uridMap->map(uridMap->handle, LV2_TIME__barBeat)),
       timeBeatUnit(uridMap->map(uridMap->handle, LV2_TIME__beatUnit)),
       timeBeatsPerBar(uridMap->map(uridMap->handle, LV2_TIME__beatsPerBar)),
       timeBeatsPerMinute(uridMap->map(uridMap->handle, LV2_TIME__beatsPerMinute)),
       timeTicksPerBeat(uridMap->map(uridMap->handle, LV2_KXSTUDIO_PROPERTIES__TimePositionTicksPerBeat)),
       timeFrame(uridMap->map(uridMap->handle, LV2_TIME__frame)),
       timeSpeed(uridMap->map(uridMap->handle, LV2_TIME__speed)) {}
  } fURIDs;

  // LV2 features
  const LV2_URID_Map* const fUridMap;
  const LV2_Worker_Schedule* const fWorker;
};

#if PLUGIN_WANT_STATE
  StringMap fStateMap;
  bool* fNeededUiSends;
  void setState(const char* const key, const char* const newValue);
#endif
  void updateParameterOutputs();
};

static LV2_Handle lv2_instantiate(const LV2_Descriptor*, double sampleRate, const char*, const LV2_Feature* const* features);
static void lv2_connect_port(LV2_Handle instance, uint32_t port, void* dataLocation);
static void lv2_activate(LV2_Handle instance);
static void lv2_run(LV2_Handle instance, uint32_t sampleCount);
static void lv2_deactivate(LV2_Handle instance);
static void lv2_cleanup(LV2_Handle instance);
static uint32_t lv2_get_options(LV2_Handle instance, LV2_Options_Option* options);
static uint32_t lv2_set_options(LV2_Handle instance, const LV2_Options_Option* options);
static const void* lv2_extension_data(const char* uri);

IPlugLV2* MakePlug();

#endif //_IPLUGAPI_