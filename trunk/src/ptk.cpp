// ------------------------------------------------------
// Protrekkr
// Written by Franck Charlet
// Based on the work of Juan Antonio Arguelles Rius 
// ------------------------------------------------------

// ------------------------------------------------------
// Includes
#include "include/ptk.h"

// ------------------------------------------------------
// Variables
unsigned char sl3 = 0;

int CONSOLE_WIDTH = 800;
int CONSOLE_HEIGHT = 600;
int CONSOLE_HEIGHT2 = 600;
int fluzy = -1;
int Scopish = SCOPE_ZONE_MOD_DIR;
char Scopish_LeftRight = FALSE;
int rev_counter2 = 0;
char visiblecolums = 0;
int rs_coef = 32768;

int gui_lx = -1;
int gui_ly = -1;
int gui_lxs = -1;
int gui_lys = -1;
int gui_pushed = 0;
char teac = 0;
int liveparam = 0;
int livevalue = 0;
int poskeynote = 0;
int poskeyval;
char trkchan = TRUE;
int pos_space = 0;
int multifactor = 4;
char seditor = 0;
char tipoftheday[256];
int ctipoftheday = 0;
char ped_split = 0;

int player_pos = -1;
int xew = 0;
char sas = FALSE;
int flagger = 0;
int ltretnote = 0;
int ltretnote_raw = 0;
int ltretvalue = 0;
int retletter[256];
int tretletter = 0;
int posletter = 0;
int last_index = -1;
int gui_action = GUI_CMD_NOP;
int ped_row = 0;
int ped_track = 0;
int gui_track = 0;
int xoffseted;
float gr = 0;
float synthsignal = 0;

char userscreen = USER_SCREEN_DISKIO_EDIT;
int c_r_release = 0;
int c_l_release = 0;

char name[MAX_PATH];
char synthname[MAX_PATH];
char instrname[MAX_PATH];
char name303[MAX_PATH];
char artist[20];
char style[20];
char autosavename[MAX_PATH];

int ped_patsam = 0;
int restx = 0;
int resty = 0;
int fsize = 0;
char draw_sampled_wave = FALSE;
char draw_sampled_wave2 = FALSE;
char draw_sampled_wave3 = FALSE;

int redraw_everything = 0;
char boing = FALSE;
int LastPedRow = -1;
char po_ctrl2 = TRUE;
char po_alt = TRUE;
char po_alt2 = TRUE;
int TRACKMIDICHANNEL[MAX_TRACKS];

int sp_TickCounter = 0;
int LVColor = 0;
int RVColor = 0;
int player_line = 0;
char actuloop = 0;
int namesize = 8;

int c_l_tvol = 32768;
int c_r_tvol = 32768;
int c_l_cvol = 32768;
int c_r_cvol = 32768;
int mlimit = 0;

int hd_isrecording = 0;

int snamesel = 0;

SDL_Surface *PFONT;
SDL_Surface *FONT;
SDL_Surface *FONT_LOW;

SDL_TimerID Timer;
Uint32 Timer_CallBack(Uint32 interval, void *param);
Uint32 (*Timer_Ptr)(Uint32 interval, void *param) = &Timer_CallBack;
char CpuStr[80];

int Asking_Exit;

int MouseWheel_Multiplier = 1;
char Rows_Decimal = FALSE;
char See_Prev_Next_Pattern = FALSE;

char cur_input_name[1024];

char *test_text = TITLE" was written by hitchhikr / Neural.";
SDL_Surface *LOGOPIC;
int wait_title;
int display_title = 0;

int key_record_first_time;
double key_ticks;
int old_key_ped_line;
Uint32 Alloc_midi_Channels[MAX_TRACKS];
int Record_Keys[37];
int Record_Keys_State[37];
int Track_Number;

void Mouse_Sliders_Master_Shuffle(void);
void Display_Beat_Time(void);
void Calc_Length(void);

extern int Ticks_Synchro_Left;
extern int Ticks_Synchro_Right;
extern int Continuous_Scroll;

void Draw_Scope(void);
void Draw_Scope_Files_Button(void);
void Display_Tracks_To_Render(void);
void Solo_Track(int track_to_solo);

int wait_AutoSave;
extern char AutoSave;
int Values_AutoSave[] =
{
    0,
    1 * 60,
    2 * 60,
    4 * 60,
    8 * 60,
    10 * 60,
    15 * 60,
    30 * 60
};

char Prog_Path[MAX_PATH];

int LOGO_WIDTH;
int LOGO_HEIGHT;
int LOGO_BKCOL;

// ------------------------------------------------------
// Load a skin picture according to the xml script
SDL_Surface *Load_Skin_Picture(char *name)
{
    char filepath[MAX_PATH];
    const char *pic_filename;
    char error[256];
    SDL_Surface *surface;

    pic_filename = XML_get_string("files", "file", name, "value");
    if(!pic_filename)
    {
        Message_Error("XML error in files tag.");
        return(NULL);
    }

#if defined(__WIN32__)
    sprintf(filepath, "skins\\%s", pic_filename);
#else
    sprintf(filepath, "skins/%s", pic_filename);
#endif

    surface = Load_Picture(filepath);
    if(surface == NULL)
    {
        sprintf(error, "Can't load %s file.", pic_filename);
        Message_Error(error);
        return(NULL);
    }
    return(surface);
}

// ------------------------------------------------------
// Load the font letters sizes according to the xml script
int Load_Font_Datas(char *name)
{
    char filepath[MAX_PATH];
    const char *pic_filename;
    char error[256];

    pic_filename = XML_get_string("files", "file", name, "value");
    
    if(!pic_filename)
    {
        Message_Error("XML error in files tag.");
        return(FALSE);
    }
    
#if defined(__WIN32__)
    sprintf(filepath, "skins\\%s", pic_filename);
#else
    sprintf(filepath, "skins/%s", pic_filename);
#endif
    if(!Create_Font_Datas(filepath))
    {
        sprintf(error, "Can't load %s file.", pic_filename);
        Message_Error(error);
        return(FALSE);
    }
    return(TRUE);
}

// ------------------------------------------------------
// Load the necessary datas and initialize the interface
int Init_Context(void)
{
    restx = CONSOLE_WIDTH - 640;
    resty = CONSOLE_HEIGHT - 492;
    CONSOLE_HEIGHT2 = CONSOLE_HEIGHT - 42;
    mlimit = 619 + restx;
    fsize = 638 + restx;
    visiblecolums = CONSOLE_WIDTH / 128;

#if defined(__WIN32__)
    srand(GetTickCount());
#else
    srand(rand());
#endif

    ctipoftheday = rand() % 12;

    switch(ctipoftheday)
    {
        case 0: sprintf(tipoftheday, "Tip Of The Hour: Pressing CTRL+I will interpolate effect value on a marked block."); break;
        case 1: sprintf(tipoftheday, "Tip Of The Hour: The right mouse button will have a secondary action on most buttons."); break;
        case 2: sprintf(tipoftheday, "Tip Of The Hour: Don't set excessive track reverb send values, to get better quality."); break;
        case 3: sprintf(tipoftheday, "Tip Of The Hour: When recording, number of positions will grow automatically when necessary."); break;
        case 4: sprintf(tipoftheday, "Remember: MIDI is not audio, realtime fx will not affect midi sound."); break;
        case 5: sprintf(tipoftheday, "Tip Of The Hour: On lower CPUs, you can renderize patterns to wav, and use them as samples without any loss of quality."); break;
        case 6: sprintf(tipoftheday, "Tip Of The Hour: Volume note-cut command 'Fx' is very useful to avoid sample-clicking."); break;
        case 7: sprintf(tipoftheday, "Tip Of The Hour: Left-Clicking on pattern editor channels numbers will mute/unmute any track (right clicking will solo it)."); break;
        case 8: sprintf(tipoftheday, "Tip Of The Hour: Pattern command '16xx' will reset the Filter LFO of the track. No parameter required."); break;
        case 9: sprintf(tipoftheday, "Tip Of The Hour: With a '90' value on the panning column you can change midi controllers values."); break;
        case 10: sprintf(tipoftheday, "Tip Of The Hour: Increase latency time if sound is distorted."); break;
        case 11: sprintf(tipoftheday, "Tip Of The Hour: Pressing right mouse button on most arrows buttons (\03\04) will speed operation up.");

        default: sprintf(tipoftheday, "Tip Of The Hour: See readme.txt for more infos about help and pattern commands."); break;
    }

    L_MaxLevel = 0;
    R_MaxLevel = 0;

    sprintf(name, "Untitled");
    sprintf(artist, "Somebody");
    sprintf(style, "Goa Trance");

    namesize = 8;
    IniCsParNames();

    Clear_Files_List();

#if !defined(__NO_MIDI__)
    midiin_changed = 2;
    midiout_changed = 2;

    Midi_InitIn();
    Midi_InitOut();
#endif

    GETCWD(Prog_Path, MAX_PATH);

    Read_SMPT();

    CHDIR(Prog_Path);

    if(!Init_Block_Work()) return(FALSE);

#if !defined(__NO_MIDI__)
    Midi_Reset();
#endif

    if(!AllocPattern())
    {
        Message_Error("Can't allocate room for patterns.");
        return(FALSE);
    }

    // Player initialization
#if defined(__WIN32__)
    if(!Ptk_InitDriver(Main_Window, AUDIO_Milliseconds))
#else
    if(!Ptk_InitDriver(AUDIO_Milliseconds))
#endif
    {
        Message_Error("Can't initialize audio driver.");
        return(FALSE);
    }

    init_sample_bank();
    Pre_Song_Init();
    Post_Song_Init();

    char *toto = test_text;

    LOGOPIC = Load_Skin_Picture("logo");
    if(!LOGOPIC) return(FALSE);

    if(!XML_get_integer("files", "file", "logo", "width", &LOGO_WIDTH))
    {
        Message_Error("logo width key missing.");
        return(FALSE);
    }
    if(!XML_get_integer("files", "file", "logo", "height", &LOGO_HEIGHT))
    {
        Message_Error("logo height key missing.");
        return(FALSE);
    }
    if(!XML_get_integer("files", "file", "logo", "bkcol", &LOGO_BKCOL))
    {
        Message_Error("logo bkcol key missing.");
        return(FALSE);
    }

    POINTER = Load_Skin_Picture("pointer");
    if(!POINTER) return(FALSE);
    SKIN303 = Load_Skin_Picture("303");
    if(!SKIN303) return(FALSE);
    PFONT = Load_Skin_Picture("pattern_font");
    if(!PFONT) return(FALSE);
    FONT = Load_Skin_Picture("font");
    if(!FONT) return(FALSE);
    FONT_LOW = Load_Skin_Picture("font");
    if(!FONT_LOW) return(FALSE);

    if(!Set_Pictures_Colors()) return(FALSE);

    if(!Load_Font_Datas("font_datas")) return(FALSE);

    SDL_SetColorKey(FONT, SDL_SRCCOLORKEY, 0);
    SDL_SetColorKey(FONT_LOW, SDL_SRCCOLORKEY, 0);
    SDL_SetColorKey(POINTER, SDL_SRCCOLORKEY, 0);

    Timer = SDL_AddTimer(1000, Timer_Ptr, NULL);

    return(TRUE);
}
   
// ------------------------------------------------------
// Release the allocated resources
extern int volatile AUDIO_Acknowledge;
void Destroy_Context(void)
{
    if(Timer) SDL_RemoveTimer(Timer);
    AUDIO_Acknowledge = TRUE;
    Ptk_ReleaseDriver();

    Free_Samples();

    // Freeing Allocated Patterns
    if(RawPatterns) free(RawPatterns);

    Destroy_UI();
    SDL_Quit();
}

