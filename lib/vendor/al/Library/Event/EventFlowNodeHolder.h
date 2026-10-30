#pragma once

#include <basis/seadTypes.h>

namespace al {
class EventFlowNode;

class EventFlowNodeHolder {
public:
    EventFlowNodeHolder(s32 maxNodes);

    void registerNode(EventFlowNode* node);
    EventFlowNode* getNode(s32 index) const;
    EventFlowNode* findNode(s32 id) const;

public:
    EventFlowNode** mNodes;
    s32 mNodeNum;
    s32 mNodeNumMax;
};

static_assert(sizeof(EventFlowNodeHolder) == 0x10);

}  // namespace al
