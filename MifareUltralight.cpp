#include "MifareUltralight.h"

#define ULTRALIGHT_PAGE_SIZE 4
byte ultraLightReadSize = 18;
#define ULTRALIGHT_DATA_START_PAGE 4
#define ULTRALIGHT_MESSAGE_LENGTH_INDEX 1
#define ULTRALIGHT_DATA_START_INDEX 2
#define ULTRALIGHT_MAX_PAGE 63

#define NFC_FORUM_TAG_TYPE_2 ("NFC Forum Type 2")

//#define MIFARE_ULTRALIGHT_DEBUG 1

/**
 *
 * From: https://github.com/miguelbalboa/rfid/blob/master/src/MFRC522.h
 *  * MIFARE Ultralight (MF0ICU1):
 * 		Has 16 pages of 4 bytes = 64 bytes.
 * 		Pages 0 + 1 is used for the 7-byte UID.
 * 		Page 2 contains the last check digit for the UID, one byte
 * manufacturer internal data, and the lock bytes (see
 * http://www.nxp.com/documents/data_sheet/MF0ICU1.pdf section 8.5.2) Page 3 is
 * OTP, One Time Programmable bits. Once set to 1 they cannot revert to 0. Pages
 * 4-15 are read/write unless blocked by the lock bytes in page 2. MIFARE
 * Ultralight C (MF0ICU2): Has 48 pages of 4 bytes = 192 bytes. Pages 0 + 1 is
 * used for the 7-byte UID. Page 2 contains the last check digit for the UID,
 * one byte manufacturer internal data, and the lock bytes (see
 * http://www.nxp.com/documents/data_sheet/MF0ICU1.pdf section 8.5.2) Page 3 is
 * OTP, One Time Programmable bits. Once set to 1 they cannot revert to 0. Pages
 * 4-39 are read/write unless blocked by the lock bytes in page 2. Page 40 Lock
 * bytes Page 41 16 bit one way counter Pages 42-43 Authentication configuration
 * 		Pages 44-47 Authentication key
 */

