#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <pthread.h>
#include <windows.h>
#include <mmsystem.h>
//link pthread && winmm

void inizializzaPac();
void inizializzaFantasma();
void resettaPuntiMappa();
void* avviaCronometro(void* a);
void* avviaMusica(void *a);
void* MovimentoFantasmaRandomico (void *a);
void refreshMap();
char controlloFrecce();
void muoviPac();


pthread_t id;
pthread_t idMusica;
int sceltaMusica = 0;
int colore=0;
int toccato = 0;
int altezza = 18;
int lunghezza = 43;
char matrice [18][43] =
{
    "|-----------------------------------------|",
    "|.........................................|",
    "|.|--------------|.|---|.|--------------|.|",
    "|.|##############|.|###|.|##############|.|",
    "|.|--------------|.|---|.|--------------|.|",
    "|...................    ..................|", // X(5;21)
    "|.|-----|.|----|.|-------|.|----|.|-----|.|",
    "|.|#####|.|####|.|#######|.|####|.|#####|.|",
    "|.|#####|.|####|.|#######|.|####|.|#####|.|",
    "|.|-----|.|----|.|-------|.|----|.|-----|.|",
    "|....................O....................|",   // O(10;21)
    "|.|---|.|----|.|-----------|.|----|.|---|.|",
    "|.|---|........|###########|........|---|.|",
    "|.......|----|.|###########|.|----|.......|",
    "|.|---|.|####|.|###########|.|####|.|---|.|",
    "|.|---|.|----|.|-----------|.|----|.|---|.|",
    "|.........................................|",
    "|-----------------------------------------|"

};

char start[13][107]=
{
    "###########################################################################################################",
    "###########|--------|####-------######--------|###########|---######---|####-------######|----\\######|--|##",
    "###########|        |###/       \\####/        |###########|   \\####/   |###/       \\#####|     \\#####|  |##",
    "###########|  |--|  |##|  |---|  |##|   /-----|###########|    \\--/    |##|  |---|  |####|  |\\  \\####|  |##",
    "###########|        |##|  |---|  |##|   |#################|            |##|  |---|  |####|  |#\\  \\###|  |##",
    "###########|   |----|##|         |##|   |#######|      |##|  |-\\  /-|  |##|         |####|  |##\\  \\##|  |##",
    "###########|   |#######|  |---|  |##|   |#################|  |##--##|  |##|  |---|  |####|  |###\\  \\#|  |##",
    "###########|   |#######|  |###|  |##|   \\-----|###########|  |######|  |##|  |###|  |####|  |####\\  \\|  |##",
    "###########|   |#######|  |###|  |###\\        |###########|  |######|  |##|  |###|  |####|  |#####\\     |##",
    "###########|---|#######|--|###|--|####--------|###########|--|######|--|##|--|###|--|####|--|######\\----|##",
    "###########################################################################################################",
    "###########################################################################################################"
};

struct PacMan
{
    int x;
    int y;
    int nx;
    int ny;
    int vite;
    int punteggio;
    int punteggioDaRaggiungere;
    char spostamentoPrecedente; // salva l'ultimo spostamento eseguito =1 alto, =2 giu, =3 destra, =4 sinistra, di default parte verso destra
} PacMan1;

struct fantasma
{
    pthread_t id;
    int x;
    int y;
    int direzione;
    int puntino;
    int toccato;
} f1, f2, f3, f4;

struct Cronometro
{
    int minuti;
    int secondi;
} Cronometro1;

void inizializzaPac()
{
    PacMan1.x=10;
    PacMan1.y=21;
    matrice[PacMan1.x][PacMan1.y]='O';
    PacMan1.nx=0;
    PacMan1.ny=0;
    PacMan1.vite=3;
    PacMan1.punteggio=0;
    PacMan1.punteggioDaRaggiungere=247;
    PacMan1.spostamentoPrecedente=' ';
}

