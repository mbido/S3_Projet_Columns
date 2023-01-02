#include <assert.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <string.h>





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

    
    printf("\n");
    for (size_t i2 = 0; i2 < w; ++i2)
    {
      printf("---");
    }
    printf("\n");

    for (size_t i = 0; i < w; ++i)
    { 
      index = linearise_co(g, i, j);
      int color = (int)g->tab[index];
      printf("|%s%d%s|", colors[color], color, colors[0]);
      
    }
  }
  printf("\n");
}

// permet d'afficher un array de size_t (pour simplifier les tests)
void print_array(size_t *array, size_t length)
{
  printf("[");
  for (size_t i = 0; i < length; ++i)
  {
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
    if (g->tab[index] == 0 && g->tab[linearise_co(g, colonne, j + 1)] != 0)
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
char alignement_vertical(struct grille *g, size_t colonne, size_t **output)
{
  size_t h = g->height;
  size_t w = g->width;
  // on se place sur la brique la plus haute :
  size_t j = h - 1;
  size_t index = linearise_co(g, colonne, j);
  while (index < h * w && g->tab[index] == 0)
  {
    --j;
    index = linearise_co(g, colonne, j);
  }
  char color = g->tab[index];
  *output[0] = index;
  char count = 1;
  for (int i = 1; i < 5; i++)
  {
    index = linearise_co(g, colonne, j - i);
    if (get_color(index, g) != color)
    {
      if (count >= 3)
      {
        break;
      }
      color = g->tab[index];
      count = 0;
    }
    // cette ligne casse tout pourquoi ?
    (*output)[(int)count] = index;
    count++;
  }
  return count;
}

// permet de sauvegarder une colonne dans une liste de liste
// utile lors de la simulation de coups étant donné que l'on doit
// reellement le faire puis reinitialiser la colonne
void save_colonne(struct grille *g, size_t colonne, size_t ***output)
{
  size_t *save = malloc(g->height * sizeof(size_t));
  for (size_t j = 0; get_color(linearise_co(g, colonne, j), g) != 0; ++j)
  {
    save[j] = g->tab[linearise_co(g, colonne, j)];
  }
  (*output)[colonne] = save;
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
      }
    }
  }
  return res;
}

// permet de compter les points qui serons gagnes lors de la suppression
// des alignements presents en fonction de lambda, le coeficient de points
// elle prend en argument la colonne ou le bloc de 3 briques est pose
// elle sauvegarde de plus dans une liste 'output' chaque index des briques qui
// serons supprimees il y a 29 index differents au maximum mais comme ils peuvent etre 
// appeles plusieur fois en tout il faut un tableau de taille 48
int count_points(struct grille *g, size_t colonne, int lambda, size_t **output)
{
  size_t h = g->height;
  size_t w = g->width;
  int index_output = 0;
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
  // (ne devrait pas arriver)
  if (get_color(index, g) == 0)
  {
    return 0;
  }

  // sinon (la colonne n'est pas vide)
  // pour chacune des trois premieres briques :
  for (int i = 0; i < 3; ++i)
  {
    index = linearise_co(g, colonne, j - i);
    char *als = alignement(g, index);
    // pour chacuns des trois alignements (ne compte pas le vertical)
    for (int i2 = 0; i2 < 3; ++i2)
    {
      if (als[i2] >= 3)
      {
        // ajout des points :
        res += lambda * (als[i2] - 2);

        // sauvegarde des index :
        size_t *list_ind = get_index_from_alignement(g, index, i2, als[i2], h * w);
        for (int i3 = 0; i3 < 5 && list_ind[i3] != h * w; ++i3)
        {
          (*output)[index_output] = list_ind[i3];
          ++index_output;
        }
        free(list_ind);
      }
    }
    free(als);
  }
  // pour l'alignement vertical :
  size_t *list_ind = malloc(5 * sizeof(size_t));
  list_ind[0] = list_ind[1] = list_ind[2] = list_ind[3] = list_ind[4] = h * w;
  char c = alignement_vertical(g, colonne, &list_ind);
  if (c >= 3)
  {
    // ajout des points :
    res += lambda * (c - 2);

    // sauvegarde des index :
    for (int i3 = 0; i3 < 5 && list_ind[i3] != h * w; ++i3)
    {
      (*output)[index_output] = list_ind[i3];
      ++index_output;
    }
  }
  free(list_ind);
  return res;
}

/////////////// fonction main ///////////////

int main(int argc, char const *argv[])
{ 
  char briques1[3] = {1, 1, 1};

  struct grille g;
  create_grille(&g, 5);

  insert(&g, 1, briques1);
  insert(&g, 1, briques1);
  insert(&g, 2, briques1);
  insert(&g, 2, briques1);
  insert(&g, 3, briques1);
  insert(&g, 3, briques1);




  size_t *output = malloc(5 * sizeof(size_t));
  output[0] = output[1] = output[2] = output[3] = output[4] = 0;

  char av = alignement_vertical(&g, 2, &output);

  printf("alignement verticale : %d\nen position : ", av);
  print_array(output, 5);
  
  print_tableau(&g);

  free(output);

  size_t *index_a_retirer = calloc(48, sizeof(size_t));
  int points = count_points(&g, 2, 1, &index_a_retirer);
  printf("les points gagnes seront : %d\n", points);
  printf("les briques a retirer seront : ");
  print_array(index_a_retirer, 48);


  destroy_grille(&g);

  return 0;
}