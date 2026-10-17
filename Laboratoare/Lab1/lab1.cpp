#define ORDER_NUMBER 20
#define PRODUCT_PRICE 10
#include <iostream>
#include<stdlib.h>
#include<stdio.h>
using namespace std;


 void preia_comenzi(int portii[], bool livrari[])  
 {
     for (int i = 0; i < ORDER_NUMBER; i++) {
        portii[i] = rand() % 6 +1; 
        livrari[i] = rand() % 2;
     }


 }


void calculeaza_produse(int portii[],double subtotal[], int pret)  
{
    for ( int i = 0; i < ORDER_NUMBER; i++)
    {
      subtotal[i] = portii[i] * pret;
    }

}

void aplica_reducere(int portii[],double subtotal[])  {

    for ( int i = 0; i < ORDER_NUMBER; i++)
    {
        if (portii[i] >= 4)
        {
            subtotal[i] = double(subtotal[i] * 0.9f);
        }
    }

}

void adauga_livrare(bool livrari[], double subtotal[])  {
 for ( int i = 0; i < ORDER_NUMBER; i++)
    {
        if (livrari[i] == true)
        {
            subtotal[i] += 5;
        }
    }


}

void pregateste_bon(int numar_bon[])  {

    for( int i = 0; i < ORDER_NUMBER; i++)
    {
        numar_bon[i] = i + 1001;
    }

}

int main() 
{
    int portii[ORDER_NUMBER];
    bool livrari[ORDER_NUMBER];
    double subtotal[ORDER_NUMBER];
    int pret = PRODUCT_PRICE;
    int numar_bon[ORDER_NUMBER];


    preia_comenzi(portii, livrari);
    calculeaza_produse(portii, subtotal, pret);
    aplica_reducere(portii, subtotal);
    adauga_livrare(livrari, subtotal); 
    pregateste_bon(numar_bon);  

    for (int i = 0; i < ORDER_NUMBER; i++) {
        printf("Comanda %d are %d, costa %f si are bonul %d\n", i + 1, portii[i], subtotal[i], numar_bon[i]);
    }
  
}