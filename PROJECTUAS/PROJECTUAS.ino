#include <Servo.h> // Menyertakan library servo ke dalam program
#include "Wire.h"
#include "Adafruit_TCS34725.h"
#include <LiquidCrystal_I2C.h>

// Pin untuk LED RGB
#define redpin 3
#define greenpin 5
#define bluepin 6

// Tentukan apakah LED menggunakan anoda atau katoda bersama
#define commonAnode true

// Tabel gamma untuk koreksi warna
byte gammatable[256];

// Inisialisasi sensor warna
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

// Inisialisasi LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

long totalValue = 0;

// Fungsi untuk normalisasi nilai RGB
void normalizeRGB(uint16_t &r, uint16_t &g, uint16_t &b) {
  uint16_t sum = r + g + b;
  if (sum > 0) {
    r = (r * 255) / sum;
    g = (g * 255) / sum;
    b = (b * 255) / sum;
  }
}


const int pinSensor = 7;
const int pinLed = 2;
const int trigPin = 9;
const int echoPin = 10;

// Inisialisasi motor servo
Servo myservo;
int pos = 0;

// Fungsi untuk mengukur jarak menggunakan sensor ultrasonik
long readUltrasonicDistance(int triggerPin, int echoPin) {
  pinMode(triggerPin, OUTPUT);
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  pinMode(echoPin, INPUT);
  return pulseIn(echoPin, HIGH);
}

// Fungsi untuk mendeteksi uang Rp 1000 paten
bool uang1000(uint16_t r, uint16_t g, uint16_t b) {
  return (r >= 80 && r <= 90 && g >= 100 && g <= 110 && b >= 60 && b <= 70);
}
// Fungsi untuk mendeteksi uang Rp 2000 paten
bool uang2000(uint16_t r, uint16_t g, uint16_t b) {
  return (r >= 60 && r <= 75 && g >= 90 && g <= 100 && b >= 70 && b <= 90);
}


// Fungsi untuk mendeteksi uang Rp 5000 paten
bool uang5000(uint16_t r, uint16_t g, uint16_t b) {
  return (r >= 90 && r <= 95 && g >= 100 && g <= 105 && b >= 78 && b <= 85);
}

// Fungsi untuk mendeteksi uang Rp 10000 paten
bool uang10000(uint16_t r, uint16_t g, uint16_t b) {
  return (r >= 90 && r <= 95) && (g >= 95 && g <= 105) && (b >= 99 && b <= 105);
}

// Fungsi untuk mendeteksi uang Rp 20000
bool uang20000(uint16_t r, uint16_t g, uint16_t b) {
  return (r >= 69 && r <= 75 && g >= 105 && g <= 110 && b >= 85 && b <= 90);
}

// Fungsi untuk mendeteksi uang Rp 50000
bool uang50000(uint16_t r, uint16_t g, uint16_t b) {
  return (r >= 55 && r <= 70 && g >= 93 && g <= 102 && b >= 90 && b <= 102);
}

// Fungsi untuk mendeteksi uang Rp 100000 paten
bool uang100000(uint16_t r, uint16_t g, uint16_t b) {
  return (r > 80 && g < 90 && b < 85);
}

void setup() {
  // Menghubungkan motor servo ke pin 3 pada Arduino
  myservo.attach(3);
  pinMode(pinSensor, INPUT_PULLUP);
  pinMode(pinLed, OUTPUT);
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  Serial.begin(9600);

  lcd.init();
  lcd.backlight();

  // Mulai komunikasi I2C
  Wire.begin();

  // Pindai bus I2C untuk perangkat
  scanI2C();

  //SENSOR WARNA SETUP
  if (!tcs.begin()) {
    Serial.println("Sensor TCS34725 tidak ditemukan ... periksa koneksi Anda");
    while (1); // Berhenti!
  }

  pinMode(redpin, OUTPUT);
  pinMode(greenpin, OUTPUT);
  pinMode(bluepin, OUTPUT);

  for (int i = 0; i < 256; i++) {
    float x = i;
    x /= 255;
    x = pow(x, 2.5);
    x *= 255;

    if (commonAnode) {
      gammatable[i] = 255 - x;
    } else {
      gammatable[i] = x;
    }
  }
}

