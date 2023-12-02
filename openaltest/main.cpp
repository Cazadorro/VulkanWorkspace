//
// Created by Bolt on 10/18/2023.
//

#include "al.h"
#include "alc.h"

#include <uul/assert.h>

#include <tl/expected.hpp>
#include <fmt/core.h>

#include <string>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <type_traits>

#define alCall(function, ...) alCallImpl(__FILE__, __LINE__, function, __VA_ARGS__)

bool check_al_errors(const std::string& filename, const std::uint_fast32_t line)
{
    ALenum error = alGetError();
    if(error != AL_NO_ERROR)
    {
        std::cerr << "***ERROR*** (" << filename << ": " << line << ")\n" ;
        switch(error)
        {
            case AL_INVALID_NAME:
                std::cerr << "AL_INVALID_NAME: a bad name (ID) was passed to an OpenAL function";
                break;
            case AL_INVALID_ENUM:
                std::cerr << "AL_INVALID_ENUM: an invalid enum value was passed to an OpenAL function";
                break;
            case AL_INVALID_VALUE:
                std::cerr << "AL_INVALID_VALUE: an invalid value was passed to an OpenAL function";
                break;
            case AL_INVALID_OPERATION:
                std::cerr << "AL_INVALID_OPERATION: the requested operation is not valid";
                break;
            case AL_OUT_OF_MEMORY:
                std::cerr << "AL_OUT_OF_MEMORY: the requested operation resulted in OpenAL running out of memory";
                break;
            default:
                std::cerr << "UNKNOWN AL ERROR: " << error;
        }
        std::cerr << std::endl;
        return false;
    }
    return true;
}

template<typename alFunction, typename... Params>
auto alCallImpl(const char* filename,
                const std::uint_fast32_t line,
                alFunction function,
                Params... params)
->typename std::enable_if_t<!std::is_same_v<void, decltype(function(params...))>, decltype(function(params...))>
{
    auto ret = function(std::forward<Params>(params)...);
    check_al_errors(filename, line);
    return ret;
}

template<typename alFunction, typename... Params>
auto alCallImpl(const char* filename,
                const std::uint_fast32_t line,
                alFunction function,
                Params... params)
->typename std::enable_if_t<std::is_same_v<void, decltype(function(params...))>, bool>
{
    function(std::forward<Params>(params)...);
    return check_al_errors(filename, line);
}



#define alcCall(function, device, ...) alcCallImpl(__FILE__, __LINE__, function, device, __VA_ARGS__)

bool check_alc_errors(const std::string& filename, const std::uint_fast32_t line, ALCdevice* device)
{
    ALCenum error = alcGetError(device);
    if(error != ALC_NO_ERROR)
    {
        std::cerr << "***ERROR*** (" << filename << ": " << line << ")\n" ;
        switch(error)
        {
            case ALC_INVALID_VALUE:
                std::cerr << "ALC_INVALID_VALUE: an invalid value was passed to an OpenAL function";
                break;
            case ALC_INVALID_DEVICE:
                std::cerr << "ALC_INVALID_DEVICE: a bad device was passed to an OpenAL function";
                break;
            case ALC_INVALID_CONTEXT:
                std::cerr << "ALC_INVALID_CONTEXT: a bad context was passed to an OpenAL function";
                break;
            case ALC_INVALID_ENUM:
                std::cerr << "ALC_INVALID_ENUM: an unknown enum value was passed to an OpenAL function";
                break;
            case ALC_OUT_OF_MEMORY:
                std::cerr << "ALC_OUT_OF_MEMORY: an unknown enum value was passed to an OpenAL function";
                break;
            default:
                std::cerr << "UNKNOWN ALC ERROR: " << error;
        }
        std::cerr << std::endl;
        return false;
    }
    return true;
}

template<typename alcFunction, typename... Params>
auto alcCallImpl(const char* filename,
                 const std::uint_fast32_t line,
                 alcFunction function,
                 ALCdevice* device,
                 Params... params)
        ->typename std::enable_if_t<std::is_same_v<void,decltype(function(params...))>,bool>
{
function(std::forward<Params>(params)...);
return check_alc_errors(filename,line,device);
}

