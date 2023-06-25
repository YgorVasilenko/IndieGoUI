# Example application for IndieGo UI
Refer to [top-level readme](../README.md) for instructions to build and run

# UI Embedding guide
Refer to [example CMakeLists.txt](CMakeLists.txt) to get idea of how to build your project with UI.

1. Include header in your source files:
```C++
#include <IndieGoUI.h>
```

2. Add source following files to to your project compilation:

- ```IndieGoUI.cpp```
- ```IndieGoUI.pb.cc```
- ```backends\Nuklear\renderer.cpp```

F.e. using cmake:
```cmake
add_executable(
    My_porject
    src/my_source.cpp
    IndieGoUI.cpp
    IndieGoUI.pb.cc
    backends/Nuklear/renderer.cpp
)
```

3. Make sure overall includes contain IndieGoUI root and backends\Nuklear:

```cmake
target_include_directories(
    My_porject
    ${ENV:MY_INCLUDES}
    ${ENV:INDIEGO_UI_ROOT}
    ${ENV:INDIEGO_UI_ROOT}/backends/Nuklear
)
```

4. Link to supplied protobuf (or skip this and see 4.1, if you already have some protobuf version in your project):

```cmake
target_link_libraries(
    My_porject
    PUBLIC "My/library"

    PUBLIC ${ENV:INDIEGO_UI_ROOT}/protobuf/build/Win/libprotobufd.lib
)

```

4.1. If you already using some protobuf version<br>
You should re-generate ```IndieGoUI.pb.cc``` and ```IndieGoUI.pb.h``` with your ```protoc``` version!

## What's next
Refer to [example cpp](src/app.cpp) to get idea of what code should be written for integration! Check comments in code.