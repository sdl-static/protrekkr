// ------------------------------------------------------
// Protrekkr
// Written by Franck Charlet
// Based on the work of Juan Antonio Arguelles Rius 
// ------------------------------------------------------

#if !defined(__NO_MIDI__)

// ------------------------------------------------------
// Includes
#include "../include/Variables.h"
#include "../include/ptk.h"

#include "include/midi.h"

// ------------------------------------------------------
// Variables
int midi_start_playing;
float milliseconds_midi;
int iTicks;
double ticks;
int last_ped_line;
int first_midi_time;
double first_midi_timer;
double ticks_elapsed;
extern int Midi_Sub_Channel_Number; 
int Midi_Track_Notes[MAX_TRACKS][MAX_POLYPHONY];
extern CSynth Synthesizer[MAX_TRACKS][MAX_POLYPHONY];
extern JAZZ_KEY Sub_Channels_Jazz[MAX_TRACKS][MAX_POLYPHONY];

PtkTimer Midi_Timer;

// ------------------------------------------------------
// Return the instrument associated with the midi program
int Midi_GetProgram(int midi_program)
{
    int i;

    for(i = 0; i < 128; i++)
    {
        if(midi_program == Midiprg[i]) return(i);
    }
    return(-1);
}

// ------------------------------------------------------
// Handle the midi events
void Midi_CallBackIn(Uint32 dwParam1, Uint32 dwParam2)
{
    int Midi_Channel_Number;
    int Midi_Command;
    int Midi_Datas_1;
    int Midi_Datas_2;
    int Midi_Velocity;
    int Instrument_Number; 
    int xoffseted;
    int tmp_note;
    int Unknown_Message;
    int old_ped_line;
    int i_search;
    int search_track;
    double new_time_stamp;
    double int_part;
    double frac_part;
    Uint32 Param1 = dwParam1;
    Uint32 Param2 = dwParam2;

    Midi_Channel_Number = Param1 & 0xf;
    Midi_Command = Param1 & 0xf0;
    if(Midi_Command == 0x90 || Midi_Command == 0x80)
    {
        if(is_recording && !is_recording_2)
        {
            // Start recording
            is_editing = 1;
            is_recording_2 = 1;
            midi_start_playing = 1;
            ticks_elapsed = 0;
            iTicks = 0;
            milliseconds_midi = 0;
            first_midi_time = FALSE;
            is_editing = 1;
            is_recording_2 = 1;
            Nbr_Sub_NoteOff = 0;
            midi_start_playing = 0;
        }
    }
    if(is_editing)
    {
        switch(Midi_Command)
        {
            // Control Change
            case 0xb0:
                Midi_Datas_1 = (Param1 >> 8) & 0xff;
                Midi_Datas_2 = (Param1 >> 16) & 0xff;
                switch(Midi_Datas_1)
                {
                    case 0x40:
                        // Sustain pedal
                        break;

                    case 0x7:
                        // Master volume
                        mas_vol = ((float) Midi_Datas_2 / 128.0f);
                        if(mas_vol < 0.01f) mas_vol = 0.01f;
                        if(mas_vol > 1.0f) mas_vol = 1.0f;
                        Display_Master_Volume();
                        break;

                    default:
                        Unknown_Message = Param1;
                        break;
                }
                break;

            // Program Change
            case 0xc0:
                Instrument_Number = Midi_GetProgram((Param1 >> 8) & 0xff);
                if(Instrument_Number > -1)
                {
                    ped_patsam = Instrument_Number;
                    if(snamesel == 2) snamesel = 0;
                    Actualize_Patterned();
                    RefreshSample();
                    NewWav();
                    Actualize_Synth_Ed(UPDATE_SYNTH_ED_ALL);
                }
                break;

            // Note Off
            case 0x80:
                // Make sure the velocity is 0
                Param1 &= 0x00ffff;

            // Note On
            case 0x90:

                Midi_Velocity = (Param1 >> 16) & 0xff;
                if(Midi_Velocity > 0xfe) Midi_Velocity = 0xfe;
                tmp_note = (Param1 >> 8) & 0xff;
                if(tmp_note > 119) tmp_note = 119;

                if(!first_midi_time)
                {
                    if(!is_recording) Midi_Timer.Set_Frames_Counter();
                    first_midi_time = TRUE;
                    first_midi_timer = (double) Param2;
                    old_ped_line = ped_line;
                } 

                // Nasty hack with the timing but i have no idea how to handle this correctly
                if(!is_recording)
                {
                    // Ticks per minute
                    float Time_Unit = (BeatsPerMin * TicksPerBeat);
                    // Ticks per seconds
                    Time_Unit /= 60.0f;
                    // Ticks per milliseconds
                    Time_Unit /= 1000.0f;
                    ticks = Midi_Timer.Get_Frames_Delay() * Time_Unit;
                }
                else
                {
                    new_time_stamp = (double) Param2;
                    ticks = ((((((new_time_stamp - first_midi_timer) / 1000.0) / 60.0) * (double) BeatsPerMin) * TicksPerBeat));
                }

                first_midi_timer = new_time_stamp;

                frac_part = modf(ticks, &int_part);
                iTicks = (int) fabs(int_part);
                if(!is_recording)
                {
                    if(!Midi_Velocity && iTicks == 0) iTicks = 1;
                }

                // See if current track is occupied
                xoffseted = (ped_track * PATTERN_BYTES) + (ped_line * PATTERN_ROW_LEN) + (pSequence[cPosition] * PATTERN_LEN);
                if(*(RawPatterns + xoffseted) >= 120)
                {
                    Midi_Sub_Channel_Number = ped_track;
                }
                else
                {
                    Midi_Sub_Channel_Number = -1;
                    for(i_search = 0; i_search < MAX_TRACKS; i_search++)
                    {
                        search_track = Get_Midi_Channel(i_search);
                        xoffseted = (search_track * PATTERN_BYTES) + (ped_line * PATTERN_ROW_LEN) + (pSequence[cPosition] * PATTERN_LEN);
                        if(*(RawPatterns + xoffseted) >= 120)
                        {
                            Midi_Sub_Channel_Number = search_track;
                            break;
                        }
                    }
                }
                if(Midi_Sub_Channel_Number != -1)
                {
                    xoffseted = (Midi_Sub_Channel_Number * PATTERN_BYTES) + (ped_line * PATTERN_ROW_LEN) + (pSequence[cPosition] * PATTERN_LEN);
                    if(*(RawPatterns + xoffseted) < 120)
                    {
                        if(!Midi_Velocity && iTicks == 0) iTicks = 1;
                    }
                }
                ped_line += (int) iTicks;

//                Next_Line_Pattern_Auto();

                if(Midi_Sub_Channel_Number > -1)
                {
                    // Channel already allocated (multi notes) ?
                    if(Midi_Velocity)
                    {
                        Midi_Sub_Channel_Number = Get_Free_Midi_Sub_Channel(ped_track);
                    }
                    // Record nothing if there's no more channels
                    if(Midi_Sub_Channel_Number > -1)
                    {
                        if(Midi_Velocity)
                        {
                            xoffseted = (Midi_Sub_Channel_Number * PATTERN_BYTES) + (ped_line * PATTERN_ROW_LEN) + (pSequence[cPosition] * PATTERN_LEN);
                            Alloc_midi_Channels[Midi_Sub_Channel_Number] = (Param1 & 0x00ff00) + 0x100;
                            *(RawPatterns + xoffseted) = tmp_note;             // Note
                            *(RawPatterns + xoffseted + 1) = ped_patsam;       // Instrument
                            *(RawPatterns + xoffseted + 2) = Midi_Velocity;    // Volume

                            int Sub_Channel = Get_Free_Sub_Channel(Midi_Sub_Channel_Number);
                            Sub_Channels_Jazz[Midi_Sub_Channel_Number][Sub_Channel].Note = (Param1 & 0x00ff00) + 0x100;
                            Sub_Channels_Jazz[Midi_Sub_Channel_Number][Sub_Channel].Channel = Midi_Sub_Channel_Number;
                            Sub_Channels_Jazz[Midi_Sub_Channel_Number][Sub_Channel].Sub_Channel = Sub_Channel;
                            Play_Instrument(Midi_Sub_Channel_Number, Sub_Channel, (float) tmp_note, ped_patsam, (Midi_Velocity / 128.0f), 0, 0, FALSE);
                        }
                        else
                        {
                            // (Some devices send "note on" as "note off" with velocity 0)
                            // Search if we know the note
                            Midi_Sub_Channel_Number = Search_Corresponding_Midi_Sub_Channel(ped_track, (Param1 & 0x00ff00) + 0x100);
                            // Not found: get the one that was mentioned
                            if(Midi_Sub_Channel_Number == -1)
                            {
                                Midi_Sub_Channel_Number = Get_Midi_Channel(Midi_Channel_Number);
                            }
                            xoffseted = (Midi_Sub_Channel_Number * PATTERN_BYTES) + (ped_line * PATTERN_ROW_LEN) + (pSequence[cPosition] * PATTERN_LEN);
                            Alloc_midi_Channels[Midi_Sub_Channel_Number] = 0;
                            *(RawPatterns + xoffseted) = 120;
                            *(RawPatterns + xoffseted + 1) = 0xff;             // no instrument
                            *(RawPatterns + xoffseted + 2) = 0xff;             // no volume

                            LPJAZZ_KEY Channel = Get_Jazz_Key_Off((Param1 & 0x00ff00) + 0x100);
                            if(Channel)
                            {
                                Synthesizer[Channel->Channel][Channel->Sub_Channel].NoteOff();
                                if(sp_Stage[Channel->Channel][Channel->Sub_Channel] == PLAYING_SAMPLE)
                                {
                                    sp_Stage[Channel->Channel][Channel->Sub_Channel] = PLAYING_SAMPLE_NOTEOFF;
                                }
                                
                                // That's a possibility
                                noteoff303(Channel->Channel);

                                Channel->Note = 0;
                                Channel->Channel = 0;
                                Channel->Sub_Channel = 0;
                            }

                            Midi_NoteOff(Midi_Sub_Channel_Number);
                        }
                    }
                }
                if(old_ped_line != ped_line)
                {
                    Actualize_Sequencer();
                    Actupated(0);
                }
                old_ped_line = ped_line;
                break;

            default:

                Unknown_Message = Param1;
                break;
        }
    }
    else
    {
        switch(Midi_Command)
        {
            // Program Change
            case 0xc0:
                Instrument_Number = Midi_GetProgram((Param1 >> 8) & 0xff);
                if(Instrument_Number > -1)
                {
                    ped_patsam = Instrument_Number;
                    if(snamesel == 2) snamesel = 0;
                    Actualize_Patterned();
                    RefreshSample();
                    NewWav();
                    Actualize_Synth_Ed(UPDATE_SYNTH_ED_ALL);
                }
                break;

            // Note Off
            case 0x80:
                // Make sure the velocity is 0
                Param1 &= 0x00ffff;

                // No break on purpose

            // Note On
            case 0x90:

                Midi_Velocity = (Param1 >> 16) & 0xff;
                if(Midi_Velocity > 0xfe) Midi_Velocity = 0xfe;
                tmp_note = (Param1 >> 8) & 0xff;
                if(tmp_note > 119) tmp_note = 119;

                Midi_Sub_Channel_Number = Get_Midi_Channel(Midi_Channel_Number);

                if(Midi_Sub_Channel_Number > -1)
                {
                    // Channel already allocated (multi notes) ?
                    if(Alloc_midi_Channels[Midi_Sub_Channel_Number] != 0 && Midi_Velocity)
                    {
                        Midi_Sub_Channel_Number = Get_Free_Midi_Sub_Channel(ped_track);
                    }
                    // Record nothing if there's no more channels available
                    if(Midi_Sub_Channel_Number > -1)
                    {
                        if(Midi_Velocity)
                        {
                            Alloc_midi_Channels[Midi_Sub_Channel_Number] = (Param1 & 0x00ff00) + 0x100;
                            int Chan = ped_track;
                            int Sub_Channel = Get_Free_Sub_Channel(Chan);
                            Sub_Channels_Jazz[Chan][Sub_Channel].Note = (Param1 & 0x00ff00) + 0x100;
                            Sub_Channels_Jazz[Chan][Sub_Channel].Channel = Midi_Sub_Channel_Number;
                            Sub_Channels_Jazz[Chan][Sub_Channel].Sub_Channel = Sub_Channel;
                            Play_Instrument(Chan, Sub_Channel, (float) tmp_note, ped_patsam, (Midi_Velocity / 128.0f), 0, 0, FALSE);
                        }
                        else
                        {
                            // (Some devices send "note on" as "note off" with velocity 0)
                            // Search if we know the note
                            Midi_Sub_Channel_Number = Search_Corresponding_Midi_Sub_Channel(ped_track, Param1 + 0x100);
                            // Not found: get the one that was mentioned
                            if(Midi_Sub_Channel_Number == -1)
                            {
                                Midi_Sub_Channel_Number = Get_Midi_Channel(Midi_Channel_Number);
                            }
                            Alloc_midi_Channels[Midi_Sub_Channel_Number] = 0;

                            LPJAZZ_KEY Channel = Get_Jazz_Key_Off((Param1 & 0x00ff00) + 0x100);
                            if(Channel)
                            {
                                Synthesizer[Channel->Channel][Channel->Sub_Channel].NoteOff();
                                if(sp_Stage[Channel->Channel][Channel->Sub_Channel] == PLAYING_SAMPLE)
                                {
                                    sp_Stage[Channel->Channel][Channel->Sub_Channel] = PLAYING_SAMPLE_NOTEOFF;
                                }
                                
                                // That's a possibility
                                noteoff303(Channel->Channel);

                                Channel->Note = 0;
                                Channel->Channel = 0;
                                Channel->Sub_Channel = 0;
                            }

                            Midi_NoteOff(Midi_Sub_Channel_Number);
                        }
                    }
                }
                break;

            default:

                Unknown_Message = Param1;
                break;
        }
    }
}

// ------------------------------------------------------
// Turn all midi notes off
void Midi_AllNotesOff(void)
{
    if(c_midiout != -1)
    {
        for(int no_track = 0; no_track < MAX_TRACKS; no_track++)
        {
            Midi_NoteOff(no_track);
        }
    }
}

// ------------------------------------------------------
// Reset midi programs
void Midi_Reset(void)
{
    Midi_AllNotesOff();

    for(int mreset = 0; mreset < 16; mreset++)
    {
        LastProgram[mreset] = -1;
    }
}

#endif