// ------------------------------------------------------
// Update the world
int Screen_Update(void)
{
    int FineTune_Value;

    redraw_everything = 0;
    if(Env_Change)
    {
        redraw_everything = 1;
        Env_Change = FALSE;
    }

    if(sp_Stage[ped_track] == PLAYING_SAMPLE && ped_patsam == sp_channelsample[ped_track] && ped_split == sp_split[ped_track])
    {
        draw_sampled_wave2 = TRUE;
        boing = TRUE;
    }
    else if(boing)
    {
        boing = FALSE;
        draw_sampled_wave3 = TRUE;
    }

    if(display_title)
    {
        if(Scopish == SCOPE_ZONE_SCOPE) Draw_Scope();

        // Sample ed.
        Draw_Sampled_Wave();

        int Lt_vu = MIN_VUMETER + L_MaxLevel / LARG_VUMETER;
        int Rt_vu = MIN_VUMETER + R_MaxLevel / LARG_VUMETER;
        int Lt_vu_Peak = Lt_vu;
        int Rt_vu_Peak = Rt_vu;
        if(Lt_vu_Peak > MAX_VUMETER - 1) Lt_vu_Peak = MAX_VUMETER - 1;
        if(Rt_vu_Peak > MAX_VUMETER - 1) Rt_vu_Peak = MAX_VUMETER - 1;
        if(Lt_vu > MIN_PEAK) Lt_vu = MIN_PEAK;
        if(Rt_vu > MIN_PEAK) Rt_vu = MIN_PEAK;

        // Draw the vu meters
        DrawHLine(10, MIN_VUMETER, Lt_vu, COL_VUMETER);
        DrawHLine(11, MIN_VUMETER, Lt_vu, COL_VUMETER);
        DrawHLine(12, MIN_VUMETER, Lt_vu, COL_VUMETER);
        DrawHLine(13, MIN_VUMETER, Lt_vu, COL_VUMETER);

        DrawHLine(15, MIN_VUMETER, Rt_vu, COL_VUMETER);
        DrawHLine(16, MIN_VUMETER, Rt_vu, COL_VUMETER);
        DrawHLine(17, MIN_VUMETER, Rt_vu, COL_VUMETER);
        DrawHLine(18, MIN_VUMETER, Rt_vu, COL_VUMETER);

        DrawHLine(10, Lt_vu, Lt_vu_Peak, COL_VUMETERPEAK);
        DrawHLine(11, Lt_vu, Lt_vu_Peak, COL_VUMETERPEAK);
        DrawHLine(12, Lt_vu, Lt_vu_Peak, COL_VUMETERPEAK);
        DrawHLine(13, Lt_vu, Lt_vu_Peak, COL_VUMETERPEAK);

        DrawHLine(15, Rt_vu, Rt_vu_Peak, COL_VUMETERPEAK);
        DrawHLine(16, Rt_vu, Rt_vu_Peak, COL_VUMETERPEAK);
        DrawHLine(17, Rt_vu, Rt_vu_Peak, COL_VUMETERPEAK);
        DrawHLine(18, Rt_vu, Rt_vu_Peak, COL_VUMETERPEAK);

        DrawHLine(10, Lt_vu_Peak, MAX_VUMETER - 1, COL_BACKGROUND);
        DrawHLine(11, Lt_vu_Peak, MAX_VUMETER - 1, COL_BACKGROUND);
        DrawHLine(12, Lt_vu_Peak, MAX_VUMETER - 1, COL_BACKGROUND);
        DrawHLine(13, Lt_vu_Peak, MAX_VUMETER - 1, COL_BACKGROUND);

        DrawHLine(15, Rt_vu_Peak, MAX_VUMETER - 1, COL_BACKGROUND);
        DrawHLine(16, Rt_vu_Peak, MAX_VUMETER - 1, COL_BACKGROUND);
        DrawHLine(17, Rt_vu_Peak, MAX_VUMETER - 1, COL_BACKGROUND);
        DrawHLine(18, Rt_vu_Peak, MAX_VUMETER - 1, COL_BACKGROUND);

        if(actuloop) Afloop();
    }

    if(gui_action != 0)
    { // There are some for me today.....:)

        // Files list slider
        if(gui_action == GUI_CMD_SET_FILES_LIST_SLIDER)
        {
            lt_ykar = Mouse.y - 72;
            ltActualize(0);
        }

        if(gui_action == GUI_CMD_SET_FILES_LIST_SELECT_FILE)
        {
            int broadcast = lt_index + (Mouse.y - 44) / 12;
            last_index = -1;
            if(broadcast != lt_curr)
            {
                lt_curr = broadcast;
                ltActualize(1);
            }
            else
            {
                if(Get_Current_FileType() != _A_SUBDIR)
                {
                    AUDIO_Stop();
                    StopIfSp();
                    LoadFile(ped_patsam, Get_Current_FileName());
                    AUDIO_Play();
                }
                else
                {
                    Set_Current_Dir();
                    Read_SMPT();
                    ltActualize(0);
                }
            }
        }

        // --- Sequence ---------------------------------------

        if(gui_action == GUI_CMD_PREVIOUS_POSITION)
        {
            if(Songplaying)
            {
                cPosition_delay--;
                cPosition = cPosition_delay;
            }
            else
            {
                cPosition--;
                Bound_Patt_Pos();          
            }
            Actualize_Sequencer();
            Actupated(0);
        }

        if(gui_action == GUI_CMD_NEXT_POSITION)
        {
            if(Songplaying)
            {
                cPosition_delay++;
                cPosition = cPosition_delay;
            }
            else
            {
                cPosition++;
                Bound_Patt_Pos();          
            }
            Actualize_Sequencer();
            Actupated(0);
        }

        if(gui_action == GUI_CMD_SET_PATTERN_LENGTH)
        {
            Actualize_Sequencer();
            Actupated(0);
        }

        if(gui_action == GUI_CMD_PREV_PATT)
        {
            if(Songplaying)
            {
                pSequence[cPosition_delay]--;
            }
            else
            {
                pSequence[cPosition]--;
                Bound_Patt_Pos();          
            }
            Actualize_Sequencer();
            Actupated(0);
            if(Songplaying)
            {
                Anat(cPosition_delay);
            }
            else
            {
                Anat(cPosition);
            }
        }

        if(gui_action == GUI_CMD_NEXT_PATT)
        {
            if(Songplaying)
            {
                pSequence[cPosition_delay]++;
            }
            else
            {
                pSequence[cPosition]++;
                Bound_Patt_Pos();          
            }
            Actualize_Sequencer();
            Actupated(0);
            if(Songplaying)
            {
                Anat(cPosition_delay);
            }
            else
            {
                Anat(cPosition);
            }
        }

        if(gui_action == GUI_CMD_REDUCE_SONG_LENGTH)
        {
            sLength--;
            Actualize_Sequencer();
            Actupated(0);
        }

        if(gui_action == GUI_CMD_INCREASE_SONG_LENGTH)
        {
            sLength++;
            Actualize_Sequencer();
            Actupated(0);
        }

        if(gui_action == GUI_CMD_UPDATE_SEQUENCER)
        {
            Actualize_Sequencer();
        }

        if(gui_action == GUI_CMD_SET_FOCUS_TRACK)
        {
            Actualize_Track_Ed(0);
            Actualize_Track_Fx_Ed(0);
        }

        if(gui_action == GUI_CMD_LOWER_OCTAVE)
        {
            ped_patoct--;
            Actualize_Patterned();
        }

        if(gui_action == GUI_CMD_HIGHER_OCTAVE)
        {
            ped_patoct++;
            Actualize_Patterned();
        }

        if(gui_action == GUI_CMD_DECREASE_STEP_ADD)
        {
            ped_pattad--;
            if(ped_pattad < 0) ped_pattad = 0;
            Actualize_Patterned();
        }

        if(gui_action == GUI_CMD_INCREASE_STEP_ADD)
        {
            ped_pattad++;
            if(ped_pattad > 16) ped_pattad = 16;
            Actualize_Patterned();
        }

        if(gui_action == GUI_CMD_PREV_INSTR)
        {
            ped_patsam--;

            if(snamesel == 2) snamesel = 0;

            Actualize_Patterned();
            RefreshSample();
            NewWav();
            Actualize_Synth_Ed(UPDATE_SYNTH_ED_ALL);
        }

        if(gui_action == GUI_CMD_NEXT_INSTR)
        {
            ped_patsam++;

            if(snamesel == 2) snamesel = 0;

            Actualize_Patterned();
            RefreshSample();
            NewWav();
            Actualize_Synth_Ed(UPDATE_SYNTH_ED_ALL);
        }

        if(gui_action == GUI_CMD_SWITCH_TRACK_MUTE_STATE)
        {
            int tmp_track = Get_Track_Over_Mouse();

            if(TRACKSTATE[tmp_track] == 0) TRACKSTATE[tmp_track] = 1;
            else TRACKSTATE[tmp_track] = 0;

            if(userscreen == USER_SCREEN_TRACK_EDIT) Actualize_Track_Ed(10);
            Actupated(0);
        }

        if(gui_action == GUI_CMD_PLAY_SONG)
        {
            SongPlay();
        }

        if(gui_action == GUI_CMD_STOP_SONG)
        {
            is_recording = 0;
            is_recording_2 = 0;
            is_editing = 0;
            StartEdit();
            SongStop();
            Actupated(0);
        }

        if(gui_action == GUI_CMD_RECORD_303)
        {
            StartRec();
        }

        if(gui_action == GUI_CMD_EDIT_MODE)
        {
            SongStop();
            Actupated(0);
            StartEdit();
        }

        if(gui_action == GUI_CMD_RECORD_MODE)
        {
            SongStop();
            is_recording ^= 1;
            is_recording_2 = 0;
            is_editing = 0;
            key_record_first_time = TRUE;
            for(int i = 0; i < MAX_TRACKS; i++)
            {
                Alloc_midi_Channels[i] = 0;
            }
            Actupated(0);
            StartEdit();
        }

        if(gui_action == GUI_CMD_CHANGE_BPM_TICKS_NBR)
        {
            Display_Beat_Time();
            Actualize_Master(teac);
        }

        if(gui_action == GUI_CMD_CHANGE_TRACKS_NBR)
        {
            Actualize_Master(teac);
            Set_Track_Slider(gui_track);
            Draw_Scope();
            Display_Tracks_To_Render();
        }

        if(gui_action == GUI_CMD_SET_TRACK_CUTOFF_FREQ)
        {
            TCut[ped_track] = float(Mouse.x - 88);
            Actualize_Track_Ed(1);
            liveparam = LIVE_PARAM_TRACK_CUTOFF;
            livevalue = (Mouse.x - 88) * 2;
        }

        if(gui_action == GUI_CMD_SET_TRACK_RESONANCE)
        {
            FRez[ped_track] = Mouse.x - 88;
            Actualize_Track_Ed(2);
            liveparam = LIVE_PARAM_TRACK_RESONANCE;
            livevalue = (Mouse.x - 88) * 2;
        }

        if(gui_action == GUI_CMD_SET_TRACK_INERTIA)
        {
            ICut[ped_track] = (Mouse.x - 88.0f) * 0.00006103515625f;
            Actualize_Track_Ed(3);
        }

        if(gui_action == GUI_CMD_UPDATE_TRACK_ED)
        {
            Actualize_Track_Ed(teac);
        }

        if(gui_action == GUI_CMD_SET_TRACK_THRESHOLD)
        {
            DThreshold[ped_track] = float((Mouse.x - 318) * 512);
            Actualize_Track_Ed(7);
            liveparam = LIVE_PARAM_TRACK_THRESHOLD;
            livevalue = (Mouse.x - 318) * 2;
        }

        if(gui_action == GUI_CMD_SET_TRACK_CLAMP)
        {
            DClamp[ped_track] = float((Mouse.x - 318) * 512);
            Actualize_Track_Ed(8);
            liveparam = LIVE_PARAM_TRACK_CLAMP;
            livevalue = (Mouse.x - 318) * 2;
        }

        if(gui_action == GUI_CMD_SET_TRACK_REVERB_SEND)
        {
            DSend[ped_track] = float(((float) Mouse.x - 318) / 128.0f);
            Actualize_Track_Ed(5);
            liveparam = LIVE_PARAM_TRACK_REVERB_SEND;
            livevalue = (Mouse.x - 318) * 2;
        }

        if(gui_action == GUI_CMD_SET_TRACK_PANNING)
        {
            TPan[ped_track] = ((float) Mouse.x - 318) / 128.0f;
            Actualize_Track_Ed(9);
            liveparam = LIVE_PARAM_TRACK_PANNING;
            livevalue = Mouse.x - 318;
        }

        // Tabs select
        if(gui_action == GUI_CMD_SELECT_DISKIO)
        {
            retletter[71] = TRUE;
            Actualize_Input();
            retletter[71] = FALSE;
            userscreen = USER_SCREEN_DISKIO_EDIT;
            Draw_DiskIO_Ed(); 
            Actualize_DiskIO_Ed();
        }

        if(gui_action == GUI_CMD_SELECT_TRACK_EDIT)
        {
            retletter[71] = TRUE;
            Actualize_Input();
            retletter[71] = FALSE;
            userscreen = USER_SCREEN_TRACK_EDIT;
            Draw_Track_Ed();
            Actualize_Track_Ed(0);
        }

        if(gui_action == GUI_CMD_SELECT_INSTRUMENT_EDIT)
        {
            retletter[71] = TRUE;
            Actualize_Input();
            retletter[71] = FALSE;
            userscreen = USER_SCREEN_INSTRUMENT_EDIT;
            Draw_Instrument_Ed();
            Actualize_Instrument_Ed(2, 0);
        }

        if(gui_action == GUI_CMD_SELECT_FX_EDIT)
        {
            retletter[71] = TRUE;
            Actualize_Input();
            retletter[71] = FALSE;
            userscreen = USER_SCREEN_FX_SETUP_EDIT;
            Draw_Fx_Ed();
            Actualize_Fx_Ed(teac);
        }

        if(gui_action == GUI_CMD_SELECT_SEQUENCER)
        {
            retletter[71] = TRUE;
            Actualize_Input();
            retletter[71] = FALSE;
            userscreen = USER_SCREEN_SEQUENCER;
            Draw_Sequencer_Ed();
            Actualize_Seq_Ed();
        }

        if(gui_action == GUI_CMD_SELECT_SCREEN_SETUP_EDIT)
        {
            retletter[71] = TRUE;
            Actualize_Input();
            retletter[71] = FALSE;
            userscreen = USER_SCREEN_SETUP_EDIT;
            Draw_Master_Ed();
            Actualize_Master_Ed(0);
        }

        if(gui_action == GUI_CMD_SELECT_SYNTH_EDIT)
        {
            retletter[71] = TRUE;
            Actualize_Input();
            retletter[71] = FALSE;
            userscreen = USER_SCREEN_SYNTH_EDIT;
            Draw_Synth_Ed();
            Actualize_Synth_Ed(UPDATE_SYNTH_ED_ALL);
        }

        if(gui_action == GUI_CMD_SELECT_TRACK_FX_EDIT)
        {
            retletter[71] = TRUE;
            Actualize_Input();
            retletter[71] = FALSE;
            userscreen = USER_SCREEN_TRACK_FX_EDIT;
            Draw_Track_Fx_Ed();
            Actualize_Track_Fx_Ed(teac);
        }

        if(gui_action == GUI_CMD_SELECT_SAMPLE_EDIT)
        {
            retletter[71] = TRUE;
            Actualize_Input();
            retletter[71] = FALSE;
            userscreen = USER_SCREEN_SAMPLE_EDIT;
            Draw_Sample_Ed();
            Actualize_Wave_Ed(0);
        }

        if(gui_action == GUI_CMD_SELECT_TB303_EDIT)
        {
            retletter[71] = TRUE;
            Actualize_Input();
            retletter[71] = FALSE;
            userscreen = USER_SCREEN_TB303_EDIT;
            Draw_303_Ed();
        }

        if(gui_action == GUI_CMD_SAVE_INSTRUMENT)
        {
            SaveInst();
        }

        if(gui_action == GUI_CMD_MODULE_INFOS)
        {
            ShowInfo();
        }

        if(gui_action == GUI_CMD_REFRESH_TB303_PARAMS)
        {
            Actualize_303_Ed(teac);
        }

        if(gui_action == GUI_CMD_SAVE_303_PATTERN)
        {
            Save303();
        }

        if(gui_action == GUI_CMD_FILELIST_SCROLL)
        {
            ltActualize(1);
        }

        if(gui_action == GUI_CMD_UPDATE_LOOP_EDITOR_ED)
        {
            Actualize_Instrument_Ed(1, teac);
        }

        if(gui_action == GUI_CMD_SAVE_MODULE)
        {
            Pack_Module(name);
        }

        // Save final mod
        if(gui_action == GUI_CMD_SAVE_FINAL)
        {
            SaveMod(name, TRUE, SAVE_WRITE, NULL);
        }

        // Calculate packed mod size
        if(gui_action == GUI_CMD_CALC_FINAL)
        {
            Final_Mod_Length = TestMod();
            outlong(254, 506, Final_Mod_Length, 7);
        }

        if(gui_action == GUI_CMD_SET_INSTRUMENT_AMPLI)
        {
            SampleVol[ped_patsam][ped_split] = float((Mouse.x - 436) / 32.0f);
            Actualize_Instrument_Ed(0, 1);
        }

        if(gui_action == GUI_CMD_SET_INSTRUMENT_FINETUNE)
        {
            FineTune_Value = ((Mouse.x - 436) - 64) * 2;
            if(FineTune_Value > 127) FineTune_Value = 127;
            if(FineTune_Value < -127) FineTune_Value = -127;
            Finetune[ped_patsam][ped_split] = FineTune_Value;
            Actualize_Instrument_Ed(0, 2);
        }

        if(gui_action == GUI_CMD_SET_INSTRUMENT_DECAY)
        {
            FDecay[ped_patsam][ped_split] = float(Mouse.x - 62) / 8192.0f;
            Actualize_Instrument_Ed(0, 3);
        }

        if(gui_action == GUI_CMD_UPDATE_INSTRUMENT_ED)
        {
            Actualize_Instrument_Ed(0, teac);
        }

        if(gui_action == GUI_CMD_UPDATE_DISKIO_ED)
        {
            Actualize_DiskIO_Ed();
        }

        if(gui_action == GUI_CMD_NEW_MODULE)
        {
            Newmod();
        }

        if(gui_action == GUI_CMD_UPDATE_SEQ_ED)
        {
            Actualize_Seq_Ed();
        }

        if(gui_action == GUI_CMD_UPDATE_FX_ED)
        {
            Actualize_Fx_Ed(teac);
        }

        if(gui_action == GUI_CMD_UPDATE_SETUP_ED)
        {
            Actualize_Master_Ed(teac);
        }

        if(gui_action == GUI_CMD_UPDATE_SYNTH_ED)
        {
            Actualize_Synth_Ed(teac);
        }

        if(gui_action == GUI_CMD_MIDI_NOTE_OFF_1_TRACK)
        {
            mess_box("Notes Off command sent to this track...");
        }

        if(gui_action == GUI_CMD_MIDI_NOTE_OFF_ALL_TRACKS)
        {
            mess_box("Notes Off command sent to all tracks...");
        }

        if(gui_action == GUI_CMD_UPDATE_TRACK_FX_ED)
        {
            Actualize_Track_Fx_Ed(teac);
        }

        if(gui_action == GUI_CMD_UPDATE_MIDI_303_ED)
        {
            Actualize_303_Ed(teac);
        }

        if(gui_action == GUI_CMD_PREVIOUS_16_INSTR)
        {
            ped_patsam -= 16;
            Actualize_Patterned();
            RefreshSample();
            NewWav();
            Actualize_Synth_Ed(UPDATE_SYNTH_ED_ALL);
        }

        if(gui_action == GUI_CMD_NEXT_16_INSTR)
        {
            ped_patsam += 16;
            Actualize_Patterned();
            RefreshSample();
            NewWav();
            Actualize_Synth_Ed(UPDATE_SYNTH_ED_ALL);
        }

        if(gui_action == GUI_CMD_UPDATE_PATTERN_ED)
        {
            Actualize_Patterned();
        }

        if(gui_action == GUI_CMD_REDUCE_POSITIONS_10)
        {
            if(Songplaying)
            {
                cPosition_delay -= 10;
                cPosition = cPosition_delay;
            }
            else
            {
                cPosition -= 10;
                Bound_Patt_Pos();
            }
            Actualize_Sequencer();
            Actupated(0);
        }

        if(gui_action == GUI_CMD_INCREASE_POSITIONS_10)
        {
            if(Songplaying)
            {
                cPosition_delay += 10;
                cPosition = cPosition_delay;
            }
            else
            {
                cPosition += 10;
                Bound_Patt_Pos();
            }
            Actualize_Sequencer();
            Actupated(0);
        }

        if(gui_action == GUI_CMD_REDUCE_PATTERNS_10)
        {
            if(Songplaying)
            {
                if(pSequence[cPosition_delay] > 9) pSequence[cPosition_delay] -= 10;
                else pSequence[cPosition_delay] = 0;
                Anat(cPosition_delay);
            }
            else
            {
                if(pSequence[cPosition] > 9) pSequence[cPosition] -= 10;
                else pSequence[cPosition] = 0;
                Bound_Patt_Pos();          
                Anat(cPosition);
            }
            Actualize_Sequencer();
            Actupated(0);
        }

        if(gui_action == GUI_CMD_INCREASE_PATTERNS_10)
        {
            if(Songplaying) {
                if(pSequence[cPosition_delay] < PAT_COL_MAX) pSequence[cPosition_delay] += 10;
                else pSequence[cPosition_delay] = 127;
                Anat(cPosition_delay);
            }
            else
            {
                if(pSequence[cPosition] < PAT_COL_MAX) pSequence[cPosition] += 10;
                else pSequence[cPosition] = 127;
                Bound_Patt_Pos();          
                Anat(cPosition);
            }
            Actualize_Sequencer();
            Actupated(0);
        }

        if(gui_action == GUI_CMD_RENDER_WAV)
        {
            WavRenderizer();
        }

        if(gui_action == GUI_CMD_TIMED_REFRESH_SEQUENCER)
        {
            Actualize_Sequencer();
            if(Songplaying) player_pos = cPosition_delay;
            else player_pos = cPosition;
        }

        if(gui_action == GUI_CMD_DELETE_INSTRUMENT)
        {
            DeleteInstrument();
        }

        if(gui_action == GUI_CMD_SAVE_SYNTH)
        {
            SaveSynth();
        }

        if(gui_action == GUI_CMD_PATTERNS_POOL_EXHAUSTED)
        {
            mess_box("Maximum number of patterns reached.");
        }

        if(gui_action == GUI_CMD_REFRESH_WAVE_ED)
        {
            Actualize_Wave_Ed(teac);
        }

        if(gui_action == GUI_CMD_ASK_EXIT)
        {
            Asking_Exit = TRUE;
            Draw_Ask_Exit();
        }

        if(gui_action == GUI_CMD_EXIT)
        {
            SongStop();
            mess_box("Seppuku...");
            return(FALSE);
        }

        gui_action = GUI_CMD_NOP;
    }

    // Draw the main windows layout
    if(redraw_everything == 1)
    {
        if(!display_title)
        {
            SetColor(LOGO_BKCOL);
            Fillrect(0, 0, 800, 600);
            Copy(LOGOPIC, (800 - LOGO_WIDTH) / 2, (600 - LOGO_HEIGHT) / 2, 0, 0, LOGO_WIDTH, LOGO_HEIGHT);
        }
        else
        {
            last_index = -1;
            Gui_Draw_Button_Box(MIN_VUMETER - 4, 6, (MAX_VUMETER - MIN_VUMETER) + 6, 16, "", BUTTON_NORMAL | BUTTON_DISABLED);

            Display_Master_Comp();
            Display_Master_Volume();
            Display_Shuffle();

            Draw_Scope_Files_Button();

            mess_box(tipoftheday);
            Gui_Draw_Button_Box(0, 178, fsize, 4, "", BUTTON_NORMAL | BUTTON_DISABLED);

            Gui_Draw_Button_Box(0, 24, 96, 98, "Player", BUTTON_NORMAL | BUTTON_DISABLED);
            Gui_Draw_Button_Box(8, 44, 80, 16, "Play Sng./Patt.", BUTTON_NORMAL);
            Gui_Draw_Button_Box(8, 62, 80, 16, "Stop     ", BUTTON_NORMAL);
            StartRec();
            StartEdit();

            Gui_Draw_Button_Box(98, 24, 156, 98, "Sequence", BUTTON_NORMAL | BUTTON_DISABLED);
            Gui_Draw_Button_Box(106, 44, 80, 16, "Position", BUTTON_NORMAL | BUTTON_DISABLED);
            Gui_Draw_Button_Box(106, 62, 80, 16, "Pattern", BUTTON_NORMAL | BUTTON_DISABLED);
            Gui_Draw_Button_Box(106, 80, 80, 16, "Song Length", BUTTON_NORMAL | BUTTON_DISABLED);
            Gui_Draw_Button_Box(106, 98, 80, 16, "Pattern Lines", BUTTON_NORMAL | BUTTON_DISABLED);

            Gui_Draw_Button_Box(256, 24, 136, 98, "Song Settings", BUTTON_NORMAL | BUTTON_DISABLED);

            Gui_Draw_Button_Box(262, 44, 60, 16, "Tracks", BUTTON_NORMAL | BUTTON_DISABLED);
            Gui_Draw_Button_Box(262, 62, 60, 16, "Beats/Min.", BUTTON_NORMAL | BUTTON_DISABLED);
            Gui_Draw_Button_Box(262, 80, 60, 16, "Ticks/Beat", BUTTON_NORMAL | BUTTON_DISABLED);
            Display_Beat_Time();

            Set_Track_Slider(gui_track);

            NewWav();

            Gui_Draw_Button_Box(0, 124, 392, 52, "", BUTTON_NORMAL | BUTTON_DISABLED);
            Gui_Draw_Button_Box(8, 152, 80, 16, "Step Add", BUTTON_NORMAL | BUTTON_DISABLED);
            Gui_Draw_Button_Box(166, 152, 90, 16, "Keyboard Octave", BUTTON_NORMAL | BUTTON_DISABLED);
            Gui_Draw_Button_Box(8, 134, 80, 16, "Instrument", BUTTON_NORMAL | BUTTON_DISABLED);
            Gui_Draw_Button_Box(320, 134, 64, 16, "Delete", BUTTON_NORMAL);

            Refresh_UI_Context();

            // CPU meter was here
            Gui_Draw_Button_Box(586 + 60 + 100, 6, 52, 16, CpuStr, BUTTON_NORMAL | BUTTON_DISABLED);

            ltActualize(0);

            // Vu meters background
            SetColor(COL_BACKGROUND);
            Fillrect(MIN_VUMETER - 1, 9, MAX_VUMETER, 20);

            Actupated(0);
        }
    }

    if(Songplaying && ped_line_delay != player_line)
    {
        if(!sr_isrecording) Actualize_Track_Ed(0);
        Actupated(0);
        player_line = ped_line_delay;
    }

    // Checking for mouse and keyboard events ------------------------------------
    Mouse_Handler();
    Keyboard_Handler();

    // Refresh the sequencer each time the song position is different
    if(Songplaying)
    {
        if(player_pos != cPosition_delay)
        {
            gui_action = GUI_CMD_TIMED_REFRESH_SEQUENCER;
        }
    }

    return(TRUE);
}

// ------------------------------------------------------
// Allocate space for a waveform
void AllocateWave(int n_index, long lenfir, int samplechans)
{
    // Freeing if memory was allocated before -----------------------
    if(SampleType[n_index][ped_split] != 0)
    {
        if(RawSamples[n_index][0][ped_split]) free(RawSamples[n_index][0][ped_split]);
        RawSamples[n_index][0][ped_split] = NULL;
        if(SampleChannels[n_index][ped_split] == 2)
        {
            if(RawSamples[n_index][1][ped_split]) free(RawSamples[n_index][1][ped_split]);
            RawSamples[n_index][1][ped_split] = NULL;
        }
    }

    SampleType[n_index][ped_split] = 1;
    // Gsm by default

#if !defined(__NO_CODEC__)
    SampleCompression[n_index] = SMP_PACK_GSM;
#else
    SampleCompression[n_index] = SMP_PACK_NONE;
#endif

    SampleChannels[n_index][ped_split] = samplechans;
    RawSamples[n_index][0][ped_split] = (short *) malloc(lenfir * 2);

    if(samplechans == 2) RawSamples[n_index][1][ped_split] = (short *) malloc(lenfir * 2);    

    LoopStart[n_index][ped_split] = 0;
    LoopEnd[n_index][ped_split] = lenfir;
    LoopType[n_index][ped_split] = SMP_LOOP_NONE;
    SampleNumSamples[n_index][ped_split] = lenfir;
    Finetune[n_index][ped_split] = 0;
    SampleVol[n_index][ped_split] = 1.0f;
}

