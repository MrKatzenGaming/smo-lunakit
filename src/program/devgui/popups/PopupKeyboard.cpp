#include "devgui/popups/PopupKeyboard.h"
#include <cctype>

#include "imgui.h"
#include "helpers/InputHelper.h"
#include "nn/hid.h"

#include "logger/Logger.hpp"
#include "prim/seadSafeString.h"

void PopupKeyboard::update()
{
    if(!mIsKeyboardOpen)
        return;
    if(!(*mIsKeyboardOpen))
        return;
    
    if(mIsFirstStep) {
        mIsFirstStep = false;
        ImGui::OpenPopup("Keyboard");
    }
    
    ImGuiWindowFlags winFlags = ImGuiWindowFlags_NoResize;

    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(.5f, .5f));
    ImGui::SetNextWindowSize(ImVec2(0.f, 0.f), ImGuiCond_Always);

    bool isPopupOpen = ImGui::BeginPopupModal("Keyboard", mIsKeyboardOpen, winFlags);
    if(!isPopupOpen && *mIsKeyboardOpen)
        endKeyboard();

    if(isPopupOpen) {
        ImGui::SetWindowFontScale(2.f);

        if(mBuffer.isEmpty())
            ImGui::TextDisabled("Use keyboard\n");
        else
            ImGui::Text(mBuffer.cstr());

        ImGui::SameLine();
        ImGui::TextDisabled("%i/%u", mBuffer.calcLength(), mMaxCharacters);
        


        switch(mKeyboardType) {

            case PopupKeyboardType::KEYTYPE_QWERTY:
                drawQuertyKeyset();
                checkInput(mMaxCharacters, mBuffer, mIsShift, mIsCapsLock, false);
                break;
            case PopupKeyboardType::KEYTYPE_NUMBER:
                drawNumberKeyset();
                ImGui::Text("   ");
                ImGui::SameLine();
                drawKeyboardLine("0");
                checkInput(mMaxCharacters, mBuffer, mIsShift, mIsCapsLock, true);
                break;
            case PopupKeyboardType::KEYTYPE_IP:
                drawNumberKeyset();
                ImGui::Text("   ");
                ImGui::SameLine();
                drawKeyboardLine("0.");
                break;
        }

        ImGui::EndPopup();
    }
}

bool PopupKeyboard::tryOpenKeyboard(uint16_t maxChars, PopupKeyboardType keyType, const char** output, bool* isKeyboardOpen)
{
    if(mIsKeyboardOpen && *mIsKeyboardOpen)
        return false;
    
    Logger::log("Opened keyboard (Type ID: %i)\n", keyType);

    mOutputDest = output;
    mIsKeyboardOpen = isKeyboardOpen;

    *mIsKeyboardOpen = true;
    mIsFirstStep = true;
    mIsShift = false;
    mIsCapsLock = false;

    mMaxCharacters = maxChars;
    mBuffer.clear();

    mKeyboardType = keyType;

    return true;
}

bool PopupKeyboard::tryOpenKeyboard(uint16_t maxChars, PopupKeyboardType keyType, sead::FixedSafeString<0xff>* output, bool* isKeyboardOpen)
{
    if(mIsKeyboardOpen && *mIsKeyboardOpen)
        return false;

    Logger::log("Opened keyboard (Type ID: %i)\n", keyType);

    mOutputDest = nullptr;
    output = &mBuffer;
    mIsKeyboardOpen = isKeyboardOpen;

    *mIsKeyboardOpen = true;
    mIsFirstStep = true;
    mIsShift = false;
    mIsCapsLock = false;

    mMaxCharacters = maxChars;
    mBuffer.clear();

    mKeyboardType = keyType;

    return true;
}

void PopupKeyboard::endKeyboard()
{
    *mIsKeyboardOpen = false;
    *mOutputDest = mBuffer.cstr();
}

