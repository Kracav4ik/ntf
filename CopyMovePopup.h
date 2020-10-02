#pragma once

#include "Lines.h"
#include "Popup.h"

class Screen;

class CopyMovePopup : Popup {
public:
    CopyMovePopup(SHORT w, SHORT h);

    void show(bool isCopy);
    void registerKeys(Screen& screen);
    void drawOn(Screen& screen);

private:
    SHORT w;
    SHORT h;
    bool showCopy = false;
};