template<typename alcFunction, typename ReturnType, typename... Params>
auto alcCallImpl(const char* filename,
                 const std::uint_fast32_t line,
                 alcFunction function,
                 ReturnType& returnValue,
                 ALCdevice* device,
                 Params... params)
->typename std::enable_if_t<!std::is_same_v<void,decltype(function(params...))>,bool>
{
    returnValue = function(std::forward<Params>(params)...);
    return check_alc_errors(filename,line,device);
}

std::int32_t convert_to_int(char* buffer, std::size_t len)
{
    std::int32_t a = 0;
    if(std::endian::native == std::endian::little)
        std::memcpy(&a, buffer, len);
    else
        for(std::size_t i = 0; i < len; ++i)
            reinterpret_cast<char*>(&a)[3 - i] = buffer[i];
    return a;
}

bool load_wav_file_header(std::ifstream& file,
                          std::uint8_t& channels,
                          std::int32_t& sampleRate,
                          std::uint8_t& bitsPerSample,
                          ALsizei& size)
{

//    char buffer2[128];
//    file.read(buffer2, 128);
    char buffer[4];
    if(!file.is_open())
        return false;

    // the RIFF
    if(!file.read(buffer, 4))
    {
        std::cerr << "ERROR: could not read RIFF" << std::endl;
        return false;
    }
    if(std::strncmp(buffer, "RIFF", 4) != 0)
    {
        std::cerr << "ERROR: file is not a valid WAVE file (header doesn't begin with RIFF)" << std::endl;
        return false;
    }

    // the size of the file
    if(!file.read(buffer, 4))
    {
        std::cerr << "ERROR: could not read size of file" << std::endl;
        return false;
    }

    // the WAVE
    if(!file.read(buffer, 4))
    {
        std::cerr << "ERROR: could not read WAVE" << std::endl;
        return false;
    }
    if(std::strncmp(buffer, "WAVE", 4) != 0)
    {
        std::cerr << "ERROR: file is not a valid WAVE file (header doesn't contain WAVE)" << std::endl;
        return false;
    }

    // "fmt/0"
    if(!file.read(buffer, 4))
    {
        std::cerr << "ERROR: could not read fmt/0" << std::endl;
        return false;
    }

    // this is always 16, the size of the fmt data chunk
    if(!file.read(buffer, 4))
    {
        std::cerr << "ERROR: could not read the 16" << std::endl;
        return false;
    }

    // PCM should be 1?
    if(!file.read(buffer, 2))
    {
        std::cerr << "ERROR: could not read PCM" << std::endl;
        return false;
    }

    // the number of channels
    if(!file.read(buffer, 2))
    {
        std::cerr << "ERROR: could not read number of channels" << std::endl;
        return false;
    }
    channels = convert_to_int(buffer, 2);

    // sample rate
    if(!file.read(buffer, 4))
    {
        std::cerr << "ERROR: could not read sample rate" << std::endl;
        return false;
    }
    sampleRate = convert_to_int(buffer, 4);

    // (sampleRate * bitsPerSample * channels) / 8
    if(!file.read(buffer, 4))
    {
        std::cerr << "ERROR: could not read (sampleRate * bitsPerSample * channels) / 8" << std::endl;
        return false;
    }

    // ?? dafaq
    if(!file.read(buffer, 2))
    {
        std::cerr << "ERROR: could not read dafaq" << std::endl;
        return false;
    }

    // bitsPerSample
    if(!file.read(buffer, 2))
    {
        std::cerr << "ERROR: could not read bits per sample" << std::endl;
        return false;
    }
    bitsPerSample = convert_to_int(buffer, 2);

    // data chunk header "data"
    if(!file.read(buffer, 4))
    {
        std::cerr << "ERROR: could not read data chunk header" << std::endl;
        return false;
    }
    if(std::strncmp(buffer, "data", 4) != 0)
    {
        std::cerr << "ERROR: file is not a valid WAVE file (doesn't have 'data' tag)" << std::endl;
        return false;
    }

    // size of data
    if(!file.read(buffer, 4))
    {
        std::cerr << "ERROR: could not read data size" << std::endl;
        return false;
    }
    size = convert_to_int(buffer, 4);

    /* cannot be at the end of file */
    if(file.eof())
    {
        std::cerr << "ERROR: reached EOF on the file" << std::endl;
        return false;
    }
    if(file.fail())
    {
        std::cerr << "ERROR: fail state set on the file" << std::endl;
        return false;
    }

    return true;
}

