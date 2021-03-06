# iPlug 2
### C++ audio plug-in framework for desktop and web

[![Build Status](https://dev.azure.com/iplug2/iplug2/_apis/build/status/iPlug2?branchName=master)](https://dev.azure.com/iplug2/iplug2/_build/latest?definitionId=2?branchName=master)

iPlug 2 is a simple-to-use C++ framework for developing cross platform audio plug-ins/apps and targeting multiple plug-in APIs with the same minimalistic code. It abstracts an audio plug-in (IPlug) and it's drawing engine/GUI toolkit (IGraphics). IGraphics uses a retained mode paradigm and contains a collection of common controls well suited for audio plug-in GUIs, either using bitmap or vector graphics. 

The original version of iPlug was developed by [John Schwartz aka schwa](https://www.cockos.com/team.php) and released in 2008 as part of Cockos' WDL library. iPlug 2 (2018) is a substantial reworking that brings multiple vector graphics backends (including GPU accelerated options and HiDPI/scaling), a better approach to concurrency, support for distributed plug-in formats and compiling to WebAssembly via [emscripten](https://github.com/kripken/emscripten), amongst many other things.

iPlug 2 targets the VST2, VST3, AudioUnit, AAX (Native) and the [Web Audio Module](https://webaudiomodules.org) (WAM) plug-in APIs. It can also produce standalone win32/macOS apps with audio and MIDI I/O, as well as [Reaper extensions](https://www.reaper.fm/sdk/plugin/plugin.php).

iPlug 2 includes support for [the FAUST programming language](http://faust.grame.fr), and the libfaust JIT compiler. It was the winner of the 2018 FAUST award.

You can discuss iPlug 2 on the [WDL forum](http://forum.cockos.com/forumdisplay.php?f=32
) or on the [iPlug user's slack channel.](https://join.slack.com/t/iplug-users/shared_invite/enQtMzA1NzA1NzE0OTY1LWYyODdjNzkyYTk4MDRmYzZjZTI4ZGVkYTIxZTk0OWRiYWE2MTA0ZWVlODM1NjkzNDAyNDFhMDdjNGI4OTY2YTU)

Please consider supporting the project by helping out with bug fixes, features or documentation. You can support the project financially [via patreon.](https://www.patreon.com/olilarkin)

**NOTE: THIS IS NOT YET PRODUCTION READY - zero commercial/free plug-ins are shipping with iPlug 2 - there are many bugs still to fix and a few features to add!** 


You may like to check out the [Web Audio Module version of VirtualCZ](https://virtualcz.io/) which has been made using iPlug 2. 

[![patreon](Documentation/img/become_a_patron_button.png)](https://www.patreon.com/olilarkin) [![paypal](Documentation/img/paypal_donate_button.gif)](https://paypal.me/olilarkin?locale.x=en_GB)[![slack](Documentation/img/join_slack_button.png)](https://join.slack.com/t/iplug-users/shared_invite/enQtMzA1NzA1NzE0OTY1LWYyODdjNzkyYTk4MDRmYzZjZTI4ZGVkYTIxZTk0OWRiYWE2MTA0ZWVlODM1NjkzNDAyNDFhMDdjNGI4OTY2YTU)