void inizializzaFantasma()
{
    f1.x=5;
    f1.y=21;
    f2.x=5;
    f2.y=22;
    f3.x=5;
    f3.y=20;
    f4.x=5;
    f4.y=23;
    f1.puntino = 0;
    f2.puntino = 0;
    f3.puntino = 0;
    f4.puntino = 0;
    f1.direzione=0;
    f2.direzione=0;
    f3.direzione=0;
    f4.direzione=0;
}

void inizializzaCronometro()
{
    Cronometro1.minuti=0;
    Cronometro1.secondi=0;
}

void controlloPuntino(struct fantasma* f)
{
    if(f->puntino == 0)
    {
        matrice[f->x][f->y] = ' ';
    }
    else
    {
        matrice[f->x][f->y] = '.';
    }
}

int main()
{

    system("COLOR F2");
    printf("\n\n\n\n\n\n\n");
    for (int i = 0; i < 13 ; i++)
    {
        for (int j = 0; j < 107 ; j++)
        {
            if(start[i][j]==' '){
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),72|BACKGROUND_INTENSITY);
                printf("%c",start[i][j]);
            } else{
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),254|BACKGROUND_INTENSITY);
                printf(" ");
            }
        }
        printf("\n");
    }
    printf("\x1b[31m");
    printf("                                              BY YASSINE & GIOVANNI");
    pthread_create(&idMusica, NULL, avviaMusica,(void *)&sceltaMusica);
    Sleep(4500);
    system("COLOR 0");
    int doveAndavaIlFantasma=0;
    srand(time (NULL));

    inizializzaPac();
    inizializzaFantasma();
    inizializzaCronometro();
    sceltaMusica = 1;

    pthread_create(&id, NULL, avviaCronometro,NULL);
    pthread_create(&idMusica, NULL, avviaMusica,(void *)&sceltaMusica);
    pthread_create(&f1.id, NULL, MovimentoFantasmaRandomico,(void *)&f1);
    pthread_create(&f2.id, NULL, MovimentoFantasmaRandomico,(void *)&f2);
    pthread_create(&f3.id, NULL, MovimentoFantasmaRandomico,(void *)&f3);
    pthread_create(&f4.id, NULL, MovimentoFantasmaRandomico,(void *)&f4);

    while(PacMan1.vite>0 && PacMan1.punteggio<PacMan1.punteggioDaRaggiungere)
    {
        refreshMap();
        switch(controlloFrecce())
        {
        case 'a':
            PacMan1.nx=-1;
            PacMan1.ny=0;
            PacMan1.spostamentoPrecedente='a';
            break;

        case 'g':
            PacMan1.nx=1;
            PacMan1.ny=0;
            PacMan1.spostamentoPrecedente='g';
            break;

        case 'd':
            PacMan1.nx=0;
            PacMan1.ny=1;
            PacMan1.spostamentoPrecedente='d';
            break;

        case 's':
            PacMan1.nx=0;
            PacMan1.ny=-1;
            PacMan1.spostamentoPrecedente='s';
            break;
        }

        muoviPac();

        Sleep(100); //ms
    }
    refreshMap();
    if (PacMan1.vite>0){
        sceltaMusica = 2;
        pthread_create(&idMusica, NULL, avviaMusica,(void *)&sceltaMusica);
    } else{
        sceltaMusica = 3;
        pthread_create(&idMusica, NULL, avviaMusica,(void *)&sceltaMusica);
    }
    printf("                END");
    Sleep(4000);
    return 0;
}


void *avviaCronometro(void *a)
{
    while(PacMan1.vite>0 || PacMan1.punteggio<PacMan1.punteggioDaRaggiungere)
    {
        if(Cronometro1.secondi==59)
        {
            Sleep(1000);
            Cronometro1.secondi=0;
            Cronometro1.minuti++;
        }
        else
        {
            Sleep(1000);
            Cronometro1.secondi++;
        }
    }
    return NULL;
}

