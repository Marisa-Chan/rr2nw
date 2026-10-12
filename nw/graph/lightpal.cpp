#include <windows.h>
#include <stdio.h>
#include <math.h>

#include "sd1_epal.h"

void main(int nAr, char * ar[])
{  unsigned char pal[768];
   unsigned char *lPal = (unsigned char *) new char [8*32*256];
   FILE *fp = fopen(ar[1],"rb");
   int i,j,k;
   TExtendedPalette  ePal;

   fread(pal, 768, 1, fp);
   fclose(fp);

   epal_Load6BitPal( ePal, pal, 256 );

   for(j = 0;j < 8;j++) {
      unsigned long lR, lG, lB;
      unsigned char *ptr = lPal + j*32*256;

      fprintf(stderr,"%d\n",j);

      if ((j&1) || j==0) lR = 255;
      else lR = 0;

      if ((j&2) || j==0) lG = 255;
      else lG = 0;

      if ((j&4) || j==0) lB = 255;
      else lB = 0;

      //RGB_i lC(255&maskR, 255&maskG, 255&maskB);

      for(i = 0;i < 32;i++) {
         unsigned long lR1 = (lR*i)/31,
                       lG1 = (lG*i)/31,
                       lB1 = (lB*i)/31;

         for(k = 0;k < 256;k++) {
            unsigned long r,g,b;

            if (j != 0) {
               r = lR1 + (((unsigned long)pal[k*3])<<2);
               if (r > 255) r = 255;

               g = lG1 + (((unsigned long)pal[k*3+1])<<2);
               if (g > 255) g = 255;

               b = lB1 + (((unsigned long)pal[k*3+2])<<2);
               if (b > 255) b = 255;
            }
            else {
               r = ((255-lR1) * (((unsigned long)pal[k*3])<<2))/255;
               if (r > 255) r = 255;

               g = ((255-lG1) * (((unsigned long)pal[k*3+1])<<2))/255;
               if (g > 255) g = 255;

               b = ((255-lB1) * (((unsigned long)pal[k*3+2])<<2))/255;
               if (b > 255) b = 255;

            }

            *ptr++ = (unsigned char) epal_Match( ePal, RGB_i(r,g,b));
         }

      }
   }

   fp = fopen(ar[2],"wb");

   fwrite(lPal,8*256*32,1,fp);
   fclose(fp);

}
