#pragma once

#include "Lines.h"
#include "Popup.h"

#include <string>
#include <vector>

class Screen;

class MessagePopup : Popup {
public:
    static void show(std::vector<std::wstring> text);
    static void registerKeys(Screen& screen);
    static void drawOn(Screen& screen);

private:
    MessagePopup() = default;
    bool isPopupVisible() const override;
    static MessagePopup& get();

    Lines lines;
    bool isVisible = false;
    int maxWidth = 0;
    int linesCount = 0;
};
