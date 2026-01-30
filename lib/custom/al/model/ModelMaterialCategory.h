#pragma once

#include "Library/Yaml/ByamlIter.h"

namespace al {
struct MaterialCategoryKeeper;
class ModelCtrl;

class ModelMaterialCategory {
public:
    ModelMaterialCategory(al::ModelCtrl*, al::MaterialCategoryKeeper*);
    void init(const char*);
    void init(const al::ByamlIter&);
    void getCategoryIdFromMaterialName(const char*) const;
    void getCategoryIdFromMaterialIndex(int) const;
    static void tryCreate(al::ModelCtrl*, const al::Resource*, const char*, al::MaterialCategoryKeeper*);
    static void tryCreate(al::ModelCtrl*, const char*, al::MaterialCategoryKeeper*);
};
}  // namespace al
