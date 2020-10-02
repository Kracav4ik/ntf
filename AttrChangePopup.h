#pragma once

#include "Lines.h"
#include "Popup.h"

#include <windows.h>

#include <string>

class Screen;

class AttrChangePopup : Popup {
public:
    AttrChangePopup(SHORT w, SHORT h);

    void show(const std::wstring& root, const std::wstring& name);
    void registerKeys(Screen& screen);
    void drawOn(Screen& screen);

private:
    void updateAttr();

    SHORT w;
    SHORT h;
    std::wstring root;
    std::wstring name;
    DWORD oldAttr = 0;
    DWORD newAttr = 0;
    Lines attrText;
    FILETIME creationTime;
    FILETIME accessTime;
    FILETIME modifyTime;
    bool badAttr = false;
    bool badTime = false;
};
