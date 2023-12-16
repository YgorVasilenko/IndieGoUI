Use any Immediate-mode gui - see [Nuklear backend](https://github.com/YgorVasilenko/IndieGoUI/blob/main/backends/Nuklear/renderer.cpp), [App-side inclulde](https://github.com/YgorVasilenko/IndieGoUI/blob/main/IndieGoUI.h) for what required to be implemented.<br>

## Support for arbitrary Immediate-mode GUI libriary
All rendering should be provided by user. F.e. one could have ```renderer.cpp`` module for specific imgui backend. This module should include [IndieGoUI.h](https://github.com/YgorVasilenko/IndieGoUI/blob/main/IndieGoUI.h), provide (implement) several functions.<br>
List of what should be implemented goes like so:<br>
1. All virtual methods of ```UI::WIDGET``` [struct](https://github.com/YgorVasilenko/IndieGoUI/blob/main/IndieGoUI.h#L569):
```C++
virtual void callImmediateBackend(UI_elements_map & UIMap);
virtual void allocateRow(unsigned int cols, float min_height, bool in_pixels);
virtual void endRow();
```
2. Following methods of ```UI::UI_element``` [struct](https://github.com/YgorVasilenko/IndieGoUI/blob/main/IndieGoUI.h#L227):
```C++
virtual void callUIfunction(float x, float y, float widget_w, float widget_h);
virtual void initImage(unsigned int texID, unsigned int w, unsigned int h, region<float> crop);
virtual void useSkinImage(
	unsigned int texID,
	unsigned short w,
	unsigned short h,
	region<float> crop,
	IMAGE_SKIN_ELEMENT elt
);
virtual region<float> getImgCrop(IMAGE_SKIN_ELEMENT elt);
```
3. Following methods of ```UI::Manager``` [struct](https://github.com/YgorVasilenko/IndieGoUI/blob/main/IndieGoUI.h#L703):
```C++
void loadFont(std::string path, const std::string & winID, float font_size = 16.f, bool useProjectDir = false, bool cutProjDirFromPath = true);
void init(std::string winID, void * initData = NULL);
void addWindow(std::string winID, void * initData = NULL);
void removeWindow(std::string winID, void * initData = NULL);
void drawFrameStart(std::string & winID);
void drawFrameEnd(std::string & winID);
static void addImage(unsigned int texID,unsigned short w, unsigned short h, region<float> crop);
```

Where to store data from ```init*()```, ```use*()``` and ```load*()``` methods - user's decision.

## Runtime elements addition and modification
Proper docs - TODO :)<br>
Check [ui_editor](https://github.com/YgorVasilenko/IndieGoUI/blob/main/ui_editor/src/init.cpp)'s ```init.cpp``` module, [test_app](https://github.com/YgorVasilenko/IndieGoUI/blob/main/test_app/src/app.cpp) and [IndieGoUI.h](https://github.com/YgorVasilenko/IndieGoUI/blob/main/IndieGoUI.h) header to get idea of what could be done.<br>