// processUBX states
#define looking_for_B5          0
#define looking_for_62          1
#define looking_for_class       2
#define looking_for_ID          3
#define looking_for_length_LSB  4
#define looking_for_length_MSB  5
#define processing_payload      6
#define looking_for_checksum_A  7
#define looking_for_checksum_B  8
#define sync_lost               9
#define frame_valid             10

// Global variables for UBX parsing
int ubx_state               = looking_for_B5;
int ubx_length              = 0;
int ubx_class               = 0;
int ubx_ID                  = 0;
int ubx_checksum_A          = 0;
int ubx_checksum_B          = 0;
int ubx_expected_checksum_A = 0;
int ubx_expected_checksum_B = 0;

// Process incoming data bytes according to ubx_state
// Only allow a new file to be opened when a complete packet has been processed and ubx_state has returned to "looking_for_B5"
// Or when a data error is detected (sync_lost)

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

bool processUBX(char c)
{
  switch (ubx_state) {
    case (looking_for_B5):
      {
        if (c == 0xB5) // Have we found Sync Char 1 (0xB5) if we were expecting one?
        {
          ubx_state = looking_for_62; // Now look for Sync Char 2 (0x62)
        }
        else
        {
          if (settings.printMajorDebugMessages)
          {
            Serial.println(F("processUBX: expecting Sync Char 0xB5 but did not receive one"));
          }
          ubx_state = sync_lost;
        }
      }
      break;
    case (looking_for_62):
      {
        if (c == 0x62) // Have we found Sync Char 2 (0x62) when we were expecting one?
        {
          ubx_expected_checksum_A = 0; // Reset the expected checksum
          ubx_expected_checksum_B = 0;
          ubx_state = looking_for_class; // Now look for Class byte
        }
        else
        {
          if (settings.printMajorDebugMessages)
          {
            Serial.println(F("processUBX: expecting Sync Char 0x62 but did not receive one"));
          }
          ubx_state = sync_lost;
        }
      }
      break;
    case (looking_for_class):
      {
        ubx_class = c;
        ubx_expected_checksum_A = ubx_expected_checksum_A + c; // Update the expected checksum
        ubx_expected_checksum_B = ubx_expected_checksum_B + ubx_expected_checksum_A;
        ubx_state = looking_for_ID; // Now look for ID byte
      }
      break;
    case (looking_for_ID):
      {
        ubx_ID = c;
        ubx_expected_checksum_A = ubx_expected_checksum_A + c; // Update the expected checksum
        ubx_expected_checksum_B = ubx_expected_checksum_B + ubx_expected_checksum_A;
        ubx_state = looking_for_length_LSB; // Now look for length LSB
      }
      break;
    case (looking_for_length_LSB):
      {
        ubx_length = c; // Store the length LSB
        ubx_expected_checksum_A = ubx_expected_checksum_A + c; // Update the expected checksum
        ubx_expected_checksum_B = ubx_expected_checksum_B + ubx_expected_checksum_A;
        ubx_state = looking_for_length_MSB; // Now look for length MSB
      }
      break;
    case (looking_for_length_MSB):
      {
        ubx_length = ubx_length + (c * 256); // Add the length MSB
        ubx_expected_checksum_A = ubx_expected_checksum_A + c; // Update the expected checksum
        ubx_expected_checksum_B = ubx_expected_checksum_B + ubx_expected_checksum_A;
        ubx_state = processing_payload; // Now look for payload bytes (length: ubx_length)
      }
      break;
    case (processing_payload):
      {
        // TO DO: extract useful things from the incoming message based on ubx_class, ubx_ID and ubx_length (byte count)
        ubx_length = ubx_length - 1; // Decrement length by one
        ubx_expected_checksum_A = ubx_expected_checksum_A + c; // Update the expected checksum
        ubx_expected_checksum_B = ubx_expected_checksum_B + ubx_expected_checksum_A;
        if (ubx_length == 0)
        {
          ubx_expected_checksum_A = ubx_expected_checksum_A & 0xff; // Limit checksums to 8-bits
          ubx_expected_checksum_B = ubx_expected_checksum_B & 0xff;
          ubx_state = looking_for_checksum_A; // If we have received length payload bytes, look for checksum bytes
        }
      }
      break;
    case (looking_for_checksum_A):
      {
        ubx_checksum_A = c;
        ubx_state = looking_for_checksum_B;
      }
      break;
    case (looking_for_checksum_B):
      {
        ubx_checksum_B = c;
        ubx_state = looking_for_B5; // All bytes received so go back to looking for a new Sync Char 1 unless there is a checksum error
        if ((ubx_expected_checksum_A != ubx_checksum_A) or (ubx_expected_checksum_B != ubx_checksum_B))
        {
          if (settings.printMajorDebugMessages)
          {
            Serial.println(F("processUBX: UBX checksum error"));
          }
          ubx_state = sync_lost;
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
            Serial.printf("UBX Class: 0x%02X ID: 0x%02X", ubx_class, ubx_ID);

            switch (ubx_class)
            {
              case UBX_CLASS_NAV:
                {
                  switch (ubx_ID)
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
                  switch (ubx_ID)
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
                  switch (ubx_ID)
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
          ubx_state = frame_valid;
        }
      }
      break;
  }
  return (true);
}