// ------------------------------------------------------
// Attempt to load any supported file format
void LoadFile(int Freeindex, const char *str)
{
    long Datalen = 0;
    int ld0 = 0;
    int ld1 = 0;
    int ld2 = 0;
    int ld3 = 0;
    int Freeindex2 = 0;
    int idchk = 0;
    FILE *in;
    WaveFile Wav_File;

    int rate = 0;
    int bits = 0;
    short *csamples, *csamples2;
    char st_type = 0;
    short inx = 0;
    int fmtchklen = 0;

    const char *Wavfile = str;
    idchk = 0;

    if(Wavfile != NULL && (in = fopen(Wavfile, "rb")) != NULL)
    {
        char extension[10];
        unsigned extension_New;
        int modext;
        int found_mod;
        int i;
        //char rebext[5];

        fseek(in, 1080, SEEK_SET);
        fread(&modext, sizeof(char), 4, in);

        fseek(in, 0, SEEK_SET);
        fread(extension, sizeof(char), 9, in);

        fseek(in, 0, SEEK_SET);
        fread(&extension_New, sizeof(char), 4, in);

        found_mod = 0;
        for(i = 0; i < sizeof(mt_tags) / sizeof(int); i++)
        {
            if(Swap_32(mt_tags[i]) == modext)
            {
                found_mod = mt_channels[i];
                break;
            }
        }

        if(found_mod)
        {
            sprintf(name, "%s", Wavfile);
            // name / number of channels
            LoadAmigaMod(name, found_mod);
            NewWav();
            fclose(in);
            gui_action = GUI_CMD_NONE;
            Actualize_DiskIO_Ed();
            return;
        }

        //fseek(in, 8, SEEK_SET);
        //fread(rebext, sizeof(char), 4, in);

        if(strcmp(extension, "TWNNINS0") == 0 ||
           strcmp(extension, "TWNNINS1") == 0 ||
           strcmp(extension, "TWNNINS2") == 0 ||
           strcmp(extension, "TWNNINS3") == 0 ||
           strcmp(extension, "TWNNINS4") == 0)
        {
           sprintf(instrname, "%s", Wavfile);
            LoadInst(instrname);
            NewWav();
        }
        else if(strcmp(extension, "TWNNSNG2") == 0 ||
                strcmp(extension, "TWNNSNG3") == 0 ||
                strcmp(extension, "TWNNSNG4") == 0 ||
                strcmp(extension, "TWNNSNG5") == 0 ||
                strcmp(extension, "TWNNSNG6") == 0 ||
                strcmp(extension, "TWNNSNG7") == 0 ||
                strcmp(extension, "TWNNSNG8") == 0 ||
                strcmp(extension, "TWNNSNG9") == 0 ||
                strcmp(extension, "TWNNSNGA") == 0 ||
                extension_New == 'KTRP')
        {
            sprintf(name, "%s", Wavfile);
            LoadMod(name);
            NewWav();
        }
        else if(strcmp(extension, "TWNNSYN0") == 0 ||
                strcmp(extension, "TWNNSYN1") == 0)
        {
            sprintf(synthname, "%s", Wavfile);
            LoadSynth(synthname);
        }
        else if(strcmp(extension, "TWNN3030") == 0 ||
                strcmp(extension, "TWNN3031") == 0)
        {
            sprintf(name303, "%s", Wavfile);
            Load303(name303);
        }
        else
        {
            mess_box("Attempting of loading a wav file...");   

            fseek(in, 8, SEEK_SET);
            idchk = fgetc(in) * fgetc(in) * fgetc(in) * fgetc(in);

            // We need the length
            if(Wav_File.OpenForRead(Wavfile) == DDC_SUCCESS)
            {
                int bits = Wav_File.BitsPerSample();
                int channels = Wav_File.NumChannels();
                if(bits != 8 && bits != 16 && bits != 32)
                {
                    mess_box("Protrekkr can only load 8, 16 or 32 bits samples.");
                }
                else
                {
                    switch(channels)
                    {
                        case 1:
                            AllocateWave(Freeindex, Wav_File.NumSamples(), 1);
                            csamples = RawSamples[Freeindex][0][ped_split];
                            for(i = 0; i < Wav_File.NumSamples(); i++)
                            {
                                Wav_File.ReadMonoSample(&csamples[i]);
                            }
                            break;

                        case 2:
                            AllocateWave(Freeindex, Wav_File.NumSamples(), 2);
                            csamples = RawSamples[Freeindex][0][ped_split];
                            csamples2 = RawSamples[Freeindex][1][ped_split];
                            for(i = 0; i < Wav_File.NumSamples(); i++)
                            {
                                Wav_File.ReadStereoSample(&csamples[i], &csamples2[i]);
                            }
                            break;
                    }
                    LoopType[Freeindex][ped_split] = Wav_File.LoopType();
                    LoopStart[Freeindex][ped_split] = Wav_File.LoopStart();
                    LoopEnd[Freeindex][ped_split] = Wav_File.LoopEnd();

                    sprintf(SampleName[Freeindex][ped_split], "%s", Wavfile); 
                    Actualize_Patterned();
                    Actualize_Instrument_Ed(2, 0);
                    NewWav();
                    if(bits == 32) mess_box("32 bits WAV PCM converted into 16 bits format.");
                    else mess_box("WAV PCM loaded.");
                }

                Wav_File.Close();

            }
            else
            {
                mess_box("Invalid file format. I accept only '.wav' '.ptk' '.ptp' '.pti' '.pts' or '.mod'.");
            }
        }
        fclose(in);
    }
    else
    {
        mess_box("File loading error. (Probably: file not found)"); 
    }
    gui_action = GUI_CMD_NONE;
    Actualize_DiskIO_Ed();
}

// ------------------------------------------------------
// Function to search for a free wave on board
int GetFreeWave(void)
{
    int inico = 0;
    int exiter = 0;
    int valuer = -1;

    do
    {
        if(SampleType[inico] == 0)
        {
            exiter = 1;
            valuer = inico;
        }
        inico++;
    }
    while(exiter == 0);

    return valuer;
}

// ------------------------------------------------------
// Start replaying
void SongPlay(void)
{
    SongStop();
    liveparam = 0;
    livevalue = 0;
    player_pos = -1;

    Post_Song_Init();
    Ptk_Play();

    if(plx == 0)
    {
        Gui_Draw_Button_Box(8, 44, 80, 16, "Playing Song", BUTTON_PUSHED);
        mess_box("Playing song...");
    }
    else
    {
        Gui_Draw_Button_Box(8, 44, 80, 16, "Playing Pattern", BUTTON_PUSHED);
        mess_box("Playing pattern...");
    }
}

// ------------------------------------------------------
// Turn live recording on/off
// "startrec" get it ? haha :(
void StartRec(void)
{
    liveparam = 0;
    livevalue = 0;
    if(sr_isrecording) Gui_Draw_Button_Box(8, 80, 80, 16, "Live Rec: ON", BUTTON_PUSHED);
    else Gui_Draw_Button_Box(8, 80, 80, 16, "Live Rec: OFF", BUTTON_NORMAL);
}

// ------------------------------------------------------
// Turn edit on/off
void StartEdit(void)
{
    if(is_editing && !is_recording)
    {
        Gui_Draw_Button_Box(8, 98, 80, 16, "Editing...", BUTTON_PUSHED);
    }
    if(is_recording)
    {
        Gui_Draw_Button_Box(8, 98, 80, 16, "Recording...", BUTTON_PUSHED);
    }
    if(!is_editing && !is_recording)
    {
        Gui_Draw_Button_Box(8, 98, 80, 16, "Edit/Record", BUTTON_NORMAL);
    }
}

// ------------------------------------------------------
// Stop replaying
void SongStop(void)
{
    Gui_Draw_Button_Box(8, 44, 80, 16, "Play Sng./Patt.", BUTTON_NORMAL);
    mess_box("Ready...");

    // Store the visual position (not the replayed one)
    if(Songplaying)
    {
        cPosition = cPosition_delay;
        ped_line = ped_line_delay;
    }

#if !defined(__NO_MIDI__)
    Midi_AllNotesOff();
#endif

    // Clear all channels
    for(int i = 0; i < MAX_TRACKS; i++)
    {
        Alloc_midi_Channels[i] = 0;
    }

    Ptk_Stop();
}

// ------------------------------------------------------
// Create a new song
void Newmod(void)
{
    SongStop();
    StopIfSp();
    mess_box("Freeing all allocated buffers and restarting...");   

    Free_Samples();

    for(int api = 0; api < 128; api++)
    {
        patternLines[api] = 64;
    }

    for(int inicial = 0; inicial < PBLEN; inicial += 6)
    {
        *(RawPatterns + inicial) = 121;     // 121
        *(RawPatterns + inicial + 1) = 255; // 255
        *(RawPatterns + inicial + 2) = 255; // 255
        *(RawPatterns + inicial + 3) = 255; // 255
        *(RawPatterns + inicial + 4) = 0;   // 0
        *(RawPatterns + inicial + 5) = 0;   // 0
    }

    nPatterns = 1;

#if !defined(__NO_MIDI__)
    Midi_Reset();
#endif

    Pre_Song_Init();
    Post_Song_Init();

    sprintf(name, "Untitled");
    sprintf(artist, "Somebody");
    sprintf(style, "Goa Trance");
    namesize = 8;
    sLength = 1;
    BeatsPerMin = 125;
    TicksPerBeat = 4;
    SubCounter = 0;
    PosInTick = 0;
    DelayType = 1;
    player_line = 0;
    mas_vol = 1.0f;
    mas_comp_threshold = 100.0f;
    mas_comp_ratio = 0.0f;
    ped_track = 0;
    ped_patsam = 0;
    ped_row = 0;
    ped_line = 0;
    ped_line_delay = 0;
    cPosition = 0;
    cPosition_delay = 0;

    allow_save = TRUE;

    lchorus_feedback = 0.6f;
    rchorus_feedback = 0.5f;
    lchorus_delay = 10584;
    rchorus_delay = 15876;

    lchorus_counter = 44100;
    rchorus_counter = 44100;
    lchorus_counter2 = 44100 - lchorus_delay;
    rchorus_counter2 = 44100 - rchorus_delay;
    compressor = 0;
    c_threshold = 32;
    DelayType = 1;
    delay_time = 0;
    seditor = 0;

    Final_Mod_Length = 0;

    for(int spl = 0; spl < MAX_TRACKS; spl++)
    {
        CCoef[spl] = float((float) CSend[spl] / 127.0f);
    }
    Actualize_Sequencer();
    Actualize_Patterned();
    Actualize_Master(0);
    if(snamesel == 1 || snamesel == 4 || snamesel == 5) snamesel = 0;
    Actualize_DiskIO_Ed();

    Reset_Song_Length();
    Display_Song_Length();
    Draw_Scope();

    mess_box("New song started...");
    SDL_Delay(300);
    NewWav();
    Actupated(0);
    Reset_Tracks_To_Render();
}

// ------------------------------------------------------
// ------------------------------------------------------
// ------------------------------------------------------

// ------------------------------------------------------
// Change a name
char *table_newletter[71] =
{
    NULL, // 0
    "%sa",
    "%sb",
    "%sc",
    "%sd",
    "%se",
    "%sf",
    "%sg",
    "%sh",
    "%si",
    "%sj", // 10
    "%sk",
    "%sl",
    "%sm",
    "%sn",
    "%so",
    "%sp",
    "%sq",
    "%sr",
    "%ss",
    "%st", // 20
    "%su",
    "%sv",
    "%sw",
    "%sx",
    "%sy",
    "%sz",
    "%s0",
    "%s1",
    "%s2",
    "%s3",
    "%s4",
    "%s5",
    "%s6",
    "%s7",
    "%s8",
    "%s9",
    NULL, // 37
    NULL, // 38
    NULL, // 39
    "%s.", // 40
    "%sA",
    "%sB",
    "%sC",
    "%sD",
    "%sE",
    "%sF",
    "%sG",
    "%sH",
    "%sI",
    "%sJ", // 50
    "%sK",
    "%sL",
    "%sM",
    "%sN",
    "%sO",
    "%sP",
    "%sQ",
    "%sR",
    "%sS",
    "%sT", // 60
    "%sU",
    "%sV",
    "%sW",
    "%sX",
    "%sY",
    "%sZ",
    "%s,", // 67
    NULL,  // 68
    "%s-", // 69
    NULL   // 70
};

// ------------------------------------------------------
// Change a letter in a string
void Actualize_Name(int *newletter, char *nam)
{
    int i;

    // Cancel input
    if(newletter[71])
    {
        newletter[71] = FALSE;
        strcpy(nam, cur_input_name);
        snamesel = 0;
        return;
    }
    // Valid input
    if(newletter[39] && namesize > 0)
    {
        newletter[39] = FALSE;
        snamesel = 0;
        return;
    }

    for(i = 0; i < 71; i++)
    {
        if(i == 37)
        {
            // Backspace
            if(newletter[i])
            {
                if(namesize > 0)
                {
                    nam[strlen(nam) - 1] = '\0';
                    if(namesize > 0) namesize--;
                }
                newletter[i] = FALSE;
            }
        }
        else
        {
            if(newletter[i])
            {
                if(namesize < 18)
                {
                    if(table_newletter[i]) sprintf(nam, table_newletter[i], nam);
                    namesize++;
                }
                newletter[i] = FALSE;
            }
        }
    }
}

// ------------------------------------------------------
// Render the song as a .wav file
void WavRenderizer(void)
{
    plx = 0;
    char buffer[80];
    sprintf(buffer, "%s.wav", name);
    int Save_TRACKSTATE[MAX_TRACKS];
    int i;

    if(!hd_isrecording)
    {
        WaveFile RF;
        RF.OpenForWrite(buffer, 44100, 16, 2);
        SongStop();
        SDL_Delay(500);
        sprintf(buffer, "Rendering module to '%s.wav' file. Please wait...", name);
        mess_box(buffer);
        SDL_Delay(500);

        ped_line = 0;
        ped_line_delay = 0;
        cPosition = 0;
        cPosition_delay = 0;

        SongPlay();

        long filesize = 0;
        char bru = FALSE;

        for(i = 0; i < MAX_TRACKS; i++)
        {
            Save_TRACKSTATE[i] = TRACKSTATE[i];
            TRACKSTATE[i] = Tracks_To_Render[i];
        }

        while(cPosition > 0 || ped_line > 0 || bru == FALSE)
        {
            if(ped_line > 0) bru = TRUE;
            GetPlayerValues(mas_vol); // <-- L INT
            RF.WriteStereoSample(left_value, right_value);
            filesize += 4;
        }

        for(i = 0; i < MAX_TRACKS; i++)
        {
            TRACKSTATE[i] = Save_TRACKSTATE[i];
        }

        RF.Close();
        SongStop();
        int minutes = filesize / 10584000;
        int seconds = (filesize - minutes * 10584000) / 176400;

        sprintf(buffer, "Wav rendering finished. File size: %.2f Megabytes. Playback time: %d'%d''.", float(filesize / 1048576.0f), minutes, seconds);
        mess_box(buffer);

        // Retrurn to the start as all the values will be trashed anyway.
        ped_line = 0;
        ped_line_delay = 0;
        cPosition = 0;
        cPosition_delay = 0;
        Actualize_DiskIO_Ed();

        last_index = -1;
        Read_SMPT();
        ltActualize(0);

        mess_box(buffer);
        SDL_Delay(500);
        Actupated(0);
    }
    rawrender = FALSE;
}

// ------------------------------------------------------
// Delete an instrument
void DeleteInstrument(void)
{
    AUDIO_Stop();
    Gui_Draw_Button_Box(320, 134, 64, 16, "Delete", BUTTON_PUSHED);
    StopIfSp();
    SDL_Delay(256);

    beatsync[ped_patsam] = FALSE;
    beatlines[ped_patsam] = 16;
    sprintf(nameins[ped_patsam], "Untitled");
    ResetSynthParameters(&PARASynth[ped_patsam]);
    KillInst(ped_patsam);
    mess_box("Instrument deleted.");
    Gui_Draw_Button_Box(320, 134, 64, 16, "Delete", BUTTON_NORMAL);
    RefreshSample();
    NewWav();
    AUDIO_Play();
    Actualize_Synth_Ed(UPDATE_SYNTH_ED_ALL);
    Actualize_Patterned();
}

void StopIfSp(void)
{
   for(char u = 0; u < MAX_TRACKS; u++)
   {
      if(sp_channelsample[u] == ped_patsam)
      {
         sp_Stage[u] = PLAYING_NOSAMPLE;
         Player_FD[u] = 0;
         ramper[u] = 0;
      }
   }
}

// ------------------------------------------------------
// Refresh the view of a sample
void RefreshSample(void)
{
    seditor = 0;
    ped_split = 0;
    if(userscreen == USER_SCREEN_INSTRUMENT_EDIT)
    {
        Draw_Instrument_Ed();
        Actualize_Instrument_Ed(2, 0);
    }
}

// ------------------------------------------------------
// Show song informations (summing instruments and patterns)
void ShowInfo(void)
{
   char tmp[256];
   int pattsize = nPatterns * 12288;
   int sampsize = 0;

   for(int pp = 0; pp < 128; pp++)
   {
      for(int z = 0; z < 16; z++)
      {
         if(SampleType[pp][z] != 0)
         {
            sampsize += SampleChannels[pp][z] * SampleNumSamples[pp][z];   
         }
      }
   }
   sprintf(tmp, "Sample bank size: %d bytes, pattern bank [%d patterns] size: %d bytes.", sampsize, nPatterns, pattsize);
   mess_box(tmp);
}

// ------------------------------------------------------
// Function called each second mainly for autosaving purposes
Uint32 Timer_CallBack(Uint32 interval, void *param)
{
    if(!display_title)
    {
        wait_title++;
        if(wait_title > 6) Remove_Title();
    }

    if(display_title)
    {
        if(AutoSave)
        {
            wait_AutoSave++;
            // Autosave module
            if(wait_AutoSave > Values_AutoSave[AutoSave])
            {
                strcpy(autosavename, name);
                strcat(autosavename, "_bak");
                Pack_Module(autosavename);
            }
        }
    }

    return(interval);
}

// ------------------------------------------------------
// Validate or cancel an user input
void Actualize_Input(void)
{
    switch(snamesel)
    {
        case 1:
            Actualize_Name(retletter, name);
            gui_action = GUI_CMD_UPDATE_DISKIO_ED;
            break;

        case 2:
            Actualize_Name(retletter, nameins[ped_patsam]);
            gui_action = GUI_CMD_UPDATE_PATTERN_ED;
            break;

        case 3:
            Actualize_Name(retletter, PARASynth[ped_patsam].presetname);
            teac = UPDATE_SYNTH_CHANGE_NAME;
            gui_action = GUI_CMD_UPDATE_SYNTH_ED;
            break;

        case 4:
            Actualize_Name(retletter, artist);
            gui_action = GUI_CMD_UPDATE_DISKIO_ED;
            break;

        case 5:
            Actualize_Name(retletter, style);
            gui_action = GUI_CMD_UPDATE_DISKIO_ED;
            break;

        case 6:
            Actualize_Name(retletter, tb303[sl3].pattern_name[tb303[sl3].selectedpattern]);
            teac = 18;
            gui_action = GUI_CMD_UPDATE_MIDI_303_ED;
            break;
    }
}

