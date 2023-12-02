//
// Created by Bolt on 10/26/2023.
//

#ifndef VULKANWORKSPACE_AL_H
#define VULKANWORKSPACE_AL_H
#include <AL/al.h>
#include <AL/alext.h>
namespace openal::al{
    enum class BufferFormat : ALenum {
        Mono8 = AL_FORMAT_MONO8,
        Mono16 = AL_FORMAT_MONO16,
        Stereo8 = AL_FORMAT_STEREO8,
        Stereo16 = AL_FORMAT_STEREO16,
        MonoFloat32 = AL_FORMAT_MONO_FLOAT32,
        StereoFloat32 = AL_FORMAT_STEREO_FLOAT32,
        MonoDouble = AL_FORMAT_MONO_DOUBLE_EXT,
        StereoDouble = AL_FORMAT_STEREO_DOUBLE_EXT,
        MonoMulaw = AL_FORMAT_MONO_MULAW_EXT,
        StereoMulaw = AL_FORMAT_STEREO_MULAW_EXT,
        MonoAlaw = AL_FORMAT_MONO_ALAW_EXT,
        StereoAlaw = AL_FORMAT_STEREO_ALAW_EXT,
        QuadMulaw = AL_FORMAT_QUAD_MULAW,
        RearMulaw = AL_FORMAT_REAR_MULAW,
        SurroundSound_5Point1_ChnMulaw = AL_FORMAT_51CHN_MULAW,
        SurroundSound_6Point1_ChnMulaw = AL_FORMAT_61CHN_MULAW,
        SurroundSound_7Point1_ChnMulaw = AL_FORMAT_71CHN_MULAW,
        Quad8 = AL_FORMAT_QUAD8,
        Quad16 = AL_FORMAT_QUAD16,
        Quad32 = AL_FORMAT_QUAD32,
        Rear8 = AL_FORMAT_REAR8,
        Rear16 = AL_FORMAT_REAR16,
        Rear32 = AL_FORMAT_REAR32,
        SurroundSound_5Point1_Chn8 = AL_FORMAT_51CHN8,
        SurroundSound_5Point1_Chn16 = AL_FORMAT_51CHN16,
        SurroundSound_5Point1_Chn32 = AL_FORMAT_51CHN32,
        SurroundSound_6Point1_Chn8 = AL_FORMAT_61CHN8,
        SurroundSound_6Point1_Chn16 = AL_FORMAT_61CHN16,
        SurroundSound_6Point1_Chn32 = AL_FORMAT_61CHN32,
        SurroundSound_7Point1_Chn8 = AL_FORMAT_71CHN8,
        SurroundSound_7Point1_Chn16 = AL_FORMAT_71CHN16,
        SurroundSound_7Point1_Chn32 = AL_FORMAT_71CHN32,
        MonoIma4 = AL_FORMAT_MONO_IMA4,
        StereoIma4 = AL_FORMAT_STEREO_IMA4,
        MonoMsadpcm = AL_FORMAT_MONO_MSADPCM_SOFT,
        StereoMsadpcm = AL_FORMAT_STEREO_MSADPCM_SOFT,
        Bformat2d8 = AL_FORMAT_BFORMAT2D_8,
        Bformat2d16 = AL_FORMAT_BFORMAT2D_16,
        Bformat2dFloat32 = AL_FORMAT_BFORMAT2D_FLOAT32,
        Bformat3d8 = AL_FORMAT_BFORMAT3D_8,
        Bformat3d16 = AL_FORMAT_BFORMAT3D_16,
        Bformat3dFloat32 = AL_FORMAT_BFORMAT3D_FLOAT32,
        Bformat2dMulaw = AL_FORMAT_BFORMAT2D_MULAW,
        Bformat3dMulaw = AL_FORMAT_BFORMAT3D_MULAW,
    };
}
#endif //VULKANWORKSPACE_AL_H
