#include "info/InfoProviderEventFlow.h"

#include "Library/Event/BalloonOrderGroup.h"
#include "Library/Math/MathUtil.h"
#include "al/Library/Base/StringUtil.h"
#include "al/Library/Event/EventFlowActorTerritory.h"
#include "al/Library/Event/EventFlowDataHolder.h"
#include "al/Library/Event/EventFlowNode.h"
#include "al/Library/Event/EventFlowUtil.h"
#include "al/Library/LiveActor/ActorPoseUtil.h"
#include "al/Library/Nature/NatureUtil.h"

#include "game/Event/EventFlowNodeMessageBalloon.h"
#include "game/Player/PlayerFunction.h"
#include "game/Util/PlayerUtil.h"

#include <typeinfo>

#include "imgui.h"
#include "info/InfoProviderNerve.h"

void InfoProviderEventFlow::displayImpl() {
    if (mEventFlow->mCurrentNode)
        ImGui::Text("Node: %s", demangleTypeName(typeid(*mEventFlow->mCurrentNode).name()));
    else
        ImGui::Text("Node: <null>");

    auto balloonOrder = mEventFlow->getEventFlowDataHolder()->mBalloonOrderGroup;
    if (balloonOrder && ImGui::TreeNodeEx("Balloon Order", cFramedNode)) {
        ImGui::Value("Active", balloonOrder->mIsActive);
        ImGui::Value("Current Requester", balloonOrder->mActiveRequesterIdx);
        ImGui::Value("Balloon can appear", balloonOrder->isEnableAppearBalloon(mEventFlow->mActor));

        ImGui::SeparatorText("Requesters");

        for (s32 idx = 0; idx < balloonOrder->mRequesterCount; idx++) {
            auto requester = balloonOrder->mRequesters[idx];
            if (!requester || !requester->actor)
                continue;

            if (ImGui::TreeNode(requester, "#%d: %s", idx, demangleTypeName(typeid(*requester->actor).name()))) {
                ImGui::Value("Balloon Order", requester->ballonOrder);
                ImGui::Value("Balloon Show Step", requester->ballonShowStep);

                ImGui::TreePop();
            }
        }

        ImGui::TreePop();
    }
    if (mEventFlow->mCurrentNode && ImGui::TreeNodeEx("Node", cFramedNode)) {
        InfoProviderEventFlowNode::displayNode(mEventFlow->mCurrentNode);
        ImGui::TreePop();
    }
}
void InfoProviderEventFlow::renderImpl() {
    if (mEventFlow->mCurrentNode)
        InfoProviderEventFlowNode::renderNode(mEventFlow->mCurrentNode);
}
void InfoProviderEventFlow::getNameImpl(sead::BufferedSafeString* buffer) const {
    buffer->format("Event Flow ('%s' for '%s')", mEventFlow->mName, typeid(*mEventFlow->mActor).name());
}

void InfoProviderEventFlowNode::displayImpl() {
    ImGui::Text("Name: %s", mNode->mName);
    ImGui::Text("ID: %d", mNode->mId);
    ImGui::Text("Next ID: %d", mNode->mNextId);
    ImGui::Text("Started: %s", mNode->mIsStarted ? "true" : "false");

    if (mNode->mNerveKeeper && ImGui::TreeNodeEx("Nerves", cFramedNode)) {
        InfoProviderNerveKeeper(mNode->mNerveKeeper).display();
        ImGui::TreePop();
    }
}
void InfoProviderEventFlowNode::renderImpl() {
    if (mNode->mNerveKeeper)
        InfoProviderNerveKeeper(mNode->mNerveKeeper).render();
}
void InfoProviderEventFlowNode::getNameImpl(sead::BufferedSafeString* buffer) const {
    buffer->format("Event Flow Node ('%s' for '%s')", mNode->mName, demangleTypeName(typeid(*mNode->mActor).name()));
}

void InfoProviderEventFlowNode::displayNode(al::EventFlowNode* node) {
    const char* name = typeid(*node).name();

    if (al::isEqualString(name, InfoProviderEventFlowNodeMessageBalloon::cClassName)) {
        InfoProviderEventFlowNodeMessageBalloon(node).display();
    } else {
        InfoProviderEventFlowNode(node).display();
    }
}
void InfoProviderEventFlowNode::renderNode(al::EventFlowNode* node) {
    const char* name = typeid(*node).name();

    if (al::isEqualString(name, InfoProviderEventFlowNodeMessageBalloon::cClassName)) {
        InfoProviderEventFlowNodeMessageBalloon(node).render();
    } else {
        InfoProviderEventFlowNode(node).render();
    }
}