void loop() {
  if (digitalRead(pinSensor) == HIGH) {
    // Sensor atau tombol ditekan
    digitalWrite(pinLed, HIGH);  // Nyalakan LED
    while (digitalRead(pinSensor) == HIGH) {
      // Tunggu hingga sensor atau tombol dilepaskan
      delay(10);
    }
  } else {
    digitalWrite(pinLed, LOW);  // Mematikan LED ketika sensor tidak dipegang
  }
  delay(1000);

  long duration, distance;

  // Mengukur jarak
  duration = readUltrasonicDistance(trigPin, echoPin);
  distance = (duration / 2) / 29.1; // Mengonversi waktu perjalanan menjadi jarak (cm)

  // Mencetak jarak ke Serial Monitor
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  // Jika jarak kurang dari atau sama dengan 5 cm, gerakkan motor servo
  if (distance <= 5) {
    for (pos = 150; pos >= 1; pos -= 1) { // Fungsi perulangan untuk menggerakkan servo dari 0 ke 180 derajat
      myservo.write(pos); // Menulis data PWM ke motor servo
      delay(10); // Waktu tunda 15 ms
    }
  delay(3000);  
    for (pos = 0; pos < 170; pos += 1) { // Fungsi perulangan untuk menggerakkan servo dari 180 ke 0 derajat
      myservo.write(pos);
      delay(10);
    }
  }


  // Delay untuk memberikan jeda sebelum pengukuran berikutnya
  delay(500);


  //LOOP SENSOR WARNA
  float red, green, blue;
  tcs.setInterrupt(false);  // Nyalakan LED
  delay(60);  // Membutuhkan 50ms untuk membaca
  tcs.getRGB(&red, &green, &blue);
  tcs.setInterrupt(true);  // Matikan LED

  bool terdeteksi = false;
  uint16_t r = static_cast<uint16_t>(red);
  uint16_t g = static_cast<uint16_t>(green);
  uint16_t b = static_cast<uint16_t>(blue);

  lcd.setCursor(0, 0);
  lcd.print("                "); 

  if (uang1000(r, g, b)) {
    Serial.println("Uang terdeteksi: Rp 1000");
    totalValue += 1000;
    terdeteksi = true;
    lcd.setCursor(0,0);
    lcd.print("Uang 1000");
  } else if (uang2000(r, g, b)) {
    Serial.println("Uang terdeteksi: Rp 2000");
    totalValue += 2000;
    terdeteksi = true;
    lcd.setCursor(0,0);
    lcd.print("Uang 2000");
  } else if (uang5000(r, g, b)) {
    Serial.println("Uang terdeteksi: Rp 5000");
    totalValue += 5000;
    terdeteksi = true;
    lcd.setCursor(0,0);
    lcd.print("Uang 5000");
  } else if (uang10000(r, g, b)) {
    Serial.println("Uang terdeteksi: Rp 10000");
    totalValue += 10000;
    terdeteksi = true;
    lcd.setCursor(0,0);
    lcd.print("Uang 10000");
  } else if (uang20000(r, g, b)) {
    Serial.println("Uang terdeteksi: Rp 20000");
    totalValue += 20000;
    terdeteksi = true;
    lcd.setCursor(0,0);
    lcd.print("Uang 20000");
  } else if (uang50000(r, g, b)) {
    Serial.println("Uang terdeteksi: Rp 50000");
    totalValue += 50000;
    terdeteksi = true;
    lcd.setCursor(0,0);
    lcd.print("Uang 50000");
  } else if (uang100000(r, g, b)) {
    Serial.println("Uang terdeteksi: Rp 100000");
    totalValue += 100000;
    terdeteksi = true;
    lcd.setCursor(0,0);
    lcd.print("Uang 100000");
  }

  if (!terdeteksi) {
    Serial.println("Uang tidak dikenali.");
  }


  // Tampilkan hasil deteksi uang di LCD
  

  // Tampilkan total nilai uang yang terdeteksi di LCD
  lcd.setCursor(0, 1);
  lcd.print("Total: Rp ");
  lcd.print(totalValue);

  // Tampilkan nilai RGB yang terdeteksi
  Serial.print("R:\t"); Serial.print(int(red));
  Serial.print("\tG:\t"); Serial.print(int(green));
  Serial.print("\tB:\t"); Serial.print(int(blue));
  Serial.print("\n");

 // Tampilkan total nilai uang yang terdeteksi
  Serial.print("Total nilai uang terdeteksi: Rp ");
  Serial.println(totalValue);


  analogWrite(redpin, gammatable[(int)red]);
  analogWrite(greenpin, gammatable[(int)green]);
  analogWrite(bluepin, gammatable[(int)blue]);

  delay(1000);  // Tunggu sebelum membaca lagi

}


void scanI2C() {
  byte error, address;
  int nDevices;
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Scanning I2C...");

  nDevices = 0;
  
  for (address = 1; address < 127; address++) {
    // Mulai transmisi I2C ke alamat ini
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    
    if (error == 0) {
      // Perangkat ditemukan di alamat ini
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("I2C device at:");
      lcd.setCursor(0, 1);
      lcd.print("0x");
      if (address < 16)
        lcd.print("0");
      lcd.print(address, HEX);
      nDevices++;
      delay(2000); // Tampilkan alamat selama 2 detik
    }
  } 

  if (nDevices == 0) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("No devices found");
    delay(2000);
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Scan complete");
    delay(2000);
  }
}