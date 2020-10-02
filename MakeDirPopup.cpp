#include "MakeDirPopup.h"

#include "Screen.h"
#include "colors.h"

MakeDirPopup::MakeDirPopup(SHORT w, SHORT h)
    : w(w), h(h)
{
}

void MakeDirPopup::show(const std::wstring& root) {
    isVisible = true;
    dirRoot = root;
}

void MakeDirPopup::registerKeys(Screen& screen) {
    screen.appendOwner(this);
    registerClosing(screen);
    screen.handleKey(this, VK_RETURN, 0, [this]() {
        // TODO make dir here
        isVisible = false;
    });
}

void MakeDirPopup::drawOn(Screen& screen) {
    if (!isVisible) {
        return;
    }
    auto center = screen.center();
    Rect rect{(SHORT)(center.X - w/2), (SHORT)(center.Y - h/2), w, h};
    Rect shadow(rect.moved(2, 1));
    screen.paintRect(shadow, FG::DARK_GREY | BG::BLACK, false);
    screen.paintRect(rect, FG::BLACK | BG::GREY);

    Rect frameRect = rect.withPadding(2, 1);
    screen.frame(frameRect);

    Rect inner = frameRect.withPadding(2, 2);
    screen.textOut(inner.getLeftTop(), L"Создать папку:");

    Rect sep = frameRect.moved(0, frameRect.h - 3).withH(1);
    screen.separator(sep);

    screen.labels(sep.moved(0, 1), {L" <Enter> OK ", L" <Esc> Отмена "}, FG::BLACK | BG::DARK_CYAN);
}