void *avviaMusica(void *a)
{
    int controllo = *(int *)a;
    switch(controllo){
    case 0:
        PlaySound("Musica\\pacman_beginning.wav", NULL, SND_FILENAME);
        break;
    case 1:
        while(PacMan1.vite > 0 && PacMan1.punteggio < PacMan1.punteggioDaRaggiungere){
            PlaySound("Musica\\PacMan_Siren.wav", NULL, SND_FILENAME);
        }
        break;
    case 2:
        PlaySound("Musica\\win_jingle.wav", NULL, SND_FILENAME);
        break;
    case 3:
        PlaySound("Musica\\pacman_death.wav", NULL, SND_FILENAME);
        break;
    }
}


void* MovimentoFantasmaRandomico (void *a)
{
    srand(time(NULL));
    struct fantasma* f = (struct fantasma *)a;
    while(PacMan1.vite>0 || PacMan1.punteggio<PacMan1.punteggioDaRaggiungere)
    {
        f->direzione = rand() %4;
        while ((f->x == 5 && f->y == 21) && (f->direzione == 2 || f->direzione == 3))
        {
            f->direzione = rand() %4;
        }
        switch(f->direzione)
        {
        case 0: //sinistra
            while (1 > 0)
            {
                Sleep(200);
                if (f->toccato == 1)
                {
                    controlloPuntino(&f1);
                    controlloPuntino(&f2);
                    controlloPuntino(&f3);
                    controlloPuntino(&f4);
                    inizializzaFantasma();
                    matrice[f->x][f->y]='X';
                    f->toccato = 0;
                    break;
                }
                if (matrice[f->x][f->y-1] == '|' || matrice[f->x][f->y-1] == 'X')
                {
                    break;
                }

                if(matrice[f->x][f->y - 1]=='O')
                {
                    --PacMan1.vite;
                    matrice[PacMan1.x][PacMan1.y]=' ';
                    controlloPuntino(&f1);
                    controlloPuntino(&f2);
                    controlloPuntino(&f3);
                    controlloPuntino(&f4);
                    PacMan1.x=10;
                    PacMan1.y=21;
                    inizializzaFantasma();
                    matrice[f->x][f->y]='X';
                    matrice[PacMan1.x][PacMan1.y]='O';
                    break;
                }
                controlloPuntino(f);

                if (matrice[(f->x)][(f->y) - 1] == '.')
                {
                    f->puntino = 1;
                }
                else
                {
                    f->puntino = 0;
                }
                f->y--;
                matrice[f->x] [f->y] = 'X';
                if (matrice[f->x + 1][f->y] == ' ' || matrice[f->x - 1][f->y] == ' ' || matrice[f->x + 1][f->y] == '.' || matrice[f->x - 1][f->y] == '.')
                {
                    break;
                }
            }
            break;
        case 1: //destra
            while (1 > 0)
            {
                Sleep(200);
                if (f->toccato == 1)
                {
                    controlloPuntino(&f1);
                    controlloPuntino(&f2);
                    controlloPuntino(&f3);
                    controlloPuntino(&f4);
                    inizializzaFantasma();
                    matrice[f->x][f->y]='X';
                    f->toccato = 0;
                    break;
                }
                if (matrice[f->x][f->y+1] == '|' || matrice[f->x][f->y+1] == 'X')
                {
                    break;
                }

                if(matrice[f->x][f->y + 1]=='O')
                {
                    --PacMan1.vite;
                    matrice[PacMan1.x][PacMan1.y]=' ';
                    controlloPuntino(&f1);
                    controlloPuntino(&f2);
                    controlloPuntino(&f3);
                    controlloPuntino(&f4);
                    PacMan1.x=10;
                    PacMan1.y=21;
                    inizializzaFantasma();
                    matrice[f->x][f->y]='X';
                    matrice[PacMan1.x][PacMan1.y]='O';
                    break;
                }
                controlloPuntino(f);

                if (matrice[(f->x)][(f->y) + 1] == '.')
                {
                    f->puntino = 1;
                }
                else
                {
                    f->puntino = 0;
                }
                f->y++;
                matrice[f->x] [f->y] = 'X';
                if (matrice[f->x + 1][f->y] == ' ' || matrice[f->x - 1][f->y] == ' ' || matrice[f->x + 1][f->y] == '.' || matrice[f->x - 1][f->y] == '.')
                {
                    break;
                }
            }
            break;
        case 2: //su
            while (1 > 0)
            {
                Sleep(200);
                if (f->toccato == 1)
                {
                    controlloPuntino(&f1);
                    controlloPuntino(&f2);
                    controlloPuntino(&f3);
                    controlloPuntino(&f4);
                    inizializzaFantasma();
                    matrice[f->x][f->y]='X';
                    f->toccato = 0;
                    break;
                }
                if (matrice[f->x-1][f->y] == '|' || matrice[f->x-1][f->y] == '-' || matrice[f->x-1][f->y] == 'X')
                {
                    break;
                }

                if(matrice[f->x -1][f->y]=='O')
                {
                    --PacMan1.vite;
                    matrice[PacMan1.x][PacMan1.y]=' ';
                    controlloPuntino(&f1);
                    controlloPuntino(&f2);
                    controlloPuntino(&f3);
                    controlloPuntino(&f4);
                    PacMan1.x=10;
                    PacMan1.y=21;
                    inizializzaFantasma();
                    matrice[f->x][f->y]='X';
                    matrice[PacMan1.x][PacMan1.y]='O';
                    break;
                }
                controlloPuntino(f);

                if (matrice[(f->x)-1][f->y] == '.')
                {
                    f->puntino = 1;
                }
                else
                {
                    f->puntino = 0;
                }
                f->x--;
                matrice[f->x] [f->y] = 'X';
                if (matrice[f->x][f->y - 1] == ' ' || matrice[f->x][f->y + 1] == ' ' || matrice[f->x][f->y - 1] == '.' || matrice[f->x][f->y + 1] == '.')
                {
                    break;
                }
            }
            break;
        case 3: //giù
            while (1 > 0)
            {
                Sleep(200);
                if (f->toccato == 1)
                {
                    controlloPuntino(&f1);
                    controlloPuntino(&f2);
                    controlloPuntino(&f3);
                    controlloPuntino(&f4);
                    inizializzaFantasma();
                    matrice[f->x][f->y]='X';
                    f->toccato = 0;
                    break;
                }
                if (matrice[f->x+1][f->y] == '|' || matrice[f->x+1][f->y] == '-' || matrice[f->x+1][f->y] == 'X')
                {
                    break;
                }

                if(matrice[f->x +1][f->y]=='O')
                {
                    --PacMan1.vite;
                    matrice[PacMan1.x][PacMan1.y]=' ';
                    controlloPuntino(&f1);
                    controlloPuntino(&f2);
                    controlloPuntino(&f3);
                    controlloPuntino(&f4);
                    PacMan1.x=10;
                    PacMan1.y=21;
                    inizializzaFantasma();
                    matrice[f->x][f->y]='X';
                    matrice[PacMan1.x][PacMan1.y]='O';
                    break;
                }

                controlloPuntino(f);

                if (matrice[(f->x)+1][f->y] == '.')
                {
                    f->puntino = 1;
                }
                else
                {
                    f->puntino = 0;
                }

                f->x++;
                matrice[f->x] [f->y] = 'X';
                if (matrice[f->x][f->y - 1] == ' ' || matrice[f->x][f->y + 1] == ' ' || matrice[f->x][f->y - 1] == '.' || matrice[f->x][f->y + 1] == '.')
                {
                    break;
                }
            }
            break;
        }
    }
    pthread_exit(NULL);
    return NULL;
}

