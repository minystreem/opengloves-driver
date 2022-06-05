#include <Encode/AlphaEncodingManager.h>
#include <ctype.h>

#include <map>
#include <sstream>

static enum class VRCommDataAlphaEncodingKey : int {
  FinSplayThumb,
  FinSplayIndex,
  FinSplayMiddle,
  FinSplayRing,
  FinSplayPinky,

  FinJointThumb0,
  FinJointThumb1,
  FinJointThumb2,
  FinJointThumb3,  // unused in input but used for parity to other fingers in the array
  FinJointIndex0,
  FinJointIndex1,
  FinJointIndex2,
  FinJointIndex3,
  FinJointMiddle0,
  FinJointMiddle1,
  FinJointMiddle2,
  FinJointMiddle3,
  FinJointRing0,
  FinJointRing1,
  FinJointRing2,
  FinJointRing3,
  FinJointPinky0,
  FinJointPinky1,
  FinJointPinky2,
  FinJointPinky3,

  FinThumb,
  FinIndex,
  FinMiddle,
  FinRing,
  FinPinky,

  JoyX,
  JoyY,
  JoyBtn,

  TrgValue,
  BtnTrg,
  BtnA,
  BtnB,

  GesGrab,
  GesPinch,

  BtnMenu,
  BtnCalib,

  OutHapticDuration,
  OutHapticFrequency,
  OutHapticAmplitude,

  Null
};

static const std::string keyCharacters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ()";
static bool IsCharacterKeyCharacter(const char character) {
  return keyCharacters.find(character) != std::string::npos;
}

static const std::map<std::string, VRCommDataAlphaEncodingKey> VRCommDataAlphaEncodingInputKeyString{
    {"A", VRCommDataAlphaEncodingKey::FinThumb},   // whole thumb curl (default curl value for thumb joints)
    {"B", VRCommDataAlphaEncodingKey::FinIndex},   // whole index curl (default curl value for index joints)
    {"C", VRCommDataAlphaEncodingKey::FinMiddle},  // whole middle curl (default curl value for middle joints)
    {"D", VRCommDataAlphaEncodingKey::FinRing},    // whole ring curl (default curl value for ring joints)
    {"E", VRCommDataAlphaEncodingKey::FinPinky},   // whole pinky curl (default curl value for pinky joints)

    {"(AAA)", VRCommDataAlphaEncodingKey::FinJointThumb0},   // thumb joint 0
    {"(AAB)", VRCommDataAlphaEncodingKey::FinJointThumb1},   // thumb joint 1
    {"(AAC)", VRCommDataAlphaEncodingKey::FinJointThumb2},   // thumb joint 2
    {"(BAA)", VRCommDataAlphaEncodingKey::FinJointIndex0},   // index joint 0
    {"(BAB)", VRCommDataAlphaEncodingKey::FinJointIndex1},   // index joint 1
    {"(BAC)", VRCommDataAlphaEncodingKey::FinJointIndex2},   // index joint 2
    {"(BAD)", VRCommDataAlphaEncodingKey::FinJointIndex3},   // index joint 3
    {"(CAA)", VRCommDataAlphaEncodingKey::FinJointMiddle0},  // middle joint 0
    {"(CAB)", VRCommDataAlphaEncodingKey::FinJointMiddle1},  // middle joint 1
    {"(CAC)", VRCommDataAlphaEncodingKey::FinJointMiddle2},  // middle joint 2
    {"(CAD)", VRCommDataAlphaEncodingKey::FinJointMiddle3},  // middle joint 3
    {"(DAA)", VRCommDataAlphaEncodingKey::FinJointRing0},    // ring joint 0
    {"(DAB)", VRCommDataAlphaEncodingKey::FinJointRing1},    // ring joint 1
    {"(DAC)", VRCommDataAlphaEncodingKey::FinJointRing2},    // ring joint 2
    {"(DAD)", VRCommDataAlphaEncodingKey::FinJointRing3},    // ring joint 3
    {"(EAA)", VRCommDataAlphaEncodingKey::FinJointPinky0},   // pinky joint 0
    {"(EAB)", VRCommDataAlphaEncodingKey::FinJointPinky1},   // pinky joint 1
    {"(EAC)", VRCommDataAlphaEncodingKey::FinJointPinky2},   // pinky joint 2
    {"(EAD)", VRCommDataAlphaEncodingKey::FinJointPinky3},   // pinky joint 3

    {"(AB)", VRCommDataAlphaEncodingKey::FinSplayThumb},   // whole thumb splay
    {"(BB)", VRCommDataAlphaEncodingKey::FinSplayIndex},   // whole index splay
    {"(CB)", VRCommDataAlphaEncodingKey::FinSplayMiddle},  // whole middle splay
    {"(DB)", VRCommDataAlphaEncodingKey::FinSplayRing},    // whole ring splay
    {"(EB)", VRCommDataAlphaEncodingKey::FinSplayPinky},   // whole pinky splay

    {"F", VRCommDataAlphaEncodingKey::JoyX},      // joystick x component
    {"G", VRCommDataAlphaEncodingKey::JoyY},      // joystick y component
    {"H", VRCommDataAlphaEncodingKey::JoyBtn},    // joystick button
    {"I", VRCommDataAlphaEncodingKey::BtnTrg},    // trigger button
    {"J", VRCommDataAlphaEncodingKey::BtnA},      // A button
    {"K", VRCommDataAlphaEncodingKey::BtnB},      // B button
    {"L", VRCommDataAlphaEncodingKey::GesGrab},   // grab gesture (boolean)
    {"M", VRCommDataAlphaEncodingKey::GesPinch},  // pinch gesture (boolean)
    {"N", VRCommDataAlphaEncodingKey::BtnMenu},   // system button pressed (opens SteamVR menu)
    {"O", VRCommDataAlphaEncodingKey::BtnCalib},  // calibration button
    {"P", VRCommDataAlphaEncodingKey::TrgValue},  // analog trigger value
    {"", VRCommDataAlphaEncodingKey::Null}        // Junk key
};

