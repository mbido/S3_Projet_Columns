#include <assert.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define SIZE_T_MAX ((size_t)(~0UL))
#define BUFSIZE 256




///////////////// creation structure representant le tableau de jeu ////////////////////

// le tableau de jeu est un tableau dynamique a deux dimensions linearise
// cela permet de facilement augmenter la hauteur du jeu qui evolue au cours de la partie
// les valeurs du tableau de jeu vont de 0 a 6 un type char est donc suffisant 
struct grille
{
  char *tab;
  size_t width;
  size_t height;
};

// initialisation du tableau de jeu en fonction d'une largeur donnee
// par defaut, on lui donne une hauteur de 15 correspondant a un
// empilement de 5 blocs de briques
void create_grille(struct grille *self, size_t largeur)
{
  self->width = largeur;
  self->height = 15;
  self->tab = calloc(largeur * 15, sizeof(char));
}

// permet de liberer l'espace memoire contenue dans le tableau de jeu
// pour liberer toute memoire utilise, on n'oublie pas de liberer le tableau lui-meme 
// si ensuite, on veut le reutiliser, on devra le reinitialiser
void destroy_grille(struct grille *self)
{
  free(self->tab);
  self->tab = NULL;
}

// permet de doubler la hauteur du tableau de jeu
// on alloue donc un tableau deux fois plus grand puis on libere les donnees precedentes
// sa hauteur etant de 15 lors de l'initialisation on augmente dans tout les cas la hauteur
// du tableau
void double_hauteur(struct grille *g)
{
  size_t h = g->height;
  size_t w = g->width;
  char *new = calloc(h * w * 2, sizeof(char));
  for (size_t i = 0; i < h * w; ++i)
  {
    new[i] = g->tab[i];
  }
  free(g->tab);
  g->tab = new;
  g->height = h * 2;
}

// permet de copier un tableau de jeu dans un autre
void copie(struct grille *self, struct grille *other)
{
  self->width = other->width;
  self->height = other->height;
  self->tab = malloc(self->width * self->height * sizeof(char));
  for (size_t i = 0; i < self->width * self->height; ++i)
  {
    self->tab[i] = other->tab[i];
  }
}

///////////////// creation structure tableau dynamique ////////////////////

struct tableau
{
  size_t *data;
  size_t length;
  size_t capacity;
};

void create_tableau(struct tableau *self)
{
  self->length = 0;
  self->capacity = 10;
  self->data = malloc(10 * sizeof(size_t));
}


void destroy_tableau(struct tableau *self)
{
  free(self->data);
  self->data = NULL;
  self->length = self->capacity = 0;
}

void double_taille(struct tableau *self)
{
  size_t l = self->length;
  size_t c = self->capacity;
  size_t *new = malloc(c * 2 * sizeof(size_t));
  for (size_t i = 0; i < l; ++i)
  {
    new[i] = self->data[i];
  }
  free(self->data);
  self->data = new;
  self->capacity = c * 2;
}

void append(struct tableau * self, size_t elt)
{
  if (self->length >= self->capacity)
  {
    double_taille(self);
  }
  self->data[self->length] = elt;
  ++self->length;
}


////////////////// fonctions utiles ///////////////////////////

// permet de ne pas se tromper lors du calcul de la position d'une case
// connaissant le tableau de jeu, la colonne et la ligne dans lesquel elle se trouve
size_t linearise_co(struct grille *g, size_t i, size_t j)
{
  return j * g->width + i;
}

// permet, a partir des coordonnes linearisee d'une case, de connaitre la colonne dans laquelle elle se trouve supposement
size_t get_colonne(size_t index, struct grille *g)
{
  return index % g->width;
}

// permet, a partir des coordonnes linearisee d'une case, de connaitre la ligne dans laquelle elle se trouve supposement
size_t get_ligne(size_t index, struct grille *g) 
{ 
  return index / g->width; 
}

// permet, a partir des coordones linearisee d'une case, de connaitre sa couleur
// si l'index est invalide alors on renvoie la couleur nulle : 0
char get_color(size_t index, struct grille *g)
{
  if (get_ligne(index, g) >= g->height)
    return 0;
  return g->tab[index];
}