// ------------------------------------------------------
// KeyBoard ShortCut Handler
void Keyboard_Handler(void)
{
    int Cur_Position;

    if(Songplaying) Cur_Position = cPosition_delay;
    else Cur_Position = cPosition;

    // Exit tracker
    if(Get_LAlt() && Keys[SDLK_F4]) gui_action = GUI_CMD_EXIT;
    // Lower octave
    if(!Get_LShift() && Keys[SDLK_F1]) gui_action = GUI_CMD_LOWER_OCTAVE;
    // Higher octave
    if(!Get_LShift() && Keys[SDLK_F2]) gui_action = GUI_CMD_HIGHER_OCTAVE;

    if(Get_LShift())
    {
        if(Keys[SDLK_F1])
        {
            gui_action = GUI_CMD_SELECT_SEQUENCER;
        }
        if(Keys[SDLK_F2])
        {
            gui_action = GUI_CMD_SELECT_INSTRUMENT_EDIT;
        }
        if(Keys[SDLK_F3])
        {
            gui_action = GUI_CMD_SELECT_SYNTH_EDIT;
        }
        if(Keys[SDLK_F4])
        {
            gui_action = GUI_CMD_SELECT_SAMPLE_EDIT;
        }
        if(Keys[SDLK_F5])
        {
            gui_action = GUI_CMD_SELECT_TB303_EDIT;
        }
        if(Keys[SDLK_F6])
        {
            gui_action = GUI_CMD_SELECT_TRACK_EDIT;
        }
        if(Keys[SDLK_F7])
        {
            gui_action = GUI_CMD_SELECT_TRACK_FX_EDIT;
        }
        if(Keys[SDLK_F8])
        {
            gui_action = GUI_CMD_SELECT_FX_EDIT;
        }
        if(Keys[SDLK_F9])
        {
            gui_action = GUI_CMD_SELECT_DISKIO;
        }
        if(Keys[SDLK_F10])
        {
            gui_action = GUI_CMD_SELECT_SCREEN_SETUP_EDIT;
        }
    }

    // Previous track
    if(Get_LShift() && Keys[SDLK_TAB])
    {
        Unselect_Selection();
        ped_track--;
        ped_row = 0;
        Actupated(1);
        gui_action = GUI_CMD_SET_FOCUS_TRACK;
    }

    // Next track
    if(!Get_LShift() && !Get_LCtrl() && Keys[SDLK_TAB])
    {
        Unselect_Selection();
        ped_track++;
        ped_row = 0;
        Actupated(1);
        gui_action = GUI_CMD_SET_FOCUS_TRACK;
    }

    if(!is_recording)
    {
        if(!Get_LShift())
        {
            // Jump to row 0
            if(Keys[SDLK_F5] && ped_line != 0)
            {
                Select_Block_Keyboard(BLOCK_MARK_ROWS);
                ped_line = 0;
                Actupated(0);
                Select_Block_Keyboard(BLOCK_MARK_ROWS);
            }

            // Jump to row 16
            if(Keys[SDLK_F6] && ped_line != 16)
            {
                Select_Block_Keyboard(BLOCK_MARK_ROWS);
                ped_line = 16;
                Actupated(0);
                Select_Block_Keyboard(BLOCK_MARK_ROWS);
            }

            // Jump to row 32
            if(Keys[SDLK_F7] && ped_line != 32)
            {
                Select_Block_Keyboard(BLOCK_MARK_ROWS);
                ped_line = 32;
                Actupated(0);
                Select_Block_Keyboard(BLOCK_MARK_ROWS);
            }

            // Jump to row 48
            if(Keys[SDLK_F8] && ped_line != 48)
            {
                Select_Block_Keyboard(BLOCK_MARK_ROWS);
                ped_line = 48;
                Actupated(0);
                Select_Block_Keyboard(BLOCK_MARK_ROWS);
            }

            // Jump to row 63
            if(Keys[SDLK_F9] && ped_line != 63)
            {
                Select_Block_Keyboard(BLOCK_MARK_ROWS);
                ped_line = 63;
                Actupated(0);
                Select_Block_Keyboard(BLOCK_MARK_ROWS);
            }
        }

        // Top left of pattern
        if(Keys[SDLK_HOME])
        {
            Select_Block_Keyboard(BLOCK_MARK_ROWS | BLOCK_MARK_TRACKS);
            ped_row = 0;
            ped_track = 0;
            if(Get_LCtrl()) ped_line = 0;
            Actupated(0);
            Select_Block_Keyboard(BLOCK_MARK_ROWS | BLOCK_MARK_TRACKS);
            gui_action = GUI_CMD_SET_FOCUS_TRACK;
        }

        // Bottom right of pattern
        if(Keys[SDLK_END])
        {
            Select_Block_Keyboard(BLOCK_MARK_ROWS | BLOCK_MARK_TRACKS);
            ped_row = 0;
            ped_track = Songtracks - 1;
            if(Get_LCtrl()) ped_line = patternLines[pSequence[cPosition_delay]] - 1;
            Actupated(0);
            gui_action = GUI_CMD_SET_FOCUS_TRACK;
            Select_Block_Keyboard(BLOCK_MARK_ROWS | BLOCK_MARK_TRACKS);
        }

        // Previous column or previous track
        if(Keys[SDLK_LEFT] && !Get_LCtrl() && !Get_LAlt())
        {
            Select_Block_Keyboard(BLOCK_MARK_TRACKS);
            ped_row--;
            Actupated(0);
            Select_Block_Keyboard(BLOCK_MARK_TRACKS);
            gui_action = GUI_CMD_SET_FOCUS_TRACK;
        }

        // Next column or next track
        if(Keys[SDLK_RIGHT] && !Get_LCtrl() && !Get_LAlt())
        {
            Select_Block_Keyboard(BLOCK_MARK_TRACKS);
            ped_row++;
            Actupated(0);
            Select_Block_Keyboard(BLOCK_MARK_TRACKS);
            gui_action = GUI_CMD_SET_FOCUS_TRACK;
        }

        // Previous row
        if(Keys[SDLK_UP] && !Songplaying)
        {
            Select_Block_Keyboard(BLOCK_MARK_ROWS);
            ped_line--;
            if(Continuous_Scroll && !Cur_Position) if(ped_line < 0) ped_line = 0;
            Actupated(0);
            Select_Block_Keyboard(BLOCK_MARK_ROWS);
        }

        // Next row
        if(Keys[SDLK_DOWN] && !Songplaying)
        {
            Select_Block_Keyboard(BLOCK_MARK_ROWS);
            ped_line++;
            if(Continuous_Scroll && (Cur_Position == sLength - 1)) if(ped_line >= patternLines[pSequence[Cur_Position]]) ped_line = patternLines[pSequence[Cur_Position]] - 1;
            Actupated(0);
            Select_Block_Keyboard(BLOCK_MARK_ROWS);
        }

        // Previous page (16 rows)
        if(Keys[SDLK_PAGEUP] && !Songplaying)
        {
            Select_Block_Keyboard(BLOCK_MARK_ROWS);
            ped_line -= 16;
            if(!is_recording && !Continuous_Scroll) if(ped_line < 0) ped_line = 0;
            if(Continuous_Scroll && !Cur_Position) if(ped_line < 0) ped_line = 0;
            Actupated(0);
            Select_Block_Keyboard(BLOCK_MARK_ROWS);
        }

        // Next page (16 rows)
        if(Keys[SDLK_PAGEDOWN] && !Songplaying)
        {
            Select_Block_Keyboard(BLOCK_MARK_ROWS);
            ped_line += 16;
            if(!is_recording && !Continuous_Scroll) if(ped_line >= patternLines[pSequence[Cur_Position]]) ped_line = patternLines[pSequence[Cur_Position]] - 1;
            if(Continuous_Scroll && (Cur_Position == sLength - 1)) if(ped_line >= patternLines[pSequence[Cur_Position]]) ped_line = patternLines[pSequence[Cur_Position]] - 1;
            Actupated(0);
            Select_Block_Keyboard(BLOCK_MARK_ROWS);
        }

        // Previous pattern
        if((Keys[SDLK_KP_MINUS]) && pSequence[Cur_Position] > 0)
        {
            gui_action = GUI_CMD_PREV_PATT;
        }

        // Next pattern
        if((Keys[SDLK_KP_PLUS]) && pSequence[Cur_Position] < 254)
        {
            gui_action = GUI_CMD_NEXT_PATT;
        }

        if(Keys[SDLK_INSERT] && is_editing)
        {
            // INSERT
            if(Get_LShift()) Insert_Pattern_Line(Cur_Position);
            else Insert_Track_Line(ped_track, Cur_Position);
        }

        if(snamesel == 0)
        {
            if(Keys[SDLK_BACKSPACE] && is_editing)
            {
                // BACKSPACE
                if(ped_line)
                {
                    ped_line--;
                    if(Get_LShift()) Remove_Pattern_Line(Cur_Position);
                    else Remove_Track_Line(ped_track, Cur_Position);
                }
            }
        }
    }

    // Lower octave
    if((Keys_Sym[SDLK_KP_DIVIDE]))
    {
        gui_action = GUI_CMD_LOWER_OCTAVE;
        Keys_Sym[SDLK_KP_DIVIDE] = FALSE;
    }
    // Upper octave
    if((Keys_Sym[SDLK_KP_MULTIPLY]))
    {
        gui_action = GUI_CMD_HIGHER_OCTAVE;
        Keys_Sym[SDLK_KP_MULTIPLY] = FALSE;
    }

    int retnote = 120;      // NO NOTE
    int retnote_raw = 120;  // NO NOTE
    int retvalue = 120;     // NOVALUE
    int reelletter = FALSE;

    // -------------------------------------------
    // Editing a name
    if(snamesel > 0)
    {
        if(Keys[SDLK_a])
        {
            retletter[1] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_b])
        {
            retletter[2] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_c])
        {
            retletter[3] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_d])
        {
            retletter[4] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_e])
        {
            retletter[5] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_f])
        {
            retletter[6] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_g])
        {
            retletter[7] = TRUE;
            reelletter = TRUE;
        }     
        if(Keys[SDLK_h])
        {
            retletter[8] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_i])
        {
            retletter[9] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_j])
        {
            retletter[10] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_k])
        {
            retletter[11] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_l])
        {
            retletter[12] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_m])
        {
            retletter[13] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_n])
        {
            retletter[14] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_o])
        {
            retletter[15] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_p])
        {
            retletter[16] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_q])
        {
            retletter[17] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_r])
        {
            retletter[18] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_s])
        {
            retletter[19] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_t])
        {
            retletter[20] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_u])
        {
            retletter[21] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_v])
        {
            retletter[22] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_w])
        {
            retletter[23] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_x])
        {
            retletter[24] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_y])
        {
            retletter[25] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_z])
        {
            retletter[26] = TRUE;
            reelletter = TRUE;
        }

        if(Keys['A'])
        {
            retletter[41] = TRUE;
            reelletter = TRUE;
        }
        if(Keys['B'])
        {
            retletter[42] = TRUE;
            reelletter = TRUE;
        }
        if(Keys['C'])
        {
            retletter[43] = TRUE;
            reelletter = TRUE;
        }
        if(Keys['D'])
        {
            retletter[44] = TRUE;
            reelletter = TRUE;
        }
        if(Keys['E'])
        {
            retletter[45] = TRUE;
            reelletter = TRUE;
        }
        if(Keys['F'])
        {
            retletter[46] = TRUE;
            reelletter = TRUE;
        }
        if(Keys['G'])
        {
            retletter[47] = TRUE;
            reelletter = TRUE;
        }     
        if(Keys['H'])
        {
            retletter[48] = TRUE;
            reelletter = TRUE;
        }
        if(Keys['I'])
        {
            retletter[49] = TRUE;
            reelletter = TRUE;
        }
        if(Keys['J'])
        {
            retletter[50] = TRUE;
            reelletter = TRUE;
        }
        if(Keys['K'])
        {
            retletter[51] = TRUE;
            reelletter = TRUE;
        }
        if(Keys['L'])
        {
            retletter[52] = TRUE;
            reelletter = TRUE;
        }
        if(Keys['M'])
        {
            retletter[53] = TRUE;
            reelletter = TRUE;
        }
        if(Keys['N'])
        {
            retletter[54] = TRUE;
            reelletter = TRUE;
        }
        if(Keys['O'])
        {
            retletter[55] = TRUE;
            reelletter = TRUE;
        }
        if(Keys['P'])
        {
            retletter[56] = TRUE;
            reelletter = TRUE;
        }
        if(Keys['Q'])
        {
            retletter[57] = TRUE;
            reelletter = TRUE;
        }
        if(Keys['R'])
        {
            retletter[58] = TRUE;
            reelletter = TRUE;
        }
        if(Keys['S'])
        {
            retletter[59] = TRUE;
            reelletter = TRUE;
        }
        if(Keys['T'])
        {
            retletter[60] = TRUE;
            reelletter = TRUE;
        }
        if(Keys['U'])
        {
            retletter[61] = TRUE;
            reelletter = TRUE;
        }
        if(Keys['V'])
        {
            retletter[62] = TRUE;
            reelletter = TRUE;
        }
        if(Keys['W'])
        {
            retletter[63] = TRUE;
            reelletter = TRUE;
        }
        if(Keys['X'])
        {
            retletter[64] = TRUE;
            reelletter = TRUE;
        }
        if(Keys['Y'])
        {
            retletter[65] = TRUE;
            reelletter = TRUE;
        }
        if(Keys['Z'])
        {
            retletter[66] = TRUE;
            reelletter = TRUE;
        }

        if(Keys[SDLK_KP0])
        {
            retletter[27] = TRUE;
            reelletter = TRUE;
        }
        if(Keys_Unicode['0'])
        {
            retletter[27] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_KP1])
        {
            retletter[28] = TRUE;
            reelletter = TRUE;
        }
        if(Keys_Unicode['1'])
        {
            retletter[28] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_KP2])
        {
            retletter[29] = TRUE;
            reelletter = TRUE;
        }
        if(Keys_Unicode['2'])
        {
            retletter[29] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_KP3])
        {
            retletter[30] = TRUE;
            reelletter = TRUE;
        }
        if(Keys_Unicode['3'])
        {
            retletter[30] = TRUE;
            reelletter = TRUE;
        }     
        if(Keys[SDLK_KP4])
        {
            retletter[31] = TRUE;
            reelletter = TRUE;
        }
        if(Keys_Unicode['4'])
        {
            retletter[31] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_KP5])
        {
            retletter[32] = TRUE;
            reelletter = TRUE;
        }
        if(Keys_Unicode['5'])
        {
            retletter[32] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_KP6])
        {
            retletter[33] = TRUE;
            reelletter = TRUE;
        }
        if(Keys_Unicode['6'])
        {
            retletter[33] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_KP7])
        {
            retletter[34] = TRUE;
            reelletter = TRUE;
        }
        if(Keys_Unicode['7'])
        {
            retletter[34] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_KP8])
        {
            retletter[35] = TRUE;
            reelletter = TRUE;
        }
        if(Keys_Unicode['8'])
        {
            retletter[35] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_KP9])
        {
            retletter[36] = TRUE;
            reelletter = TRUE;
        }
        if(Keys_Unicode['9'])
        {
            retletter[36] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_BACKSPACE])
        {
            retletter[37] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_SPACE])
        {
            retletter[38] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_RETURN])
        {
            retletter[39] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_KP_ENTER])
        {
            retletter[39] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_PERIOD])
        {
            retletter[40] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_COMMA])
        {
            retletter[67] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_SLASH])
        {
            retletter[68] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_MINUS])
        {
            retletter[69] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_EQUALS])
        {
            retletter[70] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_ESCAPE])
        {
            retletter[71] = TRUE;
            reelletter = TRUE;
        }
    }

    if(reelletter)
    {
        Actualize_Input();
    }
    // -------------------------------------------

    // ------------------------------------------
    //&& po_ctrl == 1 
    if(!Keys[SDLK_MENU] && !Get_LCtrl() && !Get_LShift() && snamesel == 0)
    {
        if(Keys[SDLK_z]) retnote = 0;
        if(Keys[SDLK_s]) retnote = 1;
        if(Keys[SDLK_x]) retnote = 2;
        if(Keys[SDLK_d]) retnote = 3;
        if(Keys[SDLK_c]) retnote = 4;
        if(Keys[SDLK_v]) retnote = 5;
        if(Keys[SDLK_g]) retnote = 6;
        if(Keys[SDLK_b]) retnote = 7;
        if(Keys[SDLK_h]) retnote = 8;
        if(Keys[SDLK_n]) retnote = 9;
        if(Keys[SDLK_j]) retnote = 10;
        if(Keys[SDLK_m]) retnote = 11;
        if(Keys[SDLK_COMMA]) retnote = 12;
        if(Keys[SDLK_l]) retnote = 13;
        if(Keys[SDLK_PERIOD]) retnote = 14;
        if(Keys[SDLK_SEMICOLON]) retnote = 15;
        if(Keys[SDLK_SLASH]) retnote = 16;
        if(Keys[SDLK_q]) retnote = 12;
        if(Keys[SDLK_z]) retnote = 13;
        if(Keys[SDLK_w]) retnote = 14;
        if(Keys[SDLK_3]) retnote = 15;
        if(Keys[SDLK_e]) retnote = 16;
        if(Keys[SDLK_r]) retnote = 17;
        if(Keys[SDLK_s]) retnote = 18;
        if(Keys[SDLK_t]) retnote = 19;
        if(Keys[SDLK_6]) retnote = 20;
        if(Keys[SDLK_y]) retnote = 21;
        if(Keys[SDLK_7]) retnote = 22;
        if(Keys[SDLK_u]) retnote = 23;
        if(Keys[SDLK_i]) retnote = 24;
        if(Keys[SDLK_9]) retnote = 25;
        if(Keys[SDLK_o]) retnote = 26;
        if(Keys[SDLK_0]) retnote = 27;
        if(Keys[SDLK_p]) retnote = 28;
        if(Keys[SDLK_DELETE])
        {
            retnote = -1;
            retnote_raw = -1;
        }
        if(Keys[SDLK_INSERT])
        {
            retnote = -2;
            retnote_raw = -2;
        }
        if(Keys[SDLK_RSHIFT])
        {
            retnote = -3;
            retnote_raw = -3;
        }

        // Key jazz
        if(Keys_Raw[0x10])
        {
            retnote_raw = 12;       // Do
            Record_Keys[0] = 12 + 1;
        }
        if(Keys_Raw[3])
        {
            retnote_raw = 13;       // Do#
            Record_Keys[1] = 13 + 1;
        }
        if(Keys_Raw[0x11])
        {
            retnote_raw = 14;       // Re
            Record_Keys[2] = 14 + 1;
        }
        if(Keys_Raw[4])
        {
            retnote_raw = 15;       // Re#
            Record_Keys[3] = 15 + 1;
        }
        if(Keys_Raw[0x12])
        {
            retnote_raw = 16;       // Mi
            Record_Keys[4] = 16 + 1;
        }
        if(Keys_Raw[0x13])
        {
            retnote_raw = 17;       // Fa
            Record_Keys[5] = 17 + 1;
        }
        if(Keys_Raw[6])
        {
            retnote_raw = 18;       // Fa#
            Record_Keys[6] = 18 + 1;
        }
        if(Keys_Raw[0x14])
        {
            retnote_raw = 19;       // Sol
            Record_Keys[7] = 19 + 1;
        }
        if(Keys_Raw[7])
        {
            retnote_raw = 20;       // Sol#
            Record_Keys[8] = 20 + 1;
        }
        if(Keys_Raw[0x15])
        {
            retnote_raw = 21;       // La
            Record_Keys[9] = 21 + 1;
        }     
        if(Keys_Raw[8])
        {
            retnote_raw = 22;       // La#
            Record_Keys[10] = 22 + 1;
        }
        if(Keys_Raw[0x16])
        {
            retnote_raw = 23;       // Si
            Record_Keys[11] = 23 + 1;
        }
        if(Keys_Raw[0x17])
        {
            retnote_raw = 24;       // Do
            Record_Keys[12] = 24 + 1;
        }
        if(Keys_Raw[0xa])
        {
            retnote_raw = 25;       // Do#
            Record_Keys[13] = 25 + 1;
        }
        if(Keys_Raw[0x18])
        {
            retnote_raw = 26;       // Re
            Record_Keys[14] = 26 + 1;
        }
        if(Keys_Raw[0xb])
        {
            retnote_raw = 27;       // Re#
            Record_Keys[15] = 27 + 1;
        }
        if(Keys_Raw[0x19])
        {
            retnote_raw = 28;       // Mi
            Record_Keys[16] = 28 + 1;
        }
        if(Keys_Raw[0x1a])
        {
            retnote_raw = 29;       // Fa
            Record_Keys[17] = 29 + 1;
        }
        if(Keys_Raw[0xd])
        {
            retnote_raw = 30;       // Fa#
            Record_Keys[18] = 30 + 1;
        }
        if(Keys_Raw[0x1b])
        {
            retnote_raw = 31;       // Sol
            Record_Keys[19] = 31 + 1;
        }

        if(Keys_Raw[0x2c])
        {
            retnote_raw = 0;        // Do
            Record_Keys[20] = 0 + 1;
        }
        if(Keys_Raw[0x1f])
        {
            retnote_raw = 1;        // Do#
            Record_Keys[21] = 1 + 1;
        }
        if(Keys_Raw[0x2d])
        {
            retnote_raw = 2;        // Re
            Record_Keys[22] = 2 + 1;
        }
        if(Keys_Raw[0x20])
        {
            retnote_raw = 3;        // Re#
            Record_Keys[23] = 3 + 1;
        }
        if(Keys_Raw[0x2e])
        {
            retnote_raw = 4;        // Mi
            Record_Keys[24] = 4 + 1;
        }
        if(Keys_Raw[0x2f])
        {
            retnote_raw = 5;        // Fa
            Record_Keys[25] = 5 + 1;
        }
        if(Keys_Raw[0x22])
        {
            retnote_raw = 6;        // Fa#
            Record_Keys[26] = 6 + 1;
        }
        if(Keys_Raw[0x30])
        {
            retnote_raw = 7;        // Sol
            Record_Keys[27] = 7 + 1;
        }
        if(Keys_Raw[0x23])
        {
            retnote_raw = 8;        // Sol#
            Record_Keys[28] = 8 + 1;
        }
        if(Keys_Raw[0x31])
        {
            retnote_raw = 9;        // La
            Record_Keys[29] = 9 + 1;
        }
        if(Keys_Raw[0x24])
        {
            retnote_raw = 10;       // La#
            Record_Keys[30] = 10 + 1;
        }
        if(Keys_Raw[0x32])
        {
            retnote_raw = 11;       // Si
            Record_Keys[31] = 11 + 1;
        }
        if(Keys_Raw[0x33])
        {
            retnote_raw = 12;       // Do
            Record_Keys[32] = 12 + 1;
        }
        if(Keys_Raw[0x26])
        {
            retnote_raw = 13;       // Do#
            Record_Keys[33] = 13 + 1;
        }
        if(Keys_Raw[0x34])
        {
            retnote_raw = 14;       // Re
            Record_Keys[34] = 14 + 1;
        }
        if(Keys_Raw[0x27])
        {
            retnote_raw = 15;       // Re#
            Record_Keys[35] = 15 + 1;
        }
        if(Keys_Raw[0x35])
        {
            retnote_raw = 16;       // Mi
            Record_Keys[36] = 16 + 1;
        }

        // Key jazz release
        if(Keys_Raw_Off[0x10]) { Record_Keys[0] = (12 + 1) | 0x80;  Keys_Raw_Off[0x10] = FALSE; }
        if(Keys_Raw_Off[3])    { Record_Keys[1] = (13 + 1) | 0x80;  Keys_Raw_Off[3] = FALSE; }
        if(Keys_Raw_Off[0x11]) { Record_Keys[2] = (14 + 1) | 0x80;  Keys_Raw_Off[0x11] = FALSE; }
        if(Keys_Raw_Off[4])    { Record_Keys[3] = (15 + 1) | 0x80;  Keys_Raw_Off[4] = FALSE; }
        if(Keys_Raw_Off[0x12]) { Record_Keys[4] = (16 + 1) | 0x80;  Keys_Raw_Off[0x12] = FALSE; }
        if(Keys_Raw_Off[0x13]) { Record_Keys[5] = (17 + 1) | 0x80;  Keys_Raw_Off[0x13] = FALSE; }
        if(Keys_Raw_Off[6])    { Record_Keys[6] = (18 + 1) | 0x80;  Keys_Raw_Off[6] = FALSE; }
        if(Keys_Raw_Off[0x14]) { Record_Keys[7] = (19 + 1) | 0x80;  Keys_Raw_Off[0x14] = FALSE; }
        if(Keys_Raw_Off[7])    { Record_Keys[8] = (20 + 1) | 0x80;  Keys_Raw_Off[7] = FALSE; }
        if(Keys_Raw_Off[0x15]) { Record_Keys[9] = (21 + 1) | 0x80;  Keys_Raw_Off[0x15] = FALSE; }
        if(Keys_Raw_Off[8])    { Record_Keys[10] = (22 + 1) | 0x80; Keys_Raw_Off[8] = FALSE; }
        if(Keys_Raw_Off[0x16]) { Record_Keys[11] = (23 + 1) | 0x80; Keys_Raw_Off[0x16] = FALSE; }
        if(Keys_Raw_Off[0x17]) { Record_Keys[12] = (24 + 1) | 0x80; Keys_Raw_Off[0x17] = FALSE; }
        if(Keys_Raw_Off[0xa])  { Record_Keys[13] = (25 + 1) | 0x80; Keys_Raw_Off[0xa] = FALSE; }
        if(Keys_Raw_Off[0x18]) { Record_Keys[14] = (26 + 1) | 0x80; Keys_Raw_Off[0x18] = FALSE; }
        if(Keys_Raw_Off[0xb])  { Record_Keys[15] = (27 + 1) | 0x80; Keys_Raw_Off[0xb] = FALSE; }
        if(Keys_Raw_Off[0x19]) { Record_Keys[16] = (28 + 1) | 0x80; Keys_Raw_Off[0x19] = FALSE; }
        if(Keys_Raw_Off[0x1a]) { Record_Keys[17] = (29 + 1) | 0x80; Keys_Raw_Off[0x1a] = FALSE; }
        if(Keys_Raw_Off[0xd])  { Record_Keys[18] = (30 + 1) | 0x80; Keys_Raw_Off[0xd] = FALSE; }
        if(Keys_Raw_Off[0x1b]) { Record_Keys[19] = (31 + 1) | 0x80; Keys_Raw_Off[0x1b] = FALSE; }
        if(Keys_Raw_Off[0x2c]) { Record_Keys[20] = (0 + 1) | 0x80;  Keys_Raw_Off[0x2c] = FALSE; }
        if(Keys_Raw_Off[0x1f]) { Record_Keys[21] = (1 + 1) | 0x80;  Keys_Raw_Off[0x1f] = FALSE; }
        if(Keys_Raw_Off[0x2d]) { Record_Keys[22] = (2 + 1) | 0x80;  Keys_Raw_Off[0x2d] = FALSE; }
        if(Keys_Raw_Off[0x20]) { Record_Keys[23] = (3 + 1) | 0x80;  Keys_Raw_Off[0x20] = FALSE; }
        if(Keys_Raw_Off[0x2e]) { Record_Keys[24] = (4 + 1) | 0x80;  Keys_Raw_Off[0x2e] = FALSE; }
        if(Keys_Raw_Off[0x2f]) { Record_Keys[25] = (5 + 1) | 0x80;  Keys_Raw_Off[0x2f] = FALSE; }
        if(Keys_Raw_Off[0x22]) { Record_Keys[26] = (6 + 1) | 0x80;  Keys_Raw_Off[0x22] = FALSE; }
        if(Keys_Raw_Off[0x30]) { Record_Keys[27] = (7 + 1) | 0x80;  Keys_Raw_Off[0x30] = FALSE; }
        if(Keys_Raw_Off[0x23]) { Record_Keys[28] = (8 + 1) | 0x80;  Keys_Raw_Off[0x23] = FALSE; }
        if(Keys_Raw_Off[0x31]) { Record_Keys[29] = (9 + 1) | 0x80;  Keys_Raw_Off[0x31] = FALSE; }
        if(Keys_Raw_Off[0x24]) { Record_Keys[30] = (10 + 1) | 0x80; Keys_Raw_Off[0x24] = FALSE; }
        if(Keys_Raw_Off[0x32]) { Record_Keys[31] = (11 + 1) | 0x80; Keys_Raw_Off[0x32] = FALSE; }
        if(Keys_Raw_Off[0x33]) { Record_Keys[32] = (12 + 1) | 0x80; Keys_Raw_Off[0x33] = FALSE; }
        if(Keys_Raw_Off[0x26]) { Record_Keys[33] = (13 + 1) | 0x80; Keys_Raw_Off[0x26] = FALSE; }
        if(Keys_Raw_Off[0x34]) { Record_Keys[34] = (14 + 1) | 0x80; Keys_Raw_Off[0x34] = FALSE; }
        if(Keys_Raw_Off[0x27]) { Record_Keys[35] = (15 + 1) | 0x80; Keys_Raw_Off[0x27] = FALSE; }
        if(Keys_Raw_Off[0x35]) { Record_Keys[36] = (16 + 1) | 0x80; Keys_Raw_Off[0x35] = FALSE; }

        // Data columns
        if(Keys_Unicode['0']) retvalue = 0;
        if(Keys[SDLK_KP0]) retvalue = 0;
        if(Keys_Unicode['1']) retvalue = 1;
        if(Keys[SDLK_KP1]) retvalue = 1;
        if(Keys_Unicode['2']) retvalue = 2;
        if(Keys[SDLK_KP2]) retvalue = 2;
        if(Keys_Unicode['3']) retvalue = 3;
        if(Keys[SDLK_KP3]) retvalue = 3;
        if(Keys_Unicode['4']) retvalue = 4;
        if(Keys[SDLK_KP4]) retvalue = 4;
        if(Keys_Unicode['5']) retvalue = 5;
        if(Keys[SDLK_KP5]) retvalue = 5;
        if(Keys_Unicode['6']) retvalue = 6;
        if(Keys[SDLK_KP6]) retvalue = 6;
        if(Keys_Unicode['7']) retvalue = 7;
        if(Keys[SDLK_KP7]) retvalue = 7;
        if(Keys_Unicode['8']) retvalue = 8;
        if(Keys[SDLK_KP8]) retvalue = 8;
        if(Keys_Unicode['9']) retvalue = 9;
        if(Keys[SDLK_KP9]) retvalue = 9;
        if(Keys[SDLK_a]) retvalue = 10;
        if(Keys[SDLK_b]) retvalue = 11;
        if(Keys[SDLK_c]) retvalue = 12;
        if(Keys[SDLK_d]) retvalue = 13;
        if(Keys[SDLK_e]) retvalue = 14;
        if(Keys[SDLK_f]) retvalue = 15;
        if(Keys[SDLK_DELETE]) retvalue = 16;

        double int_part;
        double frac_part;
        int iTicks;
        int tmp_note;
        int i;

        if(is_recording)
        {
            // Start the real recording
            if(!is_recording_2 && key_on == 1 && key_record_first_time)
            {
                // Start recording
                is_recording_2 = 1;
                is_record_key = FALSE;
                key_record_first_time = FALSE;
                Set_Frames_Counter();
                old_key_ped_line = ped_line;
                for(i = 0; i < MAX_TRACKS; i++)
                {
                    Alloc_midi_Channels[Track_Number] = 0;
                }
            }
        }
        else
        {
            if(!is_recording_2 && key_on == 1)
            {
                Track_Number = ped_track;
            }
        }

        for(i = 0; i < 37; i++)
        {
            if(Record_Keys[i])
            {
                //key_on = 2;
                if(!(Record_Keys[i] & 0x80))
                {
                    if(Record_Keys_State[i] == FALSE)
                    {
                        Record_Keys_State[i] = TRUE;
                        if(Alloc_midi_Channels[Track_Number] != 0)
                        {
                            Track_Number = Get_Free_Channel();
                        }
                        tmp_note = (Record_Keys[i] - 1) + ped_patoct * 12;
                        if(tmp_note > 119) tmp_note = 119;

                        if(is_recording_2)
                        {
                            key_ticks = Get_Frames_Delay() * 16.0f;
                            frac_part = modf(key_ticks, &int_part);
                            iTicks = (int) fabs(int_part);
                            ped_line += (int) iTicks;

                            Next_Line_Pattern_Auto();

                            xoffseted = Track_Number * 6 + ped_line * 96 + pSequence[Cur_Position] * 12288;
                            *(RawPatterns + xoffseted) = tmp_note;
                            *(RawPatterns + xoffseted + 1) = ped_patsam;
                        }

                        if(!is_editing || ped_row == 0)
                        {
                            // Set the select here
                            Sp_Playwave(Track_Number, (float) tmp_note, ped_patsam, CustomVol[ped_patsam], 0, 0, TRUE);
                        }
                        Alloc_midi_Channels[Track_Number] = ((tmp_note + 1) << 8);
                    }
                }
                else
                {
                    if(Record_Keys_State[i] == TRUE)
                    {
                        Record_Keys_State[i] = FALSE;

                        tmp_note = ((Record_Keys[i] - 1) & 0x7f) + ped_patoct * 12;
                        if(tmp_note > 119) tmp_note = 119;
                        Record_Keys[i] = 0;

                        Track_Number = Search_Corresponding_Channel((tmp_note + 1) << 8);
                        // Kick it (not a very good solution)
                        if(Track_Number == -1) Track_Number = 0;

                        if(is_recording_2)
                        {
                            key_ticks = Get_Frames_Delay() * 16.0f;// / 1000.0 * 1000.0f; //(((((Get_Frames_Delay() / 1000.0) / 60.0) * (double) BeatsPerMin) * TicksPerBeat)) * 3;
                            frac_part = modf(key_ticks, &int_part);
                            iTicks = (int) fabs(int_part);
                            if(iTicks == 0) iTicks = 1;
                            ped_line += (int) iTicks;

                            Next_Line_Pattern_Auto();

                            xoffseted = Track_Number * 6 + ped_line * 96 + pSequence[Cur_Position] * 12288;

                            *(RawPatterns + xoffseted) = 120;
                            *(RawPatterns + xoffseted + 1) = 0xff;
                        }

                        Synthesizer[Track_Number].NoteOff();
                        noteoff303(Track_Number); // 303 Note Off...
                        if(sp_Stage[Track_Number]) sp_Stage[Track_Number] = PLAYING_SAMPLE_NOTEOFF;

#if !defined(__NO_MIDI__)
                        Midi_NoteOff(Track_Number);
#endif

                        Alloc_midi_Channels[Track_Number] = 0;
                    }
                }
            }
        }
        if(is_recording_2)
        {
            if(old_key_ped_line != ped_line)
            {
                Actualize_Sequencer();
                Actupated(0);
            }
            old_key_ped_line = ped_line;
        }
    }

    // Turn edit mode on/off
    if(Keys[SDLK_SPACE] && snamesel == 0 && pos_space == 1)
    {
        if(Get_LShift())
        {
            gui_action = GUI_CMD_RECORD_MODE;
        }
        else
        {
            is_recording = 0;
            is_editing ^= 1;
            gui_action = GUI_CMD_EDIT_MODE;
        }
    }

    if(pos_space == 0 && !Keys[SDLK_SPACE]) pos_space = 1;

    if(!is_recording)
    {
        // Play song
        if(Keys[SDLK_RCTRL] && snamesel == 0 && po_ctrl2)
        {
            plx = 0;
            po_ctrl2 = FALSE;
            gui_action = GUI_CMD_PLAY_SONG;
        }
        if(!Keys[SDLK_RCTRL] && !po_ctrl2) po_ctrl2 = TRUE;

        if(Keys[SDLK_RALT] && snamesel == 0 && po_alt2)
        {
            plx = 1;
            po_alt2 = FALSE;
            gui_action = GUI_CMD_PLAY_SONG;
        }
        if(!Keys[SDLK_RALT] && !po_alt2) po_alt2 = TRUE;

        // Play song
        if(Get_LAlt() && snamesel == 0 && !po_alt)
        {
            po_alt = TRUE;
        }
        if(!Get_LAlt() && po_alt) po_alt = FALSE;


// This is a nasty hack: we should have a specific ASCII buffer
// instead of using the unicode one.
#if !defined(__MACOSX__)
#define UNICODE_OFFSET1 0x20
#define UNICODE_OFFSET2 0x60
#else
#define UNICODE_OFFSET1 0
#define UNICODE_OFFSET2 0
#endif

        if(Key_Unicode)
        {
            if(Get_LShift())
            {
                if(Keys[SDLK_m - UNICODE_OFFSET1])
                {
                    if(TRACKSTATE[ped_track] == 0) TRACKSTATE[ped_track] = 1;
                    else TRACKSTATE[ped_track] = 0;
                    if(userscreen == USER_SCREEN_TRACK_EDIT) Actualize_Track_Ed(10);
                    Actupated(0);
                }
            }
        }

        if(Get_LCtrl())
        {
            if(!Get_LAlt() && !Get_LShift())
            {
                if(Keys[SDLK_LEFT])
                {
                    if(pSequence[Cur_Position] > 0)
                    {
                        gui_action = GUI_CMD_PREV_PATT;
                    }
                    Keys[SDLK_LEFT] = FALSE;
                }
                if(Keys[SDLK_RIGHT])
                {
                    if(pSequence[Cur_Position] < 254)
                    {
                        gui_action = GUI_CMD_NEXT_PATT;
                    }
                    Keys[SDLK_RIGHT] = FALSE;
                }
            }

            if(Key_Unicode)
            {
                if(Get_LShift())
                {
                    if(Keys[SDLK_m - UNICODE_OFFSET2])
                    {
                        Solo_Track(ped_track);
                        // Will unmute the correct track
                        if(TRACKSTATE[ped_track] == 0) TRACKSTATE[ped_track] = 1;
                        else TRACKSTATE[ped_track] = 0;
                        if(userscreen == USER_SCREEN_TRACK_EDIT) Actualize_Track_Ed(10);
                        Actupated(0);
                    }
                }

                if(Keys[SDLK_s - UNICODE_OFFSET2])
                {
                    gui_action = GUI_CMD_SAVE_MODULE;
                }

                // Ctrl+B
                if(Keys[SDLK_b - UNICODE_OFFSET2] && !Songplaying)
                {
                    Mark_Block_Start(ped_row, ped_track, ped_line);
                }

                // Ctrl+E
                if(Keys[SDLK_e - UNICODE_OFFSET2] && !Songplaying)
                {
                    Mark_Block_End(ped_row, ped_track, ped_line, 3);
                }

                if(Keys[SDLK_c - UNICODE_OFFSET2] && block_start_track != -1 && block_end_track != -1)
                {
                    Copy_Selection(Cur_Position);
                }

                if(Keys[SDLK_x - UNICODE_OFFSET2] && block_start_track != -1 && block_end_track != -1)
                {
                    Cut_Selection(Cur_Position);
                }

                if(Keys[SDLK_i - UNICODE_OFFSET2] && is_editing)
                {
                    Interpolate_Block(Cur_Position);
                }

                if(Keys[SDLK_r - UNICODE_OFFSET2] && is_editing)
                {
                    Randomize_Block(Cur_Position);
                }

                if(Keys[SDLK_v - UNICODE_OFFSET2] && block_start_track_nibble != -1 && block_end_track_nibble != -1 && is_editing)
                {
                    Paste_Block(Cur_Position);
                }

                if(Keys[SDLK_u - UNICODE_OFFSET2] && is_editing)
                {
                    if(Get_LShift()) Instrument_Seminote_Up_Block(Cur_Position);
                    else Seminote_Up_Block(Cur_Position);
                }

                if(Keys[SDLK_d - UNICODE_OFFSET2] && is_editing)
                {
                    if(Get_LShift()) Instrument_Seminote_Down_Block(Cur_Position);
                    else Seminote_Down_Block(Cur_Position);
                }

                if(Keys[SDLK_a - UNICODE_OFFSET2])
                {
                    if(Get_LShift()) Select_Pattern_Block();
                    else Select_Track_Block();
                }
            }
        }

        if(Get_LAlt())
        {
            if(Get_LCtrl())
            {

                // Next pos
                if(Keys[SDLK_LEFT])
                {
                    gui_action = GUI_CMD_PREVIOUS_POSITION;
                    Keys[SDLK_LEFT] = FALSE;
                }

                // Previous pos
                if(Keys[SDLK_RIGHT])
                {
                    gui_action = GUI_CMD_NEXT_POSITION;
                    Keys[SDLK_RIGHT] = FALSE;
                }
            }
            else
            {
                if(Keys[SDLK_LEFT])
                {
                    gui_action = GUI_CMD_PREV_INSTR;
                    Keys[SDLK_LEFT] = FALSE;
                }
                if(Keys[SDLK_RIGHT])
                {
                    gui_action = GUI_CMD_NEXT_INSTR;
                    Keys[SDLK_RIGHT] = FALSE;
                }
            }
        }

        if(Get_LShift())
        {
            poskeynote = 1;
            poskeyval = 1;
        }
        if(poskeyval == 0 && retvalue != ltretvalue) poskeyval = 1;

        if((ped_row == 1 || ped_row == 3 ||
            ped_row == 5 || ped_row == 7 || ped_row == 9) &&
            retvalue != 120 && poskeyval == 1  && is_editing == 1)
        {
            int ped_cell = 1;
            if(ped_row == 3) ped_cell = 2;
            if(ped_row == 5) ped_cell = 3;
            if(ped_row == 7) ped_cell = 4;
            if(ped_row == 9) ped_cell = 5;
            ltretvalue = retvalue;
            xoffseted = (ped_track * 6) + (ped_line * 96) + ped_cell;

            int oldval = *(RawPatterns + pSequence[Cur_Position] * 12288 + xoffseted);

            if(retvalue < 16)
            {
                if(oldval == 255 && ped_row == 1) oldval = 0;
                if(oldval == 255 && ped_row == 3) oldval = 0;
                if(oldval == 255 && ped_row == 5) oldval = 0;
                oldval = (oldval & 0xf) + retvalue * 16;
                *(RawPatterns + pSequence[Cur_Position] * 12288 + xoffseted) = oldval;
                if(oldval != 255 && ped_row == 5 && *(RawPatterns + pSequence[Cur_Position] * 12288 + xoffseted) > 144)
                {
                    *(RawPatterns + pSequence[Cur_Position] * 12288 + xoffseted) = 144;
                }     

                if(!is_recording) if(!Songplaying)
                {
                    ped_line += ped_pattad;
                    if(!ped_pattad)
                    {
                        ped_row++;
                        Actupated(0);
                        gui_action = GUI_CMD_SET_FOCUS_TRACK;
                    }
                }
                poskeyval = 0;
            }
            else
            {
                if(!Delete_Selection(Cur_Position))
                {
                    oldval = 0;
                    if(ped_row == 1) oldval = 255;
                    if(ped_row == 3) oldval = 255;
                    if(ped_row == 5) oldval = 255;
                    *(RawPatterns + pSequence[Cur_Position] * 12288 + xoffseted) = oldval;
                    if(oldval != 255 && ped_row == 5 &&
                       *(RawPatterns + pSequence[Cur_Position] * 12288 + xoffseted) > 144)
                    {
                        *(RawPatterns + pSequence[Cur_Position] * 12288 + xoffseted) = 144;
                    }

                    if(!is_recording) if(!Songplaying)
                    {
                        ped_line += ped_pattad;
                        if(!ped_pattad)
                        {
                            ped_row++;
                            Actupated(0);
                            gui_action = GUI_CMD_SET_FOCUS_TRACK;
                        }
                    }
                    poskeyval = 0;
                }
            }
            Actupated(0);
        }

        if((ped_row == 2 || ped_row == 4 || ped_row == 6 || ped_row == 8 || ped_row == 10)
            && retvalue != 120 && is_editing == 1 && poskeyval == 1)
        {

            int ped_cell = 1;
            if(ped_row == 2) ped_cell = 1;
            if(ped_row == 4) ped_cell = 2;
            if(ped_row == 6) ped_cell = 3;
            if(ped_row == 8) ped_cell = 4;
            if(ped_row == 10) ped_cell = 5;

            ltretvalue = retvalue;
            xoffseted = (ped_track * 6) + (ped_line * 96) + ped_cell;
            int oldval = *(RawPatterns + pSequence[Cur_Position] * 12288 + xoffseted);

            if(retvalue < 16)
            {
                if(oldval == 255 && ped_row == 2) oldval = 0;
                if(oldval == 255 && ped_row == 4) oldval = 0;
                if(oldval == 255 && ped_row == 6) oldval = 0;
                oldval = (oldval & 0xf0) + retvalue;
                *(RawPatterns + pSequence[Cur_Position] * 12288 + xoffseted) = oldval;
                if(oldval != 255 && ped_row == 6 &&
                   *(RawPatterns + pSequence[Cur_Position] * 12288 + xoffseted) > 144)
                {
                    *(RawPatterns + pSequence[Cur_Position] * 12288 + xoffseted) = 144;
                }
    
                if(!is_recording) if(!Songplaying)
                {
                    ped_line += ped_pattad;
                    if(!ped_pattad)
                    {
                        ped_row++;
                        Actupated(0);
                        gui_action = GUI_CMD_SET_FOCUS_TRACK;
                    }
                }
                poskeyval = 0;
            }
            else
            {
                if(!Delete_Selection(Cur_Position))
                {
                    oldval = 0;
                    if(ped_row == 2) oldval = 255;
                    if(ped_row == 4) oldval = 255;
                    if(ped_row == 6) oldval = 255;
                    *(RawPatterns + pSequence[Cur_Position] * 12288 + xoffseted) = oldval;
                    if(oldval != 255 && ped_row == 6 &&
                       *(RawPatterns + pSequence[Cur_Position] * 12288 + xoffseted) > 144)
                    {
                        *(RawPatterns + pSequence[Cur_Position] * 12288 + xoffseted) = 144;
                    }
                    if(!is_recording) if(!Songplaying)
                    {
                        ped_line += ped_pattad;
                        if(!ped_pattad)
                        {
                            ped_row++;
                            Actupated(0);
                            gui_action = GUI_CMD_SET_FOCUS_TRACK;
                        }
                    }
                    poskeyval = 0;
                }
            }
            Actupated(0);
        }

        if(poskeynote == 0 && retnote_raw != ltretnote_raw) poskeynote = 1;

        if(ped_row == 0 && poskeynote == 1 && (retnote_raw != 120))
        {
            ltretnote_raw = retnote_raw;

            xoffseted = ped_track * 6 + ped_line * 96 + pSequence[Cur_Position] * 12288;

            int tmp_note = retnote_raw + ped_patoct * 12;
            if(tmp_note > 119) tmp_note = 119;

            if(retnote > -1)
            {
                if(is_editing)
                {
                    *(RawPatterns + xoffseted) = tmp_note;       // 121
                    *(RawPatterns + xoffseted + 1) = ped_patsam; // 121
                    if(!is_recording) if(!Songplaying)
                    {
                        ped_line += ped_pattad;
                        if(!ped_pattad)
                        {
                            ped_row++;
                            Actupated(0);
                            gui_action = GUI_CMD_SET_FOCUS_TRACK;
                        }
                    }
                    Actupated(0);
                }
            }

            // Note off (RSHIFT)
            if(retnote == -3 && is_editing)
            {
                *(RawPatterns + xoffseted) = 120;         // 121
                *(RawPatterns + xoffseted + 1) = 0xff;    // no instrument
                if(!is_recording) if(!Songplaying)
                {
                    ped_line += ped_pattad;
                    if(!ped_pattad)
                    {
                        ped_row++;
                        Actupated(0);
                        gui_action = GUI_CMD_SET_FOCUS_TRACK;
                    }
                }
                Actupated(0);
            }

            if(retnote == -1 && is_editing)
            {
                if(!Delete_Selection(Cur_Position))
                {
                    // Delete the note under the caret
                    *(RawPatterns + xoffseted) = 121;
                    *(RawPatterns + xoffseted + 1) = 255;
                    if(!is_recording) if(!Songplaying)
                    {
                        ped_line += ped_pattad;
                        if(!ped_pattad)
                        {
                            ped_row++;
                            Actupated(0);
                            gui_action = GUI_CMD_SET_FOCUS_TRACK;
                        }
                    }
                }
                Actupated(0);
            }

            poskeynote = 0;
        }
    }
}

