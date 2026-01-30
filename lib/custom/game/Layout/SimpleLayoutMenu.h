#pragma once

#include "Library/Layout/LayoutActor.h"

class SimpleLayoutMenu : public al::LayoutActor {
public:
    SimpleLayoutMenu(al::LayoutActor*, const char*, const char*, const al::LayoutInitInfo&, const char*);
    SimpleLayoutMenu(const char*, const char*, const al::LayoutInitInfo&, const char*, bool);

    void exeAppear(void);
    void exeEnd(void);
    void exeEndWait(void);
    void exeWait(void);
    void isAppearOrWait(void);
    void isEndWait(void);
    void isWait(void);
    void startAppear(const char*);
    void startEnd(const char*);
};
