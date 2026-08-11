#pragma once

#include <cstring>

#include "devgui/windows/WindowBase.h"

__attribute__((used)) static const char* windowNameMoonRefresh = "Moon Refresh";

class WindowMoonRefresh : public WindowBase {
public:
    WindowMoonRefresh(DevGuiManager* parent, const char* winName, bool isActiveByDefault);

    bool tryUpdateWinDisplay() override;

    bool getIsGrayRefreshEnabled() { return mIsGrayRefreshEnabled; };

    bool getIsRefreshEnabled() { return mIsRefreshEnabled; };

    char* getRefreshText() { return mRefreshText; };

    void setIsGrayRefreshEnabled(bool enabled) { mIsGrayRefreshEnabled = enabled; };

    void setIsRefreshEnabled(bool enabled) { mIsRefreshEnabled = enabled; };

    void setRefreshText(char* text) { strncpy(mRefreshText, text, sizeof(mRefreshText)); };

private:
    bool mIsGrayRefreshEnabled = false;
    bool mIsRefreshEnabled = false;
    char mRefreshText[51] = {"///////////////////////"};

    bool mIsKeyboardInUse = false;
    bool mWasKeyboardOpen = false;
};
