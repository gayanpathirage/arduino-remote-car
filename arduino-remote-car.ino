/*
  MIT License

  Copyright (c) 2021 Gayan Pathirage

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.MIT License
*/

#define DECODE_NEC          1         // etc. see IRremote.h
#define EXCLUDE_EXOTIC_PROTOCOLS      // saves around 670 bytes program space if all protocols are active

#include <IRremote.h>

int IR_RECEIVE_PIN = 11;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);

  Serial.begin(9600);

  // Just to know which program is running on my Arduino
  Serial.println(F("START " __FILE__ " from " __DATE__ "\r\nUsing library version " VERSION_IRREMOTE));

  // In case the interrupt driver crashes on setup, give a clue
  // to the user what's going on.
  Serial.println("Enabling IRin");
  /*
     Start the receiver, enable feedback LED and (if not 3. parameter specified) take LED feedback pin from the internal boards definition
  */
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);

  Serial.print(F("Ready to receive IR signals at pin "));
  Serial.println(IR_RECEIVE_PIN);
}

void loop() {
  /*
     Check if received data is available and if yes, try to decode it.
     Decoded result is in the IrReceiver.decodedIRData structure.

     E.g. command is in IrReceiver.decodedIRData.command
     address is in command is in IrReceiver.decodedIRData.address
     and up to 32 bit raw data in IrReceiver.decodedIRData.decodedRawData
  */
  if (IrReceiver.decode()) {

    // Print a short summary of received data
    // Uncomment when debugging
    // IrReceiver.printIRResultShort(&Serial);


    /*
       !!!Important!!! Enable receiving of the next value,
       since receiving has stopped after the end of the current received data packet.
    */
    IrReceiver.resume();

    /*
       Perform required validation for received data
       Jump back to the loop without further processing if received data is not valid
    */
    if (IrReceiver.decodedIRData.address != 0) return;
    if (IrReceiver.decodedIRData.protocol != NEC) return;
    if (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT &&
        IrReceiver.decodedIRData.rawDataPtr->rawbuf[0] * MICROS_PER_TICK > 32000) return;

    /*
       Finally perform actions according to the received address and commands
    */
    if (IrReceiver.decodedIRData.command == 0x40) {
      Serial.println("Go forward");
      goForward(1);
    } else if (IrReceiver.decodedIRData.command == 0x19) {
      Serial.println("Go backward");
      goBackward(1);
    } else if (IrReceiver.decodedIRData.command == 0x7) {
      Serial.println("Go left");
      turnLeft(10);
    } else if (IrReceiver.decodedIRData.command == 0x9) {
      Serial.println("Go right");
      turnRight(10);
    } else if (IrReceiver.decodedIRData.command == 0x43) {
      Serial.println("Turn 180");
      turnRight(180);
    } else if (IrReceiver.decodedIRData.command == 0x16) {
      Serial.println("Turn left 90");
      turnLeft(90);
    } else if (IrReceiver.decodedIRData.command == 0xD) {
      Serial.println("Turn right 90");
      turnRight(90);
    } else {
      Serial.println("No action defined yet");
    }
  }
}

void turnLeft(int deg) {
  resetMotion();

  //turn given degrees
  digitalWrite(4 , HIGH);
  delay(90 * (deg / 10)); //assuming 90 degrees takes 630 miliseconds)

  //stop turning
  digitalWrite(4 , LOW);
}

void turnRight(int deg) {
  resetMotion();

  //turn given degrees
  digitalWrite(6 , HIGH);
  delay(90 * (deg / 10)); //assuming 90 degrees takes 630 miliseconds)

  //stop turning
  digitalWrite(6 , LOW);
}

void goBackward(int steps) {
  resetMotion();

  // move for given period
  digitalWrite(4, HIGH);
  digitalWrite(6 , HIGH);
  delay(100 * steps);

  // Now  stop motion
  digitalWrite(4, LOW);
  digitalWrite(6 , LOW);
}

void goForward(int steps) {
  resetMotion();

  // move for given period
  digitalWrite(5, HIGH);
  digitalWrite(7 , HIGH);
  delay(100 * steps);

  // Now  stop motion
  digitalWrite(5, LOW);
  digitalWrite(7 , LOW);
}

void resetMotion() {
  digitalWrite(4, LOW);
  digitalWrite(5, LOW);
  digitalWrite(6, LOW);
  digitalWrite(7, LOW);
  delay(10);  // pause between motion to protect the motor
}