// ------------------------------------------------------
// Retrieve mouse datas
char zcheckMouse_nobutton(int x, int y, int xs, int ys)
{
    if(Mouse.x > x && Mouse.x < x + xs && Mouse.y > y && Mouse.y < y + ys + 1)
    {
        return(1);
    }
    return(0);
}

char zcheckMouse(int x, int y, int xs, int ys)
{
    if(!gui_pushed && Mouse.x > x && Mouse.x < x + xs && Mouse.y > y && Mouse.y < y + ys + 1)
    {
        gui_lx = x;
        gui_ly = y;
        gui_lxs = xs;
        gui_lys = ys;
        gui_pushed = Mouse.button_oneshot;
        fluzy = -1;
        return(1);
    }
    if(gui_pushed && x == gui_lx && y == gui_ly && xs == gui_lxs && ys == gui_lys)
    {
        return(1);
    }
    return(0);
}

// ------------------------------------------------------
// Mouse Handler
void Mouse_Handler(void)
{
    int Cur_Position;

    if(Songplaying) Cur_Position = cPosition_delay;
    else Cur_Position = cPosition;

    if(gui_action == GUI_CMD_NOP) gui_action = GUI_CMD_NONE;

    teac = 0;

    // mouse wheel up
    if(Mouse.wheel == 1)
    {
        if(!Songplaying && !is_recording)
        {
            // Scroll the patterns
            if(zcheckMouse_nobutton(0, 182, CONSOLE_WIDTH, 246) == 1)
            {
                ped_line -= MouseWheel_Multiplier;
                if(Continuous_Scroll && !Cur_Position) if(ped_line < 0) ped_line = 0;
                Actupated(0);
            }
        }

        // Scroll the files list
        if(Scopish != SCOPE_ZONE_SCOPE)
        {
            if(zcheckMouse(410, 41, 390, 136) == 1)
            {
                lt_index--;
                gui_action = GUI_CMD_FILELIST_SCROLL;
            }

            // Scroll the files list
            if(zcheckMouse(394, 58, 16, 103) == 1)
            {
                lt_index--;
                gui_action = GUI_CMD_FILELIST_SCROLL;
            }
        }

        // Scroll the sequences
        if(userscreen == USER_SCREEN_SEQUENCER)
        {
            if(zcheckMouse(257, 466, 26, 90) ||
               zcheckMouse(89, 466, 26, 90) ||
               zcheckMouse(120, 466, 132, 90))
            {
                gui_action = GUI_CMD_PREVIOUS_POSITION;
            }
        }

        Mouse_Wheel_303_Ed(MouseWheel_Multiplier);
    }

    // mouse wheel down
    if(Mouse.wheel == -1)
    {
        if(!Songplaying && !is_recording)
        {
            // Scroll the patterns
            if(zcheckMouse_nobutton(0, 182, CONSOLE_WIDTH, 246) == 1)
            {
                ped_line += MouseWheel_Multiplier;
                if(Continuous_Scroll && (Cur_Position == sLength - 1))
                {
                    if(ped_line >= patternLines[pSequence[Cur_Position]])
                    {
                        ped_line = patternLines[pSequence[Cur_Position]] - 1;
                    }
                }
                Actupated(0);
            }
        }

        if(Scopish != SCOPE_ZONE_SCOPE)
        {
            // Scroll the files list
            if(zcheckMouse(410, 41, 390, 136) == 1)
            {
                lt_index++;
                gui_action = GUI_CMD_FILELIST_SCROLL;
            }

            // Scroll the files list
            if(zcheckMouse(394, 58, 16, 103) == 1)
            {
                lt_index++;
                gui_action = GUI_CMD_FILELIST_SCROLL;
            }
        }

        // Scroll the sequences
        if(userscreen == USER_SCREEN_SEQUENCER)
        {
                if(zcheckMouse(257, 466, 26, 90) ||
                   zcheckMouse(89, 466, 26, 90) ||
                   zcheckMouse(120, 466, 132, 90))
                {
                    gui_action = GUI_CMD_NEXT_POSITION;
                }
        }

        Mouse_Wheel_303_Ed(-MouseWheel_Multiplier);
    }

    if(Mouse.button == MOUSE_LEFT_BUTTON)
    {

        if(Scopish != SCOPE_ZONE_SCOPE)
        {
            if(zcheckMouse(395, 59, 16, 103)) gui_action = GUI_CMD_SET_FILES_LIST_SLIDER;
        }

        Mouse_Left_Repeat_Instrument_Ed();

        Mouse_Sliders_303_Ed();

        Mouse_Sliders_Sample_Ed();

        Mouse_Sliders_Instrument_Ed();

        Mouse_Sliders_Master_Ed();

        Mouse_Sliders_Synth_Ed();

        Mouse_Sliders_Track_Fx_Ed();

        Mouse_Sliders_Fx_Ed();

        Mouse_Sliders_Track_Ed();

        Mouse_Sliders_Master_Shuffle();

        // Current track slider
        if(zcheckMouse(726, 429, 72, 16))
        {
            float Pos_Mouse = (Mouse.x - 726);
            if(Pos_Mouse < 0) Pos_Mouse = 0;
            int disp = 6;
            if(Songtracks < disp) disp = Songtracks;
            disp = (Songtracks - disp);
            // Normalize and scale
            Pos_Mouse = (Pos_Mouse / 72.0f) * (disp + 1);
            if(Pos_Mouse > disp) Pos_Mouse = disp;
            gui_track = (int) Pos_Mouse;
            Set_Track_Slider(gui_track);
            Actupated(1);
        }

        // Position the caret on the specified track by the mouse
        if(zcheckMouse(1, 194, CONSOLE_WIDTH, 234))
        {
            ped_track = Get_Track_Over_Mouse();
            ped_row = Get_Column_Over_Mouse();
            Actupated(1);
            gui_action = GUI_CMD_SET_FOCUS_TRACK;
        }

    }
    else
    {
        // leftbutton
        sas = FALSE;
    }

    // Check Zones for GUI clicks -----------------------------------

    if(Mouse.button_oneshot == MOUSE_LEFT_BUTTON)
    {

        // Go to the row selected with the mouse
        if(!Songplaying)
        {
            if(zcheckMouse(1, 194, CONSOLE_WIDTH, 234))
            {
                if(!is_recording) ped_line = Get_Line_Over_Mouse();
                Actupated(1);
            }
        }

        if(display_title)
        {
            // Modules dir.
            if(zcheckMouse(746, 24, 18, 16))
            {
                Scopish = SCOPE_ZONE_MOD_DIR;
                Draw_Scope_Files_Button();
            }
            if(zcheckMouse(764, 24, 18, 16))
            {
                Scopish = SCOPE_ZONE_INSTR_DIR;
                Draw_Scope_Files_Button();
            }
            if(zcheckMouse(782, 24, 18, 16))
            {
                Scopish = SCOPE_ZONE_PRESET_DIR;
                Draw_Scope_Files_Button();
            }
            // Tracks/Stereo scopes.
            if(zcheckMouse(728, 24, 18, 16))
            {
                Scopish = SCOPE_ZONE_SCOPE;
                Scopish_LeftRight ^= TRUE;
                Draw_Scope_Files_Button();
            }
        }

        Mouse_Left_303_Ed();

        Mouse_Left_Sample_Ed();

        if(zcheckMouse(90, 134, 166, 16) && snamesel == 0)
        {
            snamesel = 2;
            strcpy(cur_input_name, nameins[ped_patsam]);
            sprintf(nameins[ped_patsam], "");
            namesize = 0;
            gui_action = GUI_CMD_UPDATE_PATTERN_ED;
        }

        if(Scopish != SCOPE_ZONE_SCOPE)
        {
            // Files list up
            if(zcheckMouse(394, 42, 16, 14))
            {
                lt_index--;
                gui_action = GUI_CMD_FILELIST_SCROLL;
            }
            
            // Files list down
            if(zcheckMouse(394, 162, 16, 14))
            {
                lt_index++;
                gui_action = GUI_CMD_FILELIST_SCROLL;
            }

            if(zcheckMouse(412, 43, 226 + restx, 133))
            {
                gui_action = GUI_CMD_SET_FILES_LIST_SELECT_FILE;
            }
        }

        if(zcheckMouse(258, 152, 16, 16)) gui_action = GUI_CMD_LOWER_OCTAVE;
        if(zcheckMouse(302, 152, 16, 16)) gui_action = GUI_CMD_HIGHER_OCTAVE;
        if(zcheckMouse(90, 152, 16, 16)) gui_action = GUI_CMD_DECREASE_STEP_ADD;
        if(zcheckMouse(134, 152, 16, 16)) gui_action = GUI_CMD_INCREASE_STEP_ADD;
        if(zcheckMouse(258, 134, 16, 16)) gui_action = GUI_CMD_PREV_INSTR;
        if(zcheckMouse(302, 134, 16, 16)) gui_action = GUI_CMD_NEXT_INSTR;
        if(zcheckMouse(1, 184, CONSOLE_WIDTH, 10)) gui_action = GUI_CMD_SWITCH_TRACK_MUTE_STATE;

        // --- Player --------------------------------------------

        if(zcheckMouse(8, 44, 80, 16))
        {
            plx = 0;
            gui_action = GUI_CMD_PLAY_SONG;
        }
        if(zcheckMouse(8, 62, 80, 16))
        {
            gui_action = GUI_CMD_STOP_SONG;
        }

        if(zcheckMouse(8, 80, 80, 16))
        {
            sr_isrecording++;
            if(sr_isrecording > 1) sr_isrecording = 0;
            gui_action = GUI_CMD_RECORD_303;
        }

        if(zcheckMouse(8, 98, 80, 16))
        {
            is_recording = 0;
            is_editing ^= 1;
            gui_action = GUI_CMD_EDIT_MODE;
        }

        // --- Sequence -----------------------------------------

        if(zcheckMouse(188, 44, 16, 16))
        {
            gui_action = GUI_CMD_PREVIOUS_POSITION;
        }
        if(zcheckMouse(232, 44, 16, 16))
        {
            gui_action = GUI_CMD_NEXT_POSITION;
        }
        if(zcheckMouse(188, 62, 16, 16) && pSequence[Cur_Position] > 0)
        {
            gui_action = GUI_CMD_PREV_PATT;
        }
        if(zcheckMouse(232, 62, 16, 16) && pSequence[Cur_Position] < 254)
        {
            gui_action = GUI_CMD_NEXT_PATT;
        }
        if(zcheckMouse(188, 80, 16, 16) && sLength > 1)
        {
            gui_action = GUI_CMD_REDUCE_SONG_LENGTH;
        }
        if(zcheckMouse(232, 80, 16, 16) && sLength < 255)
        {
            gui_action = GUI_CMD_INCREASE_SONG_LENGTH;
        }
        // Decrease number of lines for this pattern
        if(zcheckMouse(188, 98, 16, 16) && patternLines[pSequence[Cur_Position]] > 1)
        {
            patternLines[pSequence[Cur_Position]]--;
            if(ped_line >= patternLines[pSequence[Cur_Position]]) ped_line = patternLines[pSequence[Cur_Position]] - 1;
            gui_action = GUI_CMD_SET_PATTERN_LENGTH;
        }
        // Increase number of lines for this pattern
        if(zcheckMouse(232, 98, 16, 16) && patternLines[pSequence[Cur_Position]] < 128)
        {
            patternLines[pSequence[Cur_Position]]++;
            gui_action = GUI_CMD_SET_PATTERN_LENGTH;
        }

        // --- Song Settings --------------------------------------

        // Reduce the number of tracks
        if(zcheckMouse(324, 44, 16, 16) && Songtracks > 1)
        {
            Songtracks--;
            gui_action = GUI_CMD_CHANGE_TRACKS_NBR;
            teac = 4;
        }
        // Increase the number of tracks
        if(zcheckMouse(368, 44, 16, 16) && Songtracks < 16)
        {
            Songtracks++;
            gui_action = GUI_CMD_CHANGE_TRACKS_NBR;
            teac = 4;
        }

        // Reduce the number of BPM
        if(zcheckMouse(324, 62, 16, 16))
        {
            BeatsPerMin--;
            gui_action = GUI_CMD_CHANGE_BPM_TICKS_NBR;
            teac = 1;
        }
    
        // Increase the number of BPM
        if(zcheckMouse(368, 62, 16, 16))
        {
            BeatsPerMin++;
            gui_action = GUI_CMD_CHANGE_BPM_TICKS_NBR;
            teac = 1;
        }

        // Decrease the number of TPB       
        if(zcheckMouse(324, 80, 16, 16))
        {
            TicksPerBeat--;
            gui_action = GUI_CMD_CHANGE_BPM_TICKS_NBR;
            teac = 2;
        }
    
        // Increase the number of TPB
        if(zcheckMouse(368, 80, 16, 16))
        {
            TicksPerBeat++;
            gui_action = GUI_CMD_CHANGE_BPM_TICKS_NBR;
            teac = 2;
        }

        // Delete instrument
        if(zcheckMouse(320, 134, 64, 16))
        {
            gui_action = GUI_CMD_DELETE_INSTRUMENT;
        }

        // Tabs
        if(zcheckMouse(396, 429, 64, 16) && userscreen != USER_SCREEN_TRACK_FX_EDIT) gui_action = GUI_CMD_SELECT_TRACK_FX_EDIT;
        if(zcheckMouse(132, 429, 64, 16) && userscreen != USER_SCREEN_SYNTH_EDIT) gui_action = GUI_CMD_SELECT_SYNTH_EDIT;
        if(zcheckMouse(594, 429, 64, 16) && userscreen != USER_SCREEN_SETUP_EDIT) gui_action = GUI_CMD_SELECT_SCREEN_SETUP_EDIT;
        if(zcheckMouse(0, 429, 64, 16) && userscreen != USER_SCREEN_SEQUENCER) gui_action = GUI_CMD_SELECT_SEQUENCER;
        if(zcheckMouse(528, 429, 64, 16) && userscreen != USER_SCREEN_DISKIO_EDIT) gui_action = GUI_CMD_SELECT_DISKIO;
        if(zcheckMouse(330, 429, 64, 16) && userscreen != USER_SCREEN_TRACK_EDIT) gui_action = GUI_CMD_SELECT_TRACK_EDIT;
        if(zcheckMouse(66, 429, 64, 16) && userscreen != USER_SCREEN_INSTRUMENT_EDIT)
        {
            gui_action = GUI_CMD_SELECT_INSTRUMENT_EDIT;
            seditor = 0;
        }
        if(zcheckMouse(462, 429, 64, 16) && userscreen != USER_SCREEN_FX_SETUP_EDIT)
        {
            gui_action = GUI_CMD_SELECT_FX_EDIT;
            teac = 0;
        }  
        if(zcheckMouse(198, 429, 64, 16) && userscreen != USER_SCREEN_SAMPLE_EDIT) gui_action = GUI_CMD_SELECT_SAMPLE_EDIT;
        if(zcheckMouse(264, 429, 64, 16) && userscreen != USER_SCREEN_TB303_EDIT) gui_action = GUI_CMD_SELECT_TB303_EDIT;
        if(zcheckMouse(660, 429, 64, 16))
        {
            if(Asking_Exit) gui_action = GUI_CMD_EXIT;
            else gui_action = GUI_CMD_ASK_EXIT;
        }

        Mouse_Left_Track_Fx_Ed();

        Mouse_Left_Sequencer_Ed();

        Mouse_Left_DiskIO_Ed();

        Mouse_Left_Track_Ed();

        Mouse_Left_Instrument_Ed();

        Mouse_Left_Fx_Ed();

        Mouse_Left_Synth_Ed();

        Mouse_Left_Master_Ed();
    }

    // Right mouse button
    if(Mouse.button_oneshot == MOUSE_RIGHT_BUTTON)
    {
        gui_action = GUI_CMD_NOP;

        // Start of the marking block
        if(zcheckMouse(1, 194, CONSOLE_WIDTH, 234) && !Songplaying)
        {
            Mark_Block_Start(Get_Column_Over_Mouse(), Get_Track_Over_Mouse(), Get_Line_Over_Mouse());
        }

        if(zcheckMouse(90, 152, 16, 16))
        {
            ped_pattad = 0;
            gui_action = GUI_CMD_UPDATE_PATTERN_ED;
        }
        if(zcheckMouse(134, 152, 16, 16))
        {
            ped_pattad = 16;
            gui_action = GUI_CMD_UPDATE_PATTERN_ED;
        }

        if(zcheckMouse(188, 98, 16, 16))
        {
            int ltp = patternLines[pSequence[Cur_Position]];
            ltp -= 16;
            if(ltp < 1) ltp = 1;
            patternLines[pSequence[Cur_Position]] = ltp;
            if(ped_line >= patternLines[pSequence[Cur_Position]]) ped_line = patternLines[pSequence[Cur_Position]] - 1;
            gui_action = GUI_CMD_SET_PATTERN_LENGTH;
        }

        if(zcheckMouse(232, 98, 16, 16))
        {
            int ltp = patternLines[pSequence[Cur_Position]];
            ltp += 16;
            if(ltp > 128) ltp = 128;
            patternLines[pSequence[Cur_Position]] = ltp;
            gui_action = GUI_CMD_SET_PATTERN_LENGTH;
        }

        if(zcheckMouse(258, 134, 16, 16) == 1)
        {
            gui_action = GUI_CMD_PREVIOUS_16_INSTR;
        }
        if(zcheckMouse(302, 134, 16, 16) == 1)
        {
            gui_action = GUI_CMD_NEXT_16_INSTR;
        }

        if(zcheckMouse(8, 44, 80, 16) == 1)
        {
            plx = 1;
            gui_action = GUI_CMD_PLAY_SONG;
        }

        if(Scopish != SCOPE_ZONE_SCOPE)
        {
            if(zcheckMouse(394, 42, 16, 14)==1)
            {
                lt_index -= 10;
                gui_action = GUI_CMD_FILELIST_SCROLL;
            }
            if(zcheckMouse(394, 162, 16, 14) == 1)
            {
                lt_index += 10;
                gui_action = GUI_CMD_FILELIST_SCROLL;
            }
        }

        // Reduce the number of BPM by 16
        if(zcheckMouse(324, 62, 16, 16))
        {
            BeatsPerMin -= 16;
            gui_action = GUI_CMD_CHANGE_BPM_TICKS_NBR;
            teac = 1;
        }
        // Increase the number of BPM by 16
        if(zcheckMouse(368, 62, 16, 16))
        {
            BeatsPerMin += 16;
            gui_action = GUI_CMD_CHANGE_BPM_TICKS_NBR;
            teac = 1;
        }

        // Decrease the number of TPB       
        if(zcheckMouse(324, 80, 16, 16))
        {
            TicksPerBeat -= 10;
            gui_action = GUI_CMD_CHANGE_BPM_TICKS_NBR;
            teac = 2;
        }
    
        // Increase the number of TPB
        if(zcheckMouse(368, 80, 16, 16))
        {
            TicksPerBeat += 10;
            gui_action = GUI_CMD_CHANGE_BPM_TICKS_NBR;
            teac = 2;
        }

        // Solo track with right mouse button
        if(zcheckMouse(1, 184, CONSOLE_WIDTH, 10) == 1)
        {
            int tmp_track = gui_track + ((Mouse.x - 24) / PAT_COL_MAX);

            if(tmp_track > 15) tmp_track = 15;
            if(tmp_track < 0) tmp_track = 0;

            Solo_Track(tmp_track);
            // Will unmute the correct track
            gui_action = GUI_CMD_SWITCH_TRACK_MUTE_STATE;
        }

        // Songlength + 10
        if(zcheckMouse(188, 80, 16, 16) == 1 && sLength != 1)
        {
            int tLength = sLength;
            tLength -= 10;
            if(tLength < 1) tLength = 1;
            sLength = tLength;
            gui_action = GUI_CMD_UPDATE_SEQUENCER;
        }
        if(zcheckMouse(232, 80, 16, 16) == 1 && sLength != 255)
        {
            int tLength = sLength;
            tLength += 10;
            if(tLength > 255) tLength = 255;
            sLength = tLength;
            gui_action = GUI_CMD_UPDATE_SEQUENCER;
        }

        // -10 positions
        if(zcheckMouse(188, 44, 16, 16) == 1)
        {
            gui_action = GUI_CMD_REDUCE_POSITIONS_10;
        }
        // +10 positions
        if(zcheckMouse(232, 44, 16, 16) == 1)
        {
            gui_action = GUI_CMD_INCREASE_POSITIONS_10;
        }

        // -10 patterns
        if(zcheckMouse(188, 62, 16, 16) == 1)
        {
            gui_action = GUI_CMD_REDUCE_PATTERNS_10;
        }
        // +10 patterns
        if(zcheckMouse(232, 62, 16, 16) == 1)
        {
            gui_action = GUI_CMD_INCREASE_PATTERNS_10;
        }

        // Record mode
        if(zcheckMouse(8, 98, 80, 16))
        {
            gui_action = GUI_CMD_RECORD_MODE;
        }

        Mouse_Right_Fx_Ed();

        Mouse_Right_303_Ed();

        Mouse_Right_DiskIO_Ed();

        Mouse_Right_Sample_Ed();

        Mouse_Right_Track_Ed();

        Mouse_Right_Instrument_Ed();

        Mouse_Right_Sequencer_Ed();

        Mouse_Right_Synth_Ed();

        Mouse_Right_Master_Ed();
    } // RIGHT MOUSE

    if(Mouse.button == MOUSE_RIGHT_BUTTON)
    {

        // End of the marking stuff
        if(zcheckMouse(1, 194, CONSOLE_WIDTH, 234) && !Songplaying)
        {
            Mark_Block_End(Get_Column_Over_Mouse(), Get_Track_Over_Mouse(), Get_Line_Over_Mouse(), 3);
        }

        Mouse_Right_Repeat_Instrument_Ed();
    }
}