void InfoProviderEventFlowNodeMessageBalloon::displayImpl() {
    InfoProviderEventFlowNode::displayImpl();

    auto actor = mNode->getActor();
    auto dataHolder = mNode->getEventFlowDataHolder();
    auto territory = dataHolder->mActorTerritory;
    auto message = reinterpret_cast<EventFlowNodeMessageBalloon*>(mNode);

    ImGui::Text("Territory Radius: %.3f", territory->mRadius);
    ImGui::Text("Talkable Radius: %.3f", territory->mTalkableRadius);

    if (ImGui::TreeNode("Flags")) {
        ImGui::Value("Talk Balloon", message->mIsTalkBalloon);
        ImGui::Value("Icon Balloon", message->mIsIconBalloon);
        ImGui::Value("Can talk while Captured", message->mIsEnableTalkHacking);
        ImGui::Value("Can talk in Water", !message->mIsDisableTalkInWater);
        ImGui::Value("Multi Divide", message->mIsMultiDevide);
        ImGui::Value("Show Always", message->mIsShowAlways);
        ImGui::Value("Show only while facing Camera front", message->mIsShowOnlyFaceToCameraFront);
        ImGui::Value("Hide if Tutorial exists", message->mIsHideIfExistTutorial);
        ImGui::Value("Low Priority", message->mIsLowPriority);

        ImGui::TreePop();
    }

    ImGui::Separator();

    sead::Vector3f talkPos;
    al::multVecPose(&talkPos, actor, territory->mTerritoryLocalOffset);
    sead::Vector3f playerPos = rs::getPlayerPos(actor);
    sead::Vector3f difference = talkPos - playerPos;
    f32 distance = difference.length();

    ImGui::Text("Talk Position: %.3f, %.3f, %.3f", talkPos.x, talkPos.y, talkPos.z);
    ImGui::Text("Player Position: %.3f, %.3f, %.3f", playerPos.x, playerPos.y, playerPos.z);
    ImGui::Text("Position Difference: %.3f, %.3f, %.3f", difference.x, difference.y, difference.z);
    ImGui::Text("Distance: %.3f", distance);
    ImGui::Text("Angle: %.3f", al::wrapAngle(-sead::Mathf::rad2deg(sead::Mathf::atan2(difference.z, difference.x))));

    ImGui::Separator();

    // Requirements for talking to the NPC
    // TODO: Check for mIsShowOnlyFaceToCameraFront

    bool talkGround = rs::isPlayerEnableTalkGround(actor);
    bool talkSwim = rs::isPlayerEnableTalkSwim(actor) && al::isInWater(actor) && !message->mIsDisableTalkInWater;
    bool talkHack = message->mIsEnableTalkHacking && !al::isScare(message);

    if (al::isPlayingEntranceCamera(actor, 0)) {
        ImGui::Text("Can Talk: false (Entrance camera playing)");
    } else if (al::isDisableOpenBalloonByOrderGroup(actor, message)) {
        ImGui::Text("Can Talk: false (Disabled by Order Group)");
    } else if (!message->mIsTalkBalloon) {
        ImGui::Text("Can Talk: false (Not a Talk Balloon)");
    } else if (message->getNextId() < 0) {
        ImGui::Text("Can Talk: false (Invalid next ID)");
    } else if (PlayerFunction::isPlayerDeadStatus(actor)) {
        ImGui::Text("Can Talk: false (Player Dead)");
    } else if (rs::isPlayerBinding(actor)) {
        ImGui::Text("Can Talk: false (Player Binding)");
    } else if (!talkGround && !talkSwim) {
        if (al::isInWater(actor)) {
            if (!rs::isPlayerEnableTalkSwim(actor)) {
                ImGui::Text("Can Talk: false (Player not enabled for swim talking)");
            } else {
                ImGui::Text("Can Talk: false (Talking in water is disabled)");
            }
        } else {
            ImGui::Text("Can Talk: false (Player not enabled for ground talking)");
        }
    } else if (rs::isPlayerHack(actor) && !talkHack) {
        if (!message->mIsEnableTalkHacking) {
            ImGui::Text("Can Talk: false (Talking while captured is disabled)");
        } else {
            ImGui::Text("Can Talk: false (Actor is scared)");
        }
    } else if (distance >= territory->mTalkableRadius) {
        ImGui::Text("Can Talk: false (Outside of talkable radius; Difference: %.3f)", distance - territory->mTalkableRadius);
    } else {
        ImGui::Text("Can Talk: true");
    }
}
void InfoProviderEventFlowNodeMessageBalloon::renderImpl() {
    InfoProviderEventFlowNode::renderImpl();

    auto actor = mNode->getActor();
    auto dataHolder = mNode->getEventFlowDataHolder();
    auto territory = dataHolder->mActorTerritory;
    auto message = reinterpret_cast<EventFlowNodeMessageBalloon*>(mNode);

    sead::Vector3f talkPos;
    al::multVecPose(&talkPos, actor, territory->mTerritoryLocalOffset);
    sead::Vector3f playerPos = rs::getPlayerPos(actor);

    auto primQueue = getPrimitiveQueue();
    primQueue->pushPoint(talkPos, territory->mTalkableRadius, {1.0f, 0.0f, 0.0f, 0.5f});
    primQueue->pushPoint(playerPos, 15.0f, {0.0f, 1.0f, 0.0f, 0.5f});
    primQueue->pushLine(playerPos, talkPos, {0.0f, 0.0f, 1.0f, 0.5f});
}
