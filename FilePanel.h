#pragma once

#include "Screen.h"
#include "Lines.h"

class FilePanel {
public:
    FilePanel(Rect rect, std::wstring initialPath);

    void selectPrev();
    void selectNext();
    void selectPageUp();
    void selectPageDown();
    void selectFirst();
    void selectLast();
    void enter();

    void drawOn(Screen& s);

    const std::wstring& getPath() const;
    void setPath(std::wstring newPath);

    bool hasSelection();
    void select();
    void unselect();

private:
    void updateLines();
    int visibleHeight() const;
    void scrollToSelection();

    int lastSelectedIdx = -1;
    int dirsCount = 0;
    Rect rect;
    std::wstring path;
    Lines lines;
};