// ------------------------------------------------------
// Search the first free slot in the sequences pool
int Search_Free_Pattern(void)
{
    int i;
    int j;
    int found;

    for(i = 0; i < sLength; i++)
    {
        found = FALSE;
        for(j = 0; j < 128; j++)
        {
            if(pSequence[j] == i)
            {
                found = TRUE;
                break;
            }
        }
        if(!found) return(i);
    }
    return(-1);
}

// ------------------------------------------------------
// Next pattern/position
void Next_Line_Pattern_Auto(void)
{
    int new_pattern;
    int max_value;
    
    if(ped_line >= patternLines[pSequence[cPosition]])
    {
        // Normal end of pattern
        ped_line = ped_line - patternLines[pSequence[cPosition]];
        cPosition++;
        if(cPosition >= sLength - 1)
        {
            sLength++;
            max_value = sLength;
            if(max_value > 255)
            {
                sLength = 255;
                cPosition = 0;
            }
            // Alloc a new pattern position
            new_pattern = Search_Free_Pattern();
            if(new_pattern > -1) pSequence[cPosition] = new_pattern;
        }
    }
}

// ------------------------------------------------------
// Return the track associated with the midi channel
int Get_Midi_Channel(int midi_channel)
{
    int i;

    for(i = 0; i < MAX_TRACKS; i++)
    {
        if(midi_channel == TRACKMIDICHANNEL[i])
        {
            return(i);
        }
    }
    return(-1);
}

