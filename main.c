#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>

#include "pdi.h"

#define INPUT_IMAGE "NoAA/img1.BMP"
#define JANELA 3

#define BORDA_THRESH_MIN 0.0312
#define BORDA_THRESH_MAX 0.125

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

float dot_product(float v[], float u[], int n)
{
    float result = 0.0;
    for (int i = 0; i < n; i++)
        result += v[i]*u[i];
    return result;
}


void RGBParaLuma(Imagem *img, Imagem *luma){
    int i, j;
    float vetor_pesos[3] = {0.299, 0.587, 0.114}; 
    float vetor_rgb[3] = {0};

    for(i = 0; i < img->altura; i++){
        for(j = 0; j < img->largura; j++){
            vetor_rgb[0] = img->dados[0][i][j];
            vetor_rgb[1] = img->dados[1][i][j];
            vetor_rgb[2] = img->dados[2][i][j];


            luma->dados[0][i][j] = sqrt(dot_product(vetor_rgb, vetor_pesos, 3));
        }
    }
}


float Interpolacao(float a, float b, float c, float d, float x, float y){

    float wl, wr, wt, wb;

    // printf("a: %f\n", a);
    // printf("b: %f\n", b);
    // printf("c: %f\n", c);
    // printf("d: %f\n", d);

    // printf("x: %f\n", x);
    // printf("y: %f\n", y);


    wl = ceil(x) - x;
    wr = x - floor(x);
    wt = ceil(y) - y;
    wb = y - floor(y);

    
    // printf("wl: %f\n", wl);
    // printf("wr: %f\n", wr);
    // printf("wt: %f\n", wt);
    // printf("wb: %f\n", wb);

    float w1, w2, w3, w4;

    w1 = wl * wt;
    w2 = wl * wb;
    w3 = wr * wt;
    w4 = wr * wb;

    return (a * w1 + b * w2 + c * w3 + d * w4);
}


int floorint(float value){
    return (abs(floor(value)));
}