// permet comme le nom l'indique de permuter un bloc de trois briques
// la permutation se fait vers la gauche et le premier element se retrouve en derniere position
void permutation(char *briques)
{
  char temp = briques[0];
  briques[0] = briques[1];
  briques[1] = briques[2];
  briques[2] = temp;
}

char *rand_briques()
{
  char *res = malloc(3 * sizeof(char));
  res[0] = (rand() % 6) + 1;
  res[1] = (rand() % 6) + 1;
  res[2] = (rand() % 6) + 1;
  return res;
}



// affiche un bloc de brique (pour les tests)
void print_briques(char *briques)
{
  printf("+-+\n");
  printf("|%d|\n", (int)briques[0]);
  printf("+-+\n");
  printf("|%d|\n", (int)briques[1]);
  printf("+-+\n");
  printf("|%d|\n", (int)briques[2]);
  printf("+-+\n");
}

// affiche le tableau de jeu (pour les tests)
void print_tableau(struct grille *g)
{ 
  char *colors[7] = {"\x1B[0m", "\x1B[31m","\x1B[32m", "\x1B[33m", "\x1B[34m", "\x1B[35m", "\x1B[36m"};
  size_t h = g->height;
  size_t w = g->width;
  size_t index = h - 1;
  for (size_t j = h - 1; j < h; --j)
  {

    
    printf("|\n");
    for (size_t i2 = 0; i2 < w; ++i2)
    {
      printf("----");
    }
    printf("\n");

    for (size_t i = 0; i < w; ++i)
    { 
      index = linearise_co(g, i, j);
      int color = (int)g->tab[index];
      printf("|%s|%d|%s", colors[color], color, colors[0]);
      
    }
  }
  printf("|\n");
}

// permet d'afficher un array de size_t (pour simplifier les tests)
void print_array(size_t *array, size_t length, size_t jusqua)
{
  printf("[");
  for (size_t i = 0; i < length; ++i)
  { 
    if (array[i] == jusqua)
    {
      break;
    }
    printf("%zd, ", array[i]);
  }
  printf("]\n");
}

/////////////// fonctions principales ////////////////////


// permet de deposer un bloc de trois briques sur une colonne du tableau de jeu
// elle prend en charge l'allocation d'une hauteur plus importante si besoin grace
// a la fonction double_hauteur (cf. section : creation structure representant le tableau de jeu)
// la premiere brique du bloc se trouvera en bas, celle du millieu au millieu et la derniere en haut
void insert(struct grille *g, size_t colonne, char *briques)
{
  // on recupere les valeurs presentes :
  size_t h = g->height;
  size_t w = g->width;

  // on place les briques tout en bas ou juste au dessus de la brique la plus
  // haute dans la colonne :
  size_t j = g->height - 1;
  size_t index = linearise_co(g, colonne, j);

  while (index < h * w && g->tab[index] == 0)
  {
    // on descend donc tant que c'est possible
    --j;
    index = linearise_co(g, colonne, j);
  }
  // info : on se trouve un cran en dessous de la position des briques 

  // cas hauteur max atteinte :
  if (j + 1 > h - 3)
  {
    double_hauteur(g);
  }
  // on place maintenant les briques
  for (int i = 0; i < 3; ++i)
  {
    ++j;
    index = linearise_co(g, colonne, j);
    g->tab[index] = briques[i];
  }
}


// ceci est une fonction auxiliaire a la fonction 'gravite'
// on est dans le cas ou une brique est presente en (j+1, i) et un vide est
// present en (j, i). On descend alors la brique du dessus tout en bas du trou
void descend_brique(struct grille *g, size_t i, size_t j) 
{
  size_t h = g->height;
  size_t w = g->width;
  size_t index = linearise_co(g, i, j);

  while (index < h * w && g->tab[index] == 0)
  {
    g->tab[index] = g->tab[linearise_co(g, i, j + 1)];
    g->tab[linearise_co(g, i, j + 1)] = 0;
    --j;
    index = linearise_co(g, i, j);
  }
}

