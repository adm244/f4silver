/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
*/

//IMPORTANT(adm244): SCRATCH VERSION JUST TO GET IT UP WORKING

#ifndef CONFIG_CPP
#define CONFIG_CPP

#define CONFIG_FILE "f4silver.ini"

#define CONFIG_SETTINGS_SECTION "settings"
#define CONFIG_MESSAGE_SECTION "message"
#define CONFIG_KEYS_SECTION "keys"

#define CONFIG_PRESAVE "bSaveGamePreActivation"
#define CONFIG_POSTSAVE "bSaveGamePostActivation"
#define CONFIG_SHOWMESSAGES "bShowMessages"
#define CONFIG_SHOWMESSAGES_DEBUG "bShowMessagesDebug"
#define CONFIG_SHOWMESSAGES_RANDOM "bShowMessagesRandom"
#define CONFIG_SAVEFILE "sSaveFile"
#define CONFIG_SAVENAME "sSaveName"
#define CONFIG_MESSAGE "sMessage"
#define CONFIG_MESSAGE_FAIL "sMessageFail"
#define CONFIG_MESSAGE_RANDOM "sMessageRandom"
#define CONFIG_MESSAGE_TOGGLE_ON "sMessageToggleOn"
#define CONFIG_MESSAGE_TOGGLE_OFF "sMessageToggleOff"
#define CONFIG_TIMER "iTimeout"
#define CONFIG_AUTOSAVE "bAutoSave"
#define CONFIG_CHECK_MENU "bIgnoreInMenu"
#define CONFIG_CHECK_VATS "bIgnoreInVATS"
#define CONFIG_CHECK_DIALOGUE "bIgnoreInDialogue"

#define CONFIG_DEFAULT_SAVEFILE "f4silver_save"
#define CONFIG_DEFAULT_SAVENAME "F4Silver Save"
#define CONFIG_DEFAULT_MESSAGE "%s activated"
#define CONFIG_DEFAULT_MESSAGE_FAIL "%s failed to activate"
#define CONFIG_DEFAULT_MESSAGE_RANDOM "%s activated"
#define CONFIG_DEFAULT_MESSAGE_TOGGLE_ON "Commands are ON"
#define CONFIG_DEFAULT_MESSAGE_TOGGLE_OFF "Commands are OFF"
#define CONFIG_DEFAULT_TIMER (15 * 60 * 1000)

#define STRING_SIZE 256

struct SilverStrings {
  char SaveFileName[STRING_SIZE];
  char SaveDisplayName[STRING_SIZE];
  char Message[STRING_SIZE];
  char MessageFail[STRING_SIZE];
  char MessageRandom[STRING_SIZE];
  char MessageOn[STRING_SIZE];
  char MessageOff[STRING_SIZE];
};

struct SilverSettings {
  bool ShowMessages;
  bool ShowMessagesDebug;
  bool ShowMessagesRandom;
  bool SavePreActivation;
  bool SavePostActivation;
  bool AutoSaveEnabled;
  bool IgnoreInMenu;
  bool IgnoreInVATS;
  bool IgnoreInDialogue;
  uint Timeout;
};

internal SilverStrings Strings;
internal SilverSettings Settings;

internal void SettingsInitialize(HMODULE module)
{
  //TODO(adm244): rewrite using full path in these functions (pass it into the functions)
  Settings.SavePreActivation = IniReadBool(module, CONFIG_FILE, CONFIG_SETTINGS_SECTION, CONFIG_PRESAVE, false);
  Settings.SavePostActivation = IniReadBool(module, CONFIG_FILE, CONFIG_SETTINGS_SECTION, CONFIG_POSTSAVE, false);
  Settings.ShowMessages = IniReadBool(module, CONFIG_FILE, CONFIG_SETTINGS_SECTION, CONFIG_SHOWMESSAGES, false);
  Settings.ShowMessagesDebug = IniReadBool(module, CONFIG_FILE, CONFIG_SETTINGS_SECTION, CONFIG_SHOWMESSAGES_DEBUG, true);
  Settings.ShowMessagesRandom = IniReadBool(module, CONFIG_FILE, CONFIG_SETTINGS_SECTION, CONFIG_SHOWMESSAGES_RANDOM, true);
  Settings.AutoSaveEnabled = IniReadBool(module, CONFIG_FILE, CONFIG_SETTINGS_SECTION, CONFIG_AUTOSAVE, true);
  Settings.IgnoreInMenu = IniReadBool(module, CONFIG_FILE, CONFIG_SETTINGS_SECTION, CONFIG_CHECK_MENU, true);
  Settings.IgnoreInVATS = IniReadBool(module, CONFIG_FILE, CONFIG_SETTINGS_SECTION, CONFIG_CHECK_VATS, true);
  Settings.IgnoreInDialogue = IniReadBool(module, CONFIG_FILE, CONFIG_SETTINGS_SECTION, CONFIG_CHECK_DIALOGUE, true);
  
  IniReadString(module, CONFIG_FILE, CONFIG_SETTINGS_SECTION, CONFIG_SAVEFILE,
    CONFIG_DEFAULT_SAVEFILE, Strings.SaveFileName, STRING_SIZE);
  IniReadString(module, CONFIG_FILE, CONFIG_SETTINGS_SECTION, CONFIG_SAVENAME,
    CONFIG_DEFAULT_SAVENAME, Strings.SaveDisplayName, STRING_SIZE);
    
  IniReadString(module, CONFIG_FILE, CONFIG_MESSAGE_SECTION, CONFIG_MESSAGE,
    CONFIG_DEFAULT_MESSAGE, Strings.Message, STRING_SIZE);
  IniReadString(module, CONFIG_FILE, CONFIG_MESSAGE_SECTION, CONFIG_MESSAGE_FAIL,
    CONFIG_DEFAULT_MESSAGE_FAIL, Strings.MessageFail, STRING_SIZE);
  IniReadString(module, CONFIG_FILE, CONFIG_MESSAGE_SECTION, CONFIG_MESSAGE_RANDOM,
    CONFIG_DEFAULT_MESSAGE_RANDOM, Strings.MessageRandom, STRING_SIZE);
  IniReadString(module, CONFIG_FILE, CONFIG_MESSAGE_SECTION, CONFIG_MESSAGE_TOGGLE_ON,
    CONFIG_DEFAULT_MESSAGE_TOGGLE_ON, Strings.MessageOn, STRING_SIZE);
  IniReadString(module, CONFIG_FILE, CONFIG_MESSAGE_SECTION, CONFIG_MESSAGE_TOGGLE_OFF,
    CONFIG_DEFAULT_MESSAGE_TOGGLE_OFF, Strings.MessageOff, STRING_SIZE);
  
  Settings.Timeout = IniReadInt(module, CONFIG_FILE, CONFIG_SETTINGS_SECTION, CONFIG_TIMER, CONFIG_DEFAULT_TIMER);
}

#endif
