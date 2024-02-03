#ifndef TILE_UTILS
#define TILE_UTILS
const uint TILE_INVALID = 0xFFFFFFFFu;
const uint TILE_EMPTY = 0xFFFFFFFFu;
const uint TILE_UPDATED_BIT = 0x80000000u;
const uint TILE_IDX_NEW_BIT = 0x80000000u;
const uint ALL_INVOCATIONS_PROCESSED = 0xFFFFFFFFu;


const uint TILE_DATA_WIDTH = 32u;
const uint TILE_DATA_HEIGHT = 32u;
const uint TILE_DATA_SIZE = TILE_DATA_WIDTH*TILE_DATA_HEIGHT;

const uint TILE_ARENA_TILE_SIZE = 256u * 256u;
const uint TILE_ARENA_POINTER_LIST_SIZE = 256u * 128u;

const uint TILE_ARENA_BITS = 0x7FFF0000u;
const uint ARENA_TILE_BITS = 0x0000FFFFu;

layout(buffer_reference, buffer_reference_align=4, scalar) buffer TileArena_ref {
    u8vec8 data;
};

layout(buffer_reference, buffer_reference_align= 8, scalar) buffer TileArenaPointerList_ref {
    TileArena_ref data;
};

//TODO need to figure out where to put this, pack with tile arena? Or put in it's own thing?
layout(buffer_reference, buffer_reference_align= 4, scalar) buffer TileHasValueBitArray_ref {
    uint data;
};

layout(buffer_reference, buffer_reference_align= 8, scalar) buffer TileHasValueBitArrayList_ref {
    TileHasValueBitArray_ref data;
};

struct ArenaMetaData{
    uint tile_count;
};

layout(buffer_reference, buffer_reference_align= 4, scalar) buffer TileArenaMetadata_ref {
    ArenaMetaData data;
};




struct TileMemoryData{
    TileArenaPointerList_ref area_pointers;
    TileHasValueBitArrayList_ref arena_valid_tile_bitmasks;
    TileArenaMetadata_ref arena_meta_data;
};

layout(buffer_reference, buffer_reference_align= 8, scalar) TileMemoryData_ref{
    TileMemoryData data;
};


layout(buffer_reference, buffer_reference_align= 8, scalar) buffer BrushSettings_ref {
    BrushSettings data;
};


layout(buffer_reference, buffer_reference_align= 8, scalar) buffer TileMap_ref {
    uint data;
};

struct CounterData{
    uint latest_avilible_tile_idx;
    uint new_tile_count;
    uint updated_tile_count;
};


layout(buffer_reference, buffer_reference_align = 16, scalar) buffer CounterData_ref {
    CounterData data;
};

layout(buffer_reference, buffer_reference_align= 4, scalar) buffer TileIndex_ref {
    uint data;
};

#endif //TILE_UTILS