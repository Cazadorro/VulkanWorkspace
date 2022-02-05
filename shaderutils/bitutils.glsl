#ifndef BITUTILS_GLSL
#define BITUTILS_GLSL

uint8_t set_bit(uint8_t value, uint8_t index){
    return value | (uint8_t(1) << index);
}

uint8_t clear_bit(uint8_t value, uint8_t index){
    return value & ~(uint8_t(1) << index);
}

uint8_t toggle_bit(uint8_t value, uint8_t index){
    return value ^ (uint8_t(1) << index);
}

bool get_bit(uint8_t value, uint8_t index){
    return bool((value >> index) & uint8_t(1));
}

uint8_t change_bit(uint8_t value, uint8_t index, bool x){
    return (value & ~(uint8_t(1) << index)) | (uint8_t(x) << index);
}

uint16_t set_bit(uint16_t value, uint16_t index){
    return value | (uint8_t(1) << index);
}

uint16_t clear_bit(uint16_t value, uint16_t index){
    return value & ~(uint16_t(1) << index);
}

uint16_t toggle_bit(uint16_t value, uint16_t index){
    return value ^ (uint16_t(1) << index);
}

bool get_bit(uint16_t value, uint16_t index){
    return bool((value >> index) & uint16_t(1));
}

uint16_t change_bit(uint16_t value, uint16_t index, bool x){
    return (value & ~(uint16_t(1) << index)) | (uint16_t(x) << index);
}


uint32_t set_bit(uint32_t value, uint32_t index){
    return value | (uint32_t(1) << index);
}

uint32_t clear_bit(uint32_t value, uint32_t index){
    return value & ~(uint32_t(1) << index);
}

uint32_t toggle_bit(uint32_t value, uint32_t index){
    return value ^ (uint32_t(1) << index);
}

bool get_bit(uint32_t value, uint32_t index){
    return bool((value >> index) & uint32_t(1));
}

uint32_t change_bit(uint32_t value, uint32_t index, bool x){
    return (value & ~(uint32_t(1) << index)) | (uint32_t(x) << index);
}


uint64_t set_bit(uint64_t value, uint64_t index){
    return value | (uint64_t(1) << index);
}

uint64_t clear_bit(uint64_t value, uint64_t index){
    return value & ~(uint64_t(1) << index);
}

uint64_t toggle_bit(uint64_t value, uint64_t index){
    return value ^ (uint64_t(1) << index);
}

bool get_bit(uint64_t value, uint64_t index){
    return bool((value >> index) & uint64_t(1));
}

uint64_t change_bit(uint64_t value, uint64_t index, bool x){
    return (value & ~(uint64_t(1) << index)) | (uint64_t(x) << index);
}

int8_t set_bit(int8_t value, int8_t index){
    return int8_t(set_bit(uint8_t(value), uint8_t(index)));
}

int8_t clear_bit(int8_t value, int8_t index){
    return int8_t(clear_bit(uint8_t(value), uint8_t(index)));
}

int8_t toggle_bit(int8_t value, int8_t index){
    return int8_t(toggle_bit(uint8_t(value), uint8_t(index)));
}

bool get_bit(int8_t value, int8_t index){
    return get_bit(uint8_t(value), uint8_t(index));
}

int8_t change_bit(int8_t value, int8_t index, bool x){
    return int8_t(change_bit(uint8_t(value), uint8_t(index), x));
}

int16_t set_bit(int16_t value, int16_t index){
    return int16_t(set_bit(uint16_t(value), uint16_t(index)));
}

int16_t clear_bit(int16_t value, int16_t index){
    return int16_t(clear_bit(uint16_t(value), uint16_t(index)));
}

int16_t toggle_bit(int16_t value, int16_t index){
    return int16_t(toggle_bit(uint16_t(value), uint16_t(index)));
}

bool get_bit(int16_t value, int16_t index){
    return get_bit(uint16_t(value), uint16_t(index));
}

int16_t change_bit(int16_t value, int16_t index, bool x){
    return int16_t(change_bit(uint16_t(value), uint16_t(index), x));
}



int32_t set_bit(int32_t value, int32_t index){
    return int32_t(set_bit(uint32_t(value), uint32_t(index)));
}

int32_t clear_bit(int32_t value, int32_t index){
    return int32_t(clear_bit(uint32_t(value), uint32_t(index)));
}

int32_t toggle_bit(int32_t value, int32_t index){
    return int32_t(toggle_bit(uint32_t(value), uint32_t(index)));
}

bool get_bit(int32_t value, int32_t index){
    return get_bit(uint32_t(value), uint32_t(index));
}

int32_t change_bit(int32_t value, int32_t index, bool x){
    return int32_t(change_bit(uint32_t(value), uint32_t(index), x));
}


int64_t set_bit(int64_t value, int64_t index){
    return int64_t(set_bit(uint64_t(value), uint64_t(index)));
}

int64_t clear_bit(int64_t value, int64_t index){
    return int64_t(clear_bit(uint64_t(value), uint64_t(index)));
}

int64_t toggle_bit(int64_t value, int64_t index){
    return int64_t(toggle_bit(uint64_t(value), uint64_t(index)));
}

bool get_bit(int64_t value, int64_t index){
    return get_bit(uint64_t(value), uint64_t(index));
}

int64_t change_bit(int64_t value, int64_t index, bool x){
    return int64_t(change_bit(uint64_t(value), uint64_t(index), x));
}
    

#endif //BITUTILS_GLSL
