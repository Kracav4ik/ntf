#pragma once

#include <windows.h>

#include <string>

class Screen;
class Popup;

class LineEdit {
public:
    void setText(const std::wstring& newText);
    std::wstring getText() const;

    void registerKeys(Screen& screen, Popup* owner);
    void drawOn(Screen& screen, COORD pos, SHORT w, WORD colorAttr);

private:
    std::wstring text;
};
