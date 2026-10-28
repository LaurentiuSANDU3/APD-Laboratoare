#define ORDER_NUMBER 20
#define PRODUCT_PRICE 10
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
using namespace std;

void preia_comenzi(int portii[], bool livrari[])
{
    int randomNum[ORDER_NUMBER];
    int randomLivrari[ORDER_NUMBER];

    // Secvential: rand() foloseste o stare globala interna, deci NU e thread-safe.
    // Daca l-am apela din mai multe thread-uri, am avea data race si rezultate nesigure.
    for (int i = 0; i < ORDER_NUMBER; i++) {
        randomNum[i] = rand() % 6 + 1;
        randomLivrari[i] = rand() % 2;
    }

    // #pragma omp parallel for -> creeaza o echipa de thread-uri si imparte
    //   iteratiile buclei intre ele (fiecare thread primeste un bloc de iteratii).
    // shared(...)  -> toti thread-urile folosesc ACELEASI vectori (nu se copiaza).
    //   E sigur aici pentru ca fiecare thread scrie la indici diferiti (portii[i], livrari[i]),
    //   deci nu apar conflicte.
    // Variabila de bucla "i" este automat privata fiecarui thread.
    #pragma omp parallel for shared(portii, livrari, randomNum, randomLivrari)
    for (int i = 0; i < ORDER_NUMBER; i++) {
        portii[i]  = randomNum[i];
        livrari[i] = randomLivrari[i];
    }
    // La finalul lui "parallel for" exista o bariera implicita: toate thread-urile
    // asteapta aici pana termina toate, abia apoi continua programul.
}

void calculeaza_produse(int portii[], double subtotal[], int pret)
{
    // firstprivate(pret) -> fiecare thread primeste o COPIE proprie a lui "pret",
    //   initializata cu valoarea de dinainte de regiunea paralela.
    //   (Pentru o valoare doar citita ar merge si shared; ambele sunt corecte.)
    // Fiecare iteratie scrie in subtotal[i], independent de celelalte -> paralelizabil.
    #pragma omp parallel for shared(portii, subtotal) firstprivate(pret)
    for (int i = 0; i < ORDER_NUMBER; i++) {
        subtotal[i] = portii[i] * pret;
    }
}

void aplica_reducere(int portii[], double subtotal[])
{
    // Aceeasi idee: iteratiile sunt independente. Daca o comanda are >= 4 portii,
    // se aplica 10% reducere doar pe subtotal[i]-ul ei.
    #pragma omp parallel for shared(portii, subtotal)
    for (int i = 0; i < ORDER_NUMBER; i++) {
        if (portii[i] >= 4) {
            subtotal[i] = subtotal[i] * 0.9;
        }
    }
}

void adauga_livrare(bool livrari[], double subtotal[])
{
    // Era secventiala. Fiecare iteratie modifica doar subtotal[i] -> fara dependente
    // intre iteratii, deci se poate paraleliza fara probleme.
    #pragma omp parallel for shared(livrari, subtotal)
    for (int i = 0; i < ORDER_NUMBER; i++) {
        if (livrari[i]) {
            subtotal[i] += 5;
        }
    }
}

void pregateste_bon(int numar_bon[])
{
    // Era secventiala. numar_bon[i] depinde doar de i (nu de numar_bon[i-1]),
    // deci iteratiile sunt independente si se pot imparti intre thread-uri.
    #pragma omp parallel for shared(numar_bon)
    for (int i = 0; i < ORDER_NUMBER; i++) {
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

    // Ordinea apelurilor conteaza: fiecare functie foloseste rezultatul celei dinainte.
    // Intre ele exista bariera implicita de la finalul fiecarui "parallel for",
    // deci datele sunt gata cand incepe urmatoarea functie.
    preia_comenzi(portii, livrari);
    calculeaza_produse(portii, subtotal, pret);
    aplica_reducere(portii, subtotal);
    adauga_livrare(livrari, subtotal);
    pregateste_bon(numar_bon);

    // Afisarea ramane secventiala: printf din mai multe thread-uri ar amesteca liniile
    // si ar scoate comenzile in ordine aleatoare.
    for (int i = 0; i < ORDER_NUMBER; i++) {
        printf("Comanda %d are %d, costa %f si are bonul %d\n",
               i + 1, portii[i], subtotal[i], numar_bon[i]);
    }
}