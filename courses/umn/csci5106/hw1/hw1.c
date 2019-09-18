/*
 * hw1.c
 *
 * Author: Yang He
 * Date: Sept. 15, 2019
 */

#include <stdio.h>

#define MAX_LENGTH 256

typedef struct {
  char content[MAX_LENGTH + 1];
  int len;
} mystr;

int mystr_new(mystr*, char*, const int);
int mystr_is_empty(const mystr*);
int mystr_compare(const mystr*, const mystr*);
int mystr_length(const mystr*);
int mystr_find(const mystr*, const char);
int mystr_at(const mystr*, const int, char*);
int mystr_concat(mystr*, const mystr*);
int mystr_slice(const mystr*, const int, const int, mystr*);
int mystr_copy(mystr*, const mystr*, const int);
void mystr_print(const mystr*);

int mystr_odd(const mystr*, mystr*);

int main(int argc, char* argv[])
{
  mystr empty_mystr = {}, one_char_mystr = {},
        odd_elements_mystr = {}, even_elements_mystr = {},
        odd_result_mystr = {};

  mystr_new(&empty_mystr, "", 0);
  mystr_new(&one_char_mystr, "a", 1);
  mystr_new(&odd_elements_mystr, "abcdefghijklmno", 15);
  mystr_new(&even_elements_mystr, "abcdefghijklmnop", 16);
  mystr_new(&odd_result_mystr, "", 0);

  printf("- Test 1, input sequence is an empty sequence");
  printf(", output sequence is: ");
  mystr_odd(&empty_mystr, &odd_result_mystr);
  mystr_print(&odd_result_mystr);
  
  printf("\n- Test 2, input sequence is ");
  mystr_print(&one_char_mystr);
  printf(", output sequence is: ");
  mystr_odd(&one_char_mystr, &odd_result_mystr);
  mystr_print(&odd_result_mystr);

  printf("\n- Test 3, input sequence is ");
  mystr_print(&odd_elements_mystr);
  printf(", output sequence is: ");
  mystr_odd(&odd_elements_mystr, &odd_result_mystr);
  mystr_print(&odd_result_mystr);

  printf("\n- Test 4, input sequence is ");
  mystr_print(&even_elements_mystr);
  printf(", output sequence is: ");
  mystr_odd(&even_elements_mystr, &odd_result_mystr);
  mystr_print(&odd_result_mystr);

  printf("\n");

  return 0;  
}

/*
 * Store a sequence of characters to a mystr type variable.
 *
 * Return length of content in mystr variable str.
 * Return negative value if len is an illegal value.
 */
int mystr_new(mystr* str, char* src, const int len) 
{
  if (len >= MAX_LENGTH || len < 0)
    return -1;

  for (int i = 0;i < len; ++i) 
    str->content[i] = src[i];
  str->content[len] = '\0';

  str->len = len;

  return str->len;
}

/*
 * Check if the characeter sequence stored in mystr variable is empty or not.
 *
 * Return 0 means the variable is empty now. 
 * Return positive values means the variable is not empty now. 
 */
int mystr_is_empty(const mystr* str)
{
  return str->len;
}

/*
 * Get length of character sequence stored in the mystr variable.
 *
 * Return length of the content.
 */
int mystr_length(const mystr* str)
{
  return str->len;
}

/*
 * Compare two mystr variables by comparing their content.
 * 
 * Return 0 if their contents are exactly same.
 * Return postive value if str1 is lexicographically larger than str2.
 * Return negative value if str1 is lexicographically less than str2.
 */

int mystr_compare(const mystr* str1, const mystr* str2)
{
  int len = (str1->len > str2->len ? str2->len : str1->len);
  for (int i = 0;i < len; ++i)
    if (str1->content[i] > str2->content[i])
      return 1;
    else if (str1->content[i] < str2->content[i])
      return -1;
  
  if (str1->len > len) return 1;
  if (str2->len > len) return -1;

  return 0;
}

/*
 * Locate character ch in the content of a mystr variable.
 *
 * Return location of the character.
 * Return negative value if cannot locate the character.
 */
int mystr_find(const mystr* str, const char ch)
{
  for (int i = 0; i < str->len; ++i)
    if (str->content[i] == ch)
      return i;

  return -1;
}

/*
 * Fetch the character whose index is pos in the content of str.
 * Target character will be stored into ch.
 *
 * Return 0 if the function works normally.
 * Return -1 if the pos is an illegal value.
 *
 */

int mystr_at(const mystr* str, const int pos, char* ch)
{
  if (pos < 0 || pos > str->len)
    return -1;
  else
    *ch = str->content[pos];

  return 0;
}

/*
 * Concatenate mystr src to mystr dst. No new mystr produced.
 *
 * Return 0 if the functions works normally.
 * Return -1 if no enough space in dst to contain src.
 */

int mystr_concat(mystr* dst, const mystr* src)
{
  int new_len = dst->len + src->len;
  if (new_len > MAX_LENGTH)
    return -1;

  for (int i = dst->len; i < new_len; ++i)
    dst->content[i] = src->content[i - src->len];
  dst->content[new_len] = '\0';
  dst->len = new_len;

  return 0;
}

/*
 * Extract a section of content in the mystr variable str and 
 * store it into new_str without modifying the original one. 
 *
 * It starts extracting from the 'start' position and terminates 
 * one position right before 'end' position.
 *
 * If 'end' is larger than str->len, then treat it as str->len.
 * If 'start' is less than 0, then treat it as 0.
 *
 * Return length of new_str.
 * Return negative value if end-start is larger than str->len
 * or either start or end is illegal.
 */

int mystr_slice(
  const mystr* str, const int start, const int end, mystr* new_str
)
{
  if (start >= str->len || 
      end < 0 || 
      end - start > str->len || 
      start > end)
    return -1;

  int start_pos = (start < 0 ? 0 : start);
  int end_pos = (end > str->len ? str->len : end);

  for (int i = start_pos; i < end_pos; ++i)
    new_str->content[i - start_pos] = str->content[i];
  new_str->len = end_pos - start_pos;
  new_str->content[new_str->len] = '\0';

  return 0;
}

/*
 * Copy copy_len characters from src to dst.
 * This function will modify mystr dst and overwrite it.
 *
 * If copy_len is larger than the length of content in src,
 * then copy the entire chararcter sequence from src to dst.
 *
 * Return number of character copied if copy is successful.
 * Return negative value if copy is failed.
 */
int mystr_copy(mystr* dst, const mystr* src, const int copy_len)
{
  if (copy_len > MAX_LENGTH) 
    return -1;

  int actual_copy_len = (copy_len > src->len ? src->len : copy_len);

  for (int i = 0;i < actual_copy_len; ++i)
    dst->content[i] = src->content[i];
  dst->content[actual_copy_len] = '\0';
  dst->len = actual_copy_len;

  return actual_copy_len;
}

/*
 * Print content of mystr variable str.
 */
void mystr_print(const mystr* str)
{
  for (int i = 0; i < str->len; ++i) 
    printf("%c", str->content[i]);
}

/*
 * Extract characters located at the odd position (not index)
 * in the sequence and store them into mystr variable odd 
 * without modifying the original mystr variable str.
 *
 * Return length of content in mystr variable odd.
 */
int mystr_odd(const mystr* str, mystr* odd)
{
  int len = 0;

  for (int i = 1;i <= str->len; ++i) {
    if (i % 2 == 1) {
      odd->content[i / 2] = str->content[i - 1]; 
      ++ len;
    }
  }
  odd->content[len] = '\0';
  odd->len = len;

  return len;
}

