// seed : 0
// c'est modifie
#include <assert.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>




#include <string.h>

struct resultat {
  int num_colonne;
  int num_permutation;
};

struct grille {
  char *tab;
  size_t width;
  size_t height;
};

void create_grille(struct grille *self, size_t largeur) {
  self->width = largeur;
  self->height = 15;
  self->tab = calloc(largeur * 15, sizeof(char));
}

void destroy_grille(struct grille *self) {
  free(self->tab);
  self->tab = NULL;
}

size_t linearise_co(struct grille *g, size_t i, size_t j) {
  return j * g->width + i;
}

void descend_brique(struct grille *g, size_t i, size_t j) {
  // on est dans le cas ou une brique est presente en j+1, i et un vide est
  // present en j, i. On descend alors la brique du dessus tout en bas du trou
  size_t h = g->height;
  size_t w = g->width;
  size_t index = linearise_co(g, i, j);
  while (index < h * w && g->tab[index] == 0) {
    g->tab[index] = g->tab[linearise_co(g, i, j - 1)];
    g->tab[linearise_co(g, i, j - 1)] = 0;
    --j;
    index = linearise_co(g, i, j);
  }
}

bool gravite(struct grille *g, size_t colonne) {
  // on applique la gravite a une colonne et renvoie si des briques sont tombees
  bool res = false;
  size_t h = g->height;
  size_t w = g->width;
  // on demarre a partir de la brique la plus haute :
  size_t j = g->height - 1;
  size_t index = linearise_co(g, colonne, j);
  while (index < h * w && g->tab[index] == 0) {
    --j;
    index = linearise_co(g, colonne, j);
  }

  while (index < h * w) {
    // la deuxieme condition (apres le "&&") est la pour le cas ou on fait
    // descendre la brique la plus haute
    if (g->tab[index] == 0 && g->tab[linearise_co(g, colonne, j - 1)] != 0) {
      res = true;
      descend_brique(g, colonne, j);
      ++j;
      index = linearise_co(g, colonne, j);
    } else
      --j;
    index = linearise_co(g, colonne, j);
  }
  return res;
}

void double_hauteur(struct grille *g) {
  size_t h = g->height;
  size_t w = g->width;
  char *new = calloc((h * 2 + 1) * w, sizeof(char));
  for (size_t i = 0; i < h * w; ++i) {
    new[i] = g->tab[i];
  }
  free(g->tab);
  g->tab = new;
  g->height = (h * 2 + 1) * w;
}

void insert(struct grille *g, size_t colonne, char *briques) {
  size_t h = g->height;
  size_t w = g->width;
  assert(colonne < w);

  // on demarre juste avant la brique la plus haute :
  size_t j = g->height - 1;
  size_t index = linearise_co(g, colonne, j);
  while (index < h * w && g->tab[index] == 0) {
    --j;
    index = linearise_co(g, colonne, j);
  }
  ++j;
  index = linearise_co(g, colonne, j);

  // si la taille est insuffisante
  if (j > h - 3)
    double_hauteur(g);
  g->tab[j] = briques[2];
  g->tab[j + 1] = briques[1];
  g->tab[j + 2] = briques[0];
}

void permutation(char *briques) {
  char temp = briques[0];
  char briques[0] = briques[1];
  char briques[1] = briques[2];
  char briques[2] = temp;
}

size_t get_colonne(size_t index, struct *grille g) { return index % g->width; }

size_t get_ligne(size_t index, struct *grille g) { return index / g->width; }

char get_color(size_t index, struct *grille g) {
  if (get_ligne(index, g) >= g->height)
    return 0;
  return g->tab[index];
}

