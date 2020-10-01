#pragma once

#include "Lines.h"

#include <string>
#include <vector>

class Screen;

class MessagePopup {
public:
    static void show(std::vector<std::wstring> text);
    static void registerKeys(Screen& screen);
    static void drawOn(Screen& screen);

private:
    MessagePopup() = default;
    static MessagePopup& get();

    Lines lines;
    bool isVisible = false;
    int maxWidth = 0;
    int linesCount = 0;
};
