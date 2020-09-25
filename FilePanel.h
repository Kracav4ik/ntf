#pragma once

#include "Screen.h"
#include "Lines.h"

class FilePanel {
public:
    FilePanel(Rect rect, std::wstring path);

    void selectPrev();
    void selectNext();
    void selectFirst();
    void selectLast();
    void enter();

    void drawOn(Screen& s);

    const std::wstring& getPath() const;

    bool hasSelection();
    void select();
    void unselect();

private:
    void updateLines();
    void scrollToSelection();

    int lastSelectedIdx = -1;
    int dirsCount = 0;
    Rect rect;
    std::wstring path;
    Lines lines;
};
