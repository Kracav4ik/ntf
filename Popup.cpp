#include "Popup.h"

#include "Screen.h"

bool Popup::isPopupVisible() const {
    return isVisible;
}

void Popup::registerClosing(Screen& screen) {
    screen.handleKey(this, VK_ESCAPE, 0, [this]() {
        isVisible = false;
    });
}
