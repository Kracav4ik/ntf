#pragma once

#include "Lines.h"

#include <windows.h>

#include <functional>
#include <string>

class Screen;

class DiskPopup {
public:
    DiskPopup(SHORT xLeft, SHORT xRight, SHORT y, SHORT w, SHORT h);

    bool isLeftPopup() const;
    std::wstring selectedDisk() const;

    void registerKeys(Screen& screen);
    void setOnSelectFunc(std::function<void()> func);
    void drawOn(Screen& screen);

private:
    Rect popupRect() const;
    Rect frameRect() const;
    Rect innerRect() const;
    Rect diskListRect() const;
    Rect diskInfoRect() const;
    void show(bool left);
    void hide();
    void fillDrivesList();
    void updateDiskInfo();

    SHORT xLeft;
    SHORT xRight;
    SHORT y;
    SHORT w;
    SHORT h;
    Lines diskList;
    Lines diskInfo;
    bool isLeft = false;
    bool isVisible = false;
    std::function<void()> selectDisk;
};
