#define MEDIAN_WINDOW 10

#define PIN_LED  9
#define PIN_TRIG 12
#define PIN_ECHO 13

#define SND_VEL 346.0
#define INTERVAL 25
#define PULSE_DURATION 10
#define _DIST_MIN 100
#define _DIST_MAX 300
#define TIMEOUT ((INTERVAL / 2) * 1000.0)
#define SCALE (0.001 * 0.5 * SND_VEL)

float dist_samples[MEDIAN_WINDOW];
int sample_index = 0;
bool samples_filled = false;

unsigned long last_sampling_time = 0;

void setup() {
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  digitalWrite(PIN_TRIG, LOW);
  Serial.begin(57600);
  last_sampling_time = millis();
}

void loop() {
  float dist_raw, dist_median;

  if (millis() < last_sampling_time + INTERVAL) return;

  dist_raw = USS_measure(PIN_TRIG, PIN_ECHO);

  if ((dist_raw != 0.0) && (dist_raw >= _DIST_MIN) && (dist_raw <= _DIST_MAX)) {
    dist_samples[sample_index] = dist_raw;
    sample_index = (sample_index + 1) % MEDIAN_WINDOW;
    if (sample_index == 0) samples_filled = true;
  }

  int valid_N = samples_filled ? MEDIAN_WINDOW : sample_index;

  float median_arr[MEDIAN_WINDOW];
  for (int i = 0; i < valid_N; i++) median_arr[i] = dist_samples[i];

  for (int i = 0; i < valid_N - 1; i++) {
    for (int j = 0; j < valid_N - i - 1; j++) {
      if (median_arr[j] > median_arr[j + 1]) {
        float temp = median_arr[j];
        median_arr[j] = median_arr[j + 1];
        median_arr[j + 1] = temp;
      }
    }
  }

  if (valid_N == 0) dist_median = 0.0;
  else if (valid_N % 2 == 1) dist_median = median_arr[valid_N / 2];
  else dist_median = (median_arr[valid_N / 2 - 1] + median_arr[valid_N / 2]) / 2.0;

  Serial.print("raw:");    Serial.print(min(dist_raw, _DIST_MAX + 100));
  Serial.print(",median:");Serial.print(min(dist_median, _DIST_MAX + 100));
  Serial.print(",N:");     Serial.print(MEDIAN_WINDOW);
  Serial.println();

  if ((dist_median < _DIST_MIN) || (dist_median > _DIST_MAX))
    digitalWrite(PIN_LED, HIGH);
  else
    digitalWrite(PIN_LED, LOW);

  last_sampling_time += INTERVAL;
}

float USS_measure(int TRIG, int ECHO) {
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(TRIG, LOW);
  return pulseIn(ECHO, HIGH, TIMEOUT) * SCALE;
}
