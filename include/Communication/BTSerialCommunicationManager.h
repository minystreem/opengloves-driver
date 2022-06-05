#pragma once

#include <WinSock2.h>
#include <bluetoothapis.h>

#include <atomic>
#include <memory>
#include <string>

#include "Communication/CommunicationManager.h"
#include "DeviceConfiguration.h"
#include "Encode/EncodingManager.h"

class BTSerialCommunicationManager : public CommunicationManager {
 public:
  BTSerialCommunicationManager(const VRCommunicationConfiguration& configuration, std::unique_ptr<EncodingManager> encodingManager);

  bool IsConnected() override;

 protected:
  bool Connect() override;
  bool DisconnectFromDevice() override;
  void LogError(const char* message) override;
  void LogMessage(const char* message) override;
  bool ReceiveNextPacket(std::string& buff) override;
  bool SendMessageToDevice() override;

 private:
  bool ConnectToDevice(const BTH_ADDR& deviceBtAddress);
  bool GetPairedDeviceBtAddress(BTH_ADDR* deviceBtAddress);
  bool StartupWindowsSocket();

  VRCommunicationBTSerialConfiguration btSerialConfiguration_;

  std::atomic<bool> isConnected_ = false;

  std::atomic<SOCKET> btClientSocket_ = NULL;
};