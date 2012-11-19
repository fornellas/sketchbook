#include "Pressure.h"

#include <math.h>

#define CONV_PA_INHG 3377.0
#define CONVPA_HPA 100.0

double Pressure::readInHg(){
  return (double)readPa()/CONV_PA_INHG;
}

double Pressure::readHPa(){
  return (double)readPa()/CONVPA_HPA;
}

double Pressure::readAtm(){
  return (double)readPa()*9.87E-6;
}

double Pressure::calcAltitude(uint32_t seaLevelPa, uint32_t Pa){
  return 44330.0 * (1.0 - pow((double)Pa /(double)seaLevelPa,0.1903));
}

double Pressure::readCalcAltitude(uint32_t seaLevelPa){
  return calcAltitude(seaLevelPa, readPa());
}
