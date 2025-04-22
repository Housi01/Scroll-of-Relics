#ifndef ENNEMIE_H
#define ENNEMIE_H

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>

#define BACKGND_W 640
#define BACKGND_H 480
#define Ennemi_WIDTH 100
#define Ennemi_HEIGHT 150
#define SPRITE_ENNEMI_NbL 4
#define SPRITE_ENNEMI_NbCol 3

typedef struct {
    int i;
    int j;
} Frame;

typedef struct {
    SDL_Surface* image;
    SDL_Surface* imagep1;  // potion00.png
    SDL_Surface* imagep2;  // potion11.png
    SDL_Rect positionAbsolue;
    SDL_Rect positionES;   // Position statique pour potion00.png
    SDL_Rect positionES2;  // Position statique pour potion11.png
    SDL_Rect positionAnimation[SPRITE_ENNEMI_NbL][SPRITE_ENNEMI_NbCol];
    Frame Frame;
    int Direction;
    int State;
    int health;
    int isAlive;
    int isAttacking;  // Nouveau champ pour indiquer si l'ennemi attaque
    int showPotion2;  // Contrôle l'affichage de potion11.png
    Uint32 potionTimer;  // Timer pour afficher potion11.png après un délai
    TTF_Font* font;
} Ennemi;

int loadEnnemiImages(Ennemi* A);
void initEnnemiAttributes(Ennemi* E);
int init_ennemi(Ennemi* E);
int initES(Ennemi* ES);
void display_ennemi(Ennemi E, SDL_Surface* screen);
void display_ennemi2(Ennemi E, SDL_Surface* screen);
void display_ES(Ennemi ES, SDL_Surface* screen);
void display_health(Ennemi E, SDL_Surface* screen);
void move_ennemi(Ennemi* E, int level);
void animateEnnemi(Ennemi* E);
void freeEnnemi(Ennemi* E);

#endif
