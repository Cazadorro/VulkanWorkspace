//
// Created by Bolt on 10/18/2023.
//

#include "openal_util.h"
#include "openal_bool.h"
#include "uul/assert.h"
#include <glm/gtc/type_ptr.hpp>
#include <fmt/core.h>
#include <range/v3/view/enumerate.hpp>
#include <array>

namespace openal {


    std::vector<std::string> get_device_name_list() {
        char null_char = '\0';
        const ALchar *device_names_seperated_by_null = alcGetString(nullptr,
                                                                    static_cast<ALenum>(AlcStringAttribute::DeviceSpecifier));

        auto alc_get_string_error = alcGetError(nullptr);
        UUL_ASSERT(alc_get_string_error == 0,
                   fmt::format("Expected no errors when querying for devices, found error # {}",
                               alc_get_string_error).c_str());
        std::vector<std::string> device_name_list;


        if (*device_names_seperated_by_null == null_char) {
            return {};
        }
        // assumes list is not empty, already returned if it was, each string seperated by null character, end reached when
        // only null char left.
        do {
            device_name_list.emplace_back(device_names_seperated_by_null);
            device_names_seperated_by_null += device_name_list.back().size() + 1;
        } while (*(device_names_seperated_by_null) != null_char);
        return device_name_list;
    }

    std::string get_default_device_name() {
        const ALchar *default_device_name_cstring = alcGetString(nullptr,
                                                                 static_cast<ALenum>(AlcStringAttribute::DefaultDeviceSpecifier));
        auto alc_get_string_error = alcGetError(nullptr);
        UUL_ASSERT(alc_get_string_error == 0,
                   fmt::format("Expected no errors when querying for default device, found error # {}",
                               alc_get_string_error).c_str());
        return {default_device_name_cstring};
    }

    std::vector<std::string> get_capture_device_name_list() {
        char null_char = '\0';
        const ALchar *device_names_seperated_by_null = alcGetString(nullptr,
                                                                    static_cast<ALenum>(AlcStringAttribute::CaptureDeviceSpecifier));

        auto alc_get_string_error = alcGetError(nullptr);
        UUL_ASSERT(alc_get_string_error == 0,
                   fmt::format("Expected no errors when querying for capture devices, found error # {}",
                               alc_get_string_error).c_str());
        std::vector<std::string> device_name_list;
        if (*device_names_seperated_by_null == null_char) {
            return {};
        }
        // assumes list is not empty, already returned if it was, each string seperated by null character, end reached when
        // only null char left.
        do {
            device_name_list.emplace_back(device_names_seperated_by_null);
            device_names_seperated_by_null += device_name_list.back().size() + 1;
        } while (*(device_names_seperated_by_null) != null_char);
        return device_name_list;
    }

    std::string get_default_capture_device_name() {
        const ALchar *default_device_name_cstring = alcGetString(nullptr,
                                                                 static_cast<ALenum>(AlcStringAttribute::CaptureDefaultDeviceSpecifier));
        auto alc_get_string_error = alcGetError(nullptr);
        UUL_ASSERT(alc_get_string_error == 0,
                   fmt::format("Expected no errors when querying for default capture device, found error # {}",
                               alc_get_string_error).c_str());
        return {default_device_name_cstring};
    }

    std::string to_string(AlcErrorState error_state) {
        switch (error_state) {
            case AlcErrorState::NoError:
                return "NoError";
            case AlcErrorState::InvalidDevice:
                return "InvalidDevice";
            case AlcErrorState::InvalidContext:
                return "InvalidContext";
            case AlcErrorState::InvalidEnum:
                return "InvalidEnum";
            case AlcErrorState::InvalidValue:
                return "InvalidValue";
            case AlcErrorState::OutOfMemory:
                return "OutOfMemory";
            default:
                return "Unknown";
        }
    }

    std::string to_string(AlErrorState error_state) {
        switch (error_state) {
            case AlErrorState::NoError:
                return "NoError";
            case AlErrorState::InvalidName:
                return "InvalidName";
            case AlErrorState::InvalidEnum:
                return "InvalidEnum";
            case AlErrorState::InvalidValue:
                return "InvalidValue";
            case AlErrorState::InvalidOperation:
                return "InvalidOperation";
            case AlErrorState::OutOfMemory:
                return "OutOfMemory";
            default:
                return "Unknown";
        }
    }

    openal::Format make_format(std::uint8_t channel_count, std::uint8_t bits_per_sample) {
        UUL_ASSERT(channel_count >= 1 && channel_count <= 2 && (bits_per_sample == 8 || bits_per_sample == 16),
                   fmt::format("Invalid channel count {} and bits per sample combination {}", channel_count,
                               bits_per_sample).c_str());
        if (channel_count == 1 && bits_per_sample == 8) {
            return Format::Mono8;
        }
        if (channel_count == 1 && bits_per_sample == 16) {
            return Format::Mono16;
        }
        if (channel_count == 2 && bits_per_sample == 8) {
            return Format::Stereo8;
        }
        if (channel_count == 2 && bits_per_sample == 16) {
            return Format::Stereo16;
        }
    }
}





