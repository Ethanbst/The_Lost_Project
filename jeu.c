#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include "logs_utils/log.h"
#include "pause_menu/pause_menu.c"



//Contient la texture d'une map ainsi que ça matrice d'objets (mur, vide, etc)
typedef struct Map{
    SDL_Texture *texture; //Texture des murs
    int **matrice; //Matrice de la map
    int pMposX; //Position du joueur en x dans la matrice
    int pMposY; //Position du joueur en y dans la matrice
    int pPXposX; //Position du joueur en x sur la fenêtre
    int pPXposY; //Position du joueur en y sur la fenêtre
}Map;

enum Direction {HAUT, BAS, GAUCHE, DROITE};


// Fonction pour vérifier les collisions en fonction du centre du joueur
int no_obstacle(enum Direction direction, Map *map, SDL_Rect *playerRect, int cellSize) {
    int centerX = playerRect->x + playerRect->w / 2;
    int centerY = playerRect->y + playerRect->h;

    if (direction == HAUT) {
        if (map->matrice[(centerY - 50) / cellSize][centerX / cellSize] == 1) {
            add_log("JEU", "Obstacle!\n");
            return 0;
        }
    }

    if (direction == BAS) {
        if (map->matrice[(centerY + 10) / cellSize][centerX / cellSize] == 1) {
            add_log("JEU", "Obstacle!\n");
            return 0;
        }
    }

    if (direction == GAUCHE) {
        if (map->matrice[centerY / cellSize][(centerX - 50) / cellSize] == 1) {
            add_log("JEU", "Obstacle!\n");
            return 0;
        }
    }

    if (direction == DROITE) {
        if (map->matrice[centerY / cellSize][(centerX + 50) / cellSize] == 1) {
            add_log("JEU", "Obstacle!\n");
            return 0;
        }
    }

    return 1;
}

// Taille d'une cellule de la matrice en pixels
int cellSize;
//On récupère la taille de la fenêtre pour bien placer les boutons par la suite
int width, height;

// Fonction pour initialiser la taille de la fenêtre
void initWindowSize() {
    SDL_DisplayMode taille_fenetre;
    SDL_GetCurrentDisplayMode(0, &taille_fenetre);
    width = taille_fenetre.w;
    height = taille_fenetre.h;
    cellSize = width / 20;
}

// Fonction pour mettre à jour la position du joueur dans la matrice en fonction de sa position dans la fenêtre
void updatePlayerPositionInMatrix(Map *map, SDL_Rect *playerRect, int cellSize) {
    map->pMposX = playerRect->x / cellSize;
    map->pMposY = playerRect->y / cellSize;
}

void mouvement( const Uint8 *state, SDL_Rect *playerRect, SDL_Texture *img_dir_joueur[4], SDL_Texture **playerTexture, Map *map){
    int oldMposX = map->pMposX;
    int oldMposY = map->pMposY;

    printf("Joueur in matrix: %d, %d\n", map->pMposX, map->pMposY);
    
    if(state[SDL_SCANCODE_W] && no_obstacle(HAUT, map, playerRect, cellSize)){
        add_log("MOUVEMENT","Z pressee\n");
        playerRect->y -= 10;
        *playerTexture = img_dir_joueur[HAUT];
    }

    if(state[SDL_SCANCODE_A] && no_obstacle(GAUCHE, map, playerRect, cellSize)){
        add_log("MOUVEMENT","Q pressee\n");
        playerRect->x -= 10;
        *playerTexture = img_dir_joueur[GAUCHE];
    }

    if(state[SDL_SCANCODE_S] && no_obstacle(BAS, map, playerRect, cellSize)){
        add_log("MOUVEMENT","S pressee\n");
        playerRect->y += 10;
        *playerTexture = img_dir_joueur[BAS];
    }

    if(state[SDL_SCANCODE_D] && no_obstacle(DROITE, map, playerRect, cellSize)){
        add_log("MOUVEMENT","D pressee\n");
        playerRect->x += 10;
        *playerTexture = img_dir_joueur[DROITE];
        
    }
    
    // Mettre à jour la position du joueur dans la matrice si elle a changé
    updatePlayerPositionInMatrix(map, playerRect, cellSize);

    // Vérifier si la position dans la matrice a changé
    if (map->pMposX != oldMposX || map->pMposY != oldMposY) {
        add_log("MOUVEMENT", "Position dans la matrice mise à jour\n");
    }
}

//Génère la matrice correspondante au fichier et la retourne
int** read_map_from_file2(const char* filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        add_log("MAP", "Erreur d'ouverture du fichier\n");
        return NULL;
    }

    int **matrix = (int**)malloc(13 * sizeof(int*));
    if (matrix == NULL) {
        add_log("MAP", "Erreur d'allocation de mémoire pour les lignes\n");
        fclose(file);
        return NULL;
    }

    for (int i = 0; i < 13; i++) {
        matrix[i] = (int *)malloc(20 * sizeof(int));
        if (matrix[i] == NULL) {
            add_log("MAP", "Erreur d'allocation de mémoire pour les colonnes\n");
            for (int j = 0; j < i; j++) {
                free(matrix[j]);
            }
            free(matrix);
            fclose(file);
            return NULL;
        }

        for (int j = 0; j < 20; j++) {
            if (fscanf(file, "%d", &matrix[i][j]) != 1) {
                add_log("MAP", "Erreur de lecture du fichier\n");
                for (int k = 0; k <= i; k++) {
                    free(matrix[k]);
                }
                free(matrix);
                fclose(file);
                return NULL;
            }
        }
    }

    fclose(file);
    return matrix;
}

