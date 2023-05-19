/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: vb
 *
 * Created on 27 de Abril de 2023, 16:31
 */

#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

int main(int argc, char** argv) {

    if (argc != 2) { printf("\n\nUsage: ./bvectors inputfile\n\n"); return 0; }

    //FILE *f = fopen("bigann_learn.bvecs", "rb");
    FILE *f = fopen(argv[1], "rb");
    if (f == NULL) { printf("could not open input! sorry!"); return 0; }
    
    int dim;
    unsigned char vet[10000];
    int count = 0;
    while (1) {
        fread(&dim, 1, sizeof(int), f);
        fread(vet, dim, sizeof(unsigned char), f);
        if (feof(f))
            break;
        printf("%d ", dim);
        for (int i = 0; i < dim; i++) {
            if (i < dim-1)
                printf("%d ", vet[i]);
            else
                printf("%d", vet[i]);
        }
        printf("\n"); //getchar();
        count++;
        if (count == 100000000)
            break;
    }
    
    return 0;
}
