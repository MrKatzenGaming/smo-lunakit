#pragma once

#include <sead/math/seadVector.h>
#include <sead/prim/seadDelegate.h>

namespace al {
class CollisionParts;
class HitInfo;
class CollisionCheckInfoBase;
class SphereHitResultBuffer;
class ArrowHitResultBuffer;
class DiskHitResultBuffer;
class SphereCheckInfo;
class ArrowCheckInfo;
class DiskCheckInfo;

class ICollisionPartsKeeper {
public:
    virtual void endInit() = 0;
    virtual void addCollisionParts(al::CollisionParts*) = 0;
    virtual void connectToCollisionPartsList(al::CollisionParts*) = 0;
    virtual void disconnectToCollisionPartsList(al::CollisionParts*) = 0;
    virtual void resetToCollisionPartsList(al::CollisionParts*) = 0;
    virtual bool checkStrikePoint(al::HitInfo*, al::CollisionCheckInfoBase*) const = 0;
    virtual bool checkStrikeSphere(al::SphereHitResultBuffer*, const al::SphereCheckInfo&, bool, const sead::Vector3f&) const = 0;
    virtual bool checkStrikeArrow(al::ArrowHitResultBuffer*, const al::ArrowCheckInfo&) const = 0;
    virtual bool checkStrikeSphereForPlayer(al::SphereHitResultBuffer*, const al::SphereCheckInfo&) const = 0;
    virtual bool checkStrikeDisk(al::DiskHitResultBuffer*, const al::DiskCheckInfo&) const = 0;
    virtual void searchWithSphere(const al::SphereCheckInfo&, sead::IDelegate1<al::CollisionParts*>&) const = 0;
    virtual void movement() = 0;
};
}  // namespace al
