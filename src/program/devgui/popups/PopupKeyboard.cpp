#include "devgui/popups/PopupKeyboard.h"
#include <cctype>

#include "imgui.h"
#include "helpers/InputHelper.h"
#include "nn/hid.h"

#include "logger/Logger.hpp"

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
                //FIXME: this can probably be done a lot cleaner then just hard coding every key
                if (InputHelper::isKeyPress(nn::hid::KeyboardKey::A) && mBuffer.calcLength() < mMaxCharacters) { if (mIsCapsLock || mIsShift) mBuffer.append("A"); else mBuffer.append("a"); mIsShift = false; }
                if (InputHelper::isKeyPress(nn::hid::KeyboardKey::B) && mBuffer.calcLength() < mMaxCharacters) { if (mIsCapsLock || mIsShift) mBuffer.append("B"); else mBuffer.append("b"); mIsShift = false;}
                if (InputHelper::isKeyPress(nn::hid::KeyboardKey::C) && mBuffer.calcLength() < mMaxCharacters) { if (mIsCapsLock || mIsShift) mBuffer.append("C"); else mBuffer.append("c"); mIsShift = false;}
                if (InputHelper::isKeyPress(nn::hid::KeyboardKey::D) && mBuffer.calcLength() < mMaxCharacters) { if (mIsCapsLock || mIsShift) mBuffer.append("D"); else mBuffer.append("d"); mIsShift = false;}
                if (InputHelper::isKeyPress(nn::hid::KeyboardKey::E) && mBuffer.calcLength() < mMaxCharacters) { if (mIsCapsLock || mIsShift) mBuffer.append("E"); else mBuffer.append("e"); mIsShift = false;}
                if (InputHelper::isKeyPress(nn::hid::KeyboardKey::F) && mBuffer.calcLength() < mMaxCharacters) { if (mIsCapsLock || mIsShift) mBuffer.append("F"); else mBuffer.append("f"); mIsShift = false;}
                if (InputHelper::isKeyPress(nn::hid::KeyboardKey::G) && mBuffer.calcLength() < mMaxCharacters) { if (mIsCapsLock || mIsShift) mBuffer.append("G"); else mBuffer.append("g"); mIsShift = false;}
                if (InputHelper::isKeyPress(nn::hid::KeyboardKey::H) && mBuffer.calcLength() < mMaxCharacters) { if (mIsCapsLock || mIsShift) mBuffer.append("H"); else mBuffer.append("h"); mIsShift = false;}
                if (InputHelper::isKeyPress(nn::hid::KeyboardKey::I) && mBuffer.calcLength() < mMaxCharacters) { if (mIsCapsLock || mIsShift) mBuffer.append("I"); else mBuffer.append("i"); mIsShift = false;}
                if (InputHelper::isKeyPress(nn::hid::KeyboardKey::J) && mBuffer.calcLength() < mMaxCharacters) { if (mIsCapsLock || mIsShift) mBuffer.append("J"); else mBuffer.append("j"); mIsShift = false;}
                if (InputHelper::isKeyPress(nn::hid::KeyboardKey::K) && mBuffer.calcLength() < mMaxCharacters) { if (mIsCapsLock || mIsShift) mBuffer.append("K"); else mBuffer.append("k"); mIsShift = false;}
                if (InputHelper::isKeyPress(nn::hid::KeyboardKey::L) && mBuffer.calcLength() < mMaxCharacters) { if (mIsCapsLock || mIsShift) mBuffer.append("L"); else mBuffer.append("l"); mIsShift = false;}
                if (InputHelper::isKeyPress(nn::hid::KeyboardKey::M) && mBuffer.calcLength() < mMaxCharacters) { if (mIsCapsLock || mIsShift) mBuffer.append("M"); else mBuffer.append("m"); mIsShift = false;}
                if (InputHelper::isKeyPress(nn::hid::KeyboardKey::N) && mBuffer.calcLength() < mMaxCharacters) { if (mIsCapsLock || mIsShift) mBuffer.append("N"); else mBuffer.append("n"); mIsShift = false;}
                if (InputHelper::isKeyPress(nn::hid::KeyboardKey::O) && mBuffer.calcLength() < mMaxCharacters) { if (mIsCapsLock || mIsShift) mBuffer.append("O"); else mBuffer.append("o"); mIsShift = false;}
                if (InputHelper::isKeyPress(nn::hid::KeyboardKey::P) && mBuffer.calcLength() < mMaxCharacters) { if (mIsCapsLock || mIsShift) mBuffer.append("P"); else mBuffer.append("p"); mIsShift = false;}
                if (InputHelper::isKeyPress(nn::hid::KeyboardKey::Q) && mBuffer.calcLength() < mMaxCharacters) { if (mIsCapsLock || mIsShift) mBuffer.append("Q"); else mBuffer.append("q"); mIsShift = false;}
                if (InputHelper::isKeyPress(nn::hid::KeyboardKey::R) && mBuffer.calcLength() < mMaxCharacters) { if (mIsCapsLock || mIsShift) mBuffer.append("R"); else mBuffer.append("r"); mIsShift = false;}
                if (InputHelper::isKeyPress(nn::hid::KeyboardKey::S) && mBuffer.calcLength() < mMaxCharacters) { if (mIsCapsLock || mIsShift) mBuffer.append("S"); else mBuffer.append("s"); mIsShift = false;}
                if (InputHelper::isKeyPress(nn::hid::KeyboardKey::T) && mBuffer.calcLength() < mMaxCharacters) { if (mIsCapsLock || mIsShift) mBuffer.append("T"); else mBuffer.append("t"); mIsShift = false;}
                if (InputHelper::isKeyPress(nn::hid::KeyboardKey::U) && mBuffer.calcLength() < mMaxCharacters) { if (mIsCapsLock || mIsShift) mBuffer.append("U"); else mBuffer.append("u"); mIsShift = false;}
                if (InputHelper::isKeyPress(nn::hid::KeyboardKey::V) && mBuffer.calcLength() < mMaxCharacters) { if (mIsCapsLock || mIsShift) mBuffer.append("V"); else mBuffer.append("v"); mIsShift = false;}
                if (InputHelper::isKeyPress(nn::hid::KeyboardKey::W) && mBuffer.calcLength() < mMaxCharacters) { if (mIsCapsLock || mIsShift) mBuffer.append("W"); else mBuffer.append("w"); mIsShift = false;}
                if (InputHelper::isKeyPress(nn::hid::KeyboardKey::X) && mBuffer.calcLength() < mMaxCharacters) { if (mIsCapsLock || mIsShift) mBuffer.append("X"); else mBuffer.append("x"); mIsShift = false;}
                if (InputHelper::isKeyPress(nn::hid::KeyboardKey::Y) && mBuffer.calcLength() < mMaxCharacters) { if (mIsCapsLock || mIsShift) mBuffer.append("Y"); else mBuffer.append("y"); mIsShift = false;}
                if (InputHelper::isKeyPress(nn::hid::KeyboardKey::Z) && mBuffer.calcLength() < mMaxCharacters) { if (mIsCapsLock || mIsShift) mBuffer.append("Z"); else mBuffer.append("z"); mIsShift = false;}
                
                if (InputHelper::isKeyPress(nn::hid::KeyboardKey::Space) && mBuffer.calcLength() < mMaxCharacters) mBuffer.append(" ");
                if (InputHelper::isKeyPress(nn::hid::KeyboardKey::Return) && mBuffer.calcLength() < mMaxCharacters) mBuffer.append("\n");
                if (InputHelper::isKeyPress(nn::hid::KeyboardKey::Backspace) && mBuffer.calcLength() < mMaxCharacters) mBuffer.chop(1);
                if (InputHelper::isModifierHold(nn::hid::KeyboardModifier::Shift) && mBuffer.calcLength() < mMaxCharacters) mIsShift = true;
                if (InputHelper::isKeyPress(nn::hid::KeyboardKey::CapsLock) && mBuffer.calcLength() < mMaxCharacters) mIsCapsLock = !mIsCapsLock;
                if (InputHelper::isKeyPress(nn::hid::KeyboardKey::D0) && mBuffer.calcLength() < mMaxCharacters) mBuffer.append("0");
                if (InputHelper::isKeyPress(nn::hid::KeyboardKey::D1) && mBuffer.calcLength() < mMaxCharacters) mBuffer.append("1");
                if (InputHelper::isKeyPress(nn::hid::KeyboardKey::D2) && mBuffer.calcLength() < mMaxCharacters) mBuffer.append("2");
                if (InputHelper::isKeyPress(nn::hid::KeyboardKey::D3) && mBuffer.calcLength() < mMaxCharacters) mBuffer.append("3");
                if (InputHelper::isKeyPress(nn::hid::KeyboardKey::D4) && mBuffer.calcLength() < mMaxCharacters) mBuffer.append("4");
                if (InputHelper::isKeyPress(nn::hid::KeyboardKey::D5) && mBuffer.calcLength() < mMaxCharacters) mBuffer.append("5");
                if (InputHelper::isKeyPress(nn::hid::KeyboardKey::D6) && mBuffer.calcLength() < mMaxCharacters) mBuffer.append("6");
                if (InputHelper::isKeyPress(nn::hid::KeyboardKey::D7) && mBuffer.calcLength() < mMaxCharacters) mBuffer.append("7");
                if (InputHelper::isKeyPress(nn::hid::KeyboardKey::D8) && mBuffer.calcLength() < mMaxCharacters) mBuffer.append("8");
                if (InputHelper::isKeyPress(nn::hid::KeyboardKey::D9) && mBuffer.calcLength() < mMaxCharacters) mBuffer.append("9");


                break;
            case PopupKeyboardType::KEYTYPE_NUMBER:
                drawNumberKeyset();
                ImGui::Text("   ");
                ImGui::SameLine();
                drawKeyboardLine("0");
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