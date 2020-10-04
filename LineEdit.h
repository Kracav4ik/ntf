#pragma once

#include <windows.h>

#include <string>

class Screen;
class Popup;
class EditableText;

class LineEdit {
public:
    LineEdit(Screen& screen, Popup* owner, SHORT w);

    void setText(std::wstring newText);
    std::wstring getText() const;
    void setReadOnly(bool newReadOnly);

    void drawOn(Screen& screen, COORD pos, WORD colorAttr);

private:
    EditableText& editable;
    Popup* owner;
    SHORT w;
};