void refreshMap()
{
    system("@cls||clear");
    //printf("\x1b[35m");
    printf("\x1b[36mVite: \x1b[32m%d \x1b[36mPunteggio: \x1b[33m%d \x1b[36mCompletamento: \x1b[33m%.2f%% \n",PacMan1.vite,PacMan1.punteggio,((float)PacMan1.punteggio/PacMan1.punteggioDaRaggiungere*100));
    for (int i = 0; i < altezza ; i++)
    {
        for (int j = 0; j < lunghezza ; j++)
        {
            if(matrice[i][j]=='.')
            {
                printf("\x1b[33m");
                printf(".");
            }
            if(matrice[i][j]=='O')
            {
                switch(colore%2)
                {
                case 0:
                    printf("\x1b[32m");
                    break;
                default:
                    printf("\x1b[36m");
                    break;
                }
                printf("O");
            }
            if(matrice[i][j]=='-')
            {
                printf("\x1b[34m");
                printf("-");
            }
            if(matrice[i][j]=='|')
            {
                printf("\x1b[34m");
                printf("|");
            }
            if(matrice[i][j]=='#')
            {
                colore++;
                switch(colore)
                {
                case 0:
                    printf("\x1b[31m");
                    break;
                case 1:
                    printf("\x1b[33m");
                    break;
                case 2:
                    printf("\x1b[32m");
                    break;
                    break;
                case 3:
                    printf("\x1b[34m");
                    break;
                case 4:
                    printf("\x1b[35m");
                    break;
                case 5:
                    colore=0;
                }
                printf("#");
            }
            if(matrice[i][j]=='X')
            {
                printf("\x1b[31m");
                printf("X");
            }
            if(matrice[i][j]==' ')
            {
                printf("\x1b[34m");
                printf(" ");
            }
            printf("\x1b[0m");

        }
        printf("\n");
    }
    printf("\x1b[36mTempo trascorso: \x1b[31m%d:%d",Cronometro1.minuti,Cronometro1.secondi);
}

