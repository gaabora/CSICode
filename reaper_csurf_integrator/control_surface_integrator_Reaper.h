//
//  control_surface_integrator_Reaper.h
//  reaper_csurf_integrator
//
//

#ifndef control_surface_integrator_Reaper_h
#define control_surface_integrator_Reaper_h

#define REAPERAPI_WANT_TrackFX_GetParamNormalized
#define REAPERAPI_WANT_TrackFX_SetParamNormalized
#include "reaper_plugin_functions.h"

using namespace std;

extern HWND g_hwnd;

const int MEDBUF = 512;
const int SMLBUF = 256;

static vector<string> ExplodeString(const char separator, const string value)
{
    vector<string> result;
    size_t start = 0;
    size_t end = value.find(';');
    while (end != string::npos) {
        result.push_back(value.substr(start, end - start));
        start = end + 1;
        end = value.find(';', start);
    }
    result.push_back(value.substr(start));
    return result;
}

struct osd_data
{
    string origValue;
    string message;
    int timeoutMs = 0;
    vector<string> bgColors;

    string lastValue;
    bool awaitsFeedback;
    string bgColor;

    osd_data() = default; 

    osd_data(string osdValue) {
        origValue = osdValue;

        if (osdValue.front() == '\"') osdValue.erase(0, 1);
        if (osdValue.back()  == '\"') osdValue.pop_back();

        vector<string> osdParams = ExplodeString(';', osdValue);

        message = osdParams[0];
        if (osdParams.size() >= 2 && !osdParams[1].empty()) bgColors = ExplodeString(' ', osdParams[2]);
        if (osdParams.size() >= 3 && !osdParams[2].empty()) timeoutMs = atoi(osdParams[1].c_str());
    }

    const string toString() const {
        return  message + ";" + bgColor + ";" + to_string(timeoutMs);
    }
};

struct rgba_color
{
    int r;
    int g;
    int b;
    int a;
        
    bool operator == (const rgba_color &other) const
    {
        return r == other.r && g == other.g && b == other.b && a == other.a ;
    }
    
    bool operator != (const rgba_color &other) const
    {
        return r != other.r || g != other.g || b != other.b || a != other.a;
    }
    
    const char *rgba_to_string(char *buf) const // buf must be at least 10 bytes
    {
      snprintf(buf,10,"#%02x%02x%02x%02x",r,g,b,a);
      return buf;
    }
    
    rgba_color()
    {
        r = 0;
        g = 0;
        b = 0;
        a = 255;
    }
};

static bool GetColorValue(const char *hexColor, rgba_color &colorValue)
{
    if (strlen(hexColor) == 7)
    {
      return sscanf(hexColor, "#%2x%2x%2x", &colorValue.r, &colorValue.g, &colorValue.b) == 3;
    }
    if (strlen(hexColor) == 9)
    {
      return sscanf(hexColor, "#%2x%2x%2x%2x", &colorValue.r, &colorValue.g, &colorValue.b, &colorValue.a) == 4;
    }
    return false;
}

struct MIDI_event_ex_t : MIDI_event_t
{
    MIDI_event_ex_t()
    {
        frame_offset = 0;
        size = 3;
        midi_message[0] = 0x00;
        midi_message[1] = 0x00;
        midi_message[2] = 0x00;
        midi_message[3] = 0x00;
    };
    
    MIDI_event_ex_t(const unsigned char first, const unsigned char second, const unsigned char third)
    {
        size = 3;
        midi_message[0] = first;
        midi_message[1] = second;
        midi_message[2] = third;
        midi_message[3] = 0x00;
    };
    
    bool IsEqualTo(const MIDI_event_ex_t *other) const
    {
        if (this->size != other->size)
            return false;
        
        for (int i = 0; i < size; ++i)
            if (this->midi_message[i] != other->midi_message[i])
                return false;
        
        return true;
    }
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class DAW
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
public:
    static void SendCommandMessage(WPARAM wparam) { ::SendMessage(g_hwnd, WM_COMMAND, wparam, 0); }
    
    static MediaTrack *GetSelectedTrack(int seltrackidx) { return ::GetSelectedTrack(NULL, seltrackidx); }
    
    static bool ValidateTrackPtr(MediaTrack *track) { return ::ValidatePtr(track, "MediaTrack*"); }
    
    static bool CanUndo()
    {
        if (::Undo_CanUndo2(NULL))
           return true;
        else
            return false;
    }
    
    static bool CanRedo()
    {
        if (::Undo_CanRedo2(NULL))
           return true;
        else
            return false;
    }
    
    static void Undo()
    {
        if (CanUndo())
            ::Undo_DoUndo2(NULL);
    }
    
    static void Redo()
    {
        if (CanRedo())
            ::Undo_DoRedo2(NULL);
    }
       
    static MediaTrack *GetTrack(int trackidx)
    {
        trackidx--;
        
        if (trackidx < 0)
            trackidx = 0;
        
        return ::GetTrack(NULL, trackidx) ;
    }
    
    static rgba_color GetTrackColor(MediaTrack *track)
    {
        rgba_color color;
        
        if (ValidateTrackPtr(track))
            ::ColorFromNative(::GetTrackColor(track), &color.r, &color.g, &color.b);
        
        if (color.r == 0 && color.g == 0 && color.b == 0)
        {
            color.r = 64;
            color.g = 64;
            color.b = 64;
        }
        
        return color;
    }
    
    static unsigned int GetTrackGroupMembership(MediaTrack *track, const char *groupname)
    {
        if (ValidateTrackPtr(track))
            return ::GetSetTrackGroupMembership(track, groupname, 0, 0);
        else
            return 0;
    }
    
    static unsigned int GetTrackGroupMembershipHigh(MediaTrack *track, const char *groupname)
    {
        if (ValidateTrackPtr(track))
            return ::GetSetTrackGroupMembershipHigh(track, groupname, 0, 0);
        else
            return 0;
    }
    
    static const char* GetCommandName(int cmdID)
    {
        const char* actionName = ::kbd_getTextFromCmd(cmdID, ::SectionFromUniqueID(1));
        if (actionName)
            return actionName;
        else
            return "NOT FOUND!";
    }
    
    static bool ShowOSD(const osd_data osdData)
    {
        static string lastValue;
        static DWORD lastUpdateTs = 0;
        DWORD now = GetTickCount();

        if (lastValue == osdData.lastValue) {
            if (osdData.timeoutMs == -1) return false;
            if (osdData.timeoutMs >= 0 && (now - lastUpdateTs) < (DWORD)osdData.timeoutMs) return false;
        }

        lastValue = osdData.toString();
        lastUpdateTs = now;
        ::SetExtState("CSI_TMP", "OSD", lastValue.c_str(), false);
        return true;
    }

    static void SetExtState(const char* section, const char* key, const char* value, bool persist)
    {
        ::SetExtState(section, key, value, persist);
    }
};

#endif /* control_surface_integrator_Reaper_h */
