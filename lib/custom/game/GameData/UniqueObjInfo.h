#pragma once

class UniqObjInfo {
public:
    bool isEqual(const char*, const char*);
    void set();

    char structSize[0x138];
};

static_assert(sizeof(UniqObjInfo) == 0x138);