char controlloFrecce()
{
    if (_kbhit())
    {
        char c1 = _getch();

        if (c1 == -32)
        {
            char c2 = _getch();

            switch (c2)
            {
            case 72:
                return 'a';
            case 80:
                return 'g';
            case 77:
                return 'd';
            case 75:
                return 's';
            }
        }
    }
    return PacMan1.spostamentoPrecedente;
}

void muoviPac()
{
    if (matrice[PacMan1.x+PacMan1.nx][PacMan1.y+PacMan1.ny]=='.')
    {
        matrice[PacMan1.x][PacMan1.y]=' ';
        matrice[PacMan1.x+=PacMan1.nx][PacMan1.y+=PacMan1.ny]='O';
        ++PacMan1.punteggio;
        MessageBeep(MB_ICONINFORMATION);
    }
    else if (matrice[PacMan1.x+PacMan1.nx][PacMan1.y+PacMan1.ny]==' ')
    {
        matrice[PacMan1.x][PacMan1.y]=' ';
        matrice[PacMan1.x+=PacMan1.nx][PacMan1.y+=PacMan1.ny]='O';

    }
    else if(matrice[PacMan1.x+PacMan1.nx][PacMan1.y+PacMan1.ny]=='X')
    {
        --PacMan1.vite;
        matrice[PacMan1.x][PacMan1.y]=' ';
        controlloPuntino(&f1);
        controlloPuntino(&f2);
        controlloPuntino(&f3);
        controlloPuntino(&f4);
        PacMan1.x=10;
        PacMan1.y=21;
        f1.toccato = 1;
        f2.toccato = 1;
        f3.toccato = 1;
        f4.toccato = 1;
        matrice[f1.x][f1.y]='X';
        matrice[f2.x][f2.y]='X';
        matrice[f3.x][f3.y]='X';
        matrice[f4.x][f4.y]='X';
        matrice[PacMan1.x][PacMan1.y]='O';
    }
}



