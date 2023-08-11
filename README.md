# Super Hexagon always start at last stage mod

This mod makes it so that when you start the hexagonest stage the stage you see
after surviving for 60 seconds in hyper hexagonest (last stage) is launched instead.

# `WARNING`

I've had my game records reset several times during the mod testing. Provided, I've
experienced a lot of segmentation faults which means I could have corrupted the
memory of the process which could've been the reason behind said resets, I still
highly recommend backing up all game data before trying this mod. See README in
[SuperHexagonRE](https://github.com/sumguytho/SuperHexagonRE)
repository for an example of why no modification to this game can be trusted
to not break anything (section "How comprehensive are the modifications?").

# What does it do exactly?

The last_stage_mod(.exe) binary searches for SuperHexagon on Unix and SuperHexagon.exe on Windows
in the current directory, launches it and modifies the launched copy. The original binary remains
the same which means you can switch between the usual version and the mod seamlessly.

That said, the modified version uses the same game data as the original, hence the data
loss warning above.

# Installing

Just drop the last_stage_mod(.exe) binary to the Super Hexagon installation directory and run it. It doesn't
require admin privileges or anything.

# Building

For Windows:

```
cmake -B build -G "Visual Studio 16 2019" -T host=x86 -A win32
cmake --build build --config Release --target ALL_BUILD
```

For Unix:

```
cmake -B build -DCMAKE_BUILD_TYPE:STRING=Release -DCMAKE_C_COMPILER:FILEPATH=/usr/lib64/ccache/gcc -DCMAKE_CXX_COMPILER:FILEPATH=/usr/lib64/ccache/g++ -G Ninja
cmake --build build --config Release --target all
```

Build system agnostic version would be:

```
cmake -B build
cmake --build build --config Release
```

There is also CMakeTools for VS Code that will do those for you (those commands
are just straight up what CMakeTools says it does, btw).

The location of resulting binaries depends on compiler used.

**Build requirements:**

 - CMake 3.25 (perhaps, earlier versions will do too)
 - any C++ compiler with c++17 support
