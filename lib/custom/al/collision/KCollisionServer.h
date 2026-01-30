#pragma once

#include <sead/container/seadPtrArray.h>

#include "al/collision/KCPrism.h"

namespace sead {
template <typename T, s32 N>
class FixedRingBuffer;
}

namespace al {
class ByamlIter;
class KCHitInfo;

class KCollisionServer {
public:
    KCollisionServer();
    void areaOffsetSpaceToObjectSpace(sead::Vector3f*, const sead::Vector3<unsigned int>&, const al::KCPrismHeader*) const;
    int calcAreaBlockOffset(const sead::Vector3<unsigned int>&, const al::KCPrismHeader*) const;
    static int calcChildBlockOffset(const sead::Vector3<unsigned int>&, int);
    float calcFarthestVertexDistance();
    void calcPosLocal(sead::Vector3f*, const al::KCPrismData*, int, const al::KCPrismHeader*) const;
    static void calXVec(const sead::Vector3f*, const sead::Vector3f*, sead::Vector3f*);
    void checkArrow(const sead::Vector3f&, const sead::Vector3f&, sead::FixedRingBuffer<al::KCHitInfo, 512>*, unsigned int*, unsigned int) const;
    // check*
    void getAreaSpaceSize(sead::Vector3f*, const al::KCPrismHeader*) const;
    void getAreaSpaceSize(int*, int*, int*, const al::KCPrismHeader*) const;
    void getAreaSpaceSize(sead::Vector3<unsigned int>*, const al::KCPrismHeader*) const;
    int getAttributeElementNum() const;
    bool getAttributes(al::ByamlIter*, unsigned int, const al::KCPrismHeader*) const;
    bool getAttributes(al::ByamlIter*, const al::KCPrismData*) const;
    // getBlockData
    sead::Vector3f* getEdgeNormal1(const al::KCPrismData*, const al::KCPrismHeader*) const;
    sead::Vector3f* getEdgeNormal2(const al::KCPrismData*, const al::KCPrismHeader*) const;
    sead::Vector3f* getEdgeNormal3(const al::KCPrismData*, const al::KCPrismHeader*) const;
    sead::Vector3f* getFaceNormal(const al::KCPrismData*, const al::KCPrismHeader*) const;
    void* getInnerKcl(int) const;
    void getMinMax(sead::Vector3f*, sead::Vector3f*) const;
    inline sead::Vector3f* getNormal(unsigned int, const al::KCPrismHeader*) const;
    inline int getNormalNum(const al::KCPrismHeader*) const;

    inline int getNumInnerKcl() const { return *(int*)(this->gap1 + 0xc); }

    inline al::KCPrismData* getPrismData(unsigned int idx, const al::KCPrismHeader* prismHeader) const {
        return (al::KCPrismData*)(&prismHeader->mPositionsOffset + prismHeader->mTrianglesOffset + idx * 0x14);
    }

    inline int getTriangleNum(const al::KCPrismHeader* prismHeader) const {
        return (prismHeader->mOctreeOffset - prismHeader->mTrianglesOffset) / 0x14;
    }

    inline al::KCPrismHeader* getV1Header(int) const;

    // getVertexData (0xc)

    inline int getVertexNum(const al::KCPrismHeader* prismHeader) const {
        return ((prismHeader->mNormalsOffset - prismHeader->mPositionsOffset) >> 2) * -0x55555555;
    }

    void initKCollisionServer(void*, const void*);
    inline bool isInsideMinMaxInAreaOffsetSpace(const sead::Vector3<unsigned int>&, const al::KCPrismHeader*) const;
    inline bool isNanPrism(const al::KCPrismData*, const al::KCPrismHeader*) const;
    bool isNearParallelNormal(const al::KCPrismData*, const al::KCPrismHeader*) const;
    bool isParallelNormal(const al::KCPrismData*, const al::KCPrismHeader*) const;
    // KCHit*
    void objectSpaceToAreaOffsetSpace(sead::Vector3<unsigned int>*, const sead::Vector3f&, const al::KCPrismHeader*) const;
    void objectSpaceToAreaOffsetSpaceV3f(sead::Vector3f*, const sead::Vector3f&, const al::KCPrismHeader*) const;
    bool outCheckAndCalcArea(sead::Vector3<unsigned int>*, sead::Vector3<unsigned int>*, const sead::Vector3f&, const sead::Vector3f&,
                             const al::KCPrismHeader*) const;
    // search*
    void setData(void*);
    inline int toIndex(const al::KCPrismData*, const al::KCPrismHeader*) const;

    sead::PtrArray<al::KCPrismHeader> mPrismHeaders;
    unsigned char* gap1;  // al::Resource? contains binary contents of kcl file
    al::ByamlIter* mAttributeByml;
    int* piVar1;
    unsigned long uVar1;
    int iVar1;
    int iVar2;
    int iVar3;
    int iVar4;
    int iVar5;
    int iVar6;
    float mFarthestVertexDistance;
};
}  // namespace al
