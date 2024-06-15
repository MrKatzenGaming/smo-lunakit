#pragma once

#include "program/devgui/windows/WindowBase.h"

__attribute__((used)) static const char* moonRefreshWindowName = "Moon Refresh";
    
class WindowMoonRefresh : public WindowBase {

public:
        WindowMoonRefresh(DevGuiManager* parent, const char* winName, bool isActiveByDefault);

        void updateWin() override;
        bool tryUpdateWinDisplay() override;
        
        static bool getIsGrayRefreshEnabled() { return mIsGrayRefreshEnabled; };
        static bool getIsRefreshEnabled() { return mIsRefreshEnabled; };
        static char* getRefreshText() { return mRefreshText; };

        static void setIsGrayRefreshEnabled(bool enabled) { mIsGrayRefreshEnabled = enabled; };
        static void setIsRefreshEnabled(bool enabled) { mIsRefreshEnabled = enabled; };
        static void setRefreshText(char* text) { mRefreshText = text; };


    private:
        static bool mIsGrayRefreshEnabled;
        static bool mIsRefreshEnabled;
        static char* mRefreshText;

        bool mIsKeyboardInUse = false;
        bool mWasKeyboardOpen = false;

};