// permet d'appliquer une certaine gravite a une colonne
// elle fait en sorte qu'aucune brique ne se trouve au dessus d'une case vide
// l'appliquer a une colonne permet de gagner en efficacite sachant que l'on poura
// appliquer cette fonction a toutes colonne ayant subit une suppression sans toucher au autres
// enfin, la fonction renvoie 'true' si elle a du deplacer des briques, 'false' sinon
bool gravite(struct grille *g, size_t colonne)
{
  bool res = false;
  size_t h = g->height;
  size_t w = g->width;

  // on part de la brique la plus haute :
  size_t j = g->height - 1;
  size_t index = linearise_co(g, colonne, j);
  while (index < h * w && g->tab[index] == 0)
  {
    --j;
    index = linearise_co(g, colonne, j);
  }
  //info : on se trouve maintenant soit sur la brique la plus haute, soit en dehors des limites du tableau

  while (index < h * w)
  {
    // la deuxieme condition (apres le "&&") est la pour le cas ou on fait
    // descendre la brique la plus haute et il y a donc plus de brique a descendre
    if (g->tab[index] == 0 && linearise_co(g, colonne, j + 1) < h * w && g->tab[linearise_co(g, colonne, j + 1)] != 0)
    {
      descend_brique(g, colonne, j);
      res = true;
      ++j;
    }
    else
    {
      --j;
    }
    index = linearise_co(g, colonne, j);
  }
  return res;
} 


// permet de connaitre les alignements de couleur d'une briques
// la fonction sera appliquee a l'index de chaque brique venant d'etre pose
// le cas de l'alignement vertical ne se verifie qu'une fois donc dans une autre fonction 
// elle retourne un tableau de 3 char representant respectivement
// - 0 : taille de l'alignement horizontal
// - 1 : taille de l'alignement premiere diagonale (haut gauche -> bas droite)
// - 2 : taille de l'alignement deuxieme diagonale (haut droite -> bas gauche)
char *alignement(struct grille *g, size_t index)
// on se trouve a l'index x :
/*
a       b
  c   d
e f x g h
  i   j
k       l
*/
//
{
  size_t colonne = get_colonne(index, g);
  char color = g->tab[index];
  char *res = malloc(3 * sizeof(char));
  // de base, l'alignement est forcement d'au moins 1 car on
  // regarde une case et un alignement de 1 veux dire qu'il y a une case de meme couleur :
  res[0] = 1;
  res[1] = 1;
  res[2] = 1;

  size_t check_left = 2;
  size_t check_right = 2;

  if (colonne < 2)
    check_left = colonne;
  if (g->width - colonne < 2)
    check_right = g->width - colonne;


  // cas ou on se trouve a deux colonnes minimum du bord gauche
  if (check_left == 2)
  {
    // cas ou on se trouve a deux colonnes minimum du bord droit
    if (check_right == 2)
    {
      res[0] += get_color(index - 1, g) == color;  // f
      res[0] += get_color(index + 1, g) == color; // g
      res[0] += get_color(index - 1, g) == color &&
                get_color(index - 2, g) == color; // e
      res[0] += get_color(index + 1, g) == color &&
                get_color(index + 2, g) == color; // h

      res[1] += get_color(index + g->width - 1, g) == color;  // c
      res[1] += get_color(index - g->width + 1, g) == color; // j
      res[1] += get_color(index + g->width - 1, g) == color &&
                get_color(index + 2 * g->width - 2, g) == color; // a
      res[1] += get_color(index - g->width + 1, g) == color &&
                get_color(index - 2 * g->width + 2, g) == color; // l

      res[2] += get_color(index + g->width + 1, g) == color;  // d
      res[2] += get_color(index - g->width - 1, g) == color; // i
      res[2] += get_color(index + g->width + 1, g) == color &&
                get_color(index + 2 * g->width + 2, g) == color; // b
      res[2] += get_color(index - g->width - 1, g) == color &&
                get_color(index - 2 * g->width - 2, g) == color; // k
    }
    // cas ou on se trouve a une colonne du bord droit
    else if (check_right == 1)
    {
      res[0] += get_color(index - 1, g) == color;  // f
      res[0] += get_color(index + 1, g) == color; // g
      res[0] += get_color(index - 1, g) == color &&
                get_color(index - 2, g) == color; // e

      res[1] += get_color(index + g->width - 1, g) == color;  // c
      res[1] += get_color(index - g->width + 1, g) == color; // j
      res[1] += get_color(index + g->width - 1, g) == color &&
                get_color(index + 2 * g->width - 2, g) == color; // a

      res[2] += get_color(index + g->width + 1, g) == color;  // d
      res[2] += get_color(index - g->width - 1, g) == color; // i
      res[2] += get_color(index - g->width - 1, g) == color &&
                get_color(index - 2 * g->width - 2, g) == color; // k
    }
    // cas ou on se trouve sur le bord droit
    else
    {
      res[0] += get_color(index - 1, g) == color; // f
      res[0] += get_color(index - 1, g) == color &&
                get_color(index - 2, g) == color; // e

      res[1] += get_color(index + g->width - 1, g) == color; // c
      res[1] += get_color(index + g->width - 1, g) == color &&
                get_color(index + 2 * g->width - 2, g) == color; // a

      res[2] += get_color(index - g->width - 1, g) == color; // i
      res[2] += get_color(index - g->width - 1, g) == color &&
                get_color(index - 2 * g->width - 2, g) == color; // k
    }
  }
  // cas ou on se trouve a une colonne du bord gauche
  else if (check_left == 1)
  {
    // cas ou on se trouve a deux colonnes minimum du bord droit
    if (check_right == 2)
    {
      res[0] += get_color(index - 1, g) == color;  // f
      res[0] += get_color(index + 1, g) == color; // g
      res[0] += get_color(index + 1, g) == color &&
                get_color(index + 2, g) == color; // h

      res[1] += get_color(index + g->width - 1, g) == color;  // c
      res[1] += get_color(index - g->width + 1, g) == color; // j
      res[1] += get_color(index - g->width + 1, g) == color &&
                get_color(index - 2 * g->width + 2, g) == color; // l

      res[2] += get_color(index + g->width + 1, g) == color;  // d
      res[2] += get_color(index - g->width - 1, g) == color; // i
      res[2] += get_color(index + g->width + 1, g) == color &&
                get_color(index + 2 * g->width + 2, g) == color; // b
    }
    // cas ou on se trouve a une colonne du bord droit
    else if (check_right == 1)
    {
      res[0] += get_color(index - 1, g) == color;  // f
      res[0] += get_color(index + 1, g) == color; // g

      res[1] += get_color(index + g->width - 1, g) == color;  // c
      res[1] += get_color(index - g->width + 1, g) == color; // j

      res[2] = get_color(index + g->width + 1, g) == color;  // d
      res[2] += get_color(index - g->width - 1, g) == color; // i
    }
  }
  // cas ou on se trouve sur le bord gauche
  else if (check_left == 0)
  {
    // cas ou on se trouve a deux colonnes minimum du bord droit
    if (check_right == 2)
    {
      res[0] += get_color(index + 1, g) == color; // g
      res[0] += get_color(index + 1, g) == color &&
                get_color(index + 2, g) == color; // h

      res[1] += get_color(index - g->width + 1, g) == color; // j
      res[1] += get_color(index - g->width + 1, g) == color &&
                get_color(index - 2 * g->width + 2, g) == color; // l

      res[2] += get_color(index + g->width + 1, g) == color; // d
      res[2] += get_color(index + g->width + 1, g) == color &&
                get_color(index + 2 * g->width + 2, g) == color; // b
    }
    // cas ou on se trouve a une colonne du bord droit
    else if (check_right == 1)
    {
      res[0] += get_color(index + 1, g) == color; // g

      res[1] += get_color(index - g->width + 1, g) == color; // j

      res[2] += get_color(index + g->width + 1, g) == color; // d
    }
  }
  return res;
}

