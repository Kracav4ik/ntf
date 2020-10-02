#pragma once

#include "Lines.h"
#include "Popup.h"

class Screen;

class MakeDirPopup : Popup {
public:
    MakeDirPopup(SHORT w, SHORT h);

    void show(const std::wstring& root);
    void registerKeys(Screen& screen);
    void drawOn(Screen& screen);

private:
    SHORT w;
    SHORT h;
    std::wstring dirRoot;
};
