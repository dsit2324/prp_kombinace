#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 100
#define INPUTFILE "startovni_listina_kombinace_zeny.txt"
#define OUTPUTFILE "vysledkova_listina_kombinace_zeny.txt"
#define DELIMITERS " :,"

typedef struct {
    int minuty;
    int sekundy;
    int setiny;
} SJEZD;

typedef struct {
    int sekundy;
    int setiny;
} SLALOM;

typedef struct {
    int cislo;
    char jmeno[15];
    char prijmeni[15];
    char stat[5];
    SJEZD sjezd;
    SLALOM slalom;
} ZENY;

ZENY *readFile(ZENY *zeny, int *pocet) {
    FILE *fr = fopen(INPUTFILE, "r");
    if (fr == NULL) {
        printf("Nepodarilo se otevrit soubor %s.\n", INPUTFILE);
        return NULL;
    }
    int i = 0;
    int j;
    char *token = NULL;
    char radek[MAX];
    ZENY *pomocny = NULL;
    while (fgets(radek, MAX, fr) != NULL) {
        if (i != 0) {
            pomocny = (ZENY *) realloc(zeny, sizeof(ZENY) * i);
            if (pomocny == NULL) {
                printf("Neuspesna realokace pametoveho bloku.\n");
                free(zeny);
            } else {
                zeny = pomocny;
                token = strtok(radek, DELIMITERS);
                j = 0;
                while (token != NULL) {
                    switch (j) {
                        case 0:
                            zeny[i - 1].cislo = atoi(token);
                            break;
                        case 1:
                            strcpy(zeny[i - 1].jmeno, token);
                            break;
                        case 2:
                            strcpy(zeny[i - 1].prijmeni, token);
                            break;
                        case 3:
                            strcpy(zeny[i - 1].stat, token);
                            break;
                        case 4:
                            zeny[i - 1].sjezd.minuty = atoi(token);
                            break;
                        case 5:
                            zeny[i - 1].sjezd.sekundy = atoi(token);
                            break;
                        case 6:
                            zeny[i - 1].sjezd.setiny = atoi(token);
                            break;
                        case 7:
                            zeny[i - 1].slalom.sekundy = atoi(token);
                            break;
                        case 8:
                            zeny[i - 1].slalom.setiny = atoi(token);
                            break;
                    }
                    j++;
                    token = strtok(NULL, DELIMITERS);
                }
            }
        }
        i++;
    }
    *pocet = i - 1;
    if (fclose(fr) == EOF) {
        printf("Nepodarilo se zavrit soubor %s.\n", INPUTFILE);
    }
    return zeny;
}

void vypis(ZENY *zeny, int pocet) {
    printf("Alpske lyzovani startovni listina K O M B I N A C E  Z E N Y\n");
    printf("------------------------------------------------------------\n");
    printf("startovni cislo      jmeno       prijmeni    stat    sjezd   slalom\n");
    printf("-------------------------------------------------------------------\n");
    for (int i = 0; i < pocet; i++) {
        printf("%10d %15s %15s %5s %3d:%02d,%02d %5.02d,%02d\n", zeny[i].cislo, zeny[i].jmeno, zeny[i].prijmeni,
               zeny[i].stat,
               zeny[i].sjezd.minuty, zeny[i].sjezd.sekundy, zeny[i].sjezd.setiny, zeny[i].slalom.sekundy,
               zeny[i].slalom.setiny);
    }
}

int prevodCasuSjezd(SJEZD cas) {
    return cas.minuty * 6000 + cas.sekundy * 100 + cas.setiny;
}

int prevodCasuSlalom(SLALOM cas) {
    return cas.sekundy * 100 + cas.setiny;
}

int soucetCasu(SJEZD sjezd, SLALOM slalom) {
    return prevodCasuSjezd(sjezd) + prevodCasuSlalom(slalom);
}

void swap(ZENY *zena1, ZENY *zena2) {
    ZENY temp = *zena1;
    *zena1 = *zena2;
    *zena2 = temp;
}

void bubbleSort(ZENY *zeny, int pocet) {
    for (int i = 0; i < pocet - 1; i++) {
        for (int j = 0; j < pocet - 1 - i; j++) {
            if (soucetCasu(zeny[j].sjezd, zeny[j].slalom) > soucetCasu(zeny[j + 1].sjezd, zeny[j + 1].slalom)) {
                swap(&zeny[j], &zeny[j + 1]);
            }
        }
    }
}

void doSouboru(ZENY *zeny, int pocet) {
    FILE *fw = fopen(OUTPUTFILE, "w");
    if (fw == NULL) {
        printf("Nepodarilo se otevrit soubor %s.\n", OUTPUTFILE);
    }
    else if (fw != NULL) {
        printf("\nSoubor %s byl vytvoren.\n", OUTPUTFILE);
    }
    int ztrata;
    bubbleSort(zeny, pocet);
    fprintf(fw, "A L P S K E  L Y Z O V A N I  -  K O M B I N A C E  Z E N Y\n");
    fprintf(fw, "-----------------------------------------------------------\n");
    fprintf(fw, "poradi      jmeno       prijmeni    stat    sjezd   slalom    celkem    ztrata\n");
    fprintf(fw, "------------------------------------------------------------------------------\n");
    for (int i = 0; i < pocet; i++) {
        if (i == 0) {
            fprintf(fw, "%10d %15s %15s %5s %3d:%02d,%02d %5.02d,%02d %3d:%02d,%02d\n", i, zeny[i].jmeno,
                    zeny[i].prijmeni, zeny[i].stat,
                    zeny[i].sjezd.minuty, zeny[i].sjezd.sekundy, zeny[i].sjezd.setiny, zeny[i].slalom.sekundy,
                    zeny[i].slalom.setiny,
                    soucetCasu(zeny[i].sjezd, zeny[i].slalom) / 6000,
                    soucetCasu(zeny[i].sjezd, zeny[i].slalom) % 6000 / 100,
                    soucetCasu(zeny[i].sjezd, zeny[i].slalom) % 100);
        } else {
            ztrata = soucetCasu(zeny[i].sjezd, zeny[i].slalom) - soucetCasu(zeny[0].sjezd, zeny[0].slalom);
            fprintf(fw, "%10d %15s %15s %5s %3d:%02d,%02d %5.02d,%02d %3d:%02d,%02d %3d:%02d,%02d\n", i, zeny[i].jmeno,
                    zeny[i].prijmeni, zeny[i].stat,
                    zeny[i].sjezd.minuty, zeny[i].sjezd.sekundy, zeny[i].sjezd.setiny, zeny[i].slalom.sekundy,
                    zeny[i].slalom.setiny,
                    soucetCasu(zeny[i].sjezd, zeny[i].slalom) / 6000,
                    soucetCasu(zeny[i].sjezd, zeny[i].slalom) % 6000 / 100,
                    soucetCasu(zeny[i].sjezd, zeny[i].slalom) % 100,
                    ztrata / 3600, ztrata % 3600 / 100, ztrata % 100);
        }
    }
    if (fclose(fw) == EOF) {
        printf("Nepodarilo se zavrit soubor %s.\n", OUTPUTFILE);
    }
}

int main(void) {
    ZENY *zeny = NULL;
    int pocet = 0;
    zeny = readFile(zeny, &pocet);
    vypis(zeny, pocet);
    doSouboru(zeny, pocet);
    free(zeny);
    return 0;
}
