#ifndef _IPLUGAPI_
#define _IPLUGAPI_

/* IPlugLV2Editor based on DISTRHOLV2UI.cpp */

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
#include "lv2/data-access.h"
#include "lv2/instance-access.h"
#include "lv2/options.h"
#include "lv2/parameters.h"
#include "lv2/ui.h"
#include "lv2/urid.h"
#include "lv2/lv2_programs.h"

#include "IPlugAPIBase.h"

/** Used to pass various instance info to the API class, where needed */
struct IPlugInstanceInfo {};

class IPlugLV2Editor : public IPlugAPIBase
{
public:
  IPlugLV2Editor(IPlugInstanceInfo instanceInfo, IPlugConfig c);
  virtual ~IPlugLV2Editor();

  //IPlugAPIBase
  void BeginInformHostOfParamChange(int idx) override { /* TODO: */}
  void InformHostOfParamChange(int idx, double normalizedValue) override { /* TODO: */}
  void EndInformHostOfParamChange(int idx) override { /* TODO: */}
  void InformHostOfProgramChange() override { /* TODO: */}
  void ResizeGraphics() override { /* TODO: */ };

  void lv2ui_port_event(const uint32_t rindex, const uint32_t bufferSize, const uint32_t format, const void* const buffer);

  int lv2ui_idle();
  int lv2ui_show();
  int lv2ui_hide();
  int lv2ui_resize(uint width, uint height);
  uint32_t lv2_get_options(LV2_Options_Option* const /*options*/);
  uint32_t lv2_set_options(const LV2_Options_Option* const options);

#if PLUGIN_WANT_PROGRAMS
  void lv2ui_select_program(const uint32_t bank, const uint32_t program);
#endif

protected:
  void editParameterValue(const uint32_t rindex, const bool started);
  void setParameterValue(const uint32_t rindex, const float value);
  void setState(const char* const key, const char* const value);
  void sendNote(const uint8_t /*channel*/, const uint8_t /*note*/, const uint8_t /*velocity*/);
  void setSize(const uint width, const uint height);

private:
  UIExporter fUI;
  // LV2 features
  const LV2_URID_Map* const fUridMap;
  const LV2UI_Resize* const fUiResize;
  const LV2UI_Touch* const fUiTouch;
  // LV2 UI stuff
  const LV2UI_Controller fController;
  const LV2UI_Write_Function fWriteFunction;
  // Need to save this
  const LV2_URID fEventTransferURID;
  const LV2_URID fKeyValueURID;

  // using ui:showInterface if true
  bool fWinIdWasNull;

  // Callbacks
  static void editParameterCallback(void* ptr, uint32_t rindex, bool started);
  static void setParameterCallback(void* ptr, uint32_t rindex, float value);
  static void setStateCallback(void* ptr, const char* key, const char* value);
  static void sendNoteCallback(void* ptr, uint8_t channel, uint8_t note, uint8_t velocity);
  static void setSizeCallback(void* ptr, uint width, uint height);
};

IPlugLV2Editor* MakePlug();

static LV2UI_Handle lv2ui_instantiate(const LV2UI_Descriptor*, const char* uri, const char* bundlePath, LV2UI_Write_Function writeFunction, LV2UI_Controller controller, LV2UI_Widget* widget, const LV2_Feature* const* features);
static void lv2ui_cleanup(LV2UI_Handle ui);
static void lv2ui_port_event(LV2UI_Handle ui, uint32_t portIndex, uint32_t bufferSize, uint32_t format, const void* buffer);
static int lv2ui_idle(LV2UI_Handle ui);
static int lv2ui_show(LV2UI_Handle ui);
static int lv2ui_hide(LV2UI_Handle ui);
static int lv2ui_resize(LV2UI_Handle ui, int width, int height);
static uint32_t lv2_get_options(LV2UI_Handle ui, LV2_Options_Option* options);
static uint32_t lv2_set_options(LV2UI_Handle ui, const LV2_Options_Option* options);
#if PLUGIN_WANT_PROGRAMS
static void lv2ui_select_program(LV2UI_Handle ui, uint32_t bank, uint32_t program);
#endif
static const void* lv2ui_extension_data(const char* uri);

#endif // _IPLUGAPI_
