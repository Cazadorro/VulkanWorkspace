//
// Created by Bolt on 10/21/2023.
//

#ifndef VULKANWORKSPACE_OPENAL_EFFECT_H
#define VULKANWORKSPACE_OPENAL_EFFECT_H

#include "openal_util.h"
#include <AL/alc.h>
#include <AL/al.h>
#include <AL/alext.h>
#include <tl/expected.hpp>
#include <span>

namespace openal {
    class UnTypedEffectHandle : public Handle<ALuint, UnTypedEffectHandle> {
    public:
        using Handle::Handle;

        enum class Attribute : ALenum {
            EffectType = AL_EFFECT_TYPE,
            /* Reverb effect parameters */
            ReverbDensity = AL_REVERB_DENSITY,
            ReverbDiffusion = AL_REVERB_DIFFUSION,
            ReverbGain = AL_REVERB_GAIN,
            ReverbGainhf = AL_REVERB_GAINHF,
            ReverbDecayTime = AL_REVERB_DECAY_TIME,
            ReverbDecayHfratio = AL_REVERB_DECAY_HFRATIO,
            ReverbReflectionsGain = AL_REVERB_REFLECTIONS_GAIN,
            ReverbReflectionsDelay = AL_REVERB_REFLECTIONS_DELAY,
            ReverbLateReverbGain = AL_REVERB_LATE_REVERB_GAIN,
            ReverbLateReverbDelay = AL_REVERB_LATE_REVERB_DELAY,
            ReverbAirAbsorptionGainhf = AL_REVERB_AIR_ABSORPTION_GAINHF,
            ReverbRoomRolloffFactor = AL_REVERB_ROOM_ROLLOFF_FACTOR,
            ReverbDecayHflimit = AL_REVERB_DECAY_HFLIMIT,
            /* EAX Reverb effect parameters */
            EaxreverbDensity = AL_EAXREVERB_DENSITY,
            EaxreverbDiffusion = AL_EAXREVERB_DIFFUSION,
            EaxreverbGain = AL_EAXREVERB_GAIN,
            EaxreverbGainhf = AL_EAXREVERB_GAINHF,
            EaxreverbGainlf = AL_EAXREVERB_GAINLF,
            EaxreverbDecayTime = AL_EAXREVERB_DECAY_TIME,
            EaxreverbDecayHfratio = AL_EAXREVERB_DECAY_HFRATIO,
            EaxreverbDecayLfratio = AL_EAXREVERB_DECAY_LFRATIO,
            EaxreverbReflectionsGain = AL_EAXREVERB_REFLECTIONS_GAIN,
            EaxreverbReflectionsDelay = AL_EAXREVERB_REFLECTIONS_DELAY,
            EaxreverbReflectionsPan = AL_EAXREVERB_REFLECTIONS_PAN,
            EaxreverbLateReverbGain = AL_EAXREVERB_LATE_REVERB_GAIN,
            EaxreverbLateReverbDelay = AL_EAXREVERB_LATE_REVERB_DELAY,
            EaxreverbLateReverbPan = AL_EAXREVERB_LATE_REVERB_PAN,
            EaxreverbEchoTime = AL_EAXREVERB_ECHO_TIME,
            EaxreverbEchoDepth = AL_EAXREVERB_ECHO_DEPTH,
            EaxreverbModulationTime = AL_EAXREVERB_MODULATION_TIME,
            EaxreverbModulationDepth = AL_EAXREVERB_MODULATION_DEPTH,
            EaxreverbAirAbsorptionGainhf = AL_EAXREVERB_AIR_ABSORPTION_GAINHF,
            EaxreverbHfreference = AL_EAXREVERB_HFREFERENCE,
            EaxreverbLfreference = AL_EAXREVERB_LFREFERENCE,
            EaxreverbRoomRolloffFactor = AL_EAXREVERB_ROOM_ROLLOFF_FACTOR,
            EaxreverbDecayHflimit = AL_EAXREVERB_DECAY_HFLIMIT,
            /* Chorus effect parameters */
            ChorusWaveform = AL_CHORUS_WAVEFORM,
            ChorusPhase = AL_CHORUS_PHASE,
            ChorusRate = AL_CHORUS_RATE,
            ChorusDepth = AL_CHORUS_DEPTH,
            ChorusFeedback = AL_CHORUS_FEEDBACK,
            ChorusDelay = AL_CHORUS_DELAY,
            /* Distortion effect parameters */
            DistortionEdge = AL_DISTORTION_EDGE,
            DistortionGain = AL_DISTORTION_GAIN,
            DistortionLowpassCutoff = AL_DISTORTION_LOWPASS_CUTOFF,
            DistortionEqcenter = AL_DISTORTION_EQCENTER,
            DistortionEqbandwidth = AL_DISTORTION_EQBANDWIDTH,
            /* Echo effect parameters */
            EchoDelay = AL_ECHO_DELAY,
            EchoLrdelay = AL_ECHO_LRDELAY,
            EchoDamping = AL_ECHO_DAMPING,
            EchoFeedback = AL_ECHO_FEEDBACK,
            EchoSpread = AL_ECHO_SPREAD,
            /* Flanger effect parameters */
            FlangerWaveform = AL_FLANGER_WAVEFORM,
            FlangerPhase = AL_FLANGER_PHASE,
            FlangerRate = AL_FLANGER_RATE,
            FlangerDepth = AL_FLANGER_DEPTH,
            FlangerFeedback = AL_FLANGER_FEEDBACK,
            FlangerDelay = AL_FLANGER_DELAY,
            /* Frequency shifter effect parameters */
            FrequencyShifterFrequency = AL_FREQUENCY_SHIFTER_FREQUENCY,
            FrequencyShifterLeftDirection = AL_FREQUENCY_SHIFTER_LEFT_DIRECTION,
            FrequencyShifterRightDirection = AL_FREQUENCY_SHIFTER_RIGHT_DIRECTION,
            /* Vocal morpher effect parameters */
            VocalMorpherPhonemea = AL_VOCAL_MORPHER_PHONEMEA,
            VocalMorpherPhonemeaCoarseTuning = AL_VOCAL_MORPHER_PHONEMEA_COARSE_TUNING,
            VocalMorpherPhonemeb = AL_VOCAL_MORPHER_PHONEMEB,
            VocalMorpherPhonemebCoarseTuning = AL_VOCAL_MORPHER_PHONEMEB_COARSE_TUNING,
            VocalMorpherWaveform = AL_VOCAL_MORPHER_WAVEFORM,
            VocalMorpherRate = AL_VOCAL_MORPHER_RATE,
            /* Pitchshifter effect parameters */
            PitchShifterCoarseTune = AL_PITCH_SHIFTER_COARSE_TUNE,
            PitchShifterFineTune = AL_PITCH_SHIFTER_FINE_TUNE,
            /* Ringmodulator effect parameters */
            RingModulatorFrequency = AL_RING_MODULATOR_FREQUENCY,
            RingModulatorHighpassCutoff = AL_RING_MODULATOR_HIGHPASS_CUTOFF,
            RingModulatorWaveform = AL_RING_MODULATOR_WAVEFORM,
            /* Autowah effect parameters */
            AutowahAttackTime = AL_AUTOWAH_ATTACK_TIME,
            AutowahReleaseTime = AL_AUTOWAH_RELEASE_TIME,
            AutowahResonance = AL_AUTOWAH_RESONANCE,
            AutowahPeakGain = AL_AUTOWAH_PEAK_GAIN,
            /* Compressor effect parameters */
            CompressorOnoff = AL_COMPRESSOR_ONOFF,
            /* Equalizer effect parameters */
            EqualizerLowGain = AL_EQUALIZER_LOW_GAIN,
            EqualizerLowCutoff = AL_EQUALIZER_LOW_CUTOFF,
            EqualizerMid1Gain = AL_EQUALIZER_MID1_GAIN,
            EqualizerMid1Center = AL_EQUALIZER_MID1_CENTER,
            EqualizerMid1Width = AL_EQUALIZER_MID1_WIDTH,
            EqualizerMid2Gain = AL_EQUALIZER_MID2_GAIN,
            EqualizerMid2Center = AL_EQUALIZER_MID2_CENTER,
            EqualizerMid2Width = AL_EQUALIZER_MID2_WIDTH,
            EqualizerHighGain = AL_EQUALIZER_HIGH_GAIN,
            EqualizerHighCutoff = AL_EQUALIZER_HIGH_CUTOFF,
        };

