#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* 
 * Solution to Problem 3, part 1.
 * A type for representing binary search trees of random type of data
 */
typedef struct bst {
  void* value;
  struct bst *left;
  struct bst *right;
} BST;

// A structure for test
typedef struct name {
  char *first;
  char *last;
} NAME;

// Function signatures
int member(BST*, void*,
	   int (*)(void*, void*),
	   int (*)(void*, void*));
BST* insert(BST*, void*, 
               int (*)(void*, void*), 
               int (*lss)(void*, void*));
void printtree(BST*, void (*)(void*));

// Functions for integer comparison and printing
int lss_int(void*, void*);
int equal_int(void*, void*);
void print_int(void*);

// Functions for string comparison and printing
int lss_str(void*, void*);
int equal_str(void*, void*);
void print_str(void*);

// Functions for struct name comparison and printing
int lss_name(void*, void*);
int equal_name(void*, void*);
void print_name(void*);

int main(int argc, char *arglv[])
{
  // Test for integers
  printf("* hw53 tests - node with integer value.\n");
  BST *tree1 = NULL;
  int int_node1, int_node2, int_node3, int_node4,
      int_node5, int_node6, int_node7, int_node8;
    
  int_node1 = 10;
  printf("- Insert a node whose value is 10 to the tree1"
         "\n  and assign the return value from insertion to tree1.\n");
  tree1 = insert(tree1, (void*)&int_node1, equal_int, lss_int);
  printf("  Elements in tree1: ");
  printtree(tree1, print_int);

  printf("\n\n- Insert a node whose value is 5 to the tree1"
         "\n  and assign the return value from insertion to tree2.\n");
  int_node2 = 5;
  BST *tree2 = insert(tree1, (void*)&int_node2, equal_int, lss_int);
  printf("  Elements in tree1 now: ");
  printtree(tree1, print_int);
  printf("\n  Elements in tree2: ");
  printtree(tree2, print_int);

  int check_var = 10;
  printf("\n\n- Check if node whose value is 10 is in tree1 " 
	 "(1 is in, 0 is not in): %d",
	 member(tree1, (void*)&check_var, equal_int, lss_int)); 
  check_var = 5;
  printf("\n- Check if node whose value is 5 is in tree1 "
         "(1 is in, 0 is not in): %d",
	 member(tree1, (void*)&check_var, equal_int, lss_int)); 

  printf("\n\n- Insert more nodes to the tree2 and assign the"
         "\n  return value from insertion to tree2.\n");
  printf("  Elements in tree2 now: ");
  int_node3 = 2;
  tree2 = insert(tree2, (void*)&int_node3, equal_int, lss_int);
  int_node4 = 6;
  tree2 = insert(tree2, (void*)&int_node4, equal_int, lss_int);
  int_node5 = 10;
  tree2 = insert(tree2, (void*)&int_node5, equal_int, lss_int);
  int_node6 = 11;
  tree2 = insert(tree2, (void*)&int_node6, equal_int, lss_int);
  int_node7 = 15;
  tree2 = insert(tree2, (void*)&int_node7, equal_int, lss_int);
  int_node8 = 9;
  tree2 = insert(tree2, (void*)&int_node8, equal_int, lss_int);
  printtree(tree2, print_int);
  printf("\n");

  // Test for strings
  printf("\n* hw53 tests - node with string.\n");
  char *str_node1 = "str_test", *str_node2 = "str_test_longer",
       *str_node3 = "s1", *str_node4 = "str_test",
       *str_node5 = "str_tesz", *str_node6 = "2s",
       *str_node7 = "abc", *str_node8 = "sqt";

  BST *str_tree1 = NULL;
  printf("- Insert a node whose value is str_test to the str_tree1"
         "\n  and assign the return value from insertion to str_tree1.\n");
  str_tree1 = insert(str_tree1, (void*)str_node1, equal_str, lss_str);
  printf("  Elements in str_tree1: ");
  printtree(str_tree1, print_str);

  printf("\n\n- Insert a node whose value is str_test_longer to the str_tree1"
         "\n  and assign the return value from insertion to str_tree2.\n");
  BST *str_tree2 = insert(str_tree1, (void*)str_node2, equal_str, lss_str);
  printf("  Elements in str_tree1 now: ");
  printtree(str_tree1, print_str);
  printf("\n  Elements in str_tree2: ");
  printtree(str_tree2, print_str);

  char *check_str_var = "str_test";
  printf("\n\n- Check if node whose value is str_test is in str_tree1 " 
	 "(1 is in, 0 is not in): %d",
	 member(str_tree1, (void*)check_str_var, equal_str, lss_str)); 
  check_str_var = "str_test_longer";
  printf("\n- Check if node whose value is str_test_longer is in str_tree1 "
         "(1 is in, 0 is not in): %d",
	 member(str_tree1, (void*)check_str_var, equal_str, lss_str)); 

  printf("\n\n- Insert more nodes to the str_tree2 and assign the"
         "\n  return value from insertion to str_tree2.\n");
  printf("  Elements in str_tree2 now: ");
  str_tree2 = insert(str_tree2, (void*)str_node3, equal_str, lss_str);
  str_tree2 = insert(str_tree2, (void*)str_node4, equal_str, lss_str);
  str_tree2 = insert(str_tree2, (void*)str_node5, equal_str, lss_str);
  str_tree2 = insert(str_tree2, (void*)str_node6, equal_str, lss_str);
  str_tree2 = insert(str_tree2, (void*)str_node7, equal_str, lss_str);
  str_tree2 = insert(str_tree2, (void*)str_node8, equal_str, lss_str);
  printtree(str_tree2, print_str);
  printf("\n\n");

  // Test for struct name
  printf("\n* hw53 tests - node with NAME value.\n");
  NAME name_node1 = {.first = "Aaron", .last = "Martin"},
       name_node2 = {.first = "Bradley", .last = "Butler"},
       name_node3 = {.first = "Tom", .last = "Cook"},
       name_node4 = {.first = "Lucas", .last = "Martin"},
       name_node5 = {.first = "Aaron", .last = "Martin"},
       name_node6 = {.first = "Ella", .last = "Ryan"},
       name_node7 = {.first = "Eleanor", .last = "Stone"},
       name_node8 = {.first = "Jenny", .last = "Taylor"};

  BST *name_tree1 = NULL;
  printf("- Insert a node whose value is Aaron Martin to the name_tree1"
         "\n  and assign the return value from insertion to name_tree1.\n");
  name_tree1 = insert(name_tree1, (void*)&name_node1, equal_name, lss_name);
  printf("  Elements in name_tree1: ");
  printtree(name_tree1, print_name);

  printf("\n\n- Insert a node whose value is Bradley Butler to the name_tree1"
         "\n  and assign the return value from insertion to name_tree2.\n");
  BST *name_tree2 = insert(name_tree1, (void*)&name_node2, equal_name, lss_name);
  printf("  Elements in name_tree1 now: ");
  printtree(name_tree1, print_name);
  printf("\n  Elements in name_tree2: ");
  printtree(name_tree2, print_name);

  NAME check_name_var = {.first = "Aaron", .last = "Martin"};
  printf("\n\n- Check if node whose value is Aaron Martin is in name_tree1 " 
	 "(1 is in, 0 is not in): %d",
	 member(name_tree1, (void*)&check_name_var, equal_name, lss_name)); 
  check_name_var.first = "Bradley";
  check_name_var.last = "Bulter";
  printf("\n- Check if node whose value is Bradley Bulter is in name_tree1 "
         "(1 is in, 0 is not in): %d",
	 member(name_tree1, (void*)&check_name_var, equal_name, lss_name)); 

  printf("\n\n- Insert more nodes to the name_tree2 and assign the"
         "\n  return value from insertion to name_tree2.\n");
  printf("  Elements in name_tree2 now: ");
  name_tree2 = insert(name_tree2, (void*)&name_node3, equal_name, lss_name);
  name_tree2 = insert(name_tree2, (void*)&name_node4, equal_name, lss_name);
  name_tree2 = insert(name_tree2, (void*)&name_node5, equal_name, lss_name);
  name_tree2 = insert(name_tree2, (void*)&name_node6, equal_name, lss_name);
  name_tree2 = insert(name_tree2, (void*)&name_node7, equal_name, lss_name);
  name_tree2 = insert(name_tree2, (void*)&name_node8, equal_name, lss_name);
  printtree(name_tree2, print_name);
  printf("\n\n");
  return 0;
}

