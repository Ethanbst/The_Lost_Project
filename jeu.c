#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

typedef struct Map{
    SDL_Texture *texture;
}Map;

enum{HAUT, BAS, GAUCHE, DROITE};

void mouvement( const Uint8 *state, SDL_Rect *playerRect, SDL_Texture *img_dir_joueur[4], SDL_Texture **playerTexture){
    if(state[SDL_SCANCODE_W]){
        add_log("MOUVEMENT","Z pressee\n");
        playerRect->y -= 10;
        *playerTexture = img_dir_joueur[HAUT];
    }

    if(state[SDL_SCANCODE_A]){
        add_log("MOUVEMENT","Q pressee\n");
        playerRect->x -= 10;
        *playerTexture = img_dir_joueur[GAUCHE];
    }

    if(state[SDL_SCANCODE_S]){
        add_log("MOUVEMENT","S pressee\n");
        playerRect->y += 10;
        *playerTexture = img_dir_joueur[BAS];
    }

    if(state[SDL_SCANCODE_D]){
        add_log("MOUVEMENT","D pressee\n");
        playerRect->x += 10;
        *playerTexture = img_dir_joueur[DROITE];
    }
    return;
}


//Génère une matrice de x lignes et y colonnes
int** generate_map(int x, int y) {
    int **matrix = (int**)malloc(x * sizeof(int*));
    if (matrix == NULL) {
        add_log("MATRIX", "Erreur d'allocation de mémoire pour les lignes\n");
        return NULL;
    }
    for (int i = 0; i < x; i++) {
        matrix[i] = (int *)malloc(y * sizeof(int));
        if (matrix[i] == NULL) {
            add_log("MATRIX", "Erreur d'allocation de mémoire pour les colonnes\n");
            for (int j = 0; j < i; j++) {
                free(matrix[j]);
            }
            free(matrix);
            return NULL;
        }
        for (int j = 0; j < y; j++) {
            matrix[i][j] = rand() % 2;
        }
    }
    return matrix;
}

//Génère la matrice correspondante au fichier et la retourne
int** read_map_from_file(const char* filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        add_log("MAP", "Erreur d'ouverture du fichier\n");
        return NULL;
    }

    int **matrix = (int**)malloc(10 * sizeof(int*));
    if (matrix == NULL) {
        add_log("MAP", "Erreur d'allocation de mémoire pour les lignes\n");
        fclose(file);
        return NULL;
    }

    for (int i = 0; i < 10; i++) {
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
Map* render_map(SDL_Window *window, int **matrice){

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

    for (int i = 0; i < 10; i++) {
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
    Map *map = (Map*)malloc(sizeof(Map));
    map->texture = map_texture;

    return map;
}


//Fonction de lancement du jeu
void jeu(SDL_Window *window, SDL_Renderer *renderer){

    add_log("JEU","Fonction jeu.\n");

    int **matrice = read_map_from_file("res/map1.txt");

    Map *map = render_map(window, matrice);

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


    SDL_Rect playerRect = {width/20, width/20, width/20, width/20}; //Rectange qui reçoit l'image du joueur



    //Implémentation des éléments du joueur:
    //enum{VIDE, MUR, JOUEUR};
    
    //SDL_Rect position, posJoueur; //Position de l'image & Position joueur

    SDL_Texture *img_dir_joueur[4]={NULL}; //Liste de valeur possible pour la rotation du joueur

    //Chargement des images du joueur en textures
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
        }
        mouvement(state, &playerRect, img_dir_joueur, &playerTexture); //Déplace le joueur
        SDL_RenderClear(renderer); //Efface l'écran
        SDL_RenderCopy(renderer, map->texture, NULL, NULL); //Rend la texture des murs
        SDL_RenderCopy(renderer, playerTexture, NULL, &playerRect); //Rend le joueur car sa position à changé
        SDL_RenderPresent(renderer); //Présente le rendu
        
        SDL_Delay(16); //Délai pour éviter de bouger trop vite
    }
    add_log("JEU","Sortie while jeu()\n");
    //sdl_utils_Quit(window, renderer);
}