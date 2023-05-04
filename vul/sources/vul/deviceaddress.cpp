//
// Created by Bolt on 5/4/2023.
//

#include "deviceaddress.h"

vul::DeviceAddress::DeviceAddress(VkDeviceAddress address) : m_address(address){

}

VkDeviceAddress &vul::DeviceAddress::get() {
    return m_address;
}

const VkDeviceAddress &vul::DeviceAddress::get() const {
    return m_address;
}

vul::DeviceAddress::operator VkDeviceAddress() const {
    return get();
}

vul::DeviceAddress &vul::DeviceAddress::operator++() {
    ++m_address;
    return *this;
}

vul::DeviceAddress vul::DeviceAddress::operator++(int) {

    auto temp = *this;
    ++m_address;
    return temp;
}

vul::DeviceAddress &vul::DeviceAddress::operator--() {
    --m_address;
    return *this;
}

vul::DeviceAddress vul::DeviceAddress::operator--(int) {
    auto temp = *this;
    --m_address;
    return temp;
}

vul::DeviceAddress &vul::DeviceAddress::operator=(VkDeviceAddress address) {
    m_address = address;
    return *this;
}
