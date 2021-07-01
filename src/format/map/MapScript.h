#pragma once

#include <inttypes.h>

namespace geck {
// Scripts
struct MapScript {
    uint32_t pid;
    uint32_t next_script; // unused
    uint32_t timer; // only if PID = 1 or 2
    uint32_t spatial_radius; // only if PID = 1
    uint32_t flags;
    uint32_t script_id;
    uint32_t unknown5;
    uint32_t script_oid;
    uint32_t local_var_offset;
    uint32_t local_var_count;
    uint32_t unknown9;
    uint32_t unknown10;
    uint32_t unknown11;
    uint32_t unknown12;
    uint32_t unknown13;
    uint32_t unknown14;
    uint32_t unknown15;
    uint32_t unknown16;
};

}  // namespace geck