// Fonction pour charger une texture à partir d'un fichier BMP
SDL_Texture* loadTexture(const char* path, SDL_Renderer* renderer) {
    SDL_Texture* texture = IMG_LoadTexture(renderer, path);
    if(texture == NULL) {
        add_log("TEXTURE", "Erreur de chargement de la texture\n");
    }
    return texture;
}

//Créér une texture pour les murs et la retourne
SDL_Texture* render_map2(SDL_Window *window, int **matrice){

    SDL_Renderer *renderer = SDL_GetRenderer(window);

    //Texture du mur
    SDL_Texture *mur_texture = loadTexture("res/mur.bmp", renderer);

    //On récupère la taille de la fenêtre pour bien placer les boutons par la suite
    int width, height;
    SDL_DisplayMode taille_fenetre;
    SDL_GetCurrentDisplayMode(0, &taille_fenetre);
    width = taille_fenetre.w;
    height = taille_fenetre.h;

    SDL_Texture *map_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);
    SDL_SetRenderTarget(renderer, map_texture);

    SDL_Rect MurRect;
    MurRect.w = width / 20;
    MurRect.h = MurRect.w;

    for (int i = 0; i < 13; i++) {
        for (int j = 0; j < 20; j++) {
            if (matrice[i][j] == 1) {
                MurRect.x = j * MurRect.w;
                MurRect.y = i * MurRect.h;
                SDL_RenderCopy(renderer, mur_texture, NULL, &MurRect);
            }
        }
    }

    SDL_SetRenderTarget(renderer, NULL);

    //Structure contenant la texture et le rectangle du mur
    //Map *map = (Map*)malloc(sizeof(Map));
    //map->texture = map_texture;

    return map_texture;
}

//Fonction de lancement du jeu
void jeu(SDL_Window *window, SDL_Renderer *renderer){

    initWindowSize();

    add_log("JEU","Fonction jeu.\n");
    Map *map = (Map*)malloc(sizeof(Map));
    map->matrice = read_map_from_file2("res/map2.txt");
    map->texture = render_map2(window, map->matrice);

    if(IMG_Init(IMG_INIT_PNG) == 0){
        add_log("JEU","Erreur d'initialisation de SDL_image\n");
    }
    SDL_Texture *playerTexture = loadTexture("res/joueur/joueurB.png", renderer);

    //On récupère la taille de la fenêtre pour bien placer les boutons par la suite
    int width, height;
    SDL_DisplayMode taille_fenetre;
    SDL_GetCurrentDisplayMode(0, &taille_fenetre);
    width = taille_fenetre.w;
    height = taille_fenetre.h;

    //Positionnement du joueur au départ dans la matrice
    map->pMposX = 1;
    map->pMposY = 1;
    //Positionnement du joueur au départ sur la fenêtre
    map->pPXposX = width/20*map->pMposX;
    map->pPXposY = width/20*map->pMposY;

    //Rectange qui reçoit l'image du joueur
    SDL_Rect playerRect = {width/20*map->pMposX, width/20*map->pMposY, width/20, width/20};

    //Liste de valeur possible pour la rotation du joueur
    SDL_Texture *img_dir_joueur[4]={NULL};

    //Tableau contenant les textures de chaques direction du joueur
    img_dir_joueur[BAS]=loadTexture("res/joueur/joueurB.png", renderer);
    img_dir_joueur[HAUT]=loadTexture("res/joueur/joueurH.png", renderer);
    img_dir_joueur[GAUCHE]=loadTexture("res/joueur/joueurG.png", renderer);
    img_dir_joueur[DROITE]=loadTexture("res/joueur/joueurD.png", renderer);

    if(!img_dir_joueur[BAS] || !img_dir_joueur[HAUT] || !img_dir_joueur[GAUCHE] || !img_dir_joueur[DROITE]){
        add_log("JEU","Erreur de chargement de l'image du joueur\n");
    }

    playerTexture = img_dir_joueur[HAUT]; //Orientation du joueur au démarrage


    int continuer = 1;
    add_log("JEU","Entré dans while jeu()\n");

    //Premier rendu des textures
    SDL_RenderCopy(renderer, map->texture, NULL, NULL);
    SDL_RenderCopy(renderer, playerTexture, NULL, &playerRect);
    SDL_RenderPresent(renderer);

    SDL_Event event; //Variable qui reçoit l'évènement
    const Uint8 *state = SDL_GetKeyboardState(NULL); //Récupère l'état du clavier
    Uint32 lastTime = SDL_GetTicks(), currentTime; //Variables pour le temps

    
    while(continuer){
        /*currentTime = SDL_GetTicks(); //Récupère le temps actuel
        Uint32 deltaTime = currentTime - lastTime; //Calcul du temps écoulé
        lastTime = currentTime; //Mise à jour du temps*/
        
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT){
                continuer = 0;
            }
            else if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE){
                continuer = pause(renderer);
            }
        }
        mouvement(state, &playerRect, img_dir_joueur, &playerTexture, map); //Déplace le joueur
        SDL_RenderClear(renderer); //Efface l'écran
        SDL_RenderCopy(renderer, map->texture, NULL, NULL); //Rend la texture des murs
        SDL_RenderCopy(renderer, playerTexture, NULL, &playerRect); //Rend le joueur car sa position à changé
        SDL_RenderPresent(renderer); //Présente le rendu
        
        SDL_Delay(16); //Délai pour éviter de bouger trop vite
    }
    
    //Libération de ressources de la fonction jeu
    for(int i=0;i<4;i++){
        SDL_DestroyTexture(img_dir_joueur[i]);
    }

    SDL_DestroyTexture(map->texture);

    for(int i=0;i<13;i++){
        free(map->matrice[i]);
    }

    free(map->matrice);
    free(map);
    add_log("JEU","Sortie while jeu()\n");
}