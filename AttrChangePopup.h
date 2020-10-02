#pragma once

#include "Lines.h"
#include "Popup.h"

class Screen;

class AttrChangePopup : Popup {
public:
    AttrChangePopup(SHORT w, SHORT h);

    void show();
    void registerKeys(Screen& screen);
    void drawOn(Screen& screen);

private:
    SHORT w;
    SHORT h;

};