// permet de connaitre la taille de l'alignement vertical max et sa position a partir d'une colonne
// on ne regarde que les 5 premieres briques, les autres ne sont pas utiles
// la fonction renvoie la taille de l'alignement et met dans le parametre 'output'
// la position de chacune des ses briques
char alignement_vertical(struct grille *g, size_t index, size_t **output)
{
  size_t j = get_ligne(index, g);
  size_t colonne = get_colonne(index, g);

  char color = g->tab[index];
  *output[0] = index;
  char count = 1;
  for (int i = 1; i < 5; i++)
  {
    index = linearise_co(g, colonne, j - i);
    char current_color = get_color(index, g);
    if (current_color != color)
    {
      if (count >= 3 || current_color == 0)
      {
        break;
      }
      color = g->tab[index];
      count = 0;
    }
    (*output)[(int)count] = index;
    count++;
  }
  return count;
}


// permet de recuperer les index d'un alignement en fonction
// d'une brique, de la taille de l'alignement et du type d'alignement :
//    -0 : horizontal
//    -1 : premiere diagonale (haut gauche -> bas droite)
//    -2 : deuxieme diagonale (haut droite -> bas gauche)
// la liste renvoyee est de taille 5 et si il y moins d'index les derniers sont mis a la valeur 'val_interdite'
size_t *get_index_from_alignement (struct grille *g, size_t index, int type, int taille, size_t val_interdite)
{
  char color = get_color(index, g);
  size_t i = get_colonne(index, g);
  size_t j = get_ligne(index, g);

  size_t *res = malloc(5 * sizeof(size_t));
  res[0] = index;
  int index_res = 1;
  res[1] = res[2] = res[3] = res[4] = val_interdite;

  // cas horizontal :
  if (type == 0)
  {
    int k = 1;
    int cote = 1; // (1 ou -1 pour savoir de quel cote on cherche)
    for(int l = 1; l < taille; ++l)
    {
      index = linearise_co(g, i + (k * cote), j);
      if (get_color(index, g) == color)
      {
        res[index_res] = index;
        ++index_res;
        ++k;
      }
      else
      {
        k = 1;
        cote = -1;
        --l;
      }
    }
  }
  // cas premiere diagonale :
  else if (type == 1)
  {
    int k = 1;
    int cote = 1; // (1 ou -1 pour savoir de quel cote on cherche)
    for(int l = 1; l < taille; ++l)
    {
      index = linearise_co(g, i + (k * cote), j - (k * cote));
      if (get_color(index, g) == color)
      {
        res[index_res] = index;
        ++index_res;
        ++k;
      }
      else
      {
        k = 1;
        cote = -1;
        --l;
      }
    }
  }
  // cas deuxieme diagonale :
  else //(type == 2)
  {
    int k = 1;
    int cote = 1; // (1 ou -1 pour savoir de quel cote on cherche)
    for(int l = 1; l < taille; ++l)
    {
      index = linearise_co(g, i + (k * cote), j + (k * cote));
      if (get_color(index, g) == color)
      {
        res[index_res] = index;
        ++index_res;
        ++k;
      }
      else
      {
        k = 1;
        cote = -1;
        --l;
      }
    }
  }
  return res;
}