int *alignement(struct grille *g, size_t index)
// retourne un tableau de 3 char representant respectivement
// - 0 : alignement horizontal
// - 1 : alignement premiere diagonale
// - 2 : alignement deuxieme diagonale

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
  int colonne = get_colonne(index, g->width);
  int ligne = get_ligne(index, g->width);
  char color = g->tab[index];
  char res[2];

  size_t check_left = 2;
  size_t check_right = 2;

  if (colonne < 2)
    check_left = colonne;
  if (g->width - colonne < 2)
    check_right = g->width - colonne;

  if (check_left == 2) {
    if (check_right == 2) {
      // a tester en une ligne sans caste
      res[0] = get_color(index - 1, g) == color;  // f
      res[0] += get_color(index + 1, g) == color; // g
      res[0] += get_color(index - 1, g) == color &&
                get_color(index - 2, g) == color; // e
      res[0] += get_color(index + 1, g) == color &&
                get_color(index + 2, g) == color; // h

      res[1] = get_color(index + g->width - 1, g) == color;  // c
      res[1] += get_color(index - g->width + 1, g) == color; // j
      res[1] += get_color(index + g->width - 1, g) == color &&
                get_color(index + 2 * g->width - 2, g) == color; // a
      res[1] += get_color(index - g->width + 1, g) == color &&
                get_color(index - 2 * g->width + 2, g) == color; // l

      res[2] = get_color(index + g->width + 1, g) == color;  // d
      res[2] += get_color(index - g->width - 1, g) == color; // i
      res[2] += get_color(index + g->width + 1, g) == color &&
                get_color(index + 2 * g->width + 2, g) == color; // b
      res[2] += get_color(index - g->width - 1, g) == color &&
                get_color(index - 2 * g->width - 2, g) == color; // k

    } else if (check_right == 1) {
      res[0] = get_color(index - 1, g) == color;  // f
      res[0] += get_color(index + 1, g) == color; // g
      res[0] += get_color(index - 1, g) == color &&
                get_color(index - 2, g) == color; // e

      res[1] = get_color(index + g->width - 1, g) == color;  // c
      res[1] += get_color(index - g->width + 1, g) == color; // j
      res[1] += get_color(index + g->width - 1, g) == color &&
                get_color(index + 2 * g->width - 2, g) == color; // a

      res[2] = get_color(index + g->width + 1, g) == color;  // d
      res[2] += get_color(index - g->width - 1, g) == color; // i
      res[2] += get_color(index - g->width - 1, g) == color &&
                get_color(index - 2 * g->width - 2, g) == color; // k
    } else {
      res[0] = get_color(index - 1, g) == color; // f
      res[0] += get_color(index - 1, g) == color &&
                get_color(index - 2, g) == color; // e

      res[1] = get_color(index + g->width - 1, g) == color; // c
      res[1] += get_color(index + g->width - 1, g) == color &&
                get_color(index + 2 * g->width - 2, g) == color; // a

      res[2] = get_color(index - g->width - 1, g) == color; // i
      res[2] += get_color(index - g->width - 1, g) == color &&
                get_color(index - 2 * g->width - 2, g) == color; // k
    }
  } else if (check_left == 1) {
    if (check_right == 2) {
      res[0] = get_color(index - 1, g) == color;  // f
      res[0] += get_color(index + 1, g) == color; // g
      res[0] += get_color(index + 1, g) == color &&
                get_color(index + 2, g) == color; // h

      res[1] = get_color(index + g->width - 1, g) == color;  // c
      res[1] += get_color(index - g->width + 1, g) == color; // j
      res[1] += get_color(index - g->width + 1, g) == color &&
                get_color(index - 2 * g->width + 2, g) == color; // l

      res[2] = get_color(index + g->width + 1, g) == color;  // d
      res[2] += get_color(index - g->width - 1, g) == color; // i
      res[2] += get_color(index + g->width + 1, g) == color &&
                get_color(index + 2 * g->width + 2, g) == color; // b

    } else if (check_right == 1) {
      res[0] = get_color(index - 1, g) == color;  // f
      res[0] += get_color(index + 1, g) == color; // g

      res[1] = get_color(index + g->width - 1, g) == color;  // c
      res[1] += get_color(index - g->width + 1, g) == color; // j

      res[2] = get_color(index + g->width + 1, g) == color;  // d
      res[2] += get_color(index - g->width - 1, g) == color; // i
    }
  } else if (check_left == 0) {
    if (check_right == 2) {
      res[0] = get_color(index + 1, g) == color; // g
      res[0] += get_color(index + 1, g) == color &&
                get_color(index + 2, g) == color; // h

      res[1] = get_color(index - g->width + 1, g) == color; // j
      res[1] += get_color(index - g->width + 1, g) == color &&
                get_color(index - 2 * g->width + 2, g) == color; // l

      res[2] = get_color(index + g->width + 1, g) == color; // d
      res[2] += get_color(index + g->width + 1, g) == color &&
                get_color(index + 2 * g->width + 2, g) == color; // b

    } else if (check_right == 1) {
      res[0] = get_color(index + 1, g) == color; // g

      res[1] = get_color(index - g->width + 1, g) == color; // j

      res[2] = get_color(index + g->width + 1, g) == color; // d
    }
  }
}

