#include "LineEdit.h"

#include "Screen.h"
#include "colors.h"
#include "Popup.h"
#include "MessagePopup.h"

LineEdit::LineEdit(Screen& screen, Popup* owner, SHORT w)
    : editable(screen.getEditable())
    , owner(owner)
    , w(w)
{
}

void LineEdit::setText(std::wstring newText) {
    editable.setText(std::move(newText), w);
    editable.setEnabledGetter([this]() {
        return !MessagePopup::isVisible() && owner->isPopupVisible();
    });
}

std::wstring LineEdit::getText() const {
    return editable.getText();
}

void LineEdit::drawOn(Screen& screen, COORD pos, WORD colorAttr) {
    screen.paintRect({pos.X, pos.Y, w, 1}, colorAttr);
    editable.drawOn(screen, pos, !MessagePopup::isVisible());
}
