#include "RemoveDirPopup.h"

#include "Screen.h"
#include "MessagePopup.h"
#include "colors.h"

#include <iostream>

RemoveDirPopup::RemoveDirPopup(SHORT w, SHORT h)
    : w(w), h(h)
{
}

void RemoveDirPopup::setOnUpdateDirs(std::function<void()> func) {
    updateDirs = std::move(func);
}

void RemoveDirPopup::show(const std::wstring& dir, const std::wstring& name) {
    isVisible = true;
    toDelete = dir + L"\\" + name;
    // TODO delete file?
    text.setLines(styledText({L"Действительно удалить папку?", toDelete}, FG::WHITE | BG::DARK_RED));
}

void RemoveDirPopup::registerKeys(Screen& screen) {
    screen.appendOwner(this);
    registerClosing(screen);
    screen.handleKey(this, VK_RETURN, 0, [this]() {
        if (RemoveDirectoryW(toDelete.c_str()) == 0) {
            DWORD lastError = GetLastError();
            switch(lastError) {
                case ERROR_DIR_NOT_EMPTY:
                    MessagePopup::show({L"Папка не пуста"});
                    return;
                case ERROR_DIRECTORY:
                    MessagePopup::show({L"Объект не является папкой"});
                    return;
                case ERROR_ACCESS_DENIED:
                    MessagePopup::show({L"Нет доступа"});
                    return;
                default:
                    MessagePopup::show({L"Ошибка " + std::to_wstring(lastError)});
                    return;
            }
        }
        updateDirs();
        isVisible = false;
    });
}

void RemoveDirPopup::drawOn(Screen& screen) {
    if (!isVisible) {
        return;
    }
    auto center = screen.center();
    Rect rect{(SHORT)(center.X - w/2), (SHORT)(center.Y - h/2), w, h};
    Rect shadow(rect.moved(2, 1));
    screen.paintRect(shadow, FG::DARK_GREY | BG::BLACK, false);
    screen.paintRect(rect, FG::WHITE | BG::DARK_RED);

    Rect frameRect = rect.withPadding(2, 1);
    screen.frame(frameRect);

    text.drawOn(screen, frameRect.withPadding(1, 1).withH(4), true);
    Rect sep = frameRect.moved(0, frameRect.h - 3).withH(1);
    screen.separator(sep);

    screen.labels(sep.moved(0, 1), {L" <Enter> Да ", L" <Esc> Нет "}, FG::BLACK | BG::GREY);
}
