#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include "ennemie.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

#define SPEED 3

// Charge les images de l'ennemi
int loadEnnemiImages(Ennemi* A) {
    if (A == NULL) return -1;
    A->image = IMG_Load("egypte.png");
    if (A->image == NULL) {
        printf("Unable to load Ennemi png: %s\n", SDL_GetError());
        return -1;
    }
    return 0;
}

// Initialise les attributs de l'ennemi
void initEnnemiAttributes(Ennemi* E) {
    if (E == NULL) return;
    int i, j;

    E->positionAbsolue.x = BACKGND_W / 2; // x=320
    E->positionAbsolue.y = 1.1* BACKGND_H - 170; // y=214
    E->positionAbsolue.w = Ennemi_WIDTH;
    E->positionAbsolue.h = Ennemi_HEIGHT;

    E->Frame.i = 0;
    E->Frame.j = 0;

    for (i = 0; i < SPRITE_ENNEMI_NbL; i++) {
        for (j = 0; j < SPRITE_ENNEMI_NbCol; j++) {
            E->positionAnimation[i][j].x = j * 145;
            E->positionAnimation[i][j].y = i * 130;
            E->positionAnimation[i][j].w = 130;
            E->positionAnimation[i][j].h = 160;
        }
    }

    E->Direction = 2;
    E->State = 0;
    E->health = 100;
    E->isAlive = 1;
    E->isAttacking = 0;
    E->showPotion2 = 0;
    E->potionTimer = 0;
}

// Initialise l'ennemi
int init_ennemi(Ennemi* E) {
    if (E == NULL) return -1;
    int OK = loadEnnemiImages(E);
    if (OK != -1) {
        initEnnemiAttributes(E);
    }
    return OK;
}

// Affiche l'ennemi 1
void display_ennemi(Ennemi E, SDL_Surface* screen) {
    if (E.image != NULL && screen != NULL && E.isAlive) {
        printf("Displaying enemy1 frame: row=%d, col=%d at position x=%d, y=%d\n",
               E.Frame.i, E.Frame.j, E.positionAbsolue.x, E.positionAbsolue.y);
        SDL_BlitSurface(E.image, &E.positionAnimation[E.Frame.i][E.Frame.j], screen, &E.positionAbsolue);
    }
}

// Affiche l'ennemi 2
void display_ennemi2(Ennemi E, SDL_Surface* screen) {
    if (E.image != NULL && screen != NULL && E.isAlive) {
        printf("Displaying enemy2 frame: row=%d, col=%d at position x=%d, y=%d\n",
               E.Frame.i, E.Frame.j, E.positionAbsolue.x, E.positionAbsolue.y);
        SDL_BlitSurface(E.image, &E.positionAnimation[E.Frame.i][E.Frame.j], screen, &E.positionAbsolue);
    }
}

// Déplace l'ennemi
// Déplace l'ennemi
void move_ennemi(Ennemi* E, int level) {
    if (E == NULL || !E->isAlive || E->isAttacking) return;

    if (level == 1) {
        // Mouvement horizontal pour ennemi1 (sprite sheet)
        if (E->Direction == 2) {
            E->positionAbsolue.x += SPEED;
            E->Frame.j++;
            if (E->Frame.j >= SPRITE_ENNEMI_NbCol)
                E->Frame.j = 0;
            printf("Moving right: col=%d, x=%d\n", E->Frame.j, E->positionAbsolue.x);

            if (E->positionAbsolue.x >= BACKGND_W - E->positionAbsolue.w) {
                E->positionAbsolue.x = BACKGND_W - E->positionAbsolue.w;
                E->Direction = 1;
                E->Frame.i = 1;
            }
        } else {
            E->positionAbsolue.x -= SPEED;
            E->Frame.j++;
            if (E->Frame.j >= SPRITE_ENNEMI_NbCol)
                E->Frame.j = 0;
            printf("Moving left: col=%d, x=%d\n", E->Frame.j, E->positionAbsolue.x);

            if (E->positionAbsolue.x <= BACKGND_W / 2) {
                E->positionAbsolue.x = BACKGND_W / 2;
                E->Direction = 2;
                E->Frame.i = 0;
            }
        }
    } else if (level == 2) {
        // Mouvement vertical pour ennemi2 (image unique)
        E->positionAbsolue.y -= SPEED; // Déplacer vers le haut
        printf("Moving up: y=%d\n", E->positionAbsolue.y);

        // Si l'ennemi atteint le haut de l'écran, réinitialiser à la position initiale
        if (E->positionAbsolue.y <= 0) {
            E->positionAbsolue.y = 1.1 * BACKGND_H - 170; // Retour à la position initiale (y=214)
            printf("Reached top, resetting to y=%d\n", E->positionAbsolue.y);
        }
    }
}

