// Arduino pin assignment
#define PIN_LED  9
#define PIN_TRIG 12
#define PIN_ECHO 13

// configurable parameters
#define SND_VEL 346.0
#define INTERVAL 100
#define PULSE_DURATION 10
#define _DIST_MIN 100.0
#define _DIST_MAX 300.0

#define TIMEOUT ((INTERVAL / 2) * 1000.0)
#define SCALE (0.001 * 0.5 * SND_VEL)

unsigned long last_sampling_time;

void setup() {
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  digitalWrite(PIN_TRIG, LOW);
  Serial.begin(2000000);
}

void loop() {
  float distance;
  int ledDuty;

  if (millis() < (last_sampling_time + INTERVAL)) {
    return;
  }
  last_sampling_time += INTERVAL;

  distance = USS_measure(PIN_TRIG, PIN_ECHO);

  if (distance >= 100.0 && distance <= 200.0) {
    ledDuty = (int) ( ( (200.0 - distance) / 100.0 ) * 255.0 );
  } else if (distance > 200.0 && distance <= 300.0) {
    ledDuty = (int) ( ( (distance - 200.0) / 100.0 ) * 255.0 );
  } else if (distance > 300.0) {
    ledDuty = 255;
  } else {
    ledDuty = 255;
  }

  if (distance >= 149.0 && distance <= 151.0) {
    ledDuty = (int)(0.5 * 255);
  }
  if (distance >= 249.0 && distance <= 251.0) {
    ledDuty = (int)(0.5 * 255);
  }

  ledDuty = constrain(ledDuty, 0, 255);

  analogWrite(PIN_LED, ledDuty);

  Serial.print("Distance:");
  Serial.print(distance);
  Serial.print("mm, LED_Duty:");
  Serial.println(ledDuty);
}

float USS_measure(int TRIG, int ECHO)
{
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(TRIG, LOW);

  return pulseIn(ECHO, HIGH, TIMEOUT) * SCALE;
}
