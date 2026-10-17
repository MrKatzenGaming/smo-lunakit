#pragma once

#include "Library/Collision/CollisionParts.h"
#include "Library/Collision/ICollisionPartsKeeper.h"

#include <container/seadObjList.h>
#include <container/seadTList.h>
#include <math/seadBoundBox.h>
#include <math/seadVector.h>
#include <prim/seadDelegate.h>

namespace al {

class CollisionPartsKeeperOctree : public ICollisionPartsKeeper {
public:
    static constexpr s32 sNodeChildNum = 8;

    struct OctNode {
        OctNode* parent;
        OctNode* octants[sNodeChildNum];

        sead::BoundBox3f bounds;
        f32 cubeSize;
        u32 depth;

        sead::TList<CollisionParts*> collisionParts;
    };

    CollisionPartsKeeperOctree(u32 maxNodes, u32 maxDepth, f32);

    void endInit() override;
    void insertMovingPartsListToOctNode();

    void addCollisionParts(CollisionParts* parts) override;
    void connectToCollisionPartsList(CollisionParts* parts) override;
    void insertLooseOctree(OctNode*, CollisionParts*);

    void disconnectToCollisionPartsList(CollisionParts* parts) override;
    void resetToCollisionPartsList(CollisionParts* parts) override;

    s32 checkStrikePoint(HitInfo* hitInfo, const CollisionCheckInfoBase& checkInfo) const override;

    using CheckSphereCallback = u32(SphereHitResultBuffer*, const sead::TList<CollisionParts*>&, const SphereCheckInfo&, bool, const sead::Vector3f&);

    s32 checkStrikeSphere(SphereHitResultBuffer* resultBuffer, const SphereCheckInfo& checkInfo, bool unk, const sead::Vector3f& unk2) const override;
    void checkStrikeSphereRecursive(SphereHitResultBuffer*, const SphereCheckInfo&, const OctNode*, bool, const sead::Vector3f&,
                                    CheckSphereCallback* callback) const;
    s32 checkStrikeSphereForPlayer(SphereHitResultBuffer* resultBuffer, const SphereCheckInfo& checkInfo) const override;

    s32 checkStrikeDisk(DiskHitResultBuffer* resultBuffer, const DiskCheckInfo& checkInfo) const override;
    void checkStrikeDiskRecursive(DiskHitResultBuffer*, const DiskCheckInfo&, const OctNode*) const;

    void searchWithSphere(const SphereCheckInfo& checkInfo, sead::IDelegate1<CollisionParts*>& callback) const override;
    void searchWithSphereRecursive(const SphereCheckInfo&, sead::IDelegate1<CollisionParts*>&, const OctNode*) const;

    s32 checkStrikeArrow(ArrowHitResultBuffer* resultBuffer, const ArrowCheckInfo& checkInfo) const override;
    void checkStrikeArrowRecursive(ArrowHitResultBuffer*, const ArrowCheckInfo&, const OctNode*) const;

    void movement() override;

    void updateOctNodeCollisionPartsRecursive(OctNode*);

    void isFitsInBox(const CollisionParts*, const sead::BoundBox3f&) const;
    f32 calcNodeCubeSizeHalf(s32 depth) const;
    bool isEnableSearchNextDepthNode(const OctNode* node);
    void isSphereFitsInBox(const sead::Vector3f&, f32, const sead::BoundBox3f&) const;

public:
    sead::BoundBox3f mBounds;

    f32 mRootCubeSize;
    OctNode mRoot;

    sead::ObjList<OctNode> mNodeList;
    sead::TList<CollisionParts*> mCollisionParts;

    s32 mMaxDepth;
    f32 _f4;
    bool mInitialized;
};

static_assert(sizeof(CollisionPartsKeeperOctree) == 0x100);
static_assert(sizeof(CollisionPartsKeeperOctree::OctNode) == 0x80);

}  // namespace al