// verifie l'alignement du haut de la colonne vers le bas
// index = haut de la colonne
// output est un tableau de taille 5 dans lequel on met les index de l'alignement
void alignement_vers_le_bas(struct grille *g, index, size_t **output) {
  unsigned char color = g->tab[index];
  size_t count = 1;
  *output[0] = index;
  
  for (size_t i = 1; i < 5; i++) {
    if (g->tab[index - i * g->width] != color) {
      if(count >= 3){
        break;
      }
      color = g->tab[index - i * g->width];
      count = 0;
    }
    *output[count] = index - i * g->width;
    count++;
  }
  if (count < 3){
    *output[0] = *output[1] = *output[2] = *output[3] = *output[4] = 0;
  }
}






int **alignement_colonne(struct *g, size_t index) {
  int *res1 = alignement(g, index);
  res1[1] = alignement_vers_le_bas(g, index);
  int *res2 = alignement(g, index - g->width);
  int *res3 = alignement(g, index - 2 * g->width);

  return [ res1, res2, res3 ];
}

void remove_by_index(struct grille *g, size_t index, int *align) {
  if (g->tab[0] == 3) {
    g->tab[index] = '0';
    g->tab[index + g->width - 1] = '0';
    g->tab[index + 2 * g->width - 2] = '0';
  }
  if (g->tab[2] == 3) {
    g->tab[index] = '0';
    g->tab[index + g->width + 1] = '0';
    g->tab[index + 2 * g->width + 2] = '0';
  }
  if (g->tab[3] == 3) {
    g->tab[index] = '0';
    g->tab[index - 1] = '0';
    g->tab[index - 2] = '0';
  }
  if (g->tab[4] == 3) {
    g->tab[index] = '0';
    g->tab[index + 1] = '0';
    g->tab[index + 2] = '0';
  }
  if (g->tab[5] == 3) {
    g->tab[index] = '0';
    g->tab[index - g->width - 1] = '0';
    g->tab[index - 2 * g->width - 2] = '0';
  }
  if (g->tab[7] == 3) {
    g->tab[index] = '0';
    g->tab[index - g->width + 1] = '0';
    g->tab[index - 2 * g->width + 2] = '0';
  }
}

// l'index est celui du haut de la colonne ajouté
void remove_colonne(strut grille *g, size_t index, int *align) {
  int *A_align = alignement(g->tab, index, g->width);
  int *B_align = alignement(g->tab, index - g->width, g->width);
  int *C_align = alignement(g->tab, index - 2 * g->width, g->width);

  size_t count = alignement_vers_le_bas(g, index);

  remove_by_index(g, index + 2 * g->width, A_align);
  remove_by_index(g, index + 2 * g->width, B_align);
  remove_by_index(g, index + 2 * g->width, C_align);

  if (count >= 3) {
    g->tab[index] = '0';
    g->tab[index - g->width] = '0';
    g->tab[index - 2 * g->width] = '0';
    if (count >= 4) {
      g->tab[index - 3 * g->width] = '0';
      if (count == 5) {
        g->tab[index - 4 * g->width] = '0';
      }
    }
  }
}

void print_colonne(char *briques) {
  printf("+-+\n");
  printf("|%d|\n", (int)briques[0]);
  printf("+-+\n");
  printf("|%d|\n", (int)briques[1]);
  printf("+-+\n");
  printf("|%d|\n", (int)briques[2]);
  printf("+-+\n");
}

//////////////////////////////////
///main////////////////////////////////////////////
int main() {
  char briques[3] = {1, 2, 3} // unsigned char *tab = init_tab(10);

  print_colonne(briques);
  printf("\napres permutation :\n");
  permutation(briques);
  print_colonne(briques);
  return 0;
}
