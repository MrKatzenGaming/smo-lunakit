#pragma once

namespace al {

class ActorItemInfo {
public:
    ActorItemInfo(const char*, const char*, const char*, int);

    bool isEqualFactor(const char*) const;
    bool isEqualTiming(const char*) const;

    const char* mChar1;       // 0x00
    const char* mTimingName;  // 0x08
    const char* mFactorName;  // 0x10
    int mUnk;                 // 0x18
    int mFactor;              // 0x1C
};

}  // namespace al