bool load_wav(const std::string& filename,
               std::uint8_t& channels,
               std::int32_t& sampleRate,
               std::uint8_t& bitsPerSample,
               std::vector<char>& soundData)
{
    ALsizei size;
    std::ifstream in(filename, std::ios::binary);
    if(!in.is_open())
    {
        std::cerr << "ERROR: Could not open \"" << filename << "\"" << std::endl;
        return false;
    }
    if(!load_wav_file_header(in, channels, sampleRate, bitsPerSample, size))
    {
        std::cerr << "ERROR: Could not load wav header of \"" << filename << "\"" << std::endl;
        return false;
    }

    soundData.resize(size);

    in.read(soundData.data(), size);

    return true;
}


bool get_available_devices(std::vector<std::string>& devicesVec, ALCdevice* device)
{
    const ALCchar* devices;
    if(!alcCall(alcGetString, devices, device, nullptr, ALC_DEVICE_SPECIFIER))
        return false;

    const char* ptr = devices;

    devicesVec.clear();

    do
    {
        devicesVec.push_back(std::string(ptr));
        ptr += devicesVec.back().size() + 1;
    }
    while(*(ptr + 1) != '\0');

    return true;
}

#include "openal_device.h"
#include "openal_context.h"
#include "openal_listener.h"
#include "openal_buffer.h"
#include "openal_source.h"

const std::size_t NUM_BUFFERS = 4;
const std::size_t BUFFER_SIZE = 65536; // 32kb of data in each buffer

void update_stream(openal::Source& source,
                   openal::Format format,
                   const std::int32_t& sampleRate,
                   const std::vector<char>& soundData,
                   std::size_t& cursor)
{
    ALint buffersProcessed = source.get_buffers_processed().value();

    if(buffersProcessed <= 0) {
        return;
    }

    while(buffersProcessed--)
    {
        openal::BufferHandle buffer_handle = source.unqueue_buffers(1).value()[0];

        ALsizei dataSize = BUFFER_SIZE;

        char* data = new char[dataSize];
        std::memset(data, 0, dataSize);

        std::size_t dataSizeToCopy = BUFFER_SIZE;
        if(cursor + BUFFER_SIZE > soundData.size()) {
            dataSizeToCopy = soundData.size() - cursor;
        }

        std::memcpy(&data[0], &soundData[cursor], dataSizeToCopy);
        cursor += dataSizeToCopy;

        if(dataSizeToCopy < BUFFER_SIZE){
            cursor = 0;
            std::memcpy(&data[dataSizeToCopy], &soundData[cursor], BUFFER_SIZE - dataSizeToCopy);
            cursor = BUFFER_SIZE - dataSizeToCopy;
        }
        buffer_handle.set_data(format, std::as_bytes(std::span(data, BUFFER_SIZE)), sampleRate).value();
        std::vector<openal::BufferHandle> buffers = {buffer_handle};
        source.queue_buffers(buffers).value();
        delete[] data;
    }
}