/* 
 * Solution to Problem 3, part 2.
 * Function "member" to check if a value exists in the given BST.
 */
int member(BST *bst, void* value,
	   int (*equal)(void*, void*),
	   int (*lss)(void*, void*))
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
 * Solution to Problem 3, part 3.
 * Function "insert" to insert a new element into the given BST.
 */
BST* insert(BST *bst, void* value,
	       int (*equal)(void*, void*),
	       int (*lss)(void*, void*))
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
 * Solution to Problem 3, part 4.
 * Function "printtree" to print the given BST on the screen
 */
void printtree(BST *bst, void (*print_element)(void*)) {
  if (bst != NULL) {
    printtree(bst->left, print_element);
    print_element(bst->value);
    printf(" ");
    printtree(bst->right, print_element);
  }
}

int lss_int(void* lhs, void* rhs)
{
  return *((int*)lhs) < *((int*)rhs);
}

int equal_int(void* lhs, void* rhs)
{
  return *((int*)lhs) == *((int*)rhs);
}

void print_int(void* ele)
{
  printf("%d", *((int*)(ele)));
}

int lss_str(void* lhs, void* rhs)
{
  return strcmp((char*)lhs, (char*)rhs) < 0;
}

int equal_str(void* lhs, void* rhs)
{
  return strcmp((char*)lhs, (char*)rhs) == 0;
}

void print_str(void* ele)
{
  printf("%s", (char*)ele);
}

int lss_name(void* lhs, void* rhs)
{
  int last_ordering = strcmp(((NAME*)lhs)->last, ((NAME*)rhs)->last);
  if (last_ordering < 0) {
    return 1;
  } else if (last_ordering == 0) {
    return strcmp(((NAME*)lhs)->first, ((NAME*)rhs)->first) < 0;
  } else {
    return 0;
  }
}

int equal_name(void* lhs, void* rhs)
{
  return strcmp(((NAME*)lhs)->first, ((NAME*)rhs)->first) == 0 &&
         strcmp(((NAME*)lhs)->last, ((NAME*)rhs)->last) == 0;
}

void print_name(void* ele)
{
  printf("%s %s.", ((NAME*)ele)->first, ((NAME*)ele)->last);
}
