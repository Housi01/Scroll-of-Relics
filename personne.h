#ifndef PERSONNE_H
#define PERSONNE_H

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>

#define STAT_SOL 0
#define STAT_AIR 1
#define STAT_ATTACK 2

typedef struct {
    SDL_Rect position;              // Position x, y
    int frame;                      // Frame d'animation courante (0 à 7)
    double vitesse;                 // Vitesse horizontale
    double acceleration;            // Accélération horizontale
    int up;                         // Indicateur de saut
    double vy;                      // Vitesse verticale
    int status;                     // État (sol, air, attaque)
    int vie;                        // Nombre de vies
    int score;                      // Score du joueur
    int direction;                  // Direction (1 = droite, -1 = gauche, 0 = idle)
    int attack_frame;               // Frame d'animation d'attaque
    int jump_frame;                 // Frame d'animation de saut
    SDL_Surface *tab_right[8];      // Images de marche à droite
    SDL_Surface *tab_left[8];       // Images de marche à gauche
    SDL_Surface *tab_attack_right[5]; // Images d'attaque à droite
    SDL_Surface *tab_attack_left[5];  // Images d'attaque à gauche
    SDL_Surface *tab_jump_right[1];   // Images de saut à droite
    SDL_Surface *tab_jump_left[1];    // Images de saut à gauche
    int show_guide;                 // Indicateur pour afficher le guide (0 = caché, 1 = affiché)
} personnage;

void initialiser_personnage(personnage *p);
void afficher_personnage(personnage p, SDL_Surface *ecran);
void animate_personnage(personnage *p);
void movePerso(personnage *p, Uint32 dt);
void handle_input(SDL_Event event, personnage *p1, personnage *p2, int *running, int active_player);

#endif
