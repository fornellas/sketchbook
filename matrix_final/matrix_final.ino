/*****************************************************************************
 * Pins                                                                      *
 *****************************************************************************/

// Thermistor
#define APIN_THERMISTOR 0

// Photoresistor
#define APIN_PHOTORESISTOR 1

// I2C: RTC
#define DPIN_I2C_SDA 4
#define DPIN_I2C_SDA 5

// Buttons
#define DPIN_BUTTON1 2
#define DPIN_BUTTON2 7
#define DPIN_BUTTON3 8

// SPI: RGB Matrix
#define DPIN_SPI_SS   10 // 10 PWM
#define	DPIN_SPI_MOSI 11 // 11 PWM
#define DPIN_SPI_SCK  13 // On board LED

// Buzzer
#define DPIN_BUZZER 5

// LED RGB
#define DPIN_RGBLED_R 9
#define DPIN_RGBLED_G 6
#define DPIN_RGBLED_B 3

/*****************************************************************************
 * Modes                                                                     *
 *****************************************************************************/

// Modes
#define MODE_CLOCK      0
#define MODE_PLASMA     1
#define MODE_SPECTRUM   2
#define MODE_FIRE       3
#define MODE_RGBLED     4
#define MODE_COUNT      5
char *modeName[]={
  "Clock",
  "Plasma"
    "Spectrum",
  "Fire",
  "RGB LED",
};

/*****************************************************************************
 * Setup                                                                     *
 *****************************************************************************/

// Current operating mode
byte mode=0;

void setup(){

  /*
   * Initialize modes
   */
  // Clock

  // Plasma

  // Spectrum

  // Fire

  // RGB LED


  // Serial
  Serial.begin(115200);

  // Beep when done
  tone(DPIN_BUZZER, 60, 100);
  delay(150);
  tone(DPIN_BUZZER, 60, 100);
  delay(150);
  tone(DPIN_BUZZER, 100, 100);
  delay(100);
}

/*****************************************************************************
 * Loop                                                                      *
 *****************************************************************************/

void loop(){
  // Do the job for each mode
  switch(mode){
  case MODE_CLOCK:
  // Dentro do loop desta funcao, chamar a que verifica botao apertado
  // Se o #1 for pressionado, retornar.
  // Se outro botao for pressionado, tomar alguma acao caso necessario dentro do modo.
    clock.main(); 
    break;
  case MODE_PLASMA:
    plasma.main();
    break;
  case MODE_SPECTRUM:
    spectrum.main();
    break;
  case MODE_FIRE:
    fire.main();
    break;
  case MODE_RGBLED:
    rgbled.main();
    break;
  };
  if(++mode==MODE_COUNT)
    mode=0;
}




