// Anime l'ennemi
void animateEnnemi(Ennemi* E) {
    if (E == NULL || !E->isAlive) return;

    if (!E->isAttacking) {
        E->Frame.j++;
        if (E->Frame.j >= SPRITE_ENNEMI_NbCol)
            E->Frame.j = 0;
    } else {
        // Animation d'attaque : utilise les rangées 2 ou 3
        E->Frame.j++;
        if (E->Frame.j >= SPRITE_ENNEMI_NbCol)
            E->Frame.j = 0;
    }
    SDL_Delay(100);
}

// Charge les images de l'ES (potions)
int loadES(Ennemi* ES) {
    if (ES == NULL) return -1;
    ES->imagep1 = IMG_Load("potion00.png");
    if (ES->imagep1 == NULL) {
        printf("Unable to load potion00.png: %s\n", SDL_GetError());
        return -1;
    }
    ES->imagep2 = IMG_Load("potion11.png");
    if (ES->imagep2 == NULL) {
        printf("Unable to load potion11.png: %s\n", SDL_GetError());
        return -1;
    }
    return 0;
}

// Initialise l'ES (potions statiques)
int initES(Ennemi* ES) {
    int lo = loadES(ES);
    if (lo != -1) {
        ES->positionES.x = 200;
        ES->positionES.y = 440;
        ES->positionES.w = 100;
        ES->positionES.h = 100;

        ES->positionES2.x = (BACKGND_W / 2) + (BACKGND_W / 4); // x = 480
        ES->positionES2.y = 200;
        ES->positionES2.w = 100;
        ES->positionES2.h = 100;

        ES->showPotion2 = 0;
        ES->potionTimer = SDL_GetTicks(); // Démarrer le timer
    }
    return lo;
}

// Affiche l'ES
void display_ES(Ennemi ES, SDL_Surface* screen) {
    if (ES.imagep1 != NULL && screen != NULL) {
        printf("Displaying potion at x=%d, y=%d\n", ES.positionES.x, ES.positionES.y);
        SDL_BlitSurface(ES.imagep1, NULL, screen, &ES.positionES);
    }
    if (ES.showPotion2 && ES.imagep2 != NULL && screen != NULL) {
        printf("Displaying potion11 at x=%d, y=%d\n", ES.positionES2.x, ES.positionES2.y);
        SDL_BlitSurface(ES.imagep2, NULL, screen, &ES.positionES2);
    }
}

// Affiche la santé de l'ennemi
void display_health(Ennemi E, SDL_Surface* screen) {
    if (!E.isAlive || E.font == NULL || screen == NULL) return;

    char healthText[32];
    snprintf(healthText, sizeof(healthText), "Enemy Health: %d", E.health);

    SDL_Color textColor = {255, 255, 255, 0};
    SDL_Surface* textSurface = TTF_RenderText_Solid(E.font, healthText, textColor);
    if (textSurface == NULL) {
        printf("Unable to render text: %s\n", TTF_GetError());
        return;
    }

    SDL_Rect textPosition = {400, 10, 0, 0};
    SDL_BlitSurface(textSurface, NULL, screen, &textPosition);
    SDL_FreeSurface(textSurface);
}

// Libère les ressources
void freeEnnemi(Ennemi* E) {
    if (E != NULL) {
        if (E->image != NULL) {
            SDL_FreeSurface(E->image);
            E->image = NULL;
        }
        if (E->imagep1 != NULL) {
            SDL_FreeSurface(E->imagep1);
            E->imagep1 = NULL;
        }
        if (E->imagep2 != NULL) {
            SDL_FreeSurface(E->imagep2);
            E->imagep2 = NULL;
        }
        if (E->font != NULL) {
            TTF_CloseFont(E->font);
            E->font = NULL;
        }
    }
}