static const std::map<VRCommDataAlphaEncodingKey, std::string> VRCommDataAlphaEncodingOutputKeyString{
    {VRCommDataAlphaEncodingKey::FinThumb, "A"},   // thumb force feedback
    {VRCommDataAlphaEncodingKey::FinIndex, "B"},   // index force feedback
    {VRCommDataAlphaEncodingKey::FinMiddle, "C"},  // middle force feedback
    {VRCommDataAlphaEncodingKey::FinRing, "D"},    // ring force feedback
    {VRCommDataAlphaEncodingKey::FinPinky, "E"},   // pinky force feedback

    {VRCommDataAlphaEncodingKey::OutHapticFrequency, "F"},
    {VRCommDataAlphaEncodingKey::OutHapticDuration, "G"},
    {VRCommDataAlphaEncodingKey::OutHapticAmplitude, "H"},
};

static std::map<VRCommDataAlphaEncodingKey, std::string> ParseInputToMap(const std::string& str) {
  std::map<VRCommDataAlphaEncodingKey, std::string> result;

  int i = 0;
  while (i < str.length()) {
    // Advance until we get an alphabetic character (no point in looking at values that don't have a key associated with them)

    if (str[i] >= 0 && str[i] <= 255 && IsCharacterKeyCharacter(str[i])) {
      std::string key = {str[i]};
      i++;

      // we're going to be parsing a "long key", i.e. (AB) for thumb finger splay. Long keys must always be enclosed in brackets
      if (key[0] == '(') {
        while (str[i] >= 0 && str[i] <= 255 && IsCharacterKeyCharacter(str[i]) && i < str.length()) {
          key += str[i];
          i++;
        }
      }

      std::string value = "";
      while (str[i] >= 0 && str[i] <= 255 && isdigit(str[i]) && i < str.length()) {
        value += str[i];
        i++;
      }

      // Even if the value is empty we still want to use the key, it means that we have a button that is pressed (it only appears in the packet if it
      // is)
      if (VRCommDataAlphaEncodingInputKeyString.find(key) != VRCommDataAlphaEncodingInputKeyString.end())
        result.insert_or_assign(VRCommDataAlphaEncodingInputKeyString.at(key), value);
      else
        DriverLog("Unable to insert key: %s into input map as it was not found", key.c_str());
    } else
      i++;
  }

  return result;
}

