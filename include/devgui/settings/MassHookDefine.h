#pragma once

// black magic

#define HOOK_CALLBACK(NAME)                                                                                                                          \
    HkTrampoline NAME = [](TrampolineStatic(), void* thisPtr) -> bool {                                                                              \
        DevGuiSettings* set = DevGuiManager::instance()->getSettings();                                                                              \
        if (set->getStateByName("Always Manually Skip Cutscenes"))                                                                                   \
            return true;                                                                                                                             \
        return orig(thisPtr);                                                                                                                        \
    };

#define EXPAND(...) EXPAND1(EXPAND1(EXPAND1(EXPAND1(__VA_ARGS__))))
#define EXPAND1(...) EXPAND2(EXPAND2(EXPAND2(EXPAND2(__VA_ARGS__))))
#define EXPAND2(...) EXPAND3(EXPAND3(EXPAND3(EXPAND3(__VA_ARGS__))))
#define EXPAND3(...) EXPAND4(EXPAND4(EXPAND4(EXPAND4(__VA_ARGS__))))
#define EXPAND4(...) __VA_ARGS__
#define PARENS ()
#define FOR_EACH_HELPER(macro, a1, ...) macro(a1) __VA_OPT__(FOR_EACH_AGAIN PARENS(macro, __VA_ARGS__))
#define FOR_EACH_AGAIN() FOR_EACH_HELPER
#define FOR_EACH2(macro, ...) __VA_OPT__(EXPAND(FOR_EACH_HELPER(macro, __VA_ARGS__)))

#define MASS_HOOK_DEFINER(...) FOR_EACH2(HOOK_CALLBACK, __VA_ARGS__)
