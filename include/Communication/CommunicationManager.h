#pragma once

#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

#include "DeviceConfiguration.h"
#include "Encode/EncodingManager.h"

class CommunicationManager {
 public:
  explicit CommunicationManager(VRCommunicationConfiguration configuration);
  CommunicationManager(VRCommunicationConfiguration configuration, std::unique_ptr<EncodingManager> encodingManager);

  virtual void BeginListener(const std::function<void(VRInputData)>& callback);
  virtual void Disconnect();
  virtual void QueueSend(const VROutput& data);

  virtual bool IsConnected() = 0;

 protected:
  virtual void ListenerThread(const std::function<void(VRInputData)>& callback);
  virtual void WaitAttemptConnection();

  virtual bool Connect() = 0;

  virtual void PrepareDisconnection(){};
  virtual bool DisconnectFromDevice() = 0;
  virtual void LogError(const char* message) = 0;
  virtual void LogMessage(const char* message) = 0;
  virtual bool ReceiveNextPacket(std::string& buff) = 0;
  virtual bool SendMessageToDevice() = 0;

  VRCommunicationConfiguration configuration_;
  std::unique_ptr<EncodingManager> encodingManager_;

  std::atomic<bool> threadActive_;
  std::thread thread_;

  std::mutex writeMutex_;
  std::string writeString_;
};