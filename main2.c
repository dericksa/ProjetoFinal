#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>

#include "pdi.h"

#define INPUT_IMAGE "NoAA/img1.BMP"
#define JANELA 3

#define CIMA_min PI/4
#define CIMA_max (3*PI)/4
#define ESQUERDA_min 3*PI/4
#define ESQUERDA_max 5*PI/4
#define DIREITA_min 5*PI/4
#define DIREITA_max 7*PI/4
#define BAIXO_min 7*PI/4
#define BAIXO_max PI/4

#define HORIZONTAL 0
#define VERTICAL 1


void insertion_sort (int *vetor, int n) {
   int i, j;
   for (j = 1; j < n; j++) {
       int chave = vetor[j];
       int i = j - 1;
       while ((vetor[i] > chave) && (i >= 0)) {
           vetor[i+1] = vetor[i];
           i--;
       }
       vetor[i+1] = chave;
   }
}

float fragColor(Imagem *imgp, Imagem *orientacao, Imagem *magnitude, int h, int w){
    int i, j;

    h = fmax (h, JANELA);
    w = fmax (w , JANELA);


    float histograma [9];
    int graus;
    float radianos;
    float pi = ;

    int pos = 0;

    for(i = h - JANELA; i <= h + JANELA; i++){
        for(j = w - JANELA; j <= w + JANELA; j++){
            histograma [pos] = orientacao->dados[0][i][j];
            pos++;
        }
    }

    insertion_sort(histograma, 9);

    float histogramaSemExtremos [5];  

    float media = 0;
    for (i = 2, j = 0; i < 7  && j < 5; i++, j++){
       histogramaSemExtremos [j] = histograma[i];
        media += histogramaSemExtremos [j];
    }

    media /= 5;

    //1 = cima
    //2 = baixo
    //3 = esquerda
    // 4 = direita

    int direcao;

    if(media >= CIMA_min && media < CIMA_max){
        direcao = VERTICAL;
    }
    else if (media >= BAIXO_min && media < BAIXO_max){
        direcao = VERTICAL;
    }
    else if (media >= ESQUERDA_min && media < ESQUERDA_max){
        direcao = HORIZONTAL;
    }
    else if (media >= DIREITA_min && media < DIREITA_max){
        direcao = HORIZONTAL;
    }


}


int main ()
{
    Imagem* img = abreImagem (INPUT_IMAGE, 3);
    Imagem* imgp = abreImagem (INPUT_IMAGE, 1);

    int i, j, k;

    if (!img)
    {
        printf ("Erro abrindo a imagem.\n");
        exit (1);
    }

    Imagem *orientacao = abreImagem(INPUT_IMAGE, imgp->n_canais);  
    Imagem *magnitude = abreImagem(INPUT_IMAGE, imgp->n_canais);  

    computaGradientes(imgp, 5, NULL, NULL, magnitude, orientacao);




    for (i = 0; i < img->altura; i++){
        for (j = 0; j < img->largura; j++){

            if (i < img->altura - JANELA && j < img->largura - JANELA &&
                magnitude->dados[0][i][j] < 0.3)
                fragColor(imgp, orientacao, magnitude, i, j);
            
        }
    }



    salvaImagem(img, "img.bmp");
    salvaImagem(imgp, "imgp.bmp");
    salvaImagem(orientacao, "Orientacao.bmp");
    salvaImagem(magnitude, "Magnitude.bmp");


    destroiImagem(img);


}




