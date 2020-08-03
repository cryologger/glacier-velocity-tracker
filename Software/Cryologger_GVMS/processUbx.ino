// processUbx states
enum ParseUbxSwitch {
  PARSE_UBX_SYNC_CHAR_1,
  PARSE_UBX_SYNC_CHAR_2,
  PARSE_UBX_CLASS,
  PARSE_UBX_ID,
  PARSE_UBX_LENGTH_LSB,
  PARSE_UBX_LENGTH_MSB,
  PARSE_UBX_PAYLOAD,
  PARSE_UBX_CHECKSUM_A,
  PARSE_UBX_CHECKSUM_B,
  SYNC_LOST,
  FRAME_VALID
};

// Default switch case
ParseUbxSwitch parseUbxState = PARSE_UBX_SYNC_CHAR_1;

// Global variables for UBX parsing
int ubxLength             = 0;
int ubxClass              = 0;
int ubxId                 = 0;
int ubxChecksumA          = 0;
int ubxChecksumB          = 0;
int ubxExpectedChecksumA  = 0;
int ubxExpectedChecksumB  = 0;

// Process incoming data bytes according to parseUbxState
// Only allow a new file to be opened when a complete packet has been processed and parseUbxState has returned to "PARSE_UBX_SYNC_CHAR_1"
// Or when a data error is detected (SYNC_LOST)

// UBX Frame Structure:
// Sync Char 1: 1-byte  0xB5
// Sync Char 2: 1-byte  0x62
// Class:       1-byte  Group of related messages
// ID byte:     1-byte  Defines message to follow
// Length:      2-byte  Payload only length. Little-Endian unsigned 16-bit integer
// Payload:     Variable number of bytes
// CK_A:        1-byte  16-bit checksum
// CK_B:        1-byte
// Example:     B5 62 02 15 0010 4E621058395C5C40000012000101C6BC 06 00