int ceilint(float value){
    return (abs(ceil(value)));
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


    /*Imagem *orientacao = criaImagem(imgp->largura, imgp->altura imgp->n_canais);  
    Imagem *magnitude = criaImagem(imgp->largura, imgp->altura imgp->n_canais); 

    computaGradientes(imgp, 5, NULL, NULL, magnitude, orientacao);*/

    Imagem *luma = criaImagem(img->largura, img->altura, 1);
    Imagem *teste = criaImagem(img->largura, img->altura, 1);
    Imagem *cinza = criaImagem(img->largura, img->altura, 1);


    RGBParaCinza(img, cinza);

    float corPixel = -100;

    float lumaCima, lumaBaixo, lumaDireita, lumaEsquerda, lumaCentro;
    float lumaNoroeste, lumaSudoeste, lumaNordeste, lumaSudeste;
    float lumaMax, lumaMin, lumaRange;

    float bordaVert, bordaHorz;
    int horizontal;

    float luma1, luma2;
    float gradiente1, gradiente2;
    int Esquerdaingreme;

    float gradienteEscalado;

    int rcpFrame[2];

    float passo;


    rcpFrame[0] = 1.0;
    rcpFrame[1] = 1.0;


    RGBParaLuma(img, luma);
    

    for(i = 1; i < img->altura-12; i++){
        for(j = 1; j < img->largura-12; j++){

            lumaCima = luma->dados[0][i-1][j];
            lumaBaixo = luma->dados[0][i+1][j];
            lumaDireita = luma->dados[0][i][j+1];
            lumaCentro= luma->dados[0][i][j];

            lumaMax = fmax(lumaCentro, fmax(fmax(lumaBaixo, lumaCima), fmax(lumaEsquerda, lumaDireita)));
            lumaMin = fmin(lumaCentro, fmin(fmin(lumaBaixo, lumaCima), fmin(lumaEsquerda, lumaDireita)));


            lumaRange = lumaMax - lumaMin;

            if(lumaRange < fmax(BORDA_THRESH_MIN, lumaMax * BORDA_THRESH_MAX)){
                imgp->dados[0][i][j] = cinza->dados[0][i][j];

            }
            else{

                    
                /*Horizontal ou Vertical*/
                lumaNoroeste = luma->dados[0][i-1][j-1];
                lumaNordeste = luma->dados[0][i-1][j+1];
                lumaSudoeste = luma->dados[0][i+1][j-1];
                lumaSudeste = luma->dados[0][i+1][j+1];



                bordaVert =
                    abs((0.25 * lumaNoroeste) + (-0.5 * lumaCima) + (0.25 * lumaNordeste)) + 
                    abs((0.50 * lumaEsquerda) + (-1.0 * lumaCentro) + (0.50 * lumaDireita)) +
                    abs((0.25 * lumaSudoeste) + (-0.5 * lumaBaixo) + (0.25 * lumaSudeste));

                bordaHorz =
                    abs((0.25 * lumaNoroeste) + (-0.5 * lumaEsquerda) + (0.25 * lumaSudoeste)) + 
                    abs((0.50 * lumaCima) + (-1.0 * lumaCentro) + (0.50 * lumaBaixo)) +
                    abs((0.25 * lumaNordeste) + (-0.5 * lumaDireita) + (0.25 * lumaSudeste));

                if(bordaHorz >= bordaVert)
                    horizontal = 1;
                else
                    horizontal = 0;

                /*Orientação da Borda*/
               
                if(!horizontal){
                    luma1 = lumaEsquerda;
                    luma2 = lumaDireita; 
                }
                else{
                    luma1 = lumaCima;
                    luma2 = lumaBaixo; 
                }

                gradiente1 = luma1 - lumaCentro;
                gradiente2 = luma2 - lumaCentro;

                if(abs(gradiente1) >= abs(gradiente2))
                    Esquerdaingreme = 1;
                else
                    Esquerdaingreme = 0;

                gradienteEscalado = 0.25 * fmax(abs(gradiente1), abs(gradiente2));


                /*Verifica qual lado é mais forte*/

                if(horizontal)
                    passo = 1;
                else
                    passo = 1;

                float lumaMedio = 0.0;


                /*Se a esquerda é mais forte, muda o passo para lá <-*/

                if(Esquerdaingreme){
                    passo = -passo; 

                    /*Tira media local*/
                    lumaMedio = 0.5 * (luma1 + lumaCentro);
                }
                else
                    lumaMedio = 0.5 * (luma2 + lumaCentro);



                float posY = i;
                float posX = j;

                if(horizontal) posY += passo * 0.5;
                else posX += passo * 0.5; 

                

                float offset [2] = {0,0};
                
                if(horizontal){
                    offset[0] = 0.5;
                    offset[1] = 0;
                } 
                else {
                    offset[0] = 0;
                    offset[1] = 0.5;
                }

                float uv1[2];
                float uv2[2];

                uv1[0] = posX - offset[0];
                uv1[1] = posY - offset[1];

                uv2[0] = posX + offset[0];
                uv2[1] = posY + offset[1];

                //printf("uv1: %f\n uv1y: %f\n", uv1[0], uv1[1]);
                //printf("posX: %f\n", posX);

                //printf("uv1: %f\n", uv1[1]);

                /*Talvez interpolar /\*/
                float a, b, c, d;

                a = luma->dados[0][floorint(uv1[1])][floorint(uv1[0])];
                b = luma->dados[0][floorint(uv1[1])][ceilint(uv1[0])];
                c = luma->dados[0][ceilint(uv1[1])][floorint(uv1[0])];
                d = luma->dados[0][ceilint(uv1[1])][ceilint(uv1[0])];


                float lumaFim1 = Interpolacao (a, b, c, d, uv1[0], uv1[1]);
                
                a = luma->dados[0][floorint(uv2[1])][floorint(uv2[0])];
                b = luma->dados[0][floorint(uv2[1])][ceilint(uv2[0])];
                c = luma->dados[0][ceilint(uv2[1])][floorint(uv2[0])];
                d = luma->dados[0][ceilint(uv2[1])][ceilint(uv2[0])];

                float lumaFim2 = Interpolacao (a, b, c, d, uv2[0], uv2[1]);

                lumaFim1 -= lumaMedio;
                lumaFim2 -= lumaMedio;

                int alcancou1 = 0;
                int alcancou2 = 0;

                if(abs(lumaFim1) >= gradienteEscalado) alcancou1 = 1;

                if(abs(lumaFim2) >= gradienteEscalado) alcancou2 = 1;

                int alcancoutudo = 0;

                if(alcancou1 && alcancou2) alcancoutudo = 1;

                if(!alcancou1) {
                    uv1[0] -= offset[0];
                    uv1[1] -= offset[1];
                }

                if(!alcancou2){
                    uv2[0] += offset[0];
                    uv2[1] += offset[1];
                }

                printf("uv1x: %f\n", uv1[0]);
                printf("uv1y: %f\n", uv1[1]);

                if(!alcancoutudo){

                    for(k = 2; k < 12; k++){
                        if(!alcancou1){
                            a = luma->dados[0][floorint(uv1[1])][floorint(uv1[0])];
                            b = luma->dados[0][floorint(uv1[1])][ceilint(uv1[0])];
                            c = luma->dados[0][ceilint(uv1[1])][floorint(uv1[0])];
                            d = luma->dados[0][ceilint(uv1[1])][ceilint(uv1[0])];


                            float lumaFim1 = Interpolacao (a, b, c, d, uv1[0], uv1[1]); 
                            lumaFim1 -= lumaMedio;
                        }
                        if(!alcancou2){
                            a = luma->dados[0][floorint(uv2[1])][floorint(uv2[0])];
                            b = luma->dados[0][floorint(uv2[1])][ceilint(uv2[0])];
                            c = luma->dados[0][ceilint(uv2[1])][floorint(uv2[0])];
                            d = luma->dados[0][ceilint(uv2[1])][ceilint(uv2[0])];

                            // printf("uv2 X %f\n uv2 Y %f\n", uv2[0], uv2[1]);

                            float lumaFim2 = Interpolacao (a, b, c, d, uv2[0], uv2[1]);

                            // printf("lumaFim2: %f\n", lumaFim2);

                            lumaFim2 -= lumaMedio;
                        }


                        if(abs(lumaFim1) >= gradienteEscalado) alcancou1 = 1;

                        if(abs(lumaFim2) >= gradienteEscalado) alcancou2 = 1;
                        if(alcancou1 && alcancou2) alcancoutudo = 1;

                        if(!alcancou1) {
                            uv1[0] -= offset[0];
                            uv1[1] -= offset[1];
                        }   

                        if(!alcancou2){
                            uv2[0] +=  offset[0];
                            uv2[1] +=  offset[1];
                        }

                        if(alcancoutudo) break;

                    }

                }



                float distancia1, distancia2;

                if(horizontal) {
                    distancia1 = j - uv1[0];
                    distancia2 = uv2[0] - j;
                }
                else{
                    distancia1 = i - uv1[1];
                    distancia2 = uv2[1] - i;
                }

                int EsquerdaBorda = 0;

                if(distancia1 < distancia2) EsquerdaBorda = 1;

                float distanciaFinal = (fmin(distancia1, distancia2));

                float grossuraBorda = distancia1 + distancia2;

                float pixelOffset = - distanciaFinal / grossuraBorda + 0.5;

                
                //printf("distancia1: %f\n", distancia2);

                //imgp->dados[0][i][j] = 1 * pixelOffset + (1 - pixelOffset) * 0;




/*
                /*Corrigir luma centro, talvez fazer*/
                int LumaCentroMenor = 0; 


                if(lumaCentro  < lumaMedio) LumaCentroMenor = 1;

                int variacaoCorreta = 0;

                int continha = (EsquerdaBorda ? lumaFim1 : lumaFim2);

                if(continha < 0.0 && LumaCentroMenor == 0){
                    variacaoCorreta = 1;
                }
                else if(continha < 0.0 && LumaCentroMenor == 1)
                    variacaoCorreta =0;
                else
                    variacaoCorreta = 0;

                float offsetFinal;

                if(variacaoCorreta)
                    offsetFinal = pixelOffset;
                else 
                    offsetFinal = 0.0;


                lumaMedio = (1.0/12.0) * (2.0 * ((lumaBaixo + lumaCima) + (lumaEsquerda + lumaDireita)) 
                    + lumaNoroeste + lumaSudoeste + lumaNordeste + lumaSudeste);

                float subPixelOffset = (lumaMedio - lumaCentro)/ lumaRange;
                if(subPixelOffset > 1) subPixelOffset = 1;
                else if(subPixelOffset < 0) subPixelOffset = 0;

                float subPixelOffset2 = (-2.0 * subPixelOffset + 3.0) * subPixelOffset * subPixelOffset;

                float subPixelOffsetFinal = subPixelOffset2 * subPixelOffset2 * 0.75;

                float finalOffset = fmax(pixelOffset, subPixelOffsetFinal);

                int finalUv[2];


                if(horizontal){
                    finalUv[1] += finalOffset * passo;
                }
                else
                    finalUv[0] += finalOffset * passo;


                imgp->dados[0][i][j] = (luma->dados[0][finalUv[1]][finalUv[0]])

            }
        }
    }



    //salvaImagem(img, "img.bmp");
    salvaImagem(imgp, "imgp.bmp");
    //salvaImagem(luma, "luma.bmp");
    salvaImagem(img, "Final.bmp");



    destroiImagem(img);


}




