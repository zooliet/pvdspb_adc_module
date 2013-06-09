#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define SAMPLES 2048

int main(void) {
        FILE    *fp;
        unsigned short buf[SAMPLES];
        unsigned int i, j;

        memset(buf, 0, 2 * SAMPLES);

        if((fp = fopen("/dev/pvdspb_adc1", "rb")) == NULL) {
                printf("File open failed.\n");
                exit(1);
        } else {
                printf("File open successfully.\n");
        }

        fread(buf, 2, SAMPLES, fp);

        for(i=0; i < SAMPLES; i++) {
                printf("0x%04x ", buf[i]);
                if( ++j%8 == 0)
                        printf("\n");
        }

        fclose(fp);

        return 0;
}
