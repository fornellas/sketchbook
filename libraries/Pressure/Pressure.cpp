#include "Pressure.h"

#include <math.h>

#define CONV_PA_INHG 3386.38866667
#define CONVPA_HPA 100.0

Pressure::Pressure(){
  loadFromSensor();
}

double Pressure::getPa(){
  return Pa;
}

double Pressure::getInHg(){
  return (double)Pa/CONV_PA_INHG;
}

double Pressure::getHPa(){
  return (double)Pa/CONVPA_HPA;
}

double Pressure::getAtm(){
  return (double)Pa*9.87E-6;
}

double Pressure::altitude(double seaLevelPa){
  return 44330.0 * (1.0 - pow((double)Pa /(double)seaLevelPa,0.1903));
}

double Pressure::expectedPressure(double seaLevelPa, double meters){
  return seaLevelPa*pow((1-meters/44330), 5.255);
}
