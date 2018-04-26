#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_HEIGHT 1000
#define INFINITY (1<<20)

typedef struct node node;
struct node
{
    int red; //red - 1, black - 0
    int content;
    struct node *child[2];
};

struct tree
{
    node *root;
};

int is_red(node *root);
node *single_rotation(node *root, int dir);
node *double_rotation(node *root, int dir);
node *create_node(int content);
node *insert_r(node *root, int content);
void insert_node(struct tree *tree, int content);
void print_paths(node* node);
void print_paths_r(node* node, int path[], int path_length);
void print_array(int ints[], int len);
node *remove_balance(node *root, int dir, int *done);
node *remove_r(node *root, int content, int *done);
void remove_node(struct tree *tree, int content);

typedef struct asciinode_struct asciinode;

struct asciinode_struct
{
  asciinode *child[2];

  int edge_length;

  int height;

  int lablen;

  int parent_dir;

  char label[11];
};


int MIN (int X, int Y);
int MAX (int X, int Y);
asciinode * build_ascii_tree_recursive(node * t);
asciinode * build_ascii_tree(node * t);
void free_ascii_tree(asciinode *node);
void compute_lprofile(asciinode *node, int x, int y);
void compute_rprofile(asciinode *node, int x, int y);
void compute_edge_lengths(asciinode *node);
void print_level(asciinode *node, int x, int level);
void print_ascii_tree(node * n);


int is_red(node *root)
{
    return root != NULL && root->red == 1;
}

node *single_rotation(node *root, int dir)
{
    node *saved = root->child[!dir];

    root->child[!dir] = saved->child[dir];
    saved->child[dir] = root;

    root->red = 1;
    saved->red = 0;

    return saved;
}

node *double_rotation(node *root, int dir)
{
    root->child[!dir] = single_rotation(root->child[!dir], !dir);

    return single_rotation(root, dir);
}


node *create_node(int content)
{
    node *new_node = malloc(sizeof *new_node);

    if (new_node)
    {
        new_node->content = content;
        new_node->red = 1;
        new_node->child[0] = NULL;
        new_node->child[1] = NULL;
    }

    return new_node;
}

node *insert_r(node *root, int content)
{
    if (root == NULL)
    {
        root = create_node(content);
    }
    else if (content != root->content)
    {
        int dir = 0;

        if (root->content < content) dir = 1;

        root->child[dir] = insert_r(root->child[dir], content);

        if (is_red(root->child[dir]))
        {
            if (is_red(root->child[!dir]))
            {
                root->red = 1;
                root->child[0]->red = 0;
                root->child[1]->red = 0;
            }
            else
            {
                if (is_red(root->child[dir]->child[dir]))
                {
                    root = single_rotation(root, !dir);
                }
                else if (is_red(root->child[dir]->child[!dir]))
                {
                    root = double_rotation(root, !dir);
                }
            }
        }
    }

    return root;
}


void insert_node(struct tree *tree, int content)
{
    tree->root = insert_r(tree->root, content);
    tree->root->red = 0;
    return;
}


node *remove_balance(node *root, int dir, int *done)
{
    node *p = root;
    node *s = root->child[!dir];

    if (s != NULL && !is_red(s))
    {
        if (!is_red(s->child[0]) && !is_red(s->child[1]))
        {
            if (is_red(p))
            {
                *done = 1;
            }

            p->red = 0;
            s->red = 1;
        }
        else
        {
            int save = root->red;

            if (is_red(s->child[!dir]))
            {
                p = single_rotation(p, dir);
            }
            else
            {
                p = double_rotation(p, dir);
            }

            p->red = save;
            p->child[0]->red = 0;
            p->child[1]->red = 0;
            *done = 1;
        }
    }
    else if (s->child[dir] != NULL)
    {
        node *r = s->child[dir];

        if (!is_red(r->child[0]) && !is_red(r->child[1]))
        {
            p = single_rotation(p, dir);
            p->child[dir]->child[!dir]->red = 1;
        }
        else
        {
            if (is_red(r->child[dir]))
            {
                s->child[dir] = single_rotation(r, !dir);
            }

            p = double_rotation(p, dir);
            s->child[dir]->red = 0;
            p->child[!dir]->red = 1;
        }

        p->red = 0;
        p->child[dir]->red = 0;
        *done = 1;
    }

    return p;
}

