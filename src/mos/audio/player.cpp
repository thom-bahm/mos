/* 
 * File:   Audio.cpp
 * Author: morgan
 *
 * Created on May 7, 2014, 9:41 PM
 */
#include <thread>
#include <chrono>
#include <glm/gtx/io.hpp>
#include <iostream>

#include "audio/player.hpp"
#include "audio/source.hpp"
#include "audio/soundsource.hpp"

/* Effect object functions */
static LPALGENEFFECTS alGenEffects;
static LPALDELETEEFFECTS alDeleteEffects;
static LPALISEFFECT alIsEffect;
static LPALEFFECTI alEffecti;
static LPALEFFECTIV alEffectiv;
static LPALEFFECTF alEffectf;
static LPALEFFECTFV alEffectfv;
static LPALGETEFFECTI alGetEffecti;
static LPALGETEFFECTIV alGetEffectiv;
static LPALGETEFFECTF alGetEffectf;
static LPALGETEFFECTFV alGetEffectfv;

// Filter object functions
static LPALGENFILTERS alGenFilters;
static LPALDELETEFILTERS alDeleteFilters;
static LPALFILTERI alFilteri;
static LPALFILTERF alFilterf;
static LPALGETFILTERI alGetFilteri;
static LPALGETFILTERF alGetFilterf;

/* Auxiliary Effect Slot object functions */
static LPALGENAUXILIARYEFFECTSLOTS alGenAuxiliaryEffectSlots;
static LPALDELETEAUXILIARYEFFECTSLOTS alDeleteAuxiliaryEffectSlots;
static LPALISAUXILIARYEFFECTSLOT alIsAuxiliaryEffectSlot;
static LPALAUXILIARYEFFECTSLOTI alAuxiliaryEffectSloti;
static LPALAUXILIARYEFFECTSLOTIV alAuxiliaryEffectSlotiv;
static LPALAUXILIARYEFFECTSLOTF alAuxiliaryEffectSlotf;
static LPALAUXILIARYEFFECTSLOTFV alAuxiliaryEffectSlotfv;
static LPALGETAUXILIARYEFFECTSLOTI alGetAuxiliaryEffectSloti;
static LPALGETAUXILIARYEFFECTSLOTIV alGetAuxiliaryEffectSlotiv;
static LPALGETAUXILIARYEFFECTSLOTF alGetAuxiliaryEffectSlotf;
static LPALGETAUXILIARYEFFECTSLOTFV alGetAuxiliaryEffectSlotfv;

