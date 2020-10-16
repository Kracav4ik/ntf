#pragma once

#include <functional>
#include "Lines.h"
#include "Popup.h"
#include "LineEdit.h"

class Screen;

class MakeDirPopup : Popup {
public:
    MakeDirPopup(SHORT w, SHORT h);

    void setOnUpdateDirs(std::function<void()> func);

    void show(const std::wstring& root);
    void registerKeys(Screen& screen);
    void drawOn(Screen& screen);

private:
    SHORT w;
    SHORT h;
    std::wstring dirRoot;
    LineEdit newName;
    std::function<void()> updateDirs;
};
