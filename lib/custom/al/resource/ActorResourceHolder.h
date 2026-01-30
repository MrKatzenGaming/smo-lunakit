#pragma once

#include "Resource.h"
#include "types.h"

namespace al {
class ActorResourceHolder {
    ActorResourceHolder(int);
    ~ActorResourceHolder();

    void tryFindActorResource(const sead::SafeString&);
    void findActorResourceImpl(const sead::SafeString&);
    void createActorResource(const sead::SafeString&, al::Resource*, al::Resource*);
    void removeAll();
    void eraseResourceUser(al::Resource*);
    void freeErasedActorResource();
};
}  // namespace al