namespace ndef_mfrc522 {
MifareUltralight::MifareUltralight(MFRC522 &nfcShield) {
  nfc = &nfcShield;
  ndefStartIndex = 0;
  messageLength = 0;
}

MifareUltralight::~MifareUltralight() {}

NfcTag MifareUltralight::read() {
  unsigned int uidLength = 7;
  byte *uid = nfc->uid.uidByte;

  if (isUnformatted()) {
#ifdef NDEF_USE_SERIAL
    Serial.println(F("WARNING: Tag is not formatted."));
#endif
    return NfcTag(uid, uidLength, NFC_FORUM_TAG_TYPE_2);
  }

  readCapabilityContainer(); // meta info for tag
  findNdefMessage();
  calculateBufferSize();

  if (messageLength == 0) { // data is 0x44 0x03 0x00 0xFE
    NdefMessage message = NdefMessage();
    message.addEmptyRecord();
    return NfcTag(uid, uidLength, NFC_FORUM_TAG_TYPE_2, message);
  }

  int success;
  uint8_t page;
  uint8_t index = 0;
  byte buffer[bufferSize];
  for (page = ULTRALIGHT_DATA_START_PAGE; page < ULTRALIGHT_MAX_PAGE; page++) {
    // read the data
    success = nfc->MIFARE_Read(page, &buffer[index], &ultraLightReadSize);
    if (success == MFRC522::StatusCode::STATUS_OK) {
#ifdef MIFARE_ULTRALIGHT_DEBUG
      Serial.print(F("Page "));
      Serial.print(page);
      Serial.print(" ");
      PrintHexChar(&buffer[index], ULTRALIGHT_PAGE_SIZE);
      Serial.print(F("Read buffer size is "));
      Serial.println(bufferSize);
#endif
    } else {
#ifdef NDEF_USE_SERIAL
      Serial.print(F("Read failed on page "));
      Serial.println(page);
#endif
      // TODO error handling
      messageLength = 0;
      break;
    }

    if (index >= (messageLength + ndefStartIndex)) {
      break;
    }

    index += ULTRALIGHT_PAGE_SIZE;
  }

  NdefMessage ndefMessage = NdefMessage(&buffer[ndefStartIndex], messageLength);
  return NfcTag(uid, uidLength, NFC_FORUM_TAG_TYPE_2, ndefMessage);
}

boolean MifareUltralight::isUnformatted() {
  uint8_t page = 4;
  byte data[ultraLightReadSize];
  int status = nfc->MIFARE_Read(page, data, &ultraLightReadSize);
  if (status == MFRC522::StatusCode::STATUS_OK) {
    return (data[0] == 0xFF && data[1] == 0xFF && data[2] == 0xFF &&
            data[3] == 0xFF);
  } else {
#ifdef NDEF_USE_SERIAL
    Serial.print(F("Error. Failed read page "));
    Serial.println(page);
#endif
    return false;
  }
}

// page 3 has tag capabilities
void MifareUltralight::readCapabilityContainer() {
  byte data[ultraLightReadSize];
  byte size = sizeof(data);
  int success = nfc->MIFARE_Read(3, data, &size);
#ifdef MIFARE_ULTRALIGHT_DEBUG
  PrintHex(data, ultraLightReadSize);
  Serial.print(F("Status of capabilities read: "));
  Serial.println(success);
  Serial.println(nfc->GetStatusCodeName(success));
#endif
  if (success == MFRC522::StatusCode::STATUS_OK) {
    // See AN1303 - different rules for Mifare Family byte2 = (additional data +
    // 48)/8
    tagCapacity = data[2] * 8;
#ifdef MIFARE_ULTRALIGHT_DEBUG
    Serial.print(F("Tag capacity "));
    Serial.print(tagCapacity);
    Serial.println(F(" bytes"));
#endif

    // TODO future versions should get lock information
  }
}

// read enough of the message to find the ndef message length
void MifareUltralight::findNdefMessage() {
  byte data[ultraLightReadSize];
  byte size = sizeof(data);
  int status =
      nfc->MFRC522::MIFARE_Read(ULTRALIGHT_DATA_START_PAGE, data, &size);

#ifdef MIFARE_ULTRALIGHT_DEBUG
  PrintHexChar(data, ULTRALIGHT_PAGE_SIZE);
#endif

  if (status == MFRC522::StatusCode::STATUS_OK) {
    if (data[0] == 0x03) {
      messageLength = data[1];
      ndefStartIndex = 2;
    } else if (data[5] == 0x3) // page 5 byte 1
    {
      // TODO should really read the lock control TLV to ensure byte[5] is
      // correct
      messageLength = data[6];
      ndefStartIndex = 7;
    }

#ifdef MIFARE_ULTRALIGHT_DEBUG
    Serial.print(F("Message length: "));
    Serial.println(messageLength);
    Serial.print(F("NDEF Start Index: "));
    Serial.println(ndefStartIndex);
#endif
  } else {
#ifdef MIFARE_ULTRALIGHT_DEBUG
    Serial.print(F("Find NdefMessage read failed: "));
    Serial.println(nfc->GetStatusCodeName(status));
#endif
  }

#ifdef MIFARE_ULTRALIGHT_DEBUG
  Serial.print(F("messageLength "));
  Serial.println(messageLength);
  Serial.print(F("ndefStartIndex "));
  Serial.println(ndefStartIndex);
#endif
} // namespace ndef_mfrc522

// buffer is larger than the message, need to handle some data before and after
// message and need to ensure we read full pages
void MifareUltralight::calculateBufferSize() {

  // TLV terminator 0xFE is 1 byte
  bufferSize = messageLength + ndefStartIndex + 1;

  if (bufferSize % ultraLightReadSize != 0) {
    // buffer must be an increment of page size
    bufferSize = ((bufferSize / ultraLightReadSize) + 1) * ultraLightReadSize;
  }

#ifdef MIFARE_ULTRALIGHT_DEBUG
  Serial.print(F("Buffer size is "));
  Serial.println(bufferSize);
#endif
}

boolean MifareUltralight::write(NdefMessage &m) {
  if (isUnformatted()) {
#ifdef NDEF_USE_SERIAL
    Serial.println(F("WARNING: Tag is not formatted."));
#endif
    return false;
  }
  readCapabilityContainer(); // meta info for tag

  messageLength = m.getEncodedSize();
  ndefStartIndex = messageLength < 0xFF ? 2 : 4;
  calculateBufferSize();

  if (bufferSize > tagCapacity) {
    Serial.print(F("Encoded Message length exceeded tag Capacity "));
    Serial.println(tagCapacity);
    return false;
  }

  uint8_t encoded[bufferSize];
  uint8_t *src = encoded;
  unsigned int position = 0;
  uint8_t page = ULTRALIGHT_DATA_START_PAGE;

  // Set message size. With ultralight should always be less than 0xFF but who
  // knows?

  encoded[0] = 0x3;
  if (messageLength < 0xFF) {
    encoded[1] = messageLength;
  } else {
    encoded[1] = 0xFF;
    encoded[2] = ((messageLength >> 8) & 0xFF);
    encoded[3] = (messageLength & 0xFF);
  }

  m.encode(encoded + ndefStartIndex);
  // this is always at least 1 byte copy because of terminator.
  memset(encoded + ndefStartIndex + messageLength, 0,
         bufferSize - ndefStartIndex - messageLength);
  encoded[ndefStartIndex + messageLength] = 0xFE; // terminator
#ifdef MIFARE_ULTRALIGHT_DEBUG

  Serial.print(F("messageLength "));
  Serial.println(messageLength);
  Serial.print(F("Tag Capacity "));
  Serial.println(tagCapacity);
  Serial.print(F("bufferSize "));
  Serial.println(bufferSize);
  Serial.print(F("position "));
  Serial.println(position);
#endif
  while (position < bufferSize) { // bufferSize is always times pagesize so no
    // "last chunk" check
    // write page
    //	StatusCode MIFARE_Ultralight_Write(byte page, byte *buffer, byte
    // bufferSize);
    byte writeStatus =
        nfc->MIFARE_Ultralight_Write(page, src, ULTRALIGHT_PAGE_SIZE);
    if (writeStatus != MFRC522::StatusCode::STATUS_OK) {
#ifdef MIFARE_ULTRALIGHT_DEBUG
      Serial.print(F("Write Status: "));
      Serial.println(nfc->GetStatusCodeName(writeStatus));
      Serial.print(F("Failed to write page "));
      Serial.println(page);
#endif
      return false;
    }
#ifdef MIFARE_ULTRALIGHT_DEBUG
    Serial.print(F("Wrote page "));
    Serial.print(page);
    Serial.print(F(" - "));
#endif
    page++;
    src += ULTRALIGHT_PAGE_SIZE;
    position += ULTRALIGHT_PAGE_SIZE;
  }
  return true;
}

// Mifare Ultralight can't be reset to factory state
// zero out tag data like the NXP Tag Write Android application
boolean MifareUltralight::clean() {
  readCapabilityContainer(); // meta info for tag

  uint8_t pages =
      (tagCapacity / ULTRALIGHT_PAGE_SIZE) + ULTRALIGHT_DATA_START_PAGE;

  // factory tags have 0xFF, but OTP-CC blocks have already been set so we use
  // 0x00
  uint8_t data[4] = {0x00, 0x00, 0x00, 0x00};

  for (int i = ULTRALIGHT_DATA_START_PAGE; i < pages; i++) {
#ifdef MIFARE_ULTRALIGHT_DEBUG
    Serial.print(F("Wrote page "));
    Serial.print(i);
    Serial.print(F(" - "));
    PrintHex(data, ULTRALIGHT_PAGE_SIZE);
#endif
    if (!nfc->MIFARE_Ultralight_Write(i, data, ULTRALIGHT_PAGE_SIZE)) {
      return false;
    }
  }
  return true;
}

} // namespace ndef_mfrc522