// permet de compter les points qui serons gagnes lors de la suppression
// des alignements presents en fonction de lambda, le coeficient de points
// elle prend en argument la colonne ou le bloc de 3 briques est pose
// elle sauvegarde de plus dans un tableau dynamique 'output' chaque index des briques qui
// serons supprimees
int count_points(struct grille *g, size_t colonne, int lambda, struct tableau *output)
{
  size_t h = g->height;
  size_t w = g->width;
  int res = 0;

  // on se place sur la brique la plus haute :
  size_t j = h - 1;
  size_t index = linearise_co(g, colonne, j);
  while (index < h * w && g->tab[index] == 0)
  {
    --j;
    index = linearise_co(g, colonne, j);
  }

  // cas ou on est en dehors des limites du tableau ie. la colonne est vide
  if (get_color(index, g) == 0)
  {
    return 0;
  }

  // sinon (la colonne n'est pas vide)
  size_t i = 0;
  index = linearise_co(g, colonne, j);
  while (get_color(index, g) != 0)
  {
    char *als = alignement(g, index);
    // pour chacuns des trois alignements (ne compte pas le vertical)
    for (int i2 = 0; i2 < 3; ++i2)
    {
      if (als[i2] >= 3)
      {
        // on separe dans 'output' les alignements par une valeur impossible
        // ici on utilise 'SIZE_T_MAX' definie en haut de fichier
        append(output, SIZE_T_MAX);


        // ajout des points :
        res += lambda * (als[i2] - 2);

        // sauvegarde des index :
        size_t *list_ind = get_index_from_alignement(g, index, i2, als[i2], SIZE_T_MAX);
        for (int i3 = 0; i3 < 5 && list_ind[i3] != SIZE_T_MAX; ++i3)
        {
          append(output, list_ind[i3]);
        }
        free(list_ind);
      }
    }
    free(als);
    ++i;
    index = linearise_co(g, colonne, j - i);
  }

  // pour l'alignement vertical :
  i = 0;
  index = linearise_co(g, colonne, j);
  bool alignement_present;
  while (get_color(index, g) != 0)
  {
    alignement_present = false;
    size_t *list_ind = malloc(5 * sizeof(size_t));
    list_ind[0] = list_ind[1] = list_ind[2] = list_ind[3] = list_ind[4] = h * w;
    char c = alignement_vertical(g, index, &list_ind);
    if (c >= 3)
    {
      alignement_present = true;
      // ajout des points :
      res += lambda * (c - 2);

      // sauvegarde des index :
      for (int i3 = 0; i3 < 5 && list_ind[i3] != h * w; ++i3)
      {
        append(output, list_ind[i3]);
      }
    }
    free(list_ind);
    if (alignement_present)
    {
      i += c;
    }
    else
    {
      i += 3;
    }
    index = linearise_co(g, colonne, j - i);
  }
  return res;
}

