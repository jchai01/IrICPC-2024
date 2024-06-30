#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main() {
  //  receive multiline input, stored in buf
  char buf[1024];
  int c;
  char *s = buf;
  while ((c = fgetc(stdin)) != EOF && s < buf + sizeof buf - 1) {
    if (c == '\n' && s > buf && s[-1] == '\n') {
      ungetc(c, stdin);
      break;
    }
    *s++ = c;
  }
  *s = '\0';

  int correct = 0;
  const char outer_delimiters[] = "\n";
  const char inner_delimiters[] = " ";

  char *outer_token;
  char *outer_saveptr = NULL;
  char *inner_saveptr = NULL;

  // get the first line of the input
  outer_token = strtok_r(buf, outer_delimiters, &outer_saveptr);
  int len = atoi(outer_token);

  // duplicate since len is used for loop, original length will be lost
  int len2 = atoi(outer_token);

  // loop from second line onwards
  outer_token = strtok_r(NULL, outer_delimiters, &outer_saveptr);

  while (outer_token != NULL) {
    int innerCount = 0;
    char* inner_token = strtok_r(outer_token, inner_delimiters, &inner_saveptr);

    while (inner_token != NULL && len > 0) {
      int num1 = 0;
      int num2 = 0;
      int ans = 0;
      char *op;

      // innerCount keeps track of the token, no check for 3 as '=' not impt
      while (inner_token != NULL) {
        if (innerCount == 0) {  // first number
          num1 = atoi(inner_token);
        } else if (innerCount == 1) {  // operator
          op = inner_token;
        } else if (innerCount == 2) {  // second number
          num2 = atoi(inner_token);
        } else if (innerCount == 4) {  // answer
          ans = atoi(inner_token);
        }
        inner_token = strtok_r(NULL, inner_delimiters, &inner_saveptr);
        innerCount++;
      }

      // evaluate
      if (strcmp(op, "+") == 0) {  // strcmp returns 0 if equal
        if (num1 + num2 == ans) {
          correct++;
        }
      } else if (strcmp(op, "-") == 0) {
        if (num1 - num2 == ans) {
          correct++;
        }
      } else if (strcmp(op, "*") == 0) {
        if (num1 * num2 == ans) {
          correct++;
        }
      } else if (strcmp(op, "//") == 0) {
        if (num1 / num2 == ans) {
          correct++;
        }
      }
    }
    len--;
    outer_token = strtok_r(NULL, outer_delimiters, &outer_saveptr);
  }

  float percentage = ((float)correct / (float)len2) * 100;
  // printf("correct answers: %d\n", correct);
  // printf("length = %d\n", len2);
  // printf("percentage=%f\n", percentage);

  if (percentage >= 70) {
    printf("A\n");
  } else if (percentage >= 60) {
    printf("B\n");
  } else if (percentage >= 50) {
    printf("C\n");
  } else if (percentage >= 40) {
    printf("D\n");
  } else {
    printf("E\n");
  }
  return 0;
}
