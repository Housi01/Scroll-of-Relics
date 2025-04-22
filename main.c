#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "personne.h"
#include "ennemie.h"

// Fonction pour vérifier la collision joueur/ennemi
int checkPlayerEnemyCollision(personnage* p, Ennemi* e) {
    if (!e->isAlive) return 0;

    int playerRight = p->position.x + p->position.w;
    int playerBottom = p->position.y + p->position.h;
    int enemyRight = e->positionAbsolue.x + e->positionAbsolue.w;
    int enemyBottom = e->positionAbsolue.y + e->positionAbsolue.h;

    int colliding = (p->position.x < enemyRight &&
                     playerRight > e->positionAbsolue.x &&
                     p->position.y < enemyBottom &&
                     playerBottom > e->positionAbsolue.y);

    return colliding;
}

// Fonction pour vérifier la collision joueur/ES1
int checkPlayerESCollision(personnage* p, Ennemi* es) {
    int playerRight = p->position.x + p->position.w;
    int playerBottom = p->position.y + p->position.h;
    int esRight = es->positionES.x + es->positionES.w;
    int esBottom = es->positionES.y + es->positionES.h;

    int colliding = (p->position.x < esRight &&
                     playerRight > es->positionES.x &&
                     p->position.y < esBottom &&
                     playerBottom > es->positionES.y);

    return colliding;
}

// Fonction pour vérifier la collision joueur/ES2
int checkPlayerES2Collision(personnage* p, Ennemi* es) {
    if (!es->showPotion2) return 0;

    int playerRight = p->position.x + p->position.w;
    int playerBottom = p->position.y + p->position.h;
    int esRight = es->positionES2.x + es->positionES2.w;
    int esBottom = es->positionES2.y + es->positionES2.h;

    int colliding = (p->position.x < esRight &&
                     playerRight > es->positionES2.x &&
                     p->position.y < esBottom &&
                     playerBottom > es->positionES2.y);

    return colliding;
}

// Fonction pour afficher la santé du joueur
void display_player_health(personnage p, SDL_Surface* screen, TTF_Font* font) {
    if (font == NULL || screen == NULL) {
        printf("Erreur : Police ou écran non défini pour afficher la santé du joueur\n");
        return;
    }

    char healthText[32];
    snprintf(healthText, sizeof(healthText), "Player Health: %d", p.vie);

    SDL_Color textColor = {255, 255, 255, 0}; // Blanc
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, healthText, textColor);
    if (textSurface == NULL) {
        printf("Unable to render player health text: %s\n", TTF_GetError());
        return;
    }

    SDL_Rect textPosition = {10, 10, 0, 0}; // Haut à gauche
    SDL_BlitSurface(textSurface, NULL, screen, &textPosition);
    SDL_FreeSurface(textSurface);
    printf("Affichage santé joueur : %s à x=%d, y=%d\n", healthText, textPosition.x, textPosition.y);
}

// Fonction pour afficher le score
void display_score(int score, SDL_Surface* screen, TTF_Font* font) {
    if (font == NULL || screen == NULL) {
        printf("Erreur : Police ou écran non défini pour afficher le score\n");
        return;
    }

    char scoreText[32];
    snprintf(scoreText, sizeof(scoreText), "Score: %d", score);

    SDL_Color textColor = {255, 255, 255, 0}; // Blanc
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, scoreText, textColor);
    if (textSurface == NULL) {
        printf("Unable to render score text: %s\n", TTF_GetError());
        return;
    }

    SDL_Rect textPosition = {10, 40, 0, 0}; // Sous la santé
    SDL_BlitSurface(textSurface, NULL, screen, &textPosition);
    SDL_FreeSurface(textSurface);
    printf("Affichage score : %s à x=%d, y=%d\n", scoreText, textPosition.x, textPosition.y);
}