bool processUbx(char c)
{
  switch (parseUbxState) {
    case (PARSE_UBX_SYNC_CHAR_1):
      {
        if (c == 0xB5) // Have we found Sync Char 1 (0xB5) if we were expecting one?
        {
          parseUbxState = PARSE_UBX_SYNC_CHAR_2; // Now look for Sync Char 2 (0x62)
        }
        else
        {
          if (settings.printMajorDebugMessages)
          {
            Serial.println(F("processUbx: expecting Sync Char 0xB5 but did not receive one"));
          }
          parseUbxState = SYNC_LOST;
        }
      }
      break;
    case (PARSE_UBX_SYNC_CHAR_2):
      {
        if (c == 0x62) // Have we found Sync Char 2 (0x62) when we were expecting one?
        {
          ubxExpectedChecksumA = 0; // Reset the expected checksum
          ubxExpectedChecksumB = 0;
          parseUbxState = PARSE_UBX_CLASS; // Now look for Class byte
        }
        else
        {
          if (settings.printMajorDebugMessages)
          {
            Serial.println(F("processUbx: expecting Sync Char 0x62 but did not receive one"));
          }
          parseUbxState = SYNC_LOST;
        }
      }
      break;
    case (PARSE_UBX_CLASS):
      {
        ubxClass = c;
        ubxExpectedChecksumA = ubxExpectedChecksumA + c; // Update the expected checksum
        ubxExpectedChecksumB = ubxExpectedChecksumB + ubxExpectedChecksumA;
        parseUbxState = PARSE_UBX_ID; // Now look for ID byte
      }
      break;
    case (PARSE_UBX_ID):
      {
        ubxId = c;
        ubxExpectedChecksumA = ubxExpectedChecksumA + c; // Update the expected checksum
        ubxExpectedChecksumB = ubxExpectedChecksumB + ubxExpectedChecksumA;
        parseUbxState = PARSE_UBX_LENGTH_LSB; // Now look for length LSB
      }
      break;
    case (PARSE_UBX_LENGTH_LSB):
      {
        ubxLength = c; // Store the length LSB
        ubxExpectedChecksumA = ubxExpectedChecksumA + c; // Update the expected checksum
        ubxExpectedChecksumB = ubxExpectedChecksumB + ubxExpectedChecksumA;
        parseUbxState = PARSE_UBX_LENGTH_MSB; // Now look for length MSB
      }
      break;
    case (PARSE_UBX_LENGTH_MSB):
      {
        ubxLength = ubxLength + (c * 256); // Add the length MSB
        ubxExpectedChecksumA = ubxExpectedChecksumA + c; // Update the expected checksum
        ubxExpectedChecksumB = ubxExpectedChecksumB + ubxExpectedChecksumA;
        parseUbxState = PARSE_UBX_PAYLOAD; // Now look for payload bytes (length: ubxLength)
      }
      break;
    case (PARSE_UBX_PAYLOAD):
      {
        // TO DO: extract useful things from the incoming message based on ubxClass, ubxId and ubxLength (byte count)
        ubxLength = ubxLength - 1; // Decrement length by one
        ubxExpectedChecksumA = ubxExpectedChecksumA + c; // Update the expected checksum
        ubxExpectedChecksumB = ubxExpectedChecksumB + ubxExpectedChecksumA;
        if (ubxLength == 0)
        {
          ubxExpectedChecksumA = ubxExpectedChecksumA & 0xff; // Limit checksums to 8-bits
          ubxExpectedChecksumB = ubxExpectedChecksumB & 0xff;
          parseUbxState = PARSE_UBX_CHECKSUM_A; // If we have received length payload bytes, look for checksum bytes
        }
      }
      break;
    case (PARSE_UBX_CHECKSUM_A):
      {
        ubxChecksumA = c;
        parseUbxState = PARSE_UBX_CHECKSUM_B;
      }
      break;
    case (PARSE_UBX_CHECKSUM_B):
      {
        ubxChecksumB = c;
        parseUbxState = PARSE_UBX_SYNC_CHAR_1; // All bytes received so go back to looking for a new Sync Char 1 unless there is a checksum error
        if ((ubxExpectedChecksumA != ubxChecksumA) or (ubxExpectedChecksumB != ubxChecksumB))
        {
          if (settings.printMajorDebugMessages)
          {
            Serial.println(F("processUbx: UBX checksum error"));
          }
          parseUbxState = SYNC_LOST;
        }
        else
        {
          if (settings.enableTerminalOutput)
          {
            // Print RTC's date and time
            rtc.getTime();
            Serial.printf("20%02d-%02d-%02d %02d:%02d:%02d.%02d ",
                          rtc.year, rtc.month, rtc.dayOfMonth,
                          rtc.hour, rtc.minute, rtc.seconds, rtc.hundredths);

            // Print UBX frame information
            Serial.printf("UBX Class: 0x%02X ID: 0x%02X", ubxClass, ubxId);

            switch (ubxClass)
            {
              case UBX_CLASS_NAV:
                {
                  switch (ubxId)
                  {
                    case UBX_NAV_CLOCK:
                      Serial.print(F(" NAV-CLOCK"));
                      break;
                    case UBX_NAV_HPPOSECEF:
                      Serial.print(F(" NAV-HPPOSECEF"));
                      break;
                    case UBX_NAV_HPPOSLLH:
                      Serial.print(F(" NAV-HPPOSLLH"));
                      break;
                    case UBX_NAV_ODO:
                      Serial.print(F(" NAV-ODO"));
                      break;
                    case UBX_NAV_POSECEF:
                      Serial.print(F(" NAV-POSECEF"));
                      break;
                    case UBX_NAV_POSLLH:
                      Serial.print(F(" NAV-POSLLH"));
                      break;
                    case UBX_NAV_PVT:
                      Serial.print(F(" NAV-PVT"));
                      break;
                    case UBX_NAV_RELPOSNED:
                      Serial.print(F(" NAV-RELPOSNED"));
                      break;
                    case UBX_NAV_STATUS:
                      Serial.print(F(" NAV-STATUS"));
                      break;
                    case UBX_NAV_TIMEUTC:
                      Serial.print(F(" NAV-TIMEUTC"));
                      break;
                    case UBX_NAV_VELECEF:
                      Serial.print(F(" NAV-VELECEF"));
                      break;
                    case UBX_NAV_VELNED:
                      Serial.print(F(" NAV-VELNED"));
                      break;
                  }
                }
                break;
              case UBX_CLASS_RXM:
                {
                  switch (ubxId)
                  {
                    case UBX_RXM_RAWX:
                      Serial.print(F(" RXM-RAWX"));
                      break;
                    case UBX_RXM_SFRBX:
                      Serial.print(F(" RXM-SFRBX"));
                      break;
                  }
                }
                break;
              case UBX_CLASS_TIM:
                {
                  switch (ubxId)
                  {
                    case UBX_TIM_TM2:
                      Serial.print(F(" TIM-TM2"));
                      break;
                  }
                }
                break;
            }
            Serial.println();
          }
          parseUbxState = FRAME_VALID;
        }
      }
      break;
  }
  return (true);
}
