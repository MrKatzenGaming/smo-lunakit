#pragma once

#include "AreaObj.h"

namespace al {
class AreaObjGroup {
public:
    AreaObjGroup(const char*, int);
    void incrementCount();
    void createBuffer();
    void createBuffer(int);
    void registerAreaObj(al::AreaObj*);
    void getAreaObj(int);
    al::AreaObj* getInVolumeAreaObj(const sead::Vector3f&);

    const char* mGroupName;
    al::AreaObj** mAreas;
    int mCurCount;
    int mMaxCount;
};
};  // namespace al
