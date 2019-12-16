#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* 
 * Solution to Problem 2, part 1.
 * A union type StrorIntType to hold either an integer or a string
 */
typedef union {
  int int_val;
  char *char_val;
} StrorIntType;

/* 
 * Solution to Problem 2, part 2.
 * A type for representing binary search trees of StrorIntType values 
 */
typedef struct bst {
  StrorIntType value;
  struct bst *left;
  struct bst *right;
} BST;

// Function signatures
int member(BST*, StrorIntType,
	   int (*)(StrorIntType, StrorIntType),
	   int (*)(StrorIntType, StrorIntType));
BST* insert(BST*, StrorIntType, 
               int (*)(StrorIntType, StrorIntType), 
               int (*lss)(StrorIntType, StrorIntType));
void printtree(BST*, void (*)(StrorIntType));

// Functions for integer comparison and printing
int lss_int(StrorIntType, StrorIntType);
int equal_int(StrorIntType, StrorIntType);
void print_int(StrorIntType);

// Functions for string comparison and printing
int lss_str(StrorIntType, StrorIntType);
int equal_str(StrorIntType, StrorIntType);
void print_str(StrorIntType);

int main(int argc, char *arglv[])
{
  // Test for integers
  printf("* hw52 tests - node with integer value.\n");
  StrorIntType int_var = {.int_val = 10};
  BST *tree1 = NULL;
  printf("- Insert a node whose value is 10 to the tree1"
         "\n  and assign the return value from insertion to tree1.\n");
  tree1 = insert(tree1, int_var, equal_int, lss_int);
  printf("  Elements in tree1: ");
  printtree(tree1, print_int);

  printf("\n\n- Insert a node whose value is 5 to the tree1"
         "\n  and assign the return value from insertion to tree2.\n");
  int_var.int_val = 5;
  BST *tree2 = insert(tree1, int_var, equal_int, lss_int);
  printf("  Elements in tree1 now: ");
  printtree(tree1, print_int);
  printf("\n  Elements in tree2: ");
  printtree(tree2, print_int);

  StrorIntType check_var = {.int_val = 10};
  printf("\n\n- Check if node whose value is 10 is in tree1 " 
	 "(1 is in, 0 is not in): %d",
	 member(tree1, check_var, equal_int, lss_int)); 
  check_var.int_val = 5;
  printf("\n- Check if node whose value is 5 is in tree1 "
         "(1 is in, 0 is not in): %d",
	 member(tree1, check_var, equal_int, lss_int)); 

  printf("\n\n- Insert more nodes to the tree2 and assign the"
         "\n  return value from insertion to tree2.\n");
  printf("  Elements in tree2 now: ");
  int_var.int_val = 2;
  tree2 = insert(tree2, int_var, equal_int, lss_int);
  int_var.int_val = 6;
  tree2 = insert(tree2, int_var, equal_int, lss_int);
  int_var.int_val = 10;
  tree2 = insert(tree2, int_var, equal_int, lss_int);
  int_var.int_val = 11;
  tree2 = insert(tree2, int_var, equal_int, lss_int);
  int_var.int_val = 15;
  tree2 = insert(tree2, int_var, equal_int, lss_int);
  int_var.int_val = 9;
  tree2 = insert(tree2, int_var, equal_int, lss_int);
  printtree(tree2, print_int);
  printf("\n");

  // Test for strings

  printf("\n* hw52 tests - node with string.\n");
  StrorIntType str_var = {.char_val = "str_test"};

  BST *str_tree1 = NULL;
  printf("- Insert a node whose value is str_test to the str_tree1"
         "\n  and assign the return value from insertion to str_tree1.\n");
  str_tree1 = insert(str_tree1, str_var, equal_str, lss_str);
  printf("  Elements in str_tree1: ");
  printtree(str_tree1, print_str);

  printf("\n\n- Insert a node whose value is str_test_longer to the str_tree1"
         "\n  and assign the return value from insertion to str_tree2.\n");
  str_var.char_val = "str_test_longer";
  BST *str_tree2 = insert(str_tree1, str_var, equal_str, lss_str);
  printf("  Elements in str_tree1 now: ");
  printtree(str_tree1, print_str);
  printf("\n  Elements in str_tree2: ");
  printtree(str_tree2, print_str);

  StrorIntType check_str_var = {.char_val = "str_test"};
  printf("\n\n- Check if node whose value is str_test is in str_tree1 " 
	 "(1 is in, 0 is not in): %d",
	 member(str_tree1, check_str_var, equal_str, lss_str)); 
  check_str_var.char_val = "str_test_longer";
  printf("\n- Check if node whose value is str_test_longer is in str_tree1 "
         "(1 is in, 0 is not in): %d",
	 member(str_tree1, check_str_var, equal_str, lss_str)); 

  printf("\n\n- Insert more nodes to the str_tree2 and assign the"
         "\n  return value from insertion to str_tree2.\n");
  printf("  Elements in str_tree2 now: ");
  str_var.char_val = "s1";
  str_tree2 = insert(str_tree2, str_var, equal_str, lss_str);
  str_var.char_val = "str_test";
  str_tree2 = insert(str_tree2, str_var, equal_str, lss_str);
  str_var.char_val = "str_tesz";
  str_tree2 = insert(str_tree2, str_var, equal_str, lss_str);
  str_var.char_val = "2s";
  str_tree2 = insert(str_tree2, str_var, equal_str, lss_str);
  str_var.char_val = "abc";
  str_tree2 = insert(str_tree2, str_var, equal_str, lss_str);
  str_var.char_val = "sqt";
  str_tree2 = insert(str_tree2, str_var, equal_str, lss_str);
  printtree(str_tree2, print_str);
  printf("\n\n");

  return 0;
}

/* 
 * Solution to Problem 2, part 3.
 * Function "member" to check if a value exists in the given BST.
 */
int member(BST *bst, StrorIntType value,
	   int (*equal)(StrorIntType, StrorIntType),
	   int (*lss)(StrorIntType, StrorIntType))
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
 * Solution to Problem 2, part 4.
 * Function "insert" to insert a new element into the given BST.
 */
BST* insert(BST *bst, StrorIntType value,
	       int (*equal)(StrorIntType, StrorIntType),
	       int (*lss)(StrorIntType, StrorIntType))
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
 * Solution to Problem 2, part 5.
 * Function "printtree" to print the given BST on the screen
 */
void printtree(BST *bst, void (*print_element)(StrorIntType)) {
  if (bst != NULL) {
    printtree(bst->left, print_element);
    print_element(bst->value);
    printf(" ");
    printtree(bst->right, print_element);
  }
}

int lss_int(StrorIntType lhs, StrorIntType rhs)
{
  return lhs.int_val < rhs.int_val;
}

int equal_int(StrorIntType lhs, StrorIntType rhs)
{
  return lhs.int_val == rhs.int_val;
}

int lss_str(StrorIntType lhs, StrorIntType rhs)
{
  return strcmp(lhs.char_val, rhs.char_val) < 0;
}

int equal_str(StrorIntType lhs, StrorIntType rhs)
{
  return strcmp(lhs.char_val, rhs.char_val) == 0;
}

void print_int(StrorIntType ele)
{
  printf("%d", ele.int_val);
}

void print_str(StrorIntType ele)
{
  printf("%s", ele.char_val);
}
