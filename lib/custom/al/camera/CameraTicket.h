#pragma once

#include "al/actor/Placement.h"
#include "al/camera/CameraPoser.h"

namespace al {

class CameraTicketId {
public:
    CameraTicketId(const al::PlacementId*, const char*);
    void isEqual(const al::CameraTicketId&);
    void isEqual(const al::CameraTicketId&, const al::CameraTicketId&);
    void isEqual(const al::ByamlIter&);
    void tryGetObjId(void);
    void getObjId(void);

    al::PlacementId* mPlacement;
    const char* mTicketName;
};

class CameraTicket {
public:
    CameraTicket(CameraPoser*, const CameraTicketId*, int);
    void setPriority(int);

    CameraPoser* mPoser;
    CameraTicketId* mTicketID;
    int mPriority;
    bool unkBool;
};
}  // namespace al