int main(int argc, char *argv[]) {
    SDL_Surface* screen;
    personnage player;
    Ennemi enemy, enemy2, secondaryEntity;
    int running = 1;
    int level = 1; // 1 pour ennemi1, 2 pour ennemi2
    int score = 0; // Score initial
    SDL_Event event;
    Uint32 t_prev;
    Uint32 lastESDamageTime = 0; // Timer pour la perte de santé avec ES1
    Uint32 lastEnemyDamageTime = 0; // Timer pour la perte de santé avec ennemi
    int es2Collected = 0; // Suivre si ES2 est collecté
    const int ES2_BASE_Y = 50; // Position y initiale de ES2
    const float ES2_AMPLITUDE = 50.0f; // Amplitude de l'oscillation
    const float ES2_FREQUENCY = 0.005f; // Fréquence de l'oscillation

    // Initialisation SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Unable to init SDL: %s\n", SDL_GetError());
        return 1;
    }
    if (TTF_Init() < 0) {
        printf("Unable to init SDL_ttf: %s\n", TTF_GetError());
        SDL_Quit();
        return 1;
    }

    screen = SDL_SetVideoMode(BACKGND_W, BACKGND_H, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
    if (screen == NULL) {
        printf("Unable to set video mode: %s\n", SDL_GetError());
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Initialisation de la police
    enemy.font = TTF_OpenFont("arial.ttf", 24);
    if (enemy.font == NULL) {
        printf("Unable to load font: %s\n", TTF_GetError());
        TTF_Quit();
        SDL_Quit();
        return 1;
    }
    enemy2.font = enemy.font;
    printf("Police chargée avec succès : arial.ttf, taille 24\n");

    // Initialisation du joueur
    initialiser_personnage(&player);
    player.position.w = 100;
    player.position.h = 150;
    player.position.x = 50;
    player.position.y = 300;

    // Initialisation des ennemis
    srand(time(NULL));
    if (init_ennemi(&enemy) == -1) {
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    enemy2.image = IMG_Load("roman.png");
    if (enemy2.image == NULL) {
        printf("Unable to load roman.png: %s\n", SDL_GetError());
        freeEnnemi(&enemy);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }
    initEnnemiAttributes(&enemy2);

    // Initialisation de l'ES
    if (initES(&secondaryEntity) == -1) {
        freeEnnemi(&enemy);
        freeEnnemi(&enemy2);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    t_prev = SDL_GetTicks();

    while (running) {
        Uint32 t_now = SDL_GetTicks();
        Uint32 dt = t_now - t_prev;
        t_prev = t_now;

        // Gestion des événements
        while (SDL_PollEvent(&event)) {
            handle_input(event, &player, NULL, &running, 1);
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_SPACE) {
                    if (level == 1) {
			if (enemy.State == 0) {
			    enemy.State = 2;
			} else {
			    enemy.State = 0;
			}
                    } else {
                        if (enemy2.State == 0) {
			    enemy2.State = 2;
			} else {
			    enemy2.State = 0;
			}
                    }
                }
            }
        }

        // Mettre à jour le joueur
        movePerso(&player, dt);

        // Vérifier le timer pour afficher potion11.png, seulement si ES2 n'a pas été collecté
        if (!es2Collected && !secondaryEntity.showPotion2 && (t_now - secondaryEntity.potionTimer >= 10000)) {
            secondaryEntity.showPotion2 = 1;
            printf("10 seconds elapsed, showing potion11.png\n");
        }

        // Mettre à jour la position de ES2 pour un mouvement sinusoïdal
   if (secondaryEntity.showPotion2 && !es2Collected) {
        const int ES2_BASE_X = 480; // Position x initiale de ES2
        secondaryEntity.positionES2.x = ES2_BASE_X + ES2_AMPLITUDE * sin(ES2_FREQUENCY * t_now);
        printf("ES2 position: x=%d, y=%d\n", secondaryEntity.positionES2.x, secondaryEntity.positionES2.y);
    }

        // Vérifier la collision joueur/ES1
        if (checkPlayerESCollision(&player, &secondaryEntity)) {
            if (t_now - lastESDamageTime >= 1000) { // 1 seconde entre chaque perte
                player.vie -= 10;
                if (player.vie < 0) player.vie = 0;
                lastESDamageTime = t_now;
                printf("Collision joueur/ES1 ! Player Health=%d\n", player.vie);
            }
        }

        // Vérifier la collision joueur/ES2
        int hasCollidedWithES2 = 0;
        if (checkPlayerES2Collision(&player, &secondaryEntity) && !hasCollidedWithES2) {
            player.vie += 10;
            if (player.vie > 100) player.vie = 100; // Limite max
            secondaryEntity.showPotion2 = 0; // Faire disparaître ES2
            es2Collected = 1; // Marquer ES2 comme collecté
            hasCollidedWithES2 = 1;
            printf("Collision joueur/ES2 ! Player Health=%d, ES2 disparu définitivement\n", player.vie);
        } else if (!checkPlayerES2Collision(&player, &secondaryEntity)) {
            hasCollidedWithES2 = 0;
        }

        // Vérifier la transition au niveau 2
        if (enemy.health==0 && level == 1 && player.position.x >= 640) {
            level = 2;
            enemy2.positionAbsolue.x = BACKGND_W / 2;
            enemy2.positionAbsolue.y = 1.1 * BACKGND_H - 170;
            enemy2.Frame.i = 0;
            enemy2.Frame.j = 0;
            enemy2.Direction = 2;
            enemy2.health = 100;
            enemy2.isAlive = 1;
            enemy2.isAttacking = 0;
            secondaryEntity.potionTimer = SDL_GetTicks();
            player.position.x = 50;
            printf("Transition vers niveau 2\n");
        }

        // Mettre à jour l'ennemi selon le niveau
        if (level == 1) {
            if (enemy.isAlive) {
                move_ennemi(&enemy, level);
                animateEnnemi(&enemy);

                // Vérifier la collision joueur/ennemi
                if (checkPlayerEnemyCollision(&player, &enemy)) {
                    enemy.isAttacking = 1;
			if (player.position.x > enemy.positionAbsolue.x) {
			    enemy.Frame.i = 2;
			} else {
			    enemy.Frame.i = 3;
			}
                    enemy.health -= 1;
                    if (t_now - lastEnemyDamageTime >= 1000) { // 1 seconde entre chaque perte
                        player.vie -= 2;
                        if (player.vie < 0) player.vie = 0;
                        lastEnemyDamageTime = t_now;
                        printf("Collision joueur/ennemi1 ! Player Health=%d\n", player.vie);
                    }
                    if (enemy.health <= 0) {
                        enemy.isAlive = 0;
                        score = 100; // Score à 100 quand ennemi1 meurt
                        printf("Ennemi1 mort, Score=%d\n", score);
                    }
                    printf("Collision joueur/ennemi1! Enemy Health=%d, isAttacking=%d, Frame.i=%d\n",
                           enemy.health, enemy.isAttacking, enemy.Frame.i);
                } else if (enemy.isAttacking) {
                    enemy.isAttacking = 0;
			if (enemy.Direction == 2) {
			    enemy.Frame.i = 0;
			} else {
			    enemy.Frame.i = 1;
			}
                    printf("Fin collision, retour à Frame.i=%d\n", enemy.Frame.i);
                }
            }
        } else {
            if (enemy2.isAlive) {
                move_ennemi(&enemy2, level);
                animateEnnemi(&enemy2);

                // Vérifier la collision joueur/ennemi2
                if (checkPlayerEnemyCollision(&player, &enemy2)) {
                    enemy2.isAttacking = 1;
			if (player.position.x > enemy2.positionAbsolue.x) {
			    enemy2.Frame.i = 2;
			} else {
			    enemy2.Frame.i = 3;
			}
                    enemy2.health -= 1;
                    if (t_now - lastEnemyDamageTime >= 1000) { // 1 seconde entre chaque perte
                        player.vie -= 1;
                        if (player.vie < 0) player.vie = 0;
                        lastEnemyDamageTime = t_now;
                        printf("Collision joueur/ennemi2 ! Player Health=%d\n", player.vie);
                    }
                    if (enemy2.health <= 0) {
                        enemy2.isAlive = 0;
                        score = 100; // Score à 100 quand ennemi2 meurt
                        printf("Ennemi2 mort, Score=%d\n", score);
                    }
                    printf("Collision joueur/ennemi2! Enemy Health=%d, isAttacking=%d, Frame.i=%d\n",
                           enemy2.health, enemy2.isAttacking, enemy2.Frame.i);
                } else if (enemy2.isAttacking) {
                    enemy2.isAttacking = 0;
                    enemy2.Frame.i = (enemy2.Direction == 2) ? 0 : 1;
                    printf("Fin collision, retour à Frame.i=%d\n", enemy2.Frame.i);
                }
            }
        }

        // Effacer l'écran
        SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));

        // Afficher selon le niveau
        if (level == 1) {
            display_ennemi(enemy, screen);
            display_ES(secondaryEntity, screen);
            display_health(enemy, screen);
            display_player_health(player, screen, enemy.font);
            display_score(score, screen, enemy.font);
        } else {
            display_ennemi2(enemy2, screen);
            display_ES(secondaryEntity, screen);
            display_health(enemy2, screen);
            display_player_health(player, screen, enemy.font);
            display_score(score, screen, enemy.font);
        }
        afficher_personnage(player, screen);

        SDL_Flip(screen);
    }

    // Nettoyage
    for (int i = 0; i < 8; i++) {
        if (player.tab_right[i]) SDL_FreeSurface(player.tab_right[i]);
        if (player.tab_left[i]) SDL_FreeSurface(player.tab_left[i]);
    }
    for (int i = 0; i < 5; i++) {
        if (player.tab_attack_right[i]) SDL_FreeSurface(player.tab_attack_right[i]);
        if (player.tab_attack_left[i]) SDL_FreeSurface(player.tab_attack_left[i]);
    }
    for (int i = 0; i < 1; i++) {
        if (player.tab_jump_right[i]) SDL_FreeSurface(player.tab_jump_right[i]);
        if (player.tab_jump_left[i]) SDL_FreeSurface(player.tab_jump_left[i]);
    }
    freeEnnemi(&enemy);
    freeEnnemi(&enemy2);
    freeEnnemi(&secondaryEntity);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
