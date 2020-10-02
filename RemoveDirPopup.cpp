#include "RemoveDirPopup.h"

#include "Screen.h"
#include "colors.h"

RemoveDirPopup::RemoveDirPopup(SHORT w, SHORT h)
    : w(w), h(h)
{
}

void RemoveDirPopup::show(const std::wstring& dir) {
    isVisible = true;
    toDelete = dir;
    // TODO delete file?
    text.setLines(styledText({L"Действительно удалить папку?", dir}, FG::WHITE | BG::DARK_RED));
}

void RemoveDirPopup::registerKeys(Screen& screen) {
    screen.appendOwner(this);
    registerClosing(screen);
    screen.handleKey(this, VK_RETURN, 0, [this]() {
        // TODO delete folder/file here
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