//////////////////// fonctions pour gerer les conflits //////////////////

//permet d'avoir la taille d'un motif
size_t taille_motif(struct tableau *tab, size_t split, size_t index_courant)
{
  size_t res = 0;
  while (index_courant < tab->length && tab->data[index_courant] != split)
  {
    ++res;
    ++index_courant;
  }
  return res;
}

// permet de comparer deux motifs
// si les motifs compares possede au moins deux elements communs, on renvoie true
bool compare_motif(size_t* array1, size_t size1, size_t* array2, size_t size2)
{
  size_t count = 0;
  for (size_t i = 0; i < size1; i++) {
    for (size_t j = 0; j < size2; j++) {
      if (array1[i] == array2[j]) {
        count++;
      }
    }
  }
  //(count >= 2)? printf("comparaison : true\n") : printf("comparaison : false\n");
  return count >= 2;
}

// permet de supprimer certain conflits qui apparaissent lors du calcul de points
// renvoie le nombre de points a retirer du total des points pour avoir quelque chose de correcte
// la variable 'split' est en gros un separateur entre les index pour informer que l'on change d'alignement
int suppr_conflit(struct tableau *tab, size_t split)
{
  int res = 0;
  // le premier element est un separateur
  size_t index_courant = 1;
  for(;index_courant < tab->length;)
  {
    size_t t1 = taille_motif(tab, split, index_courant);
    
    for(size_t index_suivant = index_courant + t1 + 1; index_suivant < tab->length;)
    {
      size_t t2 = taille_motif(tab, split, index_suivant);
      if (compare_motif(&tab->data[index_courant], t1, &tab->data[index_suivant], t2))
      {
        (t1 > t2)? (res += t2 - 2) : (res += t1 - 2);
        break;
      }
      index_suivant = index_suivant + t2 + 1;      
    }

    index_courant = index_courant + t1 + 1;
  }
  return res;
}

//////////////////// on reprend les fonctions principales ///////////////////////////

// permet de supprimer les alignements apres une insertion sur un tableau dynamique de colonnes
// renvoie le nombre de points gagne en fonction de lambda qui sera 
// par defaut et lors du premier appel a la fonction egale a 1
int suppr_alignement(struct grille *g, struct tableau *colonnes, int lambda)
{

  int points = 0;
  size_t w = g->width;
  struct tableau index_a_retirer;
  create_tableau(&index_a_retirer);
  bool *gravite_applique = calloc(w, sizeof(bool));

  for(size_t c = 0; c < colonnes->length; ++c)
  {
    size_t colonne = colonnes->data[c];
    // on regarde le nombre de point que l'on vas gagner plus les index a retirer
    points += count_points(g, colonne, lambda, &index_a_retirer);
  }

  // on met la brique index a zero'
  for (size_t i = 0; i < index_a_retirer.length; ++i)
  {
    size_t index = index_a_retirer.data[i];
    if (index == SIZE_T_MAX) {
      continue;
    }
    size_t col = get_colonne(index, g);
    
    g->tab[index] = 0;

    // on applique la gravite sur chaque colonne d'index retire
    bool grv = gravite(g, col);
    if (grv)
    {
      gravite_applique[col] = grv;
    }
  }


  // si la gravite a ete applique on fait un appel recursif sur toutes les colonnes descendues
  // avec lambda incremente on renvoie le total des points

  // on creer un tableau de colonnes a parcourir a partir de celles descendues :
  struct tableau next_cols;
  create_tableau(&next_cols);
  for (size_t i = 0; i < w; ++i)
  {
    if (gravite_applique[i])
    {
      append(&next_cols, i);
    }
  }
  // on supprime les points en trop du aux conflits :
  points -= lambda * suppr_conflit(&index_a_retirer, SIZE_T_MAX);
  //print_array(index_a_retirer.data, index_a_retirer.length, SIZE_T_MAX - 1);


  if (next_cols.length > 0)
  {
    points += suppr_alignement(g, &next_cols, lambda + 1);
  }
  

  free(gravite_applique);
  destroy_tableau(&index_a_retirer);
  destroy_tableau(&next_cols);
  return points;
}