node *remove_r(node *root, int content, int *done)
{
    if (root == NULL)
    {
        *done = 1;
    }
    else
    {

        if (root->content == content)
        {
            if (root->child[0] == NULL || root->child[1] == NULL)
            {
                node *saved = root->child[root->child[0] == NULL];

                if (is_red(root))
                {
                    *done = 1;
                }
                else if (is_red(saved))
                {
                    saved->red = 0;
                    *done = 1;
                }

                free(root);

                return saved;
            }
            else
            {
                node *heir = root->child[0];

                while (heir->child[1] != NULL)
                {
                    heir = heir->child[1];
                }

                root->content = heir->content;
                content = heir->content;
            }
        }

        int dir = 0;

        if (root->content < content) dir = 1;

        root->child[dir] = remove_r(root->child[dir], content, done);

        if (!(*done))
        {
            root = remove_balance(root, dir, done);
        }
    }

    return root;
}

void remove_node(struct tree *tree, int content)
{
    int done = 0;

    tree->root = remove_r(tree->root, content, &done);

    if (tree->root != NULL)
    {
        tree->root->red = 0;
    }

    return;
}


int lprofile[MAX_HEIGHT];
int rprofile[MAX_HEIGHT];


int gap = 4;

int print_next;

int MIN (int X, int Y)
{
  return (X < Y) ? X : Y;
}

int MAX (int X, int Y)
{
  return (X > Y) ? X : Y;
}

asciinode * build_ascii_tree_recursive(node * t)
{
  asciinode * node;

  if (t == NULL) return NULL;

  node = malloc(sizeof(asciinode));
  node->child[0] = build_ascii_tree_recursive(t->child[0]);
  node->child[1] = build_ascii_tree_recursive(t->child[1]);

  if (node->child[0] != NULL)
  {
    node->child[0]->parent_dir = -1;
  }

  if (node->child[1] != NULL)
  {
    node->child[1]->parent_dir = 1;
  }

  (t->red)==1 ? sprintf(node->label, "%dR", t->content) : sprintf(node->label, "%dB", t->content);
  node->lablen = strlen(node->label);

  return node;
}


asciinode * build_ascii_tree(node * t)
{
  asciinode *node;
  if (t == NULL) return NULL;
  node = build_ascii_tree_recursive(t);
  node->parent_dir = 0;
  return node;
}

void free_ascii_tree(asciinode *node)
{
  if (node == NULL) return;
  free_ascii_tree(node->child[0]);
  free_ascii_tree(node->child[1]);
  free(node);
}

void compute_lprofile(asciinode *node, int x, int y)
{
  int i, isleft;
  if (node == NULL) return;
  isleft = (node->parent_dir == -1);
  lprofile[y] = MIN(lprofile[y], x-((node->lablen-isleft)/2));
  if (node->child[0] != NULL)
  {
	  for (i=1; i <= node->edge_length && y+i < MAX_HEIGHT; i++)
    {
	    lprofile[y+i] = MIN(lprofile[y+i], x-i);
    }
  }
  compute_lprofile(node->child[0], x-node->edge_length-1, y+node->edge_length+1);
  compute_lprofile(node->child[1], x+node->edge_length+1, y+node->edge_length+1);
}

void compute_rprofile(asciinode *node, int x, int y)
{
  int i, notleft;
  if (node == NULL) return;
  notleft = (node->parent_dir != -1);
  rprofile[y] = MAX(rprofile[y], x+((node->lablen-notleft)/2));
  if (node->child[1] != NULL)
  {
	  for (i=1; i <= node->edge_length && y+i < MAX_HEIGHT; i++)
    {
	    rprofile[y+i] = MAX(rprofile[y+i], x+i);
    }
  }
  compute_rprofile(node->child[0], x-node->edge_length-1, y+node->edge_length+1);
  compute_rprofile(node->child[1], x+node->edge_length+1, y+node->edge_length+1);
}