        enum class Type : ALenum {
            Null = AL_EFFECT_NULL,
            Reverb = AL_EFFECT_REVERB,
            Chorus = AL_EFFECT_CHORUS,
            Distortion = AL_EFFECT_DISTORTION,
            Echo = AL_EFFECT_ECHO,
            Flanger = AL_EFFECT_FLANGER,
            FrequencyShifter = AL_EFFECT_FREQUENCY_SHIFTER,
            VocalMorpher = AL_EFFECT_VOCAL_MORPHER,
            PitchShifter = AL_EFFECT_PITCH_SHIFTER,
            RingModulator = AL_EFFECT_RING_MODULATOR,
            Autowah = AL_EFFECT_AUTOWAH,
            Compressor = AL_EFFECT_COMPRESSOR,
            Equalizer = AL_EFFECT_EQUALIZER,
            Eaxreverb = AL_EFFECT_EAXREVERB,
        };


        [[nodiscard]]
        bool is_valid() const;

        template<typename T>
        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> set(Attribute attribute, T value) = delete;

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> set(Attribute attribute, float value);

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> set(Attribute attribute, std::int32_t value);

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> set(Attribute attribute, std::span<const float> value);

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> set(Attribute attribute, std::span<const std::int32_t> value);

        template<typename T>
        [[nodiscard]]
        tl::expected<T, AlErrorState> get(Attribute attribute) const = delete;

