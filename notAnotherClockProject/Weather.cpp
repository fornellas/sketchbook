#include "Weather.h"
#include <math.h>
#include "Temperature.h"

// http://www.srh.noaa.gov/images/epz/wxcalc/rhTdFromWetBulb.pdf
double Weather::dewPointC(double humidity, double temperatureC){
  double E;
  E=humidity*(6.112*exp(17.67*temperatureC/(temperatureC+243.5)))/100;
  return (243.5 * log(E/6.112))/(17.67-log(E/6.112));
}

// http://www.srh.noaa.gov/images/ffc/pdf/ta_htindx.PDF
double Weather::heatIndexC(double humidity, double temperatureC){
  double TF;
  TF=Temperature::convC2F(temperatureC);
  return Temperature::convF2C(-42.379+2.04901523*TF+10.14333127*humidity-0.22475541*TF*humidity-6.83783E-3*TF*TF-5.481717E-2*humidity*humidity+1.22874E-3*TF*TF*humidity+8.5282E-4*TF*humidity*humidity-1.99E-6*TF*TF*humidity*humidity);
}
