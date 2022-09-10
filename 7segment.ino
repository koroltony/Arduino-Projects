
const int dataPin = 8;
const int clockPin = 10;
const int latchPin = 9;
const int digit0pin = 5;
const int digit1pin = 4;
const int digit2pin = 3;
const int digit3pin = 2;
const int buttonPin = 11;
const int analogPin = A1; // read from multiplexer using analog input A1
const int strobePin = 6; // strobe is attached to digital pin 6
const int resetPin = 7; // reset is attached to digital pin 7
int spectrumValue [7]; // array to hold analog values for the 7 bands

int digit2segments [] = {
  0b00111111 ,
  0b00000110 ,
  0b01011011 ,
  0b01001111 ,
  0b01100110 ,
  0b01101101 ,
  0b01111101 ,
  0b00000111 ,
  0b01111111 ,
  0b01101111
};
void setup ()
{

  pinMode ( strobePin , OUTPUT );
  pinMode ( analogPin , INPUT );
  pinMode ( resetPin , OUTPUT );
  pinMode (A2 , OUTPUT);
  pinMode (A3 , OUTPUT);
  pinMode (A4 , OUTPUT);
  pinMode (A5 , OUTPUT);

  // initial states for reset and strobe pins
  digitalWrite ( resetPin , LOW );
  digitalWrite ( strobePin , HIGH );

  pinMode ( latchPin , OUTPUT );
  pinMode ( dataPin , OUTPUT );
  pinMode ( clockPin , OUTPUT );
  pinMode ( digit0pin , OUTPUT );
  pinMode ( digit1pin , OUTPUT );
  pinMode ( digit2pin , OUTPUT );
  pinMode ( digit3pin , OUTPUT );
  pinMode ( buttonPin , INPUT);
  Serial.begin(9600);
  digitalWrite(A2, LOW);
  digitalWrite(A3, LOW);
  digitalWrite(A4, LOW);
  digitalWrite(A5, LOW);
}

//The MSGEQ7 provides a frequency band which is mapped to LED's and a buzzer based on intensity
//To scale the frequency values to our tolerance, we divided it by 60
void alarm (long freq1) {
  Serial.print((freq1) / 60);
  Serial.print(" ");

  if (((freq1) / 50) <= 3) {
    digitalWrite(A2, LOW);
    digitalWrite(A3, LOW);
    digitalWrite(A4, LOW);
    digitalWrite(A5, LOW);
    noTone(12);
  }
  else if (((freq1) / 50) <= 9) {
    digitalWrite(A2, HIGH);
    digitalWrite(A3, LOW);
    digitalWrite(A4, LOW);
    digitalWrite(A5, LOW);
    noTone(12);
  }
  else if (((freq1) / 50) <= 12) {
    digitalWrite(A2, HIGH);
    digitalWrite(A3, HIGH);
    digitalWrite(A4, LOW);
    digitalWrite(A5, LOW);
    tone(12, 3000);
  }
  else if (((freq1) / 50) <= 14) {
    digitalWrite(A2, HIGH);
    digitalWrite(A3, HIGH);
    digitalWrite(A4, HIGH);
    digitalWrite(A5, LOW);
    //add tone
    tone(12, 3000);

  }
  else {
    digitalWrite(A2, HIGH);
    digitalWrite(A3, HIGH);
    digitalWrite(A4, HIGH);
    digitalWrite(A5, HIGH);
    tone(12, 3000);
  }
}

void writeDigit (int value , int digit )
{

  // turn off all digits
  digitalWrite ( digit0pin , HIGH );
  digitalWrite ( digit1pin , HIGH );
  digitalWrite ( digit2pin , HIGH );
  digitalWrite ( digit3pin , HIGH );
  // write data to shift register
  digitalWrite ( latchPin , LOW );
  shiftOut ( dataPin , clockPin , MSBFIRST , digit2segments [ value ]);
  digitalWrite ( latchPin , HIGH );
  // turn on only desired digit
  switch ( digit ) {
    case 0:
      digitalWrite ( digit0pin , LOW );
      break ;
    case 1:
      digitalWrite ( digit1pin , LOW );
      break ;
    case 2:
      digitalWrite ( digit2pin , LOW );
      break ;
    case 3:
      digitalWrite ( digit3pin , LOW );
      break ;
  }

  delay (3);
}

//variables for count

int count = 0;
long prevmil = millis();
int button = 0;
int value ;
int count1 = 0;
long timeCompare = millis();



//int state = 0; // 0 waiting for countdown, 1 button pressed after waiting, 2 countdown (button unpressed)

void loop ()
{

  digitalWrite ( resetPin , HIGH );
  delayMicroseconds (1); // Reset pulse width tr >= 100 ns
  digitalWrite ( resetPin , LOW );
  delayMicroseconds (36); // Reset to 1st strobe trs >= 72 us
  for (int i = 0; i < 7; i ++)
  {
    delayMicroseconds (36); // Strobe to storbe tss >= 72 us

    digitalWrite ( strobePin , LOW );
    delayMicroseconds (36); // Output settling time to >= 36 us
    spectrumValue [i] = analogRead ( analogPin );
    digitalWrite ( strobePin , HIGH );
  }

  long timeCounter = millis();

  //AnalogRead allows the user to set the time with the potentiometer

  int userNum = (analogRead(A0) / 10 * 15);
  long t = millis();

  //every time the button is pressed, the countdown condition changes from true to false

  if (digitalRead(buttonPin) == 1) {
    long timeCompare = millis();
    Serial.println("pin11 on");
  }

  if ((timeCounter - timeCompare > 500) && (digitalRead(buttonPin) == HIGH)) {
    count1 = count1 + 1;
    timeCompare = timeCounter;
  }

  //The code below begins the countdown at a one second interval

  if ((count1) % 2 == 1) {
    if (t - prevmil > 1000) {
      count = count + 1;
      prevmil = millis();
    }
  }

  //This code adjusts the display to show minutes on the left 2 digits and seconds on the right

  if ((userNum - count) >= 0) {
    value = (userNum - count) % 10; // ones digit
    writeDigit (value , 0);
    value = ((userNum - count) % 60) / 10; // tens digit
    writeDigit (value , 1);
    value = ((userNum - count) / 60) % 10; // hundreds digit
    writeDigit (value , 2);
    value = ((userNum - count) / 600) % 10; // thousands digit
    writeDigit (value , 3);
  }

  //When the countdown reaches '0000' the alarm function is triggered
  else {
    writeDigit(0, 0);
    writeDigit(0, 1);
    writeDigit(0, 2);
    writeDigit(0, 3);
    alarm((spectrumValue[2]));
  }
}
