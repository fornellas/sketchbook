// Pinos PWM das pernas do LED de cada cor
#define PIN_RED 9
#define PIN_GREEN 10
#define PIN_BLUE 11
// Pino do LED amarelo soldado na placa
#define PIN_SMD_LED 13
// Brilho do LED RGB 0-255
#define BRIGHTNESS 255

void fade(int fr, int fg, int fb, int tr, int tg, int tb){
  float f, s;
  
  for(f=0;f<3.1415/2;f+=0.012){
    s=sin(f);
    analogWrite(PIN_RED, fr+s*(tr-fr));
    analogWrite(PIN_GREEN, fg+s*(tg-fg));
    analogWrite(PIN_BLUE, fb+s*(tb-fb));
    // pisca o LED SMD enquanto muda a cor
    digitalWrite(PIN_SMD_LED, LOW);
    delay(10);
    digitalWrite(PIN_SMD_LED, HIGH);
  }
  // Fica 3s na cor nova
  delay(3000);
  digitalWrite(PIN_SMD_LED, LOW);
}

void setup(){
  // Configura o pino para saída digital 
  pinMode(PIN_SMD_LED, OUTPUT);
  // preto > vermelho
  fade(0, 0, 0, BRIGHTNESS, 0, 0);
}
void loop(){
  // vermelho > amarelo
  fade(BRIGHTNESS, 0, 0, BRIGHTNESS, BRIGHTNESS, 0);
  // amarelo > verde
  fade(BRIGHTNESS, BRIGHTNESS, 0, 0, BRIGHTNESS, 0);
  // verde > ciano
  fade(0, BRIGHTNESS, 0, 0, BRIGHTNESS, BRIGHTNESS);
  // ciano > azul
  fade(0, BRIGHTNESS, BRIGHTNESS, 0, 0, BRIGHTNESS);
  // azul > magenta
  fade(0, 0, BRIGHTNESS, BRIGHTNESS, 0, BRIGHTNESS);
  // magenta > vermelho
  fade(BRIGHTNESS, 0, BRIGHTNESS, BRIGHTNESS, 0, 0);
}
