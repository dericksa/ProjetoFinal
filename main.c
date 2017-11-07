#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>

#include "pdi.h"

#define INPUT_IMAGE "NoAA/img1.BMP"



int main ()
{
    Imagem* img = abreImagem (INPUT_IMAGE, 3);

    if (!img)
    {
        printf ("Erro abrindo a imagem.\n");
        exit (1);
    }


    destroiImagem(img);


}


