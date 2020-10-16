#include "CopyMovePopup.h"

#include "Screen.h"
#include "colors.h"

CopyMovePopup::CopyMovePopup(Screen& screen, SHORT w, SHORT h)
    : w(w)
    , h(h)
    , newName(screen, this, w - 8)
{
}

void CopyMovePopup::show(bool isCopy, const std::wstring& fromRoot, const std::wstring& fromName, const std::wstring& toRoot) {
    visible = true;
    showCopy = isCopy;
    oldName = fromName;
    oldRoot = fromRoot;
    newName.setText(fromName);
    newRoot = toRoot;
}

void CopyMovePopup::registerKeys(Screen& screen) {
    screen.appendOwner(this);
    registerClosing(screen);
    screen.handleKey(this, VK_RETURN, 0, [this]() {
        // TODO copy/move files here
        visible = false;
    });
}

void CopyMovePopup::drawOn(Screen& screen) {
    if (!visible) {
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
    std::wstring action = showCopy ? L"Копировать " : L"Переместить ";
    screen.textOut(inner.getLeftTop(), action + oldName + L" в:");
    newName.drawOn(screen, inner.moved(0, 1).getLeftTop(), FG::BLACK | BG::DARK_CYAN);

    Rect sep = frameRect.moved(0, frameRect.h - 3).withH(1);
    screen.separator(sep);

    screen.labels(sep.moved(0, 1), {L" <Enter> OK ", L" <Esc> Отмена "}, FG::BLACK | BG::DARK_CYAN);
}
