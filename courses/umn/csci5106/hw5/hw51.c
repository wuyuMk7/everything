#include <stdlib.h>
#include <stdio.h>

/* 
 * Solution to Problem 1, part 1.
 * A type for representing binary search trees of integer values 
 */
typedef struct bst {
  int value;
  struct bst *left;
  struct bst *right;
} BST;

// Function signatures
int member(BST*, int,
	   int (*)(int, int),
	   int (*)(int, int));
BST* insert(BST*, int, 
               int (*)(int, int), 
               int (*lss)(int, int));
void printtree(BST*);

// Functions for comparison
int lss(int, int);
int equal(int, int);

int main(int argc, char *argv[])
{
  printf("* hw51 tests\n");
  BST *tree1 = NULL;
  printf("- Insert a node whose value is 10 to the tree1"
         "\n  and assign the return value from insertion to tree1.\n");
  tree1 = insert(tree1, 10, equal, lss);
  printf("  Elements in tree1: ");
  printtree(tree1);

  printf("\n\n- Insert a node whose value is 5 to the tree1"
         "\n  and assign the return value from insertion to tree2.\n");
  BST *tree2 = insert(tree1, 5, equal, lss);
  printf("  Elements in tree1 now: ");
  printtree(tree1);
  printf("\n  Elements in tree2: ");
  printtree(tree2);

  printf("\n\n- Check if 10 is in tree1 (1 is in, 0 is not in): %d",
	member(tree1, 10, equal, lss)); 
  printf("\n- Check if 5 is in tree1 (1 is in, 0 is not in): %d",
	member(tree1, 5, equal, lss)); 

  printf("\n\n- Insert more nodes to the tree2 and assign the"
         "\n  return value from insertion to tree2.\n");
  printf("  Elements in tree2 now: ");
  tree2 = insert(tree2, 2, equal, lss);
  tree2 = insert(tree2, 6, equal, lss);
  tree2 = insert(tree2, 10, equal, lss);
  tree2 = insert(tree2, 11, equal, lss);
  tree2 = insert(tree2, 15, equal, lss);
  tree2 = insert(tree2, 9, equal, lss);
  printtree(tree2);
  printf("\n\n");

  return 0;
}

/* 
 * Solution to Problem 1, part 2.
 * Function "member" to check if a value exists in the given BST.
 */
int member(BST *bst, int value,
	   int (*equal)(int, int),
	   int (*lss)(int, int))
{
  int retval = 0;
  if (bst != NULL) {
    if (equal(bst->value, value)) {
      retval = 1;
    } else if (lss(value, bst->value)){
      retval = member(bst->left, value, equal, lss);
    } else {
      retval = member(bst->right, value, equal, lss);
    }
  }
  return retval;
}

/* 
 * Solution to Problem 1, part 3.
 * Function "insert" to insert a new element into the given BST.
 */
BST* insert(BST *bst, int value,
	       int (*equal)(int, int),
	       int (*lss)(int, int))
{
  BST *ret = bst;
  if (bst == NULL) {
    BST *new = (BST*)malloc(sizeof(BST));
    new->value = value;
    new->left = NULL;
    new->right = NULL;
    ret = new;
  } else if (equal(value, bst->value)) {
    ret = bst;
  } else if (lss(value, bst->value)){
    BST *new = insert(bst->left, value, equal, lss);
    if (new != bst->left) {
      BST *new_parent = (BST*)malloc(sizeof(BST));
      new_parent->value = bst->value;
      new_parent->left = new;
      new_parent->right = bst->right;
      ret = new_parent;
    }
  } else {
    BST *new = insert(bst->right, value, equal, lss);
    if (new != bst->right) {
      BST *new_parent = (BST*)malloc(sizeof(BST));
      new_parent->value = bst->value;
      new_parent->right = new;
      new_parent->left = bst->left;
      ret = new_parent;
    }
  }

  return ret;
}

/* 
 * Solution to Problem 1, part 4.
 * Function "printtree" to print the given BST on the screen
 */
void printtree(BST *bst) {
  if (bst != NULL) {
    printtree(bst->left);
    printf("%d ", bst->value);
    printtree(bst->right);
  }
}

int lss(int lhs, int rhs)
{
  return lhs < rhs;
}

int equal(int lhs, int rhs)
{
  return lhs == rhs;
}

