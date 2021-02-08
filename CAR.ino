/*
 *
 */

#define DECODE_NEC          1         // etc. see IRremote.h
#define EXCLUDE_EXOTIC_PROTOCOLS      // saves around 670 bytes program space if all protocols are active

#include <IRremote.h>

int IR_RECEIVE_PIN = 11;

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);

    Serial.begin(9600);

    // Just to know which program is running on my Arduino
    Serial.println(F("START " __FILE__ " from " __DATE__ "\r\nUsing library version " VERSION_IRREMOTE));

    // In case the interrupt driver crashes on setup, give a clue
    // to the user what's going on.
    Serial.println("Enabling IRin");
    /*
     * Start the receiver, enable feedback LED and (if not 3. parameter specified) take LED feedback pin from the internal boards definition
     */
    IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);

    Serial.print(F("Ready to receive IR signals at pin "));
    Serial.println(IR_RECEIVE_PIN);
}

void loop() {
    /*
     * Check if received data is available and if yes, try to decode it.
     * Decoded result is in the IrReceiver.decodedIRData structure.
     *
     * E.g. command is in IrReceiver.decodedIRData.command
     * address is in command is in IrReceiver.decodedIRData.address
     * and up to 32 bit raw data in IrReceiver.decodedIRData.decodedRawData
     */
    if (IrReceiver.decode()) {

        // Print a short summary of received data
        //        IrReceiver.printIRResultShort(&Serial);
        //        Serial.print("......... >");
        //        Serial.print(IrReceiver.decodedIRData.rawDataPtr->rawbuf[0] * MICROS_PER_TICK);
        //        Serial.println();

        /*
         * !!!Important!!! Enable receiving of the next value,
         * since receiving has stopped after the end of the current received data packet.
         */
        IrReceiver.resume();

        /*
         * Perform required validation for received data
         * Jump back to the loop without further processing if received data is not valid 
         */
        if (IrReceiver.decodedIRData.address != 0) return; 
        if (IrReceiver.decodedIRData.protocol != NEC) return;
        if (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT && 
            IrReceiver.decodedIRData.rawDataPtr->rawbuf[0] * MICROS_PER_TICK > 32000) return;

        /*
         * Finally perform actions according to the received address and commands
         */
        if (IrReceiver.decodedIRData.command == 0x40) {
            Serial.println("Go forward");
        } else if (IrReceiver.decodedIRData.command == 0x19) {
            Serial.println("Go backward");
        } else if (IrReceiver.decodedIRData.command == 0x7) {
            Serial.println("Go left");
        } else if (IrReceiver.decodedIRData.command == 0x9) {
            Serial.println("Go right");
        } else if (IrReceiver.decodedIRData.command == 0x43) {
            Serial.println("Turn 180");
        } else if (IrReceiver.decodedIRData.command == 0x16) {
            Serial.println("Turn left 90");
        } else if (IrReceiver.decodedIRData.command == 0xD) {
            Serial.println("Turn right 90");
        } else {
            Serial.println("No action defined yet");
        }
    }
}