/////////////// fonctions relatives au joueur ////////////////////


// permet de choisir son coup
// renvoie le nombre de permutation et enregistre dans la variable 'output'
// sur quelle colonne placer les briques
// un coup est represente par (3 * colonne + nb_permutations)
int choix_player(struct grille *g, char *briques, size_t *output)
{
  int max_points = -1;
  struct grille jeu_temp;

  struct tableau coups;
  create_tableau(&coups);
  size_t w = g->width;

  // pour chaque colonne
  for (size_t c = 0; c < w; ++c)
  {
    // pour chaque permutation
    for (int p = 0; p < 3; ++p)
    {
      copie(&jeu_temp, g);

      // on fait le coup
      

      insert(&jeu_temp, c, briques);

      struct tableau col;
      create_tableau(&col);
      append(&col, c);
      int points = suppr_alignement(&jeu_temp, &col, 1);
      destroy_tableau(&col);

      // on enregistre points et coup si points >= max_points

      if (points > max_points)
      {
        max_points = points;
        coups.length = 0;
        append(&coups, 3 * c + p);
      }
      else if (points == max_points)
      {
        append(&coups, 3 * c + p);
      }
      destroy_grille(&jeu_temp);

      permutation(briques);
    }
  }

  // Info : on a maintenant tous les coups qui apportent le maximum de points
  // l'etape suivante est de choisir parmis ces coups lequel il faut faire
  // intuitivement on peux se dire que prendre toujours le premier est une mauvaise idee
  // car le joueur ne jouera que la premiere colonne


  // Choix aleatoire :
  printf("max points = %d\n", max_points);
  size_t choix = coups.data[rand() % coups.length];
  *output = choix / 3; // la colonne 
  int perm = choix % 3; // nombre de permutations
  destroy_tableau(&coups);

  return perm;
}


/////////////// fonction main ///////////////

int main(int argc, char const *argv[])
{ 
 



  srand(time(NULL));
  struct grille g;
  create_grille(&g, 11);

  // size_t col;
  // for (int i = 0; i < 30; ++i)
  // {
  //   col = rand() % g.width;
  //   char *b = rand_briques();
  //   insert(&g, col, b);
  //   free(b);
  // }

  g.tab[0] = 3;
  g.tab[1] = 2;
  g.tab[2] = 1;
  g.tab[5] = 1;
  g.tab[7] = 4;
  g.tab[8] = 3;
  g.tab[9] = 2;
  g.tab[10] = 2;
  g.tab[11] = 2;
  g.tab[12] = 2;
  g.tab[13] = 6;
  g.tab[16] = 5;
  g.tab[20] = 5;
  

  char *briques = malloc(3 * sizeof(char));
  briques[0] = 1;
  briques[1] = 5;
  briques[2] = 5;


  print_tableau(&g);

  size_t colonne;
  int permutations = choix_player(&g, briques, &colonne);
  
  for (int i = 0; i < permutations; ++i)
  {
    permutation(briques);
  }

  
  insert(&g, colonne, briques);
  print_tableau(&g);
  printf("choix du joueur :\ncolonne : %zd\nnombre de permutations : %d\n", colonne, permutations);  


  // size_t *output = malloc(5 * sizeof(size_t));
  // output[0] = output[1] = output[2] = output[3] = output[4] = 0;

  // char av = alignement_vertical(&g, 12, &output);

  // printf("alignement verticale : %d\nen position : ", av);
  // print_array(output, 5, 0);
  
  // print_tableau(&g);

  // free(output);



  struct tableau cols;
  create_tableau(&cols);
  append(&cols, colonne);
  int p = suppr_alignement(&g, &cols, 1);
  printf("points = %d\n", p);
  print_tableau(&g);




  destroy_tableau(&cols);
  destroy_grille(&g);
  // free(save);
  free(briques);
  return 0;
}