#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mersenne.c"

int simulare (int n) //Returneaza uniform indicele probabilitatii p
{
    int k;
    double u;
    u=genrand_real2();
    k=(int)(u*n);
    return k;
}
void eroare() //Afiseaza mesaj de eroare in cazul in care alocarea dinamica nu este efectuata corect
{
    printf("Eroare alocare dinamica a memoriei. \n");
}
void permutare(double *p, int K) // Algoritmul ce genereaza o permutare aleatoare
{
    int i,k;
    double aux;
    for(i=K-1; i>=0; i--)
    {
        k=simulare(i);
        aux=p[i];
        p[i]=p[k];
        p[k]=aux;
    }
}

double Bernoulli( double prob ) // Simuleaza distributia Bernoulli de probabilitate prob
{
    double u;
    u=genrand_real2();
    if( u <= prob )
        return 1;
    else
        return 0;
}
void castigmax(int K, double *recmed, double *cmax, int *I) // Determina recompensa medie maxima si indicele aparatului care a condus la recompensa maxima
{
    int i;
    *cmax=0;
    for(i=0; i<K; i++)
        if(*cmax<recmed[i])
        {
            *cmax=recmed[i];
            *I=i;
        }
}

void recmediu(double *recmed, double rec, int *nrselect, int i ) // Actualizarea vectorului recmed dupa determinarea recompensei aferente unui aparat in fiecare runda
{
    int s;
    s=nrselect[i];
    recmed[i] = ((s-1)*recmed[i] + rec)/s;

}
int selectap( double epsilon, int *I, int K ) // Selecteaza un aparat conform strategiei epsilon-greedy
{
    double u;
    u = genrand_real2();
    if( u < epsilon )
        return simulare(K);
    else
        return *I;
}

int main()
{
    FILE *g;
    double *p ,*recmed, *cmax, epsilon=0.1, rec, *regret, s=0;
    int K=5, *nrselect, *I, r, i, n=5000, a;

    time_t secunde;
    secunde=time(NULL);
    init_genrand(secunde);

    nrselect = (int *) calloc(K, sizeof(int));
    if (!nrselect)
      eroare();
    p = (double *) malloc(K* sizeof(double));
    if (!p)
      eroare();
    regret = (double *) calloc(n, sizeof(double));
    if (!regret)
      eroare();
    recmed = (double *) calloc(K, sizeof(double));
    if (!recmed)
      eroare();
    I = (int *) calloc(1, sizeof(int));
    if (!I)
      eroare();
    cmax = (int *) malloc(1* sizeof(int));
    if (!cmax)
      eroare();
    p[0]=0.2; p[1]=0.1; p[2]=0.1; p[3]=0.5; p[4]=0.8;

    permutare(p,K);

    for(r=0;r<n;r++)
    {
        castigmax(K, recmed, cmax, I);
        a=selectap( epsilon, I, K );
        rec=Bernoulli( p[a]);
        nrselect[a]++;
        recmediu(recmed, rec,  nrselect, a );
        s=s+rec;
        regret[r]=(*cmax)*r-s;

    }

    printf("Epsilon= %.2lf, Numarul de aparate= %d, Numarul de runde= %d \n", epsilon, K, n);
    printf("Probabilitatile de returnare a recompensei= ");
    for(i=0;i<K;i++)
      printf("%.2lf ", p[i]);
    printf("\n");

    printf("Coordonatele vectorului nrselect= ");
    for(i=0;i<K;i++)
      printf("(%d,%d) ", i, nrselect[i]);
    printf("\n");
    printf("Coordonatele vectorului recmed= ");

    for(i=0;i<K;i++)
      printf("(%d,%.2lf) ", i, recmed[i]);
    printf("\n");
    g=fopen("MAB.txt","w");
    if(!g)
    {
        perror("Eroare la deschiderea fisierului.\n");
        exit(EXIT_FAILURE);
    }
   for(i=0; i<K; i++)
        printf("(%d,%.2lf) ", i, recmed[i]);
    printf("\n");
    g=fopen("MAB.txt","w");
    if(!g)
    {
        perror("Eroare la deschiderea fisierului.\n");
        exit(EXIT_FAILURE);
    }
    fprintf(g," %.2lf %d %d \n", epsilon, K, n);
    for(i=0; i<K; i++)
        fprintf(g,"%.2lf ", p[i]);
    fprintf(g,"\n");
    for(i=0; i<K; i++)
        fprintf(g,"%d ", nrselect[i]);
    fprintf(g,"\n");
    for(i=0; i<K; i++)
        fprintf(g,"%.2lf ", recmed[i]);
    fprintf(g,"\n");
    for(i=0; i<n; i++)
        fprintf(g, "%.2lf \n", regret[i]);

    free(p);
    free(recmed);
    free(cmax);
    free(regret);
    free(nrselect);
    free(I);
    fclose(g);
    return 0;
}
