#pragma once

#include "CustomLR1121.h"
#include "RadioLibWrappers.h"
#include "LR11x0Reset.h"

class CustomLR1121Wrapper : public RadioLibWrapper {
public:
  CustomLR1121Wrapper(CustomLR1121 &radio, mesh::MainBoard &board)
      : RadioLibWrapper(radio, board) {}

  void setParams(float freq, float bw, uint8_t sf, uint8_t cr) override {
    CustomLR1121 *r = (CustomLR1121 *)_radio;
    r->setFrequency(freq);
    r->setSpreadingFactor(sf);
    r->setBandwidth(bw);
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

  float getLastRSSI() const override {
    return ((CustomLR1121 *)_radio)->getRSSI();
  }

  float getLastSNR() const override {
    return ((CustomLR1121 *)_radio)->getSNR();
  }

  uint8_t getSpreadingFactor() const override {
    return ((CustomLR1121 *)_radio)->getSpreadingFactor();
  }

  float packetScore(float snr, int packet_len) override {
    int sf = ((CustomLR1121 *)_radio)->getSpreadingFactor();
    return packetScoreInt(snr, sf, packet_len);
  }

  void onSendFinished() override {
    RadioLibWrapper::onSendFinished();
    _radio->setPreambleLength(
        preambleLengthForSF(getSpreadingFactor()));
  }

  void doResetAGC() override {
    CustomLR1121 *r = (CustomLR1121 *)_radio;
    float freq = r->getFreqMHz();

    r->standby(RADIOLIB_LR11X0_STANDBY_RC, true);
    r->calibrate(RADIOLIB_LR11X0_CALIBRATE_ALL);
    r->setFrequency(freq);
    r->setRxBoostedGainMode(r->getRxBoostedGainMode());
  }

  void setRxBoostedGainMode(bool en) override {
    ((CustomLR1121 *)_radio)->setRxBoostedGainMode(en);
  }

  bool getRxBoostedGainMode() const override {
    return ((CustomLR1121 *)_radio)->getRxBoostedGainMode();
  }

  void powerOff() override {
    ((CustomLR1121 *)_radio)->sleep(false, 0);
  }
};