void compute_edge_lengths(asciinode *node)
{
  int h, hmin, i, delta;
  if (node == NULL) return;
  compute_edge_lengths(node->child[0]);
  compute_edge_lengths(node->child[1]);

  if (node->child[1] == NULL && node->child[0] == NULL)
  {
	  node->edge_length = 0;
  }
  else
  {
    if (node->child[0] != NULL)
    {
	    for (i=0; i<node->child[0]->height && i < MAX_HEIGHT; i++)
      {
		    rprofile[i] = -INFINITY;
	    }
	    compute_rprofile(node->child[0], 0, 0);
	    hmin = node->child[0]->height;
    }
    else
    {
	    hmin = 0;
    }
	  if (node->child[1] != NULL)
    {
	    for (i=0; i<node->child[1]->height && i < MAX_HEIGHT; i++)
      {
		    lprofile[i] = INFINITY;
	    }
	    compute_lprofile(node->child[1], 0, 0);
	    hmin = MIN(node->child[1]->height, hmin);
    }
    else
    {
	    hmin = 0;
    }
	  delta = 4;
	  for (i=0; i<hmin; i++)
    {
	    delta = MAX(delta, gap + 1 + rprofile[i] - lprofile[i]);
    }

	  if (((node->child[0] != NULL && node->child[0]->height == 1) ||
	      (node->child[1] != NULL && node->child[1]->height == 1))&&delta>4)
    {
      delta--;
    }

    node->edge_length = ((delta+1)/2) - 1;
  }

  h = 1;
  if (node->child[0] != NULL)
  {
	  h = MAX(node->child[0]->height + node->edge_length + 1, h);
  }
  if (node->child[1] != NULL)
  {
	  h = MAX(node->child[1]->height + node->edge_length + 1, h);
  }
  node->height = h;
}

void print_level(asciinode *node, int x, int level)
{
  int i, isleft;
  if (node == NULL) return;
  isleft = (node->parent_dir == -1);
  if (level == 0)
  {
	  for (i=0; i<(x-print_next-((node->lablen-isleft)/2)); i++)
    {
	    printf(" ");
    }
	  print_next += i;
	  printf("%s", node->label);
	  print_next += node->lablen;
  }
  else if (node->edge_length >= level)
  {
	  if (node->child[0] != NULL)
    {
	    for (i=0; i<(x-print_next-(level)); i++)
      {
		    printf(" ");
	    }
	    print_next += i;
	    printf("/");
	    print_next++;
    }
	  if (node->child[1] != NULL)
    {
	    for (i=0; i<(x-print_next+(level)); i++)
      {
		    printf(" ");
	    }
	    print_next += i;
	    printf("\\");
	    print_next++;
    }
  }
  else
  {
	  print_level(node->child[0],
                x-node->edge_length-1,
                level-node->edge_length-1);
	  print_level(node->child[1],
                x+node->edge_length+1,
                level-node->edge_length-1);
  }
}

void print_ascii_tree(node * n)
{
  asciinode *proot;
  int xmin, i;
  if (n == NULL) return;
  proot = build_ascii_tree(n);
  compute_edge_lengths(proot);
  for (i=0; i<proot->height && i < MAX_HEIGHT; i++)
  {
	  lprofile[i] = INFINITY;
  }
  compute_lprofile(proot, 0, 0);
  xmin = 0;
  for (i = 0; i < proot->height && i < MAX_HEIGHT; i++)
  {
	  xmin = MIN(xmin, lprofile[i]);
  }
  for (i = 0; i < proot->height; i++)
  {
	  print_next = 0;
	  print_level(proot, -xmin, i);
	  printf("\n");
  }
  if (proot->height >= MAX_HEIGHT)
  {
	  printf("(This tree is taller than %d, and may be drawn incorrectly.)\n", MAX_HEIGHT);
  }
  free_ascii_tree(proot);
}


int main()
{
  struct tree *tree = malloc(sizeof *tree);
    tree -> root = NULL;

    insert_node(tree, 50);
    insert_node(tree, 40);
     print_ascii_tree(tree->root);
    printf("-------------------------------\n");
    insert_node(tree, 30);
     print_ascii_tree(tree->root);
    printf("-------------------------------\n");
    insert_node(tree, 100);
    insert_node(tree, 35);
    insert_node(tree, 2);
     print_ascii_tree(tree->root);
    printf("-------------------------------\n");
    insert_node(tree, 1);
    print_ascii_tree(tree->root);
    printf("-------------------------------\n");
    remove_node(tree, 30);
    print_ascii_tree(tree->root);
    printf("-------------------------------\n");
    remove_node(tree, 2);
    print_ascii_tree(tree->root);

    free(tree);
    return 0;
}
