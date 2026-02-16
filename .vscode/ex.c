#include <stdio.h>
#include <string.h>
#define CORPUS_SIZE 3
#define LONG_CHAR 1000
#define SHORT_CHAR 80
#define MAX 100
#define EASY 'E'
#define MEDIUM 'M'
#define HARD 'H'
typedef struct {
 char title[SHORT_CHAR];
 char category[SHORT_CHAR];
 char ingredients[LONG_CHAR];
 char challenge;
 int cooking_time;
} Recipe;
typedef struct {
 char title[SHORT_CHAR];
 int n_recipes;
 Recipe recipes[MAX];
} CookBook;
typedef CookBook Corpus [CORPUS_SIZE];



int main () {
 Corpus corpus;
 char option, buff, challenge;
 char category[SHORT_CHAR];
 char book_title[SHORT_CHAR];
 float cost = 0.0;
 int max_time = 0;
 fillCorpusData (corpus);

 printf ("Enter OPTION:\n");
 scanf("%c", &option);
 scanf("%c", &buff);
 if('a' == option){
 fgets(category, SHORT_CHAR, stdin);
 category[strlen(category)-1] = '\0';
 cost = computeCostCategory(corpus, category);
 printf("Total cost for %s is %.1f\n", category, cost);
 }
 else {
 fgets(book_title, SHORT_CHAR, stdin);
 book_title[strlen(book_title)-1] = '\0';
 scanf("%c", &challenge);
 scanf("%d", &max_time);
 filterByBook(corpus, book_title, challenge, max_time);
 }
 return (0);
}