void init_efx(){

    alGenEffects=(LPALGENEFFECTS)alGetProcAddress("alGenEffects");
    alDeleteEffects=(LPALDELETEEFFECTS)alGetProcAddress("alDeleteEffects");
    alIsEffect=(LPALISEFFECT)alGetProcAddress("alIsEffect");
    alEffecti=(LPALEFFECTI)alGetProcAddress("alEffecti");
    alEffectiv=(LPALEFFECTIV)alGetProcAddress("alEffectiv");
    alEffectf=(LPALEFFECTF)alGetProcAddress("alEffectf");
    alEffectfv=(LPALEFFECTFV)alGetProcAddress("alEffectfv");
    alGetEffecti=(LPALGETEFFECTI)alGetProcAddress("alGetEffecti");
    alGetEffectiv=(LPALGETEFFECTIV)alGetProcAddress("alGetEffectiv");
    alGetEffectf=(LPALGETEFFECTF)alGetProcAddress("alGetEffectf");
    alGetEffectfv=(LPALGETEFFECTFV)alGetProcAddress("alGetEffectfv");

    alGenFilters = (LPALGENFILTERS)alGetProcAddress("alGenFilters");
    alDeleteFilters = (LPALDELETEFILTERS)alGetProcAddress("alDeleteFilters");
    alFilteri = (LPALFILTERI)alGetProcAddress("alFilteri");
    alFilterf = (LPALFILTERF)alGetProcAddress("alFilterf");
    alGetFilteri = (LPALGETFILTERI)alGetProcAddress("alGetFilteri");
    alGetFilterf = (LPALGETFILTERF)alGetProcAddress("alGetFilterf");

    /* Auxiliary Effect Slot object functions */
    alGenAuxiliaryEffectSlots=(LPALGENAUXILIARYEFFECTSLOTS)alGetProcAddress("alGenAuxiliaryEffectSlots");
    alDeleteAuxiliaryEffectSlots=(LPALDELETEAUXILIARYEFFECTSLOTS)alGetProcAddress("alDeleteAuxiliaryEffectSlots");
    alIsAuxiliaryEffectSlot=(LPALISAUXILIARYEFFECTSLOT)alGetProcAddress("alIsAuxiliaryEffectSlot");
    alAuxiliaryEffectSloti=(LPALAUXILIARYEFFECTSLOTI)alGetProcAddress("alAuxiliaryEffectSloti");
    alAuxiliaryEffectSlotiv=(LPALAUXILIARYEFFECTSLOTIV)alGetProcAddress("alAuxiliaryEffectSlotiv");
    alAuxiliaryEffectSlotf=(LPALAUXILIARYEFFECTSLOTF)alGetProcAddress("alAuxiliaryEffectSlotf");
    alAuxiliaryEffectSlotfv=(LPALAUXILIARYEFFECTSLOTFV)alGetProcAddress("alAuxiliaryEffectSlotfv");
    alGetAuxiliaryEffectSloti=(LPALGETAUXILIARYEFFECTSLOTI)alGetProcAddress("alGetAuxiliaryEffectSloti");
    alGetAuxiliaryEffectSlotiv=(LPALGETAUXILIARYEFFECTSLOTIV)alGetProcAddress("alGetAuxiliaryEffectSlotiv");
    alGetAuxiliaryEffectSlotf=(LPALGETAUXILIARYEFFECTSLOTF)alGetProcAddress("alGetAuxiliaryEffectSlotf");
    alGetAuxiliaryEffectSlotfv=(LPALGETAUXILIARYEFFECTSLOTFV)alGetProcAddress("alGetAuxiliaryEffectSlotfv");   
}


