#ifndef SUPERHEXMOD_HPP
#define SUPERHEXMOD_HPP

#include "program_modification.hpp"

namespace superhexmod {

inline common::ProgramModification steam_linux64_8838351(){
    using namespace common;

    ProgramModification result;
    // set starting stage selected to 4
    result.add_patch(make_addr(0x46e942), uint32_t{0}, uint32_t{4});
    // set the theme of hexagonest to last stage in stage select
    // to indicate it's altered
    result.add_patch(make_addr(0x4723f0), uint32_t{0xa}, uint32_t{0x1e});
    // change the value in the check before the call to new value as well
    // so the game isn't bombarded with graphicsclass::setpal calls
    result.add_patch(make_addr(0x4723de), uint8_t{0xa}, uint8_t{0x1e});
    // set theme for hypermode to last stage when (re)starting
    // it's called once on stage start (i think?) so it doesn't matter
    // that the check still looks for value 9
    result.add_patch(make_addr(0x46d67c), uint32_t{9}, uint32_t{0x1e});
    // set music playing when starting the stage to last stage
    result.add_patch(make_addr(0x46d625), uint32_t{3}, uint32_t{4});
    // start game as if it's hypermode
    result.add_patch(make_addr(0x46d663), { 0x0f, 0x84, 0x7f, 0x1b, 0x00, 0x00 }, { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });
    // always add a minute in the middle of gamelogic
    // so that the game thinks we are in hypermode
    result.add_patch(make_addr(0x47115e), { 0x74, 0x18 }, { 0x90, 0x90 });
    // always substract a minute at the end of gamelogic
    // so that the game doesn't add a minute to our
    // playtime every time frame
    result.add_patch(make_addr(0x4712e1), { 0x74, 0x18 }, { 0x90, 0x90 });
    // this is the only data modification in the entire modifications set
    // instead of a minute, substract / add 133.(3) seconds, this is the
    // point after which the game does the spin thing every 5 waves and the
    // last point after which something changes in game logic, it's just easier
    // than doing all the other modifications
    result.add_patch(make_addr(0x55a290), bit_cast<uint32_t>(3600.f), bit_cast<uint32_t>(8000.f), true);
    // set initial value added to camera_rotation_type to 8
    // inital one for hypermode is 4, its value then makes the hexagon spin and
    // change direction from time to time, it's set in gameinput and then changed
    // in gamelogic, although some time will pass until it's changed in gamelogic
    // meaning it would spin in some direction for some time were this modification
    // not there, values of camera_rotation_type higher than 7 lock rotation in place
    // the same way you see in the last stage
    result.add_patch(make_addr(0x46d6a3), uint8_t{4}, uint8_t{8});
    // always draw black text for strings:
    //  - PRESS SPACE TO START
    //  - TAB - HIGH SCORES
    //  - ESC - RETURN TO MENU
    // in stage select
    // the first one checks whether stage selected is 1 (hyper hexagonest)
    // the second one checks whether hyper hexagonest has been unlocked
    result.add_patch(make_addr(0x450cfa), { 0x75, 0x0d }, { 0x90, 0x90 });
    result.add_patch(make_addr(0x450d03), { 0x0f, 0x84 }, { 0x48, 0xe9 });
    // always draw black text for strings:
    //  - TAB - HIGH SCORES
    //  - ESC - STAGE SELECT
    // in the score screen after a game ends
    // the first one checks whether stage current is 2 (hexagonest or hyper hexagonest)
    // the second one checks whether hypermode_switch is set to 1
    result.add_patch(make_addr(0x44fd8b), { 0x75, 0x0d }, { 0x90, 0x90 });
    result.add_patch(make_addr(0x44fd94), { 0x0f, 0x85 }, { 0x48, 0xe9 });
    #ifdef SUPERHEX_INVULN
        result.add_patch(make_addr(0x471014), uint8_t{0x7e}, uint8_t{0xeb});
    #endif
    return result;
}

inline common::ProgramModification steam_windows32_8838351(){
    using namespace common;

    ProgramModification result;
    // always have hexagonest when entering stage select
    result.add_patch(make_addr(0x425654), uint32_t{0}, uint32_t{4});
    // last stage theme on hexagonest in stage select
    result.add_patch(make_addr(0x4292db), uint8_t{0xa}, uint8_t{0x1e});
    // prevent redundant setpal calls
    result.add_patch(make_addr(0x4292d5), uint8_t{0xa}, uint8_t{0x1e});
    // set theme at start and restart
    result.add_patch(make_addr(0x427304), uint8_t{9}, uint8_t{0x1e});
    // set music to last stage
    result.add_patch(make_addr(0x4272b5), uint8_t{3}, uint8_t{4});
    // start in hypermode eveytime
    result.add_patch(make_addr(0x4272f6), { 0x74, 0x67 }, { 0x90, 0x90 });
    // always add minute
    result.add_patch(make_addr(0x42a0c5), { 0x74, 0x18 }, { 0x90, 0x90 });
    // always sub minute
    result.add_patch(make_addr(0x42cd95), { 0x74, 0x18 }, { 0x90, 0x90 });
    // add/sub 133.(3) seconds instead
    result.add_patch(make_addr(0x4f1df0), bit_cast<uint32_t>(3600.f), bit_cast<uint32_t>(8000.f), true);
    // fix starting rotation
    result.add_patch(make_addr(0x42732c), uint8_t{4}, uint8_t{8});
    // make text in score screen black
    result.add_patch(make_addr(0x40c7cd), { 0x0f, 0x85, 0xb3, 0x00, 0x00, 0x00 }, { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });
    result.add_patch(make_addr(0x40c7da), { 0x0f, 0x84, 0xa6, 0x00, 0x00, 0x00 }, { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });
    // make text in stage select black
    result.add_patch(make_addr(0x40e567), { 0x0f, 0x85, 0xb5, 0x00, 0x00, 0x00 }, { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });
    result.add_patch(make_addr(0x40e574), { 0x0f, 0x85, 0xa8, 0x00, 0x00, 0x00 }, { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });
    #ifdef SUPERHEX_INVULN
        result.add_patch(make_addr(0x429a0c), { 0x0f, 0x8e, 0xfd, 0x00, 0x00, 0x00 }, { 0xe9, 0xfe, 0x00, 0x00, 0x00, 0x90 });
    #endif
    return result;
}

} //superhexmod

#endif //SUPERHEXMOD_HPP
