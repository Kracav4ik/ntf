#pragma once

#include "Screen.h"
#include "Lines.h"

class FilePanel {
public:
    FilePanel(Rect rect, std::wstring path);

    void selectPrev();
    void selectNext();
    void enter();

    void drawOn(Screen& s);

    const std::wstring& getPath() const;

    bool hasSelection();
    void select();
    void unselect();

private:
    void updateLines();

    int lastSelectedIdx = -1;
    Rect rect;
    std::wstring path;
    Lines lines;
};
