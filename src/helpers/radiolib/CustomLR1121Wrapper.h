#pragma once

#include "CustomLR1121.h"
#include "RadioLibWrappers.h"
#include "LR11x0Reset.h"

#ifndef USE_LR1121
#define USE_LR1121
#endif

class CustomLR1121Wrapper : public RadioLibWrapper {
public:
  CustomLR1121Wrapper(CustomLR1121& radio, mesh::MainBoard& board) : RadioLibWrapper(radio, board) { }
  void doResetAGC() override { lr11x0ResetAGC((LR11x0 *)_radio, ((CustomLR1121 *)_radio)->getFreqMHz()); }

  void setParams(float freq, float bw, uint8_t sf, uint8_t cr) override {
    auto* r = (CustomLR1121*)_radio;
    r->setFrequency(freq);
    r->setBandwidth(bw);
    r->setSpreadingFactor(sf);
    r->setCodingRate(cr);
    updatePreamble(sf);
  }

  bool isReceivingPacket() override { 
    return ((CustomLR1121 *)_radio)->isReceiving();
  }
  float getCurrentRSSI() override {
    float rssi = -110;
    ((CustomLR1121 *)_radio)->getRssiInst(&rssi);
    return rssi;
  }

  uint8_t getSpreadingFactor() const override {
    return ((CustomLR1121*)_radio)->getSpreadingFactor();
  }

  void onSendFinished() override {
    RadioLibWrapper::onSendFinished();
    _radio->setPreambleLength(preambleLengthForSF(getSpreadingFactor()));
  }

  float getLastRSSI() const override { return ((CustomLR1121 *)_radio)->getRSSI(); }
  float getLastSNR() const override { return ((CustomLR1121 *)_radio)->getSNR(); }
  
  void setRxBoostedGainMode(bool en) override {
    ((CustomLR1121 *)_radio)->setRxBoostedGainMode(en);
  }
  bool getRxBoostedGainMode() const override {
    return ((CustomLR1121 *)_radio)->getRxBoostedGainMode();
  }
  void setTxPower(int8_t dbm) {
    auto* r = (CustomLR1121*)_radio;
    r->setOutputPower(dbm, true, 48);
  }
};
