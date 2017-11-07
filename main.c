#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>

#include "pdi.h"

// #define INPUT_IMAGE "img/0.BMP"
//#define INPUT_IMAGE "img/1.bmp"
//#define INPUT_IMAGE "img/2.bmp"
//#define INPUT_IMAGE "img/3.bmp"
//#define INPUT_IMAGE "img/4.bmp"
//#define INPUT_IMAGE "img/5.bmp"
//#define INPUT_IMAGE "img/6.bmp"
//#define INPUT_IMAGE "img/7.bmp"
//#define INPUT_IMAGE "img/8.bmp"
#define EDIT_IMAGE "img/imagemtroca.bmp"


/**********************************/
void spillremoval(Imagem *img);
void dessaturarVerde(Imagem *mapaverdes, Imagem *img);
void criaMascara(Imagem *img, Imagem *mapaverdes);
/**********************************/


int main ()
{
    Imagem* img = abreImagem (INPUT_IMAGE, 3);
    Imagem* edit = abreImagem (EDIT_IMAGE, 3);
    int i,j,k;


    if (!img)
    {
        printf ("Erro abrindo a imagem.\n");
        exit (1);
    }


    Imagem *final = criaImagem(img->largura, img->altura, img->n_canais);
    Imagem *redimensionada = criaImagem(img->largura, img->altura, img->n_canais);
    redimensionaBilinear (edit, redimensionada);
    Imagem *mapaverdes = criaImagem(img->largura, img->altura, 1);
    criaMascara(img, mapaverdes);
    filtroGaussiano(mapaverdes, mapaverdes, 1, 1, NULL);
    dessaturarVerde(mapaverdes, img);
    

    spillremoval (img);

    for(k = 0; k < img->n_canais; k++){
        for(i = 0; i < img->altura; i++){
            for(j = 0; j < img->largura; j++){
                final->dados[0][i][j]  = img->dados[0][i][j] * (mapaverdes->dados[0][i][j]) + redimensionada->dados[0][i][j] * (1 - mapaverdes->dados[0][i][j]);
                final->dados[1][i][j]  = img->dados[1][i][j] * (mapaverdes->dados[0][i][j]) + redimensionada->dados[1][i][j] * (1 - mapaverdes->dados[0][i][j]);
                final->dados[2][i][j]  = img->dados[2][i][j] * (mapaverdes->dados[0][i][j]) + redimensionada->dados[2][i][j] * (1 - mapaverdes->dados[0][i][j]);
            }
        }
    }


    salvaImagem(img, "ImgOriginal.BMP");
    salvaImagem(final, "final.BMP");
    salvaImagem(mapaverdes, "mapaverdes.BMP");

    destroiImagem(img);
    destroiImagem(mapaverdes);
    destroiImagem(final);
    destroiImagem(redimensionada);
    destroiImagem(edit);

}


void spillremoval(Imagem *img){
    int i,j;
    Imagem *out = criaImagem(img->largura, img->altura, img->n_canais);
    RGBParaHSL(img, out);
    for(i = 0; i < img->altura; i++){
        for(j = 0; j < img->largura; j++){
            if(img->dados[0][i][j] < img->dados[1][i][j]&& out->dados[0][i][j] >= 60 && out->dados[0][i][j] <= 150){
                img->dados[1][i][j] = img->dados[0][i][j];
            }
        }
    }

    destroiImagem(out);
}


void criaMascara(Imagem *img, Imagem *mapaverdes){
    int i, j;
    Imagem *out = criaImagem(img->largura, img->altura, img->n_canais);
    RGBParaHSL(img, out);
    float r = 0;
    float g = 0;
    float b = 0;
    float distancia;
    int n_verdes = 0;

     /*Pega os verdes
    HUE entre 80 e 140
    Saturation acima de 0.11
    Lumin entre 0.24 e 0.8*/
    for(i = 0; i < img->altura; i++){
        for(j = 0; j < img->largura; j++){
            if(
                out->dados[0][i][j] >= 100 && out->dados[0][i][j] <= 150
                && out->dados[1][i][j] > 0.2 &&
                out->dados[2][i][j] > 0.15 &&  out->dados[2][i][j] < 0.8){

                mapaverdes->dados[0][i][j] = 0;
                r += img->dados[0][i][j];
                g += img->dados[1][i][j];
                b += img->dados[2][i][j];
                n_verdes++;

            }
            else
                mapaverdes->dados[0][i][j] = 1;
        }   
    }

    r /= n_verdes;
    g /= n_verdes;
    b /= n_verdes;


    /*Calcula distância para o verde "ideal" na imagem*/
    for(i = 0; i < img->altura; i++){
        for(j = 0; j < img->largura; j++){
            if(mapaverdes->dados[0][i][j] != 1){ 
                distancia = sqrt( pow((img->dados[0][i][j] - r), 2) + pow((img->dados[1][i][j] - g), 2) + pow((img->dados[2][i][j] - b), 2));
                if(distancia <= 0.3)
                    mapaverdes->dados[0][i][j] = 0;
                else
                    mapaverdes->dados[0][i][j] = distancia;

            }   
        }
    }

    destroiImagem(out);
}


void dessaturarVerde(Imagem *mapaverdes, Imagem *img){
    int i, j;
    Imagem *hsl = criaImagem(img->largura, img->altura, img->n_canais);
    RGBParaHSL(img, hsl);

    for(i = 0; i < img->altura; i++){
        for(j = 0; j < img->largura; j++){
            if(mapaverdes->dados[0][i][j] <= 0.99){
                hsl->dados[1][i][j] *= 0.1;
            }
        }
    }


    HSLParaRGB(hsl, img);
    salvaImagem(img, "Dessaturada");
    destroiImagem(hsl);
}
        