// ------------------------------------------------------
// Return the first track which isn't allocated in the midi channels pool
int Get_Free_Channel(void)
{
    int i;
    int j;
    j = ped_track;
    for(i = 0; i < MAX_TRACKS; i++)
    {
        if(Alloc_midi_Channels[j] == 0) return(j);
        j++;
        if(j >= MAX_TRACKS) j = 0;
    }
    return(-1);
}

// ------------------------------------------------------
// Search & return the midi channel corresponding to the given data
int Search_Corresponding_Channel(Uint32 Datas)
{
    int i;

    for(i = 0; i < MAX_TRACKS; i++)
    {
        if(Alloc_midi_Channels[i] == (Datas & 0x00ff00))
        {
            return(i);
        }
    }
    return(-1);
}

// ------------------------------------------------------
// Display the current beat time in milliseconds
void Display_Beat_Time(void)
{
    char String[64];
    sprintf(String, "1/4 Beat Time: %d ms.", f2i(15000 / (float) BeatsPerMin));
    Gui_Clear_Array(268, 102, 120, 16);
    PrintXY(268, 102, USE_FONT, String);
}

// ------------------------------------------------------
// Display master compressor
void Display_Master_Comp(void)
{
    char tt[64];

    if(display_title)
    {
        Gui_Draw_Button_Box(159, 6, 54, 16, "Threshold", BUTTON_NORMAL | BUTTON_DISABLED);
        Realslider_Size(159 + 54, 6, 50, f2i(mas_comp_threshold * 0.5f), TRUE);
        if(mas_comp_ratio <= 0.01f)
        {
            sprintf(tt, "Off");
        }
        else
        {
            sprintf(tt, "%d%%", f2i(mas_comp_threshold));
        }
        Print_String(tt, 159 + 54, 8, 67, BUTTON_TEXT_CENTERED);

        Gui_Draw_Button_Box(283, 6, 41, 16, "Ratio", BUTTON_NORMAL | BUTTON_DISABLED);
        Realslider_Size(283 + 41, 6, 50, f2i(mas_comp_ratio * 0.5f), TRUE);
        if(mas_comp_ratio <= 0.01f)
        {
            sprintf(tt, "Off");
        }
        else
        {
            sprintf(tt, "%d%%", f2i(mas_comp_ratio));
        }
        Print_String(tt, 283 + 41, 8, 67, BUTTON_TEXT_CENTERED);
    }
}
         
// ------------------------------------------------------
// Display master volume
void Display_Master_Volume(void)
{
    char tt[64];

    if(display_title)
    {
        if(mas_vol < 0.01f) mas_vol = 0.01f;
        if(mas_vol > 1.0f) mas_vol = 1.0f;
        Gui_Draw_Button_Box(394, 6, 44, 16, "Mst Vol.", BUTTON_NORMAL | BUTTON_DISABLED);
        Realslider(394 + 44, 6, f2i(mas_vol * 128.0f), TRUE);
        sprintf(tt, "%d%%", f2i(mas_vol * 100.0f));
        Print_String(tt, 394 + 44, 8, 145, BUTTON_TEXT_CENTERED);
    }
}
         
// ------------------------------------------------------
// Display the shuffle amount
void Display_Shuffle(void)
{
    char tt[64];

    if(display_title)
    {
        if(shuffle > 100) shuffle = 100;
        if(shuffle < 0) shuffle = 0;
        Gui_Draw_Button_Box(586, 6, 40, 16, "Shuffle", BUTTON_NORMAL | BUTTON_DISABLED);

        Realslider_Size(586 + 40, 6, 100, shuffle, TRUE);
        sprintf(tt, "%d%%", shuffle);
        Print_String(tt, 586 + 40, 8, 116, BUTTON_TEXT_CENTERED);
    }
}

// ------------------------------------------------------
// Handle the mouse event of the top bar
void Mouse_Sliders_Master_Shuffle(void)
{
    if(display_title)
    {
        // Compressor threshold
        if(zcheckMouse(213, 6, 67, 18))
        {
            Mas_Compressor_Set_Variables((Mouse.x - 223.0f) * 2.0f, mas_comp_ratio);
            Display_Master_Comp();
        }

        // Compressor ratio
        if(zcheckMouse(324, 6, 67, 18))
        {
            Mas_Compressor_Set_Variables(mas_comp_threshold, (Mouse.x - 334.0f) * 2.0f);
            Display_Master_Comp();
        }

        // Master volume
        if(zcheckMouse(438, 6, 148, 18))
        {
            // Normalized
            mas_vol = (Mouse.x - 448.0f) / 128.0f;
            Display_Master_Volume();
        }

        // Shuffle
        if(zcheckMouse(586 + 40, 6, 120, 18))
        {
            shuffle = (int) ((Mouse.x - (586 + 40 + 10)));
            Display_Shuffle();
        }
    }
}

