void setup() {
  // put your setup code here, to run once:
pinMode(7,OUTPUT);
randomSeed(analogRead(0));
}

void loop() {
  // put your main code here, to run repeatedly:
delay(random(10,250));
digitalWrite(7,HIGH);
delay(random(10,250)); 
digitalWrite(7,LOW);
}
