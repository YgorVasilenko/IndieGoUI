# UI map structure. Allows getting data from Immediate-mode backend and store it w/o bothering with callbacks.

## Building example app:

1. Make shure cmake available in PATH:

cmd:
```
set "PATH=C:\Program Files\CMake\bin;%PATH%"
```
powershell:
```
$env:Path="C:\Program Files\CMake\bin;$env:Path"
```

2. Build:

```
cd example_app
cmake -Bbuild
cmake --build build
```

## Running:
```
build\Debug\UI_map_OpenGL.exe
```