        template<>
        [[nodiscard]]
        tl::expected<float, AlErrorState> get(Attribute attribute) const;

        template<>
        [[nodiscard]]
        tl::expected<std::int32_t, AlErrorState> get(Attribute attribute) const;

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> get(Attribute attribute, std::span<float> value) const;

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> get(Attribute attribute, std::span<std::int32_t> value) const;


        template<typename T, typename U>
        [[nodiscard]]
        tl::expected<U, AlErrorState> get(Attribute attribute) const {
            static constexpr auto array_size = sizeof(U) / sizeof(T);
            static_assert(array_size * sizeof(T) == sizeof(U), "Expected T to exactly fill U");
            std::array<T, array_size> value = {};
            return get(attribute, std::span(value)).map([value](auto result) {
                return std::bit_cast<U>(value);
            });
        }

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> set_type(Type value);

        [[nodiscard]]
        tl::expected<Type, AlErrorState> get_type() const;
    private:
    };

    static constexpr auto null_effect_handle = UnTypedEffectHandle();
//
//    class EffectHandle

    class Effect : public UnTypedEffectHandle {
    public:
        using UnTypedEffectHandle::UnTypedEffectHandle;

        ~Effect();

        Effect(Effect &&rhs) noexcept;

        Effect &operator=(Effect &&rhs) noexcept;

        Effect(const Effect &rhs) = delete;

        Effect &operator=(const Effect &rhs) = delete;

        [[nodiscard]]
        static tl::expected<Effect, AlErrorState> generate();

        [[nodiscard]]
        static tl::expected<std::vector<Effect>, AlErrorState> generate_many(ALsizei count);
    };
}

#endif //VULKANWORKSPACE_OPENAL_EFFECT_H
