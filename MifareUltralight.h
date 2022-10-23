#ifndef MifareUltralight_h
#define MifareUltralight_h

#include "Ndef.h"
#include "NfcTag.h"
#include <MFRC522.h>

namespace ndef_mfrc522 {
class MifareUltralight;
} 

class ndef_mfrc522::MifareUltralight {
public:
  MifareUltralight(MFRC522 &nfcShield);
  ~MifareUltralight();
  NfcTag read();
  boolean write(NdefMessage &ndefMessage);
  boolean clean();

private:
  MFRC522 *nfc;
  unsigned int tagCapacity;
  unsigned int messageLength;
  unsigned int bufferSize;
  unsigned int ndefStartIndex;
  boolean isUnformatted();
  void readCapabilityContainer();
  void findNdefMessage();
  void calculateBufferSize();
};

#endif
