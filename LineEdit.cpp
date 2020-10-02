#include "LineEdit.h"

#include "Screen.h"
#include "colors.h"

void LineEdit::setText(const std::wstring& newText) {
    text = newText;
}

std::wstring LineEdit::getText() const {
    return text;
}

void LineEdit::registerKeys(Screen& screen, Popup* owner) {
    // TODO
}

void LineEdit::drawOn(Screen& screen, COORD pos, SHORT w, WORD colorAttr) {
    screen.paintRect({pos.X, pos.Y, w, 1}, colorAttr);
    screen.textOut(pos, text);
}