VRInputData AlphaEncodingManager::Decode(const std::string& input) {
  VRInputData result;

  std::array<float, 5> flexion = {-1.0f, -1.0f, -1.0f, -1.0f, -1.0f};
  
  // This map contains all the inputs we've got from the packet we received
  std::map<VRCommDataAlphaEncodingKey, std::string> inputMap = ParseInputToMap(input);

  // curl is 0.0f -> 1.0f inclusive
  if (inputMap.find(VRCommDataAlphaEncodingKey::FinThumb) != inputMap.end())
    flexion[0] = std::stof(inputMap.at(VRCommDataAlphaEncodingKey::FinThumb)) / configuration_.maxAnalogValue;
  if (inputMap.find(VRCommDataAlphaEncodingKey::FinIndex) != inputMap.end())
    flexion[1] = std::stof(inputMap.at(VRCommDataAlphaEncodingKey::FinIndex)) / configuration_.maxAnalogValue;
  if (inputMap.find(VRCommDataAlphaEncodingKey::FinMiddle) != inputMap.end())
    flexion[2] = std::stof(inputMap.at(VRCommDataAlphaEncodingKey::FinMiddle)) / configuration_.maxAnalogValue;
  if (inputMap.find(VRCommDataAlphaEncodingKey::FinRing) != inputMap.end())
    flexion[3] = std::stof(inputMap.at(VRCommDataAlphaEncodingKey::FinRing)) / configuration_.maxAnalogValue;
  if (inputMap.find(VRCommDataAlphaEncodingKey::FinPinky) != inputMap.end())
    flexion[4] = std::stof(inputMap.at(VRCommDataAlphaEncodingKey::FinPinky)) / configuration_.maxAnalogValue;

  // fill all the joints
  int curJoint = (int)VRCommDataAlphaEncodingKey::FinJointThumb0;
  for (int i = 0; i < 5; i++) {
    for (int k = 0; k < 4; k++) {
      VRCommDataAlphaEncodingKey joint = static_cast<VRCommDataAlphaEncodingKey>(curJoint);
      result.flexion[i][k] = inputMap.find(joint) != inputMap.end() ? (std::stof(inputMap.at(joint)) / configuration_.maxAnalogValue) : flexion[i];
      curJoint++;
    }
  }

  // splay is -1.0f -> 1.0f inclusive
  if (inputMap.find(VRCommDataAlphaEncodingKey::FinSplayThumb) != inputMap.end())
    result.splay[0] = (std::stof(inputMap.at(VRCommDataAlphaEncodingKey::FinSplayThumb)) / configuration_.maxAnalogValue - 0.5f) * 2.0f;
  if (inputMap.find(VRCommDataAlphaEncodingKey::FinSplayIndex) != inputMap.end())
    result.splay[1] = (std::stof(inputMap.at(VRCommDataAlphaEncodingKey::FinSplayIndex)) / configuration_.maxAnalogValue - 0.5f) * 2.0f;
  if (inputMap.find(VRCommDataAlphaEncodingKey::FinSplayMiddle) != inputMap.end())
    result.splay[2] = (std::stof(inputMap.at(VRCommDataAlphaEncodingKey::FinSplayMiddle)) / configuration_.maxAnalogValue - 0.5f) * 2.0f;
  if (inputMap.find(VRCommDataAlphaEncodingKey::FinSplayRing) != inputMap.end())
    result.splay[3] = (std::stof(inputMap.at(VRCommDataAlphaEncodingKey::FinSplayRing)) / configuration_.maxAnalogValue - 0.5f) * 2.0f;
  if (inputMap.find(VRCommDataAlphaEncodingKey::FinSplayPinky) != inputMap.end())
    result.splay[4] = (std::stof(inputMap.at(VRCommDataAlphaEncodingKey::FinSplayPinky)) / configuration_.maxAnalogValue - 0.5f) * 2.0f;

  // joystick axis are -1.0f -> 1.0f inclusive
  if (inputMap.find(VRCommDataAlphaEncodingKey::JoyX) != inputMap.end())
    result.joyX = 2 * std::stof(inputMap.at(VRCommDataAlphaEncodingKey::JoyX)) / configuration_.maxAnalogValue - 1;
  if (inputMap.find(VRCommDataAlphaEncodingKey::JoyY) != inputMap.end())
    result.joyY = 2 * std::stof(inputMap.at(VRCommDataAlphaEncodingKey::JoyY)) / configuration_.maxAnalogValue - 1;

  // trigger value is 0.0f -> 1.0f inclusive
  if (inputMap.find(VRCommDataAlphaEncodingKey::TrgValue) != inputMap.end())
    result.trgValue = std::stof(inputMap.at(VRCommDataAlphaEncodingKey::TrgValue)) / configuration_.maxAnalogValue;

  result.joyButton = inputMap.find(VRCommDataAlphaEncodingKey::JoyBtn) != inputMap.end();
  result.trgButton = inputMap.find(VRCommDataAlphaEncodingKey::BtnTrg) != inputMap.end();
  result.aButton = inputMap.find(VRCommDataAlphaEncodingKey::BtnA) != inputMap.end();
  result.bButton = inputMap.find(VRCommDataAlphaEncodingKey::BtnB) != inputMap.end();
  result.grab = inputMap.find(VRCommDataAlphaEncodingKey::GesGrab) != inputMap.end();
  result.pinch = inputMap.find(VRCommDataAlphaEncodingKey::GesPinch) != inputMap.end();
  result.menu = inputMap.find(VRCommDataAlphaEncodingKey::BtnMenu) != inputMap.end();
  result.calibrate = inputMap.find(VRCommDataAlphaEncodingKey::BtnCalib) != inputMap.end();

  return result;
}

