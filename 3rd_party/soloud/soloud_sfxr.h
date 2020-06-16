/*
SFXR module for SoLoud audio engine
Copyright (c) 2014 Jari Komppa
Based on code (c) by Tomas Pettersson, re-licensed under zlib by permission

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.

   3. This notice may not be removed or altered from any source
   distribution.
*/

#ifndef SFXR_H
#define SFXR_H

#include "soloud.h"
#include "soloud_misc.h"

namespace SoLoud
{
	class File;

	struct SfxrParams
	{
        int   wave_type;            //          Wave From                       0 square, 1 sawtooth, 2 sine, 3 noise

        // Frequency
        float p_base_freq;          //  0, 1    Frequency                       Base note of sound
        float p_freq_limit;         //  0, 1    Frequency Cutoff                If sliding, frequency will stop here to prevent low notes
        float p_freq_ramp;          // -1, 1    Frequency Slide                 Slides the frequency up or down
        float p_freq_dramp;         // -1, 1    Delta Slide                     Accelerates frequency slide, can be used to change slide directions

        // Square Wave Form Only
        float p_duty;               //  0, 1    Square Duty                     Controls ratio between up and down states of Square Wave (changing timbre)
        float p_duty_ramp;          // -1, 1    Duty Sweep                      Sweeps the duty up or down

        // Vibrato
        float p_vib_strength;       //  0, 1    Vibrato Depth                   Strength of the vibrato effect
        float p_vib_speed;          //  0, 1    Vibrato Speed                   Speed of the bibrato effect (i.e. frequency)
        float p_vib_delay;          //          ??

        float p_env_attack;         //  0, 1    Attack Time                     Length of the volume envelope attack
        float p_env_sustain;        //  0, 1    Sustain Time                    Length of the volume envelope sustain
        float p_env_decay;          //  0, 1    Decay Time                      Length of the volume envelope decay (aka release)
        float p_env_punch;          //  0, 1    Sustain Punch                   Tilts the sustain envelope for more 'pop'

        // Low-pass / High-pass Filters
        bool  filter_on;            //          ??
        float p_lpf_resonance;      //  0, 1    Low-pass Filter Resonance       Changes the attenuation rate for the low-pass filter, changing the timbre
        float p_lpf_freq;           //  0, 1    Low-pass Filter Cutoff          Frequency at which the low-pass filter starts attenuating higher frequencies
        float p_lpf_ramp;           // -1, 1    Low-pass Filter Sweep           Sweeps the low-pass cutoff up or down
        float p_hpf_freq;           //  0, 1    High-pass Filter Cutoff         Frequency at which the high-pass filter starts attenuating lower frequencies
        float p_hpf_ramp;           // -1, 1    High-pass Filter Sweep          Sweeps the high-pass filter up or down

        // Phaser
        float p_pha_offset;         // -1, 1    Phaser (Flanger) Offset         Offsets a second copy of the wave by a small phase, changing the timbre
        float p_pha_ramp;           // -1, 1    Phaser (Flanger) Sweep          Sweeps the phase up or down

        // Repeat
        float p_repeat_speed;       //  0, 1    Repeat Speed                    Speed of the note repeating

        // Pitch Jump
        float p_arp_speed;          //  0, 1    Pitch Jump (Change) Speed       Larger values means more pitch jumps, useful for arpeggiation
        float p_arp_mod;            // -1, 1    Pitch Jump (Change) Amount      Jump in pitch, up or down

        // Volume
		float master_vol;
		float sound_vol;
	};

	class Sfxr;

	class SfxrInstance : public AudioSourceInstance
	{
		Sfxr *mParent;

		Misc::Prg mRand;
		SfxrParams mParams;

		bool playing_sample;
		int phase;
		double fperiod;
		double fmaxperiod;
		double fslide;
		double fdslide;
		int period;
		float square_duty;
		float square_slide;
		int env_stage;
		int env_time;
		int env_length[3];
		float env_vol;
		float fphase;
		float fdphase;
		int iphase;
		float phaser_buffer[1024];
		int ipp;
		float noise_buffer[32];
		float fltp;
		float fltdp;
		float fltw;
		float fltw_d;
		float fltdmp;
		float fltphp;
		float flthp;
		float flthp_d;
		float vib_phase;
		float vib_speed;
		float vib_amp;
		int rep_time;
		int rep_limit;
		int arp_time;
		int arp_limit;
		double arp_mod;

		void resetSample(bool aRestart);

	public:
		SfxrInstance(Sfxr *aParent);
		virtual unsigned int getAudio(float *aBuffer, unsigned int aSamplesToRead, unsigned int aBufferSize);
		virtual bool hasEnded();
	};

	class Sfxr : public AudioSource
	{
	public:
        SfxrParams mParams;

		enum SFXR_PRESETS 
		{
			COIN,
			LASER,
			EXPLOSION,
			POWERUP,
			HURT,
			JUMP,
			BLIP
		};

		Misc::Prg mRand;
		
		Sfxr();
		virtual ~Sfxr();
		void resetParams();
		result loadParams(const char* aFilename);
		result loadParamsMem(unsigned char *aMem, unsigned int aLength, bool aCopy = false, bool aTakeOwnership = true);
		result loadParamsFile(File *aFile);

		result loadPreset(int aPresetNo, int aRandSeed);
		virtual AudioSourceInstance *createInstance();
	};
};

#endif
