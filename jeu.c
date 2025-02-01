#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>


//Change la position de notre image joueur en fonction de la touche pressée
void mouvement(SDL_Event event, SDL_Rect *pos_joueur, SDL_Surface **orientation_joueur, SDL_Surface *img_joueur[4]){
    add_log("MOUVEMENT","mouvement()\n");
    if(event.key.keysym.sym == SDLK_z){
        add_log("MOUVEMENT","Z pressee\n");
        pos_joueur->y -= 100;
        *orientation_joueur = img_joueur[0];
    }

    if(event.key.keysym.sym == SDLK_q){
        add_log("MOUVEMENT","Q pressee\n");
        pos_joueur->x -= 100;
        *orientation_joueur = img_joueur[2];
    }
    if(event.key.keysym.sym == SDLK_s){
        add_log("MOUVEMENT","S pressee\n");
        pos_joueur->y += 100;
        *orientation_joueur = img_joueur[1];
    }
    if(event.key.keysym.sym == SDLK_d){
        add_log("MOUVEMENT","D pressee\n");
        pos_joueur->x += 100;
        *orientation_joueur = img_joueur[3];
    }
    if(orientation_joueur){
        add_log("MOUVEMENT","Orientation changee.");
    }
    else{
        add_log("MOUVEMENT","Erreur d'orientation.");
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


//Affiche les images du mur sur la fenêtre en fonction de la matrice entrée en paramètre
void create_map(SDL_Window *window, int **map){

    SDL_Surface *mur = IMG_Load("res/mur.bmp");
    if (!mur) {
        add_log("MAP", "Erreur de chargement de l'image du mur\n");
        return;
    }

    //On récupère la taille de la fenêtre pour bien placer les boutons par la suite
    int width, height;
    SDL_DisplayMode taille_fenetre;
    SDL_GetCurrentDisplayMode(0, &taille_fenetre);
    width = taille_fenetre.w;
    height = taille_fenetre.h;

    
    SDL_Rect destRect;
    destRect.w = width/20;
    destRect.h = destRect.w;

    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 20; j++) {
            if (map[i][j] == 1) {
                destRect.x = j * destRect.w;
                destRect.y = i * destRect.h;
                SDL_BlitScaled(mur, NULL, SDL_GetWindowSurface(window), &destRect);
            }
        }
    }

    SDL_UpdateWindowSurface(window);
    SDL_FreeSurface(mur);

}

//Fonction de lancement du jeu
void jeu(SDL_Window *window, SDL_Renderer *renderer){

    add_log("JEU","Fonction jeu.\n");
    
    SDL_Surface *ecran = SDL_GetWindowSurface(window);

    //Implémentation des éléments du joueur:
    enum{VIDE, MUR, JOUEUR};
    enum{HAUT, BAS, GAUCHE, DROITE};
    SDL_Rect position, posJoueur; //Position de l'image & Position joueur

    SDL_Surface *img_joueur[4]={NULL}; //Liste de valeur possible pour la rotation du joueur
    SDL_Surface *orientation_joueur = NULL; //Image du personnage en fonction du dernier déplacement (H, B, G, D)

    //Chargement des images du joueur:
    img_joueur[BAS]=IMG_Load("res/joueur/joueurB.bmp");
    img_joueur[HAUT]=IMG_Load("res/joueur/joueurH.bmp");
    img_joueur[GAUCHE]=IMG_Load("res/joueur/joueurG.bmp");
    img_joueur[DROITE]=IMG_Load("res/joueur/joueurD.bmp");

    if(!img_joueur[BAS] || !img_joueur[HAUT] || !img_joueur[GAUCHE] || !img_joueur[DROITE]){
        add_log("JEU","Erreur de chargement de l'image du joueur\n");
    }

    orientation_joueur = img_joueur[BAS];
    posJoueur.x = 0;
    posJoueur.y = 0;

    position.x = posJoueur.x;
    position.y = posJoueur.y;

    //Rectangle contenan; l'image du joueur
    int h_joueur = 100;
    int l_joueur = 100;
    SDL_Rect destRect = { posJoueur.x, posJoueur.y, h_joueur, l_joueur};
        
    SDL_Event event; //Variable qui reçoit l'évènement

    int continuer = 1;
    add_log("JEU","Entré dans while jeu()\n");
    while(continuer){
        SDL_BlitScaled(orientation_joueur, NULL, ecran, &destRect);
        SDL_UpdateWindowSurface(window);
        SDL_WaitEvent(&event); //Modifie la variable event avec l'évènement reçu
        switch (event.type)
        {
            case SDL_QUIT:
                continuer = 0;
                break;
            case SDL_KEYDOWN:
            //printf("Touche pressee\n");
            mouvement(event, &posJoueur, &orientation_joueur, img_joueur);
            
            //On met à jour la position du joueur:
            SDL_FillRect(ecran, NULL, SDL_MapRGB(ecran->format, 0, 0, 0)); //Met la couleur noir à l'écran
            destRect.x = posJoueur.x;
            destRect.y = posJoueur.y;
            SDL_BlitScaled(orientation_joueur, NULL, ecran, &destRect);
            SDL_UpdateWindowSurface(window);
            break;
            default:
                break;
        }
    }
    add_log("JEU","Sortie while jeu()\n");
    sdl_utils_Quit(window, renderer);
}