namespace mos {

Player::Player(): reverb_properties(EFX_REVERB_PRESET_LIVINGROOM),
    reverb_effect(0),
    reverb_slot(0),
    lowpass_filter1(0),
    lowpass_filter2(0)
 {
    ALCint contextAttr[] = {ALC_FREQUENCY, 44100, 0};
    device_ = alcOpenDevice(NULL);
    context_ = alcCreateContext(device_, contextAttr);
    alcMakeContextCurrent(context_);

    if(!alcIsExtensionPresent(alcGetContextsDevice(alcGetCurrentContext()), "ALC_EXT_EFX")){
        throw std::runtime_error("OpenAL EFX not supported.");
    }

    init_efx();

    ALuint reverb_effect = 0;
    alGenEffects(1, &reverb_effect);
    alEffecti(reverb_effect, AL_EFFECT_TYPE, AL_EFFECT_REVERB);
    alEffectf(reverb_effect, AL_REVERB_DENSITY, reverb_properties.flDensity);
    alEffectf(reverb_effect, AL_REVERB_DIFFUSION, reverb_properties.flDiffusion);
    alEffectf(reverb_effect, AL_REVERB_GAIN, reverb_properties.flGain);
    alEffectf(reverb_effect, AL_REVERB_GAINHF, reverb_properties.flGainHF);
    alEffectf(reverb_effect, AL_REVERB_DECAY_TIME, reverb_properties.flDecayTime);
    alEffectf(reverb_effect, AL_REVERB_DECAY_HFRATIO, reverb_properties.flDecayHFRatio);
    alEffectf(reverb_effect, AL_REVERB_REFLECTIONS_GAIN, reverb_properties.flReflectionsGain);
    alEffectf(reverb_effect, AL_REVERB_REFLECTIONS_DELAY, reverb_properties.flReflectionsDelay);
    alEffectf(reverb_effect, AL_REVERB_LATE_REVERB_GAIN, reverb_properties.flLateReverbGain);
    alEffectf(reverb_effect, AL_REVERB_LATE_REVERB_DELAY, reverb_properties.flLateReverbDelay);
    alEffectf(reverb_effect, AL_REVERB_AIR_ABSORPTION_GAINHF, reverb_properties.flAirAbsorptionGainHF);
    alEffectf(reverb_effect, AL_REVERB_ROOM_ROLLOFF_FACTOR, reverb_properties.flRoomRolloffFactor);
    alEffecti(reverb_effect, AL_REVERB_DECAY_HFLIMIT, reverb_properties.iDecayHFLimit);

    if (!reverb_effect) {
        throw std::runtime_error("Could not create reverb effect.");
    }

    alGenAuxiliaryEffectSlots(1, &reverb_slot);
    if(!reverb_slot) {
        throw std::runtime_error("Could not create reverb effect slot.");
    }

    alAuxiliaryEffectSloti(reverb_slot, AL_EFFECTSLOT_EFFECT, reverb_effect);

    alGenFilters(1, &lowpass_filter1);
    if (!lowpass_filter1){
        std::runtime_error("Could not create lowpass filter.");
    }
    alFilteri(lowpass_filter1, AL_FILTER_TYPE, AL_FILTER_LOWPASS);
    alFilterf(lowpass_filter1,AL_LOWPASS_GAIN, 0.6f); // 0.5f
    alFilterf(lowpass_filter1,AL_LOWPASS_GAINHF, 0.05f); // 0.01f

    alGenFilters(1, &lowpass_filter2);
    if (!lowpass_filter2) {
        std::runtime_error("Could not create lowpass filter.");
    }
    alFilteri(lowpass_filter2, AL_FILTER_TYPE, AL_FILTER_LOWPASS);
    alFilterf(lowpass_filter2,AL_LOWPASS_GAIN, 0.3f); // 0.5f
    alFilterf(lowpass_filter2,AL_LOWPASS_GAINHF, 0.01f); // 0.01f

    listener(Listener());
}

Player::~Player() {
    for (auto & thread : stream_threads){
        thread.second.running = false;
        thread.second.thread->join();
    }
    for (auto source : sources_){
        alSourceStop(source.second);        
        alDeleteSources(1, & source.second);
    }
    for (auto buffer : buffers_){
        alDeleteBuffers(1, & buffer.second);
    }
    alcMakeContextCurrent(nullptr);
    alcDestroyContext(context_);
    alcCloseDevice(device_);
}

void Player::load(const SoundSource & sound_source) {
    if (sources_.find(sound_source.source.id()) == sources_.end()) {
        ALuint al_source;
        alGenSources(1, &al_source);
        sources_.insert(SourcePair(sound_source.source.id(), al_source));

        //Reverb optional.
        alSource3i(al_source, AL_AUXILIARY_SEND_FILTER, reverb_slot, 0, AL_FILTER_NULL);

        ALuint al_filter;
        alGenFilters(1, &al_filter);
        filters_.insert(SourcePair(sound_source.source.id(), al_filter));
        alFilteri(al_filter, AL_FILTER_TYPE, AL_FILTER_LOWPASS);
        alSourcei(al_source, AL_DIRECT_FILTER, al_filter);
    }

    auto sound = sound_source.sound;
    if (buffers_.find(sound->id()) == buffers_.end()) {
        ALuint buffer;
        alGenBuffers(1, &buffer);
        {
            long data_size = std::distance(sound->begin(), sound->end());
            const ALvoid * data = sound->data();
            alBufferData(buffer, AL_FORMAT_MONO16, data, data_size * sizeof (short), sound->sample_rate());
        }
        alSourcei(sources_.at(sound_source.source.id()), AL_BUFFER, buffer);
        buffers_.insert(BufferPair(sound->id(), buffer));
    }
    alSource3f(sources_.at(sound_source.source.id()),
               AL_POSITION, sound_source.source.position.x,
               sound_source.source.position.y,
               sound_source.source.position.z);
    //alSourcePlay(sources_.at(source.id()));
}

void Player::load(const StreamSource & stream_source) {

}

void Player::update(StreamSource & sound_source, const float dt) {
    if (sources_.find(sound_source.source.id()) == sources_.end()) {
        ALuint al_source;
        alGenSources(1, &al_source);
        sources_.insert(SourcePair(sound_source.source.id(), al_source));
        alSource3i(al_source, AL_AUXILIARY_SEND_FILTER, reverb_slot, 0, AL_FILTER_NULL);

        ALuint al_filter;
        alGenFilters(1, &al_filter);
        filters_.insert(SourcePair(sound_source.source.id(), al_filter));
        alFilteri(al_filter, AL_FILTER_TYPE, AL_FILTER_LOWPASS);
        alSourcei(al_source, AL_DIRECT_FILTER, al_filter);
    };

    ALuint al_source = sources_.at(sound_source.source.id());
    alSourcei(al_source, AL_LOOPING, sound_source.source.loop);
    alSourcef(al_source, AL_PITCH, sound_source.source.pitch);
    alSourcef(al_source, AL_GAIN, sound_source.source.gain);
    alSource3f(al_source, AL_POSITION, sound_source.source.position.x,
               sound_source.source.position.y, sound_source.source.position.z);
    alSource3f(al_source, AL_VELOCITY, sound_source.source.velocity.x,
               sound_source.source.velocity.y, sound_source.source.velocity.z);

    auto al_filter = filters_[sound_source.source.id()];
    float ob = sound_source.source.obstructed >= 1.0f ? -1.0f : 1.0f;
    ALfloat al_gain;
    alGetFilterf(al_filter, AL_LOWPASS_GAIN, &al_gain);
    float gain = glm::clamp(al_gain + dt * ob, 0.5f, 1.0f);

    ALfloat al_gain_hf;
    alGetFilterf(al_filter, AL_LOWPASS_GAINHF, &al_gain_hf);
    float gain_hf = glm::clamp(al_gain_hf + dt * ob, 0.01f, 1.0f);

    alFilteri(al_filter, AL_FILTER_TYPE, AL_FILTER_LOWPASS);    
    alFilterf(al_filter,AL_LOWPASS_GAIN, gain); // 0.5f
    alFilterf(al_filter,AL_LOWPASS_GAINHF, gain_hf); // 0.01f
    alSourcei(al_source, AL_DIRECT_FILTER, al_filter);

    sound_source.source.obstructed = 0.0f;

    ALenum state;
    alGetSourcei(al_source, AL_SOURCE_STATE, &state);
		
    if (sound_source.source.playing && (state != AL_PLAYING)) {
        if (stream_threads.count(sound_source.source.id())) {
            stream_threads[sound_source.source.id()].running = false;
            stream_threads[sound_source.source.id()].thread->join();
            stream_threads.erase(sound_source.source.id());
        }
        stream_threads.insert(std::pair<unsigned int, StreamThread>(sound_source.stream->id(),
            StreamThread{std::shared_ptr<std::thread>(new std::thread([&](ALuint al_source,
                         std::shared_ptr<mos::Stream> stream_ptr,
                         const bool loop) {
            mos::Stream stream(*stream_ptr);
            ALuint buffers[2];
            alGenBuffers(2, buffers);

            int size = stream.buffer_size;
            alBufferData(buffers[0], AL_FORMAT_MONO16, stream.read().data(), size*sizeof(ALshort), stream.sample_rate());
            alBufferData(buffers[1], AL_FORMAT_MONO16, stream.read().data(), size*sizeof(ALshort), stream.sample_rate());

            alSourceQueueBuffers(al_source, 2, buffers);

            alSourcePlay(al_source);
            ALenum state;
            alGetSourcei(al_source, AL_SOURCE_STATE, &state);
            alSourcei(al_source, AL_STREAMING, AL_TRUE);
            while(stream_threads[sound_source.stream->id()].running) {
               alGetSourcei(al_source, AL_SOURCE_STATE, &state);
               ALint processed = 0;
               alGetSourcei(al_source, AL_BUFFERS_PROCESSED, &processed);
               while(processed-- && (stream_threads[sound_source.stream->id()].running)) {
                   ALuint buffer = 0;
                   alSourceUnqueueBuffers(al_source, 1, &buffer);
                   auto samples = stream.read();
                   alBufferData(buffer, AL_FORMAT_MONO16, samples.data(), size*sizeof(ALshort), stream.sample_rate());
                   alSourceQueueBuffers(al_source, 1, &buffer);
               }

               std::this_thread::sleep_for(std::chrono::milliseconds(100));
               alGetSourcei(al_source, AL_SOURCE_STATE, &state);

               if (loop && stream.done()) {
                   stream.seek_start();
               }
            }
            stream.seek_start();
            alDeleteBuffers(2, buffers);
            }, al_source, sound_source.stream, sound_source.source.loop)), true}));
    }
	
	

    ALint type;
    alGetSourcei(al_source, AL_SOURCE_TYPE, &type);
    if(!sound_source.source.playing && (state == AL_PLAYING)) {
        alSourceStop(al_source);
    }

    /*
    if (state == AL_STOPPED) {
        alSourceRewind(al_source);
        source.playing = false;
    }*/
}

Listener Player::listener() {
    Listener listener;
    alGetListener3f(AL_POSITION,
                    &listener.position.x,
                    &listener.position.y,
                    &listener.position.z);

    alGetListener3f(AL_ORIENTATION,
                    &listener.orientation.x,
                    &listener.orientation.y,
                    &listener.orientation.z);

    alGetListener3f(AL_VELOCITY,
                    &listener.velocity.x,
                    &listener.velocity.y,
                    &listener.velocity.z);

    return listener;
}

void Player::listener(const Listener &listener) {
    alListener3f(AL_POSITION, listener.position.x, listener.position.y, listener.position.z);

    alListener3f(AL_VELOCITY, listener.velocity.x, listener.velocity.y, listener.velocity.z);

    auto up = glm::vec3(0.0f, 0.0f, 1.0f);
    float orient[6] = {listener.orientation.x, listener.orientation.y, listener.orientation.z,
                       up.x, up.y, up.z};
    alListenerfv(AL_ORIENTATION, orient);
}

void Player::update(SoundSource & sound_source, const float dt)
{
    if (sources_.find(sound_source.source.id()) != sources_.end()) {
        ALuint al_source = sources_.at(sound_source.source.id());
        alSourcei(al_source, AL_LOOPING, sound_source.source.loop);
        alSourcef(al_source, AL_PITCH, sound_source.source.pitch);
        alSourcef(al_source, AL_GAIN, sound_source.source.gain);
        alSource3f(al_source, AL_POSITION, sound_source.source.position.x,
                   sound_source.source.position.y, sound_source.source.position.z);
        alSource3f(al_source, AL_VELOCITY, sound_source.source.velocity.x,
                   sound_source.source.velocity.y,
                   sound_source.source.velocity.z);

        auto al_filter = filters_[sound_source.source.id()];
        float ob = sound_source.source.obstructed >= 1.0f ? -1.0f : 1.0f;
        ALfloat al_gain;
        alGetFilterf(al_filter, AL_LOWPASS_GAIN, &al_gain);
        float gain = glm::clamp(al_gain + dt * ob, 0.5f, 1.0f);

        ALfloat al_gain_hf;
        alGetFilterf(al_filter, AL_LOWPASS_GAINHF, &al_gain_hf);
        float gain_hf = glm::clamp(al_gain_hf + dt * ob, 0.01f, 1.0f);

        alFilteri(al_filter, AL_FILTER_TYPE, AL_FILTER_LOWPASS);
        alFilterf(al_filter,AL_LOWPASS_GAIN, gain); // 0.5f
        alFilterf(al_filter,AL_LOWPASS_GAINHF, gain_hf); // 0.01f
        alSourcei(al_source, AL_DIRECT_FILTER, al_filter);

        sound_source.source.obstructed = 0.0f;

        ALenum state;
        alGetSourcei(al_source, AL_SOURCE_STATE, &state);

        if (sound_source.source.playing && (state != AL_PLAYING)) {
            alSourcePlay(al_source);
        }

        ALint type;
        alGetSourcei(al_source, AL_SOURCE_TYPE, &type);
        if(!sound_source.source.playing && (state == AL_PLAYING) && type == AL_STREAMING) {
            alSourceStop(al_source);
        }

        if (state == AL_STOPPED){
            alSourceRewind(al_source);
            sound_source.source.playing = false;
        }
    }
}

}
