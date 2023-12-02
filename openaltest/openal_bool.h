//
// Created by Bolt on 10/20/2023.
//

#ifndef VULKANWORKSPACE_OPENAL_BOOL_H
#define VULKANWORKSPACE_OPENAL_BOOL_H
#include <AL/al.h>
namespace openal{
    class Bool{
    public:
        constexpr Bool() noexcept {};
        constexpr Bool(bool value) noexcept: m_value(value ? AL_TRUE : AL_FALSE){};
        constexpr explicit Bool(ALuint value) noexcept : m_value(value){};
        constexpr explicit Bool(ALint value) noexcept : m_value(value){};
        [[nodiscard]]
        constexpr explicit operator bool() const noexcept{
            return (m_value == AL_TRUE);
        };
        [[nodiscard]]
        constexpr explicit operator ALuint() const noexcept{
            return m_value; 
        }
        [[nodiscard]]
        constexpr explicit operator ALint() const noexcept{
            return static_cast<ALint>(m_value); 
        }
        [[nodiscard]]
        constexpr const ALuint& get() const{
            return m_value;
        }
        [[nodiscard]]
        constexpr ALuint& get(){
            return m_value;
        }
        constexpr Bool& operator =(bool value) noexcept{
            *this = Bool(value);
            return *this; 
        }
    private:
        ALuint m_value = AL_FALSE;
    };

    [[nodiscard]]
    constexpr bool operator == (Bool a, Bool b) noexcept{
        return static_cast<bool>(a) == static_cast<bool>(b);
    }
    [[nodiscard]]
    constexpr bool operator != (Bool a, Bool b) noexcept{
        return static_cast<bool>(a) != static_cast<bool>(b);
    }

    [[nodiscard]]
    constexpr bool operator == (bool a, Bool b) noexcept{
        return a == static_cast<bool>(b);
    }
    [[nodiscard]]
    constexpr bool operator != (bool a, Bool b) noexcept{
        return a != static_cast<bool>(b);
    }

    [[nodiscard]]
    constexpr bool operator == (Bool a, bool b){
        return static_cast<bool>(a) == b;
    }
    [[nodiscard]]
    constexpr bool operator != (Bool a, bool b){
        return static_cast<bool>(a) != b;
    }
    [[nodiscard]]
    constexpr Bool operator !(Bool a) noexcept{
        return (a == true) ? Bool(AL_FALSE) : Bool(AL_TRUE);
    }
    [[nodiscard]]
    constexpr Bool operator &&(Bool a, Bool b) noexcept{
        return Bool{static_cast<bool>(a) && static_cast<bool>(b)};
    }
    [[nodiscard]]
    constexpr Bool operator ||(Bool a, Bool b) noexcept{
        return Bool{static_cast<bool>(a) || static_cast<bool>(b)};
    }

    [[nodiscard]]
    constexpr bool operator &&(Bool a, bool b) noexcept{
        return static_cast<bool>(a) && b;
    }
    [[nodiscard]]
    constexpr bool operator ||(Bool a, bool b) noexcept{
        return static_cast<bool>(a) || b;
    }
    [[nodiscard]]
    constexpr bool operator &&(bool a, Bool b) noexcept{
        return a && static_cast<bool>(b);
    }
    [[nodiscard]]
    constexpr bool operator ||(bool a, Bool b) noexcept{
        return a || static_cast<bool>(b);
    }

}
#endif //VULKANWORKSPACE_OPENAL_BOOL_H