int main(){

    std::array x = {1,2,3};
    auto device_name_list = openal::get_device_name_list();
    for(auto device_name : device_name_list){
        fmt::print("device_name : {} \n", device_name);
    }
    auto capture_device_name_list = openal::get_capture_device_name_list();
    for(auto device_name : capture_device_name_list){
        fmt::print("capture device_name : {} \n", device_name);
    }
    fmt::print("default device_name : {} \n", openal::get_default_device_name());
    fmt::print("default capture device_name : {} \n", openal::get_default_capture_device_name());

    auto device = openal::Device::from_default().value();

    auto context = device.create_context({}).value();

    UUL_ASSERT(context.set_current_thread_context().value());

    std::uint8_t channels;
    std::int32_t sampleRate;
    std::uint8_t bitsPerSample;
    std::vector<char> soundData;
//    if(!load_wav("C:\\Users\\Bolt\\Documents\\GitRepositories\\VulkanWorkspace\\openaltest\\sounds\\iamtheprotectorofthissystem.wav", channels, sampleRate, bitsPerSample, soundData))
//    {
//        std::cerr << "ERROR: Could not load wav" << std::endl;
//        return 0;
//    }
    if(!load_wav("C:\\Users\\Bolt\\Documents\\GitRepositories\\VulkanWorkspace\\openaltest\\sounds\\407640__drotzruhn__countdown-30-seconds2.wav", channels, sampleRate, bitsPerSample, soundData))
    {
        std::cerr << "ERROR: Could not load wav" << std::endl;
        return 0;
    }

//    auto buffer = openal::Buffer::from_data(openal::make_format(channels, bitsPerSample), std::as_bytes(std::span(soundData)), sampleRate).value();

    auto format =openal::make_format(channels, bitsPerSample);
    auto buffers = openal::Buffer::from_data_many(format, std::as_bytes(std::span(soundData)), sampleRate, NUM_BUFFERS,
                                                  BUFFER_SIZE).value();
    auto source = openal::Source::generate().value();

    source.set_pitch(1.0f).value();
    source.set_gain(1.0f).value();
    source.set_position(glm::vec3(0.0)).value();
    source.set_velocity(glm::vec3(0.0)).value();
    source.set_looping(false).value();
//    source.set_buffer(buffer);

    source.queue_buffers(buffers).value();
    source.play().value();

    auto state = openal::Source::State::Playing;
    std::size_t cursor = BUFFER_SIZE * NUM_BUFFERS;

    while(state == openal::Source::State::Playing){
        update_stream(source, format, sampleRate, soundData, cursor);
        state = source.get_source_state().value();
    }


    return 0;
}

//int main2(){
//
//    std::array x = {1,2,3};
//    auto device_name_list = openal::get_device_name_list();
//    for(auto device_name : device_name_list){
//        fmt::print("device_name : {} \n", device_name);
//    }
//    auto capture_device_name_list = openal::get_capture_device_name_list();
//    for(auto device_name : capture_device_name_list){
//        fmt::print("capture device_name : {} \n", device_name);
//    }
//    fmt::print("default device_name : {} \n", openal::get_default_device_name());
//    fmt::print("default capture device_name : {} \n", openal::get_default_capture_device_name());
//
//    auto device = openal::Device::from_default().value();
//
//    auto context = device.create_context({}).value();
//
//    UUL_ASSERT(context.make_context_thread_current().value());
//
//    std::uint8_t channels;
//    std::int32_t sampleRate;
//    std::uint8_t bitsPerSample;
//    std::vector<char> soundData;
//    if(!load_wav("C:\\Users\\Bolt\\Documents\\GitRepositories\\VulkanWorkspace\\openaltest\\sounds\\iamtheprotectorofthissystem.wav", channels, sampleRate, bitsPerSample, soundData))
//    {
//        std::cerr << "ERROR: Could not load wav" << std::endl;
//        return 0;
//    }
////    if(!load_wav("C:\\Users\\Bolt\\Documents\\GitRepositories\\VulkanWorkspace\\openaltest\\sounds\\407640__drotzruhn__countdown-30-seconds2.wav", channels, sampleRate, bitsPerSample, soundData))
////    {
////        std::cerr << "ERROR: Could not load wav" << std::endl;
////        return 0;
////    }
//
//    auto buffer = openal::Buffer::from_data(openal::make_format(channels, bitsPerSample), std::as_bytes(std::span(soundData)), sampleRate).value();
//
//    auto format =openal::make_format(channels, bitsPerSample);
////    auto buffers =  openal::Buffer::from_data_many(format, std::as_bytes(std::span(soundData)), sampleRate, NUM_BUFFERS, BUFFER_SIZE).value();
//    auto source = openal::Source::generate().value();
//
//    source.set_pitch(1.0f);
//    source.set_gain(1.0f);
//    source.set_position(glm::vec3(0.0));
//    source.set_velocity(glm::vec3(0.0));
//    source.set_looping(false);
//    source.set_buffer(buffer);
//
//
////    source.queue_buffers(buffers).value();
//    source.play().value();
//
//    auto state = openal::Source::State::Playing;
//    std::size_t cursor = BUFFER_SIZE * NUM_BUFFERS;
//
//    while(state == openal::Source::State::Playing){
////        update_stream(source, format, sampleRate, soundData, cursor);
//        state = source.get_source_state().value();
//    }
//
//
//    return 0;
//}