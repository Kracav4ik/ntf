#pragma once

#include "Lines.h"
#include "Popup.h"

#include <windows.h>

class Screen;

class RemoveDirPopup : Popup {
public:
    RemoveDirPopup(SHORT w, SHORT h);

    void show(const std::wstring& dir);
    void registerKeys(Screen& screen);
    void drawOn(Screen& screen);

private:
    SHORT w;
    SHORT h;
    Lines text;
    std::wstring toDelete;
};
