#include <stdint.h>
#include <math.h>
#include <string.h>

int32_t fix_fft(int16_t fr[], int16_t fi[], int16_t m, int16_t inverse);

void fft(int16_t *samples, int16_t *freq, int16_t log2samples){
  int16_t sampleCount=1<<log2samples;
  int16_t fi[sampleCount];
  int16_t i;

  // zero imaginary part
  memset(fi, 0, sampleCount*sizeof(int16_t));

  fix_fft(samples, fi, log2samples, 0);

  for(i=0;i<(sampleCount>>1);i++)
    freq[i]=sqrt((int32_t)samples[i]*(int32_t)samples[i]+(int32_t)fi[i]*(int32_t)fi[i]);
}