std::string AlphaEncodingManager::Encode(const VROutput& input) {
  switch (input.type) {
    case VROutputDataType::ForceFeedback: {
      const VRFFBData& data = input.data.ffbData;

      return StringFormat(
          "%s%d%s%d%s%d%s%d%s%d",
          VRCommDataAlphaEncodingOutputKeyString.at(VRCommDataAlphaEncodingKey::FinThumb).c_str(),
          data.thumbCurl,
          VRCommDataAlphaEncodingOutputKeyString.at(VRCommDataAlphaEncodingKey::FinIndex).c_str(),
          data.indexCurl,
          VRCommDataAlphaEncodingOutputKeyString.at(VRCommDataAlphaEncodingKey::FinMiddle).c_str(),
          data.middleCurl,
          VRCommDataAlphaEncodingOutputKeyString.at(VRCommDataAlphaEncodingKey::FinRing).c_str(),
          data.ringCurl,
          VRCommDataAlphaEncodingOutputKeyString.at(VRCommDataAlphaEncodingKey::FinPinky).c_str(),
          data.pinkyCurl);
    }

    case VROutputDataType::Haptic: {
      const VRHapticData& data = input.data.hapticData;

      return StringFormat(
          "%s%.2f%s%.2f%s%.2f",
          VRCommDataAlphaEncodingOutputKeyString.at(VRCommDataAlphaEncodingKey::OutHapticFrequency).c_str(),
          data.frequency,
          VRCommDataAlphaEncodingOutputKeyString.at(VRCommDataAlphaEncodingKey::OutHapticDuration).c_str(),
          data.duration,
          VRCommDataAlphaEncodingOutputKeyString.at(VRCommDataAlphaEncodingKey::OutHapticAmplitude).c_str(),
          data.amplitude);
    }
  }

  return "";
}