void PopupKeyboard::drawQuertyKeyset()
{
    drawKeyboardLine("123456789-=");
    ImGui::SameLine();
    if(ImGui::Button("<-", ImVec2(88,mKeyLineHeight)))
        mBuffer.chop(1);
    
    ImGui::Text("  ");
    ImGui::SameLine();
    drawKeyboardLine("qwertyuiop!");
    
    if(ImGui::Button("Caps", ImVec2(62,mKeyLineHeight)))
        mIsCapsLock = !mIsCapsLock;
    
    ImGui::SameLine();
    drawKeyboardLine("asdfghjkl:");
    ImGui::SameLine();
    if(ImGui::Button("Enter", ImVec2(90, mKeyLineHeight)))
        endKeyboard();

    if(ImGui::Button("Shift", ImVec2(90,mKeyLineHeight)))
        mIsShift = !mIsShift;
    ImGui::SameLine();
    drawKeyboardLine("zxcvbnm,./?");

    ImGui::Text("          ");
    ImGui::SameLine();
    if(ImGui::Button("Space", ImVec2(300, mKeyLineHeight)))
        mBuffer.append(" ");
    ImGui::SameLine();
    if (ImGui::Button("New Line", ImVec2(120, mKeyLineHeight)))
        mBuffer.append("\n");
}

void PopupKeyboard::drawNumberKeyset()
{
    drawKeyboardLine("123");
    ImGui::SameLine();
    if(ImGui::Button("<-", ImVec2(88,mKeyLineHeight)))
        mBuffer.chop(1);
    
    drawKeyboardLine("456");
    ImGui::SameLine();
    if(ImGui::Button("Enter", ImVec2(90, mKeyLineHeight)))
        endKeyboard();

    drawKeyboardLine("789");
}

void PopupKeyboard::drawKeyboardLine(const char* keys)
{
    int totalKeys = strlen(keys);

    for(int i = 0; i < totalKeys; i++) {
        char keyLabel[] = "X";
        keyLabel[0] = keys[i];

        if(!mIsShift != !mIsCapsLock) {
            int keyNum = (int)keyLabel[0];
            if(keyNum >= 97 && keyNum <= 122)
                keyNum -= 32;
            keyLabel[0] = (char)keyNum;
        }

        if(ImGui::Button(keyLabel, ImVec2(44,mKeyLineHeight)) && mBuffer.calcLength() < mMaxCharacters) {
            mBuffer.append(keyLabel);
            mIsShift = false;
            if(mOutputDest)
                *mOutputDest = mBuffer.cstr();
        }



        ImGui::SameLine();
    }

    ImGui::NewLine();
}

void PopupKeyboard::checkInput(int maxChars, sead::FixedSafeString<0xFF>& mBuffer, bool& mIsShift, bool& mIsCapsLock, bool numOnly)
{

struct funcs { static bool IsLegacyNativeDupe(ImGuiKey key) { return key < 512 && ImGui::GetIO().KeyMap[key] != -1; } }; // Hide Native<>ImGuiKey duplicates when both exists in the array
ImGuiKey start_key = (ImGuiKey)0;

for (ImGuiKey key = start_key; key < ImGuiKey_NamedKey_END; key = (ImGuiKey)(key + 1)) {
    if (funcs::IsLegacyNativeDupe(key) || !ImGui::IsKeyPressed(key)) continue;
    if (key == ImGuiKey_Backspace && mBuffer.calcLength() <= maxChars) {
        mBuffer.chop(1);
    }
    if (mBuffer.calcLength() < maxChars) {
        if (key >= ImGuiKey_A && key <= ImGuiKey_Z && !numOnly) {
            if (mIsCapsLock || mIsShift) {
                mBuffer.append(ImGui::GetKeyName(key));
                mIsShift = false;
            }
            else {
                mBuffer.append(tolower((int)*ImGui::GetKeyName(key)));
                mIsShift = false;
            }
        }
        else if (key >= ImGuiKey_0 && key <= ImGuiKey_9) {
            mBuffer.append(ImGui::GetKeyName(key));
            mIsShift = false;
        }
        else if (key == ImGuiKey_Space) {
            mBuffer.append(" ");
        }
        else if (key == ImGuiKey_Enter && !numOnly) {
            mBuffer.append("\n");
        }
        else if (!numOnly &&(key == ImGuiKey_LeftShift || key == ImGuiKey_RightShift )) {
            mIsShift = true;
        }
        else if (key == ImGuiKey_CapsLock && !numOnly) {
            mIsCapsLock = !mIsCapsLock;
        }
    } 
}
}