void Actualize_Master(char gode)
{

    if(gode == 0 || gode == 1)
    {
        if(BeatsPerMin < 32) BeatsPerMin = 32;
        if(BeatsPerMin > 255) BeatsPerMin = 255;
        Gui_Draw_Arrows_Number_Box(324, 62, BeatsPerMin, BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
    }

    if(gode == 0 || gode == 2)
    {
        if(TicksPerBeat < 1) TicksPerBeat = 1;
        if(TicksPerBeat > 16) TicksPerBeat = 16;
        if((TicksPerBeat + 1) < Ticks_Synchro_Left)
        {
            Ticks_Synchro_Left = TicksPerBeat + 1;
            Actualize_Fx_Ed(10);
            Actualize_Fx_Ed(11);
        }
        if((TicksPerBeat + 1) < Ticks_Synchro_Right)
        {
            Ticks_Synchro_Right = TicksPerBeat + 1;
            Actualize_Fx_Ed(10);
            Actualize_Fx_Ed(11);
        }
        Gui_Draw_Arrows_Number_Box2(324, 80, TicksPerBeat, BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
    }

    if(gode == 0)
    {
        Gui_Draw_Arrows_Number_Box2(324, 44, Songtracks, BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
    }

    if(gode == 4)
    {
        Gui_Draw_Arrows_Number_Box2(324, 44, Songtracks, BUTTON_NORMAL | BUTTON_TEXT_CENTERED);

        if(userscreen == USER_SCREEN_SEQUENCER) Actualize_Seq_Ed();

        SetColor(COL_BACKGROUND);
        bjbox(0, 186, fsize, 242);
        Actupated(0);
    }

    SamplesPerTick = (int) ((60 * SAMPLESPERSEC) / (BeatsPerMin * TicksPerBeat));  
    float SamplesPerBeat = (float) SAMPLESPERSEC / (((float) BeatsPerMin * 4) / 60);
    SamplesPerSub = SamplesPerTick / 6;

    if(userscreen == USER_SCREEN_SETUP_EDIT) Actualize_Master_Ed(3);
}

// ------------------------------------------------------
// Draw the stereo scope
typedef struct
{
    int nbr;
    int offset;
} POS_SCOPE, *LPPOS_SCOPE;

typedef struct
{
    int x_pos;
    int y_pos;
    int y_large;
    int x_max;
} DAT_POS_SCOPE, *LPDAT_POS_SCOPE;

POS_SCOPE Scope_Table[] =
{
    { 0, 0 },
    { 1, 0 },
    { 2, 1 },
    { 3, 3 },
    { 4, 6 },
    { 5, 10 },
    { 6, 15 },
    { 7, 21 },
    { 8, 28 },
    { 9, 36 },
    { 10, 45 },
    { 11, 55 },
    { 12, 66 },
    { 13, 78 },
    { 14, 91 },
    { 15, 105 },
    { 16, 120 },
};

DAT_POS_SCOPE Scope_Table_Dats[] =
{
    // ---
    {          394, 135 / 2, 135 / 2, 405 },                  // 0

    // ---
    {                   394, 135 / 2, 135 / 2, 405 / 2},      // 1
    { 394 + ((405 / 2) * 1), 135 / 2, 135 / 2, 405 / 2},      // 2

    // ---
    {                   394, 135 / 2, 135 / 2, 405 / 3},      // 3
    { 394 + ((405 / 3) * 1), 135 / 2, 135 / 2, 405 / 3},      // 4
    { 394 + ((405 / 3) * 2), 135 / 2, 135 / 2, 405 / 3},      // 5

    // ---
    {                   394, 135 / 2, 135 / 2, 405 / 4},      // 6
    { 394 + ((405 / 4) * 1), 135 / 2, 135 / 2, 405 / 4},      // 7
    { 394 + ((405 / 4) * 2), 135 / 2, 135 / 2, 405 / 4},      // 8
    { 394 + ((405 / 4) * 3), 135 / 2, 135 / 2, 405 / 4},      // 9

    // ---
    {             394, 135 / 2, 135 / 2, 405 / 5},            // 10
    { 394 + ((405 / 5) * 1), 135 / 2, 135 / 2, 405 / 5},      // 11
    { 394 + ((405 / 5) * 2), 135 / 2, 135 / 2, 405 / 5},      // 12
    { 394 + ((405 / 5) * 3), 135 / 2, 135 / 2, 405 / 5},      // 13
    { 394 + ((405 / 5) * 4), 135 / 2, 135 / 2, 405 / 5},      // 14

    // ---
    {                   394, 135 / 4, 135 / 4, 405 / 3},      // 15
    { 394 + ((405 / 3) * 1), 135 / 4, 135 / 4, 405 / 3},      // 16
    { 394 + ((405 / 3) * 2), 135 / 4, 135 / 4, 405 / 3},      // 17
    {                   394, (135 / 2) + (135 / 4) + 1, 135 / 4, 405 / 3},      // 18
    { 394 + ((405 / 3) * 1), (135 / 2) + (135 / 4) + 1, 135 / 4, 405 / 3},      // 19
    { 394 + ((405 / 3) * 2), (135 / 2) + (135 / 4) + 1, 135 / 4, 405 / 3},      // 20

    // ---
    {                   394, 135 / 4, 135 / 4, 405 / 4},      // 21
    { 394 + ((405 / 4) * 1), 135 / 4, 135 / 4, 405 / 4},      // 22
    { 394 + ((405 / 4) * 2), 135 / 4, 135 / 4, 405 / 4},      // 23
    { 394 + ((405 / 4) * 3), 135 / 4, 135 / 4, 405 / 4},      // 24
    {                   394, (135 / 2) + (135 / 4) + 1, 135 / 4, 405 / 3},      // 25
    { 394 + ((405 / 3) * 1), (135 / 2) + (135 / 4) + 1, 135 / 4, 405 / 3},      // 26
    { 394 + ((405 / 3) * 2), (135 / 2) + (135 / 4) + 1, 135 / 4, 405 / 3},      // 27

    // ---
    {                   394, 135 / 4, 135 / 4, 405 / 4},      // 28
    { 394 + ((405 / 4) * 1), 135 / 4, 135 / 4, 405 / 4},      // 29
    { 394 + ((405 / 4) * 2), 135 / 4, 135 / 4, 405 / 4},      // 30
    { 394 + ((405 / 4) * 3), 135 / 4, 135 / 4, 405 / 4},      // 31
    {                   394, (135 / 2) + (135 / 4) + 1, 135 / 4, 405 / 4},      // 32
    { 394 + ((405 / 4) * 1), (135 / 2) + (135 / 4) + 1, 135 / 4, 405 / 4},      // 33
    { 394 + ((405 / 4) * 2), (135 / 2) + (135 / 4) + 1, 135 / 4, 405 / 4},      // 34
    { 394 + ((405 / 4) * 3), (135 / 2) + (135 / 4) + 1, 135 / 4, 405 / 4},      // 35

    // ---
    {                   394, 135 / 4, 135 / 4, 405 / 5},      // 36
    { 394 + ((405 / 5) * 1), 135 / 4, 135 / 4, 405 / 5},      // 37
    { 394 + ((405 / 5) * 2), 135 / 4, 135 / 4, 405 / 5},      // 38
    { 394 + ((405 / 5) * 3), 135 / 4, 135 / 4, 405 / 5},      // 39
    { 394 + ((405 / 5) * 4), 135 / 4, 135 / 4, 405 / 5},      // 40
    {                   394, (135 / 2) + (135 / 4) + 1, 135 / 4, 405 / 4},      // 41
    { 394 + ((405 / 4) * 1), (135 / 2) + (135 / 4) + 1, 135 / 4, 405 / 4},      // 42
    { 394 + ((405 / 4) * 2), (135 / 2) + (135 / 4) + 1, 135 / 4, 405 / 4},      // 43
    { 394 + ((405 / 4) * 3), (135 / 2) + (135 / 4) + 1, 135 / 4, 405 / 4},      // 44

    // ---
    {                   394, 135 / 4, 135 / 4, 405 / 5},      // 45
    { 394 + ((405 / 5) * 1), 135 / 4, 135 / 4, 405 / 5},      // 46
    { 394 + ((405 / 5) * 2), 135 / 4, 135 / 4, 405 / 5},      // 47
    { 394 + ((405 / 5) * 3), 135 / 4, 135 / 4, 405 / 5},      // 48
    { 394 + ((405 / 5) * 4), 135 / 4, 135 / 4, 405 / 5},      // 49
    {                   394, (135 / 2) + (135 / 4) + 1, 135 / 4, 405 / 5},      // 50
    { 394 + ((405 / 5) * 1), (135 / 2) + (135 / 4) + 1, 135 / 4, 405 / 5},      // 51
    { 394 + ((405 / 5) * 2), (135 / 2) + (135 / 4) + 1, 135 / 4, 405 / 5},      // 52
    { 394 + ((405 / 5) * 3), (135 / 2) + (135 / 4) + 1, 135 / 4, 405 / 5},      // 53
    { 394 + ((405 / 5) * 4), (135 / 2) + (135 / 4) + 1, 135 / 4, 405 / 5},      // 54

    // ---
    {                   394, 135 / 6, 135 / 6, 405 / 4},      // 55
    { 394 + ((405 / 4) * 1), 135 / 6, 135 / 6, 405 / 4},      // 56
    { 394 + ((405 / 4) * 2), 135 / 6, 135 / 6, 405 / 4},      // 57
    { 394 + ((405 / 4) * 3), 135 / 6, 135 / 6, 405 / 4},      // 58

    {                   394, (135 / 2) + 1, 135 / 6, 405 / 4},      // 59
    { 394 + ((405 / 4) * 1), (135 / 2) + 1, 135 / 6, 405 / 4},      // 60
    { 394 + ((405 / 4) * 2), (135 / 2) + 1, 135 / 6, 405 / 4},      // 61
    { 394 + ((405 / 4) * 3), (135 / 2) + 1, 135 / 6, 405 / 4},      // 62

    {                   394, (135 / 2) + (135 / 3), 135 / 6, 405 / 3},      // 63
    { 394 + ((405 / 3) * 1), (135 / 2) + (135 / 3), 135 / 6, 405 / 3},      // 64
    { 394 + ((405 / 3) * 2), (135 / 2) + (135 / 3), 135 / 6, 405 / 3},      // 65

    // ---
    {                   394, 135 / 6, 135 / 6, 405 / 4},      // 66
    { 394 + ((405 / 4) * 1), 135 / 6, 135 / 6, 405 / 4},      // 67
    { 394 + ((405 / 4) * 2), 135 / 6, 135 / 6, 405 / 4},      // 68
    { 394 + ((405 / 4) * 3), 135 / 6, 135 / 6, 405 / 4},      // 69

    {                   394, (135 / 2) + 1, 135 / 6, 405 / 4},      // 70
    { 394 + ((405 / 4) * 1), (135 / 2) + 1, 135 / 6, 405 / 4},      // 71
    { 394 + ((405 / 4) * 2), (135 / 2) + 1, 135 / 6, 405 / 4},      // 72
    { 394 + ((405 / 4) * 3), (135 / 2) + 1, 135 / 6, 405 / 4},      // 73

    {                   394, (135 / 2) + (135 / 3), 135 / 6, 405 / 4},      // 74
    { 394 + ((405 / 4) * 1), (135 / 2) + (135 / 3), 135 / 6, 405 / 4},      // 75
    { 394 + ((405 / 4) * 2), (135 / 2) + (135 / 3), 135 / 6, 405 / 4},      // 76
    { 394 + ((405 / 4) * 3), (135 / 2) + (135 / 3), 135 / 6, 405 / 4},      // 77

    // ---
    {                   394, 135 / 6, 135 / 6, 405 / 5},      // 78
    { 394 + ((405 / 5) * 1), 135 / 6, 135 / 6, 405 / 5},      // 79
    { 394 + ((405 / 5) * 2), 135 / 6, 135 / 6, 405 / 5},      // 80
    { 394 + ((405 / 5) * 3), 135 / 6, 135 / 6, 405 / 5},      // 81
    { 394 + ((405 / 5) * 4), 135 / 6, 135 / 6, 405 / 5},      // 82

    {                   394, (135 / 2) + 1, 135 / 6, 405 / 4},      // 83
    { 394 + ((405 / 4) * 1), (135 / 2) + 1, 135 / 6, 405 / 4},      // 84
    { 394 + ((405 / 4) * 2), (135 / 2) + 1, 135 / 6, 405 / 4},      // 85
    { 394 + ((405 / 4) * 3), (135 / 2) + 1, 135 / 6, 405 / 4},      // 86

    {                   394, (135 / 2) + (135 / 3), 135 / 6, 405 / 4},      // 87
    { 394 + ((405 / 4) * 1), (135 / 2) + (135 / 3), 135 / 6, 405 / 4},      // 88
    { 394 + ((405 / 4) * 2), (135 / 2) + (135 / 3), 135 / 6, 405 / 4},      // 89
    { 394 + ((405 / 4) * 3), (135 / 2) + (135 / 3), 135 / 6, 405 / 4},      // 90

    // ---
    {                   394, 135 / 6, 135 / 6, 405 / 5},      // 91
    { 394 + ((405 / 5) * 1), 135 / 6, 135 / 6, 405 / 5},      // 92
    { 394 + ((405 / 5) * 2), 135 / 6, 135 / 6, 405 / 5},      // 93
    { 394 + ((405 / 5) * 3), 135 / 6, 135 / 6, 405 / 5},      // 94
    { 394 + ((405 / 5) * 4), 135 / 6, 135 / 6, 405 / 5},      // 95

    {                   394, (135 / 2) + 1, 135 / 6, 405 / 5},      // 96
    { 394 + ((405 / 5) * 1), (135 / 2) + 1, 135 / 6, 405 / 5},      // 97
    { 394 + ((405 / 5) * 2), (135 / 2) + 1, 135 / 6, 405 / 5},      // 98
    { 394 + ((405 / 5) * 3), (135 / 2) + 1, 135 / 6, 405 / 5},      // 99
    { 394 + ((405 / 5) * 4), (135 / 2) + 1, 135 / 6, 405 / 5},      // 100

    {                   394, (135 / 2) + (135 / 3), 135 / 6, 405 / 4},      // 101
    { 394 + ((405 / 4) * 1), (135 / 2) + (135 / 3), 135 / 6, 405 / 4},      // 102
    { 394 + ((405 / 4) * 2), (135 / 2) + (135 / 3), 135 / 6, 405 / 4},      // 103
    { 394 + ((405 / 4) * 3), (135 / 2) + (135 / 3), 135 / 6, 405 / 4},      // 104

    // ---
    {                   394, 135 / 6, 135 / 6, 405 / 5},      // 105
    { 394 + ((405 / 5) * 1), 135 / 6, 135 / 6, 405 / 5},      // 106
    { 394 + ((405 / 5) * 2), 135 / 6, 135 / 6, 405 / 5},      // 107
    { 394 + ((405 / 5) * 3), 135 / 6, 135 / 6, 405 / 5},      // 108
    { 394 + ((405 / 5) * 4), 135 / 6, 135 / 6, 405 / 5},      // 109

    {                   394, (135 / 2) + 1, 135 / 6, 405 / 5},      // 110
    { 394 + ((405 / 5) * 1), (135 / 2) + 1, 135 / 6, 405 / 5},      // 111
    { 394 + ((405 / 5) * 2), (135 / 2) + 1, 135 / 6, 405 / 5},      // 112
    { 394 + ((405 / 5) * 3), (135 / 2) + 1, 135 / 6, 405 / 5},      // 113
    { 394 + ((405 / 5) * 4), (135 / 2) + 1, 135 / 6, 405 / 5},      // 114

    {                   394, (135 / 2) + (135 / 3), 135 / 6, 405 / 5},      // 115
    { 394 + ((405 / 5) * 1), (135 / 2) + (135 / 3), 135 / 6, 405 / 5},      // 116
    { 394 + ((405 / 5) * 2), (135 / 2) + (135 / 3), 135 / 6, 405 / 5},      // 117
    { 394 + ((405 / 5) * 3), (135 / 2) + (135 / 3), 135 / 6, 405 / 5},      // 118
    { 394 + ((405 / 5) * 4), (135 / 2) + (135 / 3), 135 / 6, 405 / 5},      // 119

    // ---
    {                   394, 135 / 8, 135 / 8, 405 / 4},      // 120
    { 394 + ((405 / 4) * 1), 135 / 8, 135 / 8, 405 / 4},      // 121
    { 394 + ((405 / 4) * 2), 135 / 8, 135 / 8, 405 / 4},      // 122
    { 394 + ((405 / 4) * 3), 135 / 8, 135 / 8, 405 / 4},      // 123

    {                   394, (135 / 3) + 4, 135 / 8, 405 / 4},      // 125
    { 394 + ((405 / 4) * 1), (135 / 3) + 4, 135 / 8, 405 / 4},      // 126
    { 394 + ((405 / 4) * 2), (135 / 3) + 4, 135 / 8, 405 / 4},      // 127
    { 394 + ((405 / 4) * 3), (135 / 3) + 4, 135 / 8, 405 / 4},      // 128

    {                   394, (135 / 3) + (135 / 4) + 3, 135 / 8, 405 / 4},      // 130
    { 394 + ((405 / 4) * 1), (135 / 3) + (135 / 4) + 3, 135 / 8, 405 / 4},      // 131
    { 394 + ((405 / 4) * 2), (135 / 3) + (135 / 4) + 3, 135 / 8, 405 / 4},      // 132
    { 394 + ((405 / 4) * 3), (135 / 3) + (135 / 4) + 3, 135 / 8, 405 / 4},      // 133

    {                   394, (135 / 3) + (135 / 2) + 2, 135 / 8, 405 / 4},      // 134
    { 394 + ((405 / 4) * 1), (135 / 3) + (135 / 2) + 2, 135 / 8, 405 / 4},      // 135
    { 394 + ((405 / 4) * 2), (135 / 3) + (135 / 2) + 2, 135 / 8, 405 / 4},      // 136
    { 394 + ((405 / 4) * 3), (135 / 3) + (135 / 2) + 2, 135 / 8, 405 / 4},      // 137
};

char *table_channels_scopes[] =
{
    "0",
    "1",
    "2",
    "3",
    "4",
    "5",
    "6",
    "7",
    "8",
    "9",
    "A",
    "B",
    "C",
    "D",
    "E",
    "F",
    "L",
    "R",
};

void Draw_Scope(void)
{
    int x;
    int i;
    int pixel_color = COL_SCOPESSAMPLES;
    float pos_in_line;
    float datas;
    LPDAT_POS_SCOPE ptrTbl_Dat;

    if(Scopish == SCOPE_ZONE_SCOPE)
    {
        SetColor(COL_BACKGROUND);
        Fillrect(394, 42, 394 + 405, 42 + 135);

        if(Scopish_LeftRight)
        {
            // Left / Right
            ptrTbl_Dat = &Scope_Table_Dats[Scope_Table[2].offset];
            for(i = 0; i < Scope_Table[2].nbr; i++)
            {
                x = ptrTbl_Dat->x_pos;
                PrintXY(x + 4, 44 + (ptrTbl_Dat->y_pos - ptrTbl_Dat->y_large), USE_FONT_LOW, table_channels_scopes[i + 16]);

                for(int s = 0; s < ptrTbl_Dat->x_max; s++)
                {
                    pos_in_line = ((float) s) / ptrTbl_Dat->x_max;
                    datas = (Scope_Dats_LeftRight[i][(int) (pos_in_line * 128)] / 8192);
                    if(datas < -1.0f) datas = -1.0f;
                    if(datas > 1.0f) datas = 1.0f;
                    int y = 42 + ptrTbl_Dat->y_pos + (int) (datas * ptrTbl_Dat->y_large);
                    DrawPixel(x, y, pixel_color);
                    x++;
                }
                ptrTbl_Dat++;
                pixel_color = COL_SCOPESSAMPLES;
            }
        }
        else
        {
            // Channels
            ptrTbl_Dat = &Scope_Table_Dats[Scope_Table[Songtracks].offset];
            for(i = 0; i < Scope_Table[Songtracks].nbr; i++)
            {
                x = ptrTbl_Dat->x_pos;
                PrintXY(x + 4, 44 + (ptrTbl_Dat->y_pos - ptrTbl_Dat->y_large), USE_FONT_LOW, table_channels_scopes[i]);

                for(int s = 0; s < ptrTbl_Dat->x_max; s++)
                {
                    pos_in_line = ((float) s) / ptrTbl_Dat->x_max;
                    datas = (Scope_Dats[i][(int) (pos_in_line * 128)] / 8192) * mas_vol;
                    if(datas < -1.0f) datas = -1.0f;
                    if(datas > 1.0f) datas = 1.0f;
                    int y = 42 + ptrTbl_Dat->y_pos + (int) (datas * ptrTbl_Dat->y_large);
                    DrawPixel(x, y, pixel_color);
                    x++;
                }
                ptrTbl_Dat++;
                pixel_color = COL_SCOPESSAMPLES;
            }
        }
    }
}

void Draw_Scope_Files_Button(void)
{
    switch(Scopish)
    {
        case SCOPE_ZONE_SCOPE:
            SetColor(COL_BACKGROUND);
            bjbox(394, 42, 405, 135);
            bjbox(394, 24, 370, 17);
            Gui_Draw_Button_Box(394, 24, 332, 16, "", BUTTON_NORMAL | BUTTON_DISABLED);
            if(Scopish_LeftRight)
            {
                Gui_Draw_Button_Box(728, 24, 16, 16,"T", BUTTON_PUSHED | BUTTON_TEXT_CENTERED);
            }
            else
            {
                Gui_Draw_Button_Box(728, 24, 16, 16,"S", BUTTON_PUSHED | BUTTON_TEXT_CENTERED);
            }
            Gui_Draw_Button_Box(746, 24, 16, 16,"M", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
            Gui_Draw_Button_Box(764, 24, 16, 16,"I", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
            Gui_Draw_Button_Box(782, 24, 16, 16,"P", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
            break;
        
        case SCOPE_ZONE_MOD_DIR:
        case SCOPE_ZONE_INSTR_DIR:
        case SCOPE_ZONE_PRESET_DIR:
            Read_SMPT();
            DumpList(413, 41, lt_index);
            ltActualize(0);
            Gui_Draw_Button_Box(394, 24, 58, 16, "Current Dir:", BUTTON_NORMAL | BUTTON_DISABLED);
            if(Scopish_LeftRight)
            {
                Gui_Draw_Button_Box(728, 24, 16, 16,"T", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
            }
            else
            {
                Gui_Draw_Button_Box(728, 24, 16, 16,"S", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
            }

            switch(Scopish)
            {
                case SCOPE_ZONE_MOD_DIR:
                    Gui_Draw_Button_Box(746, 24, 16, 16,"M", BUTTON_PUSHED | BUTTON_TEXT_CENTERED);
                    Gui_Draw_Button_Box(764, 24, 16, 16,"I", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
                    Gui_Draw_Button_Box(782, 24, 16, 16,"P", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
                    break;
                case SCOPE_ZONE_INSTR_DIR:
                    Gui_Draw_Button_Box(746, 24, 16, 16,"M", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
                    Gui_Draw_Button_Box(764, 24, 16, 16,"I", BUTTON_PUSHED | BUTTON_TEXT_CENTERED);
                    Gui_Draw_Button_Box(782, 24, 16, 16,"P", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
                    break;
                case SCOPE_ZONE_PRESET_DIR:
                    Gui_Draw_Button_Box(746, 24, 16, 16,"M", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
                    Gui_Draw_Button_Box(764, 24, 16, 16,"I", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
                    Gui_Draw_Button_Box(782, 24, 16, 16,"P", BUTTON_PUSHED | BUTTON_TEXT_CENTERED);
                    break;
            }
    
            Gui_Draw_Button_Box(394, 42, 16, 14, "\01", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
            Gui_Draw_Button_Box(394, 162, 16, 14, "\02", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
            break;
    }
}

// ------------------------------------------------------
// Mute all tracks but a given one
void Solo_Track(int track_to_solo)
{
    // Unmute all if the user clicked on a solo track
    if(TRACKSTATE[track_to_solo] == 0)
    {
        int Was_Solo = FALSE;
        
        // Check if all other tracks are muted
        for(int solify = 0; solify < MAX_TRACKS; solify++)
        {
            if((solify != track_to_solo) && TRACKSTATE[solify] == 0)
            {
                Was_Solo = TRUE;
            }
        }
        if(!Was_Solo)
        {
            // Unmute all
            for(int solify = 0; solify < MAX_TRACKS; solify++)
            {
                TRACKSTATE[solify] = 0;
            }
            TRACKSTATE[track_to_solo] = 1;
        }
        else
        {
            // Else mute all
            for(int solify = 0; solify < MAX_TRACKS; solify++)
            {
                TRACKSTATE[solify] = 1;
            }
        }
    }
    else
    {
        // Else mute all
        for(int solify = 0; solify < MAX_TRACKS; solify++)
        {
            TRACKSTATE[solify] = 1;
        }
    }
}