int merah = 13;
int kuning = 12;
int hijau = 11;

void setup() {
  pinMode(merah, OUTPUT);
  pinMode(kuning, OUTPUT);
  pinMode(hijau, OUTPUT); 
}

void loop() {
  digitalWrite(merah, HIGH); 
  delay(5000);
  digitalWrite(merah, LOW); 
  digitalWrite(kuning, HIGH); 
  delay(2000);
  digitalWrite(kuning, LOW); 
  digitalWrite(hijau, HIGH);
  delay(3000); 
  digitalWrite(hijau, LOW);
}