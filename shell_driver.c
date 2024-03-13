//Driver code
//4 functions excluding main

#include "commands.h"

#define MAX_SIZE 1000
#define READ_BUFFERSIZE 1024
#define TOK_BUFFERSIZE 64
#define TOK_DELIM " \t\r\n\a"

int total = 0;  //Arguments input(single term commands(ls, uname) count as 1(without operands). mkdir folder counts as 1 operand but two arguments or tokens)

// command line arguments used to pass arguments(actual input commands and their operands)
// args is the command string

int func_search(int argc, char **args){  //function to searching commands by matching input string
  int i;

  if (args[0] == NULL) {  //args[0] holds actual command oprator like ls, date, ln
    return 1;
  }
  for (i = 0; i < func_num_builtins(); i++) {
    if (strcmp(args[0], commands_array[i]) == 0) {  //command input is searched within available commands array in "commands.h" file
      return (*func_ptr[i])(argc,args);		    //function pointer used to point to respective function for the command and pass the arguments
    }
  }
  printf("No command %s found.\n", args[0]);
  return 1;
}

char *func_read_line(void){		//returns pointer to buffer
  int buffersize = READ_BUFFERSIZE;
  int pos = 0;
  char *buffer = malloc(sizeof(char) * buffersize);
  int c;

  if (!buffer) {
    fprintf(stderr, "func: allocation error\n");
    exit(EXIT_FAILURE);
  }

  while (1) {
    c = getchar();
    // If we hit EOF, replace with null character and return.
    if (c == EOF || c == '\n') {
      buffer[pos] = '\0';
      return buffer;
    } 
    else {
      buffer[pos] = c;
    }
    pos++;

    // If we have exceed the buffer limit, reallocate the buffer.
    if (pos >= buffersize) {
      buffersize += READ_BUFFERSIZE;
      buffer = realloc(buffer, buffersize);
      if (!buffer) {
        fprintf(stderr, "func: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }
  }
}

char **func_split_line(char *line){	//returns double pointer to tokens(2d array of characters in commands)
  int buffersize = TOK_BUFFERSIZE;
  int pos = 0;
  char **tokens = malloc(buffersize * sizeof(char*)); //stores seperated commands and operands
  char *token;
  total = 0;

  if (!tokens) {
    fprintf(stderr, "func: allocation error\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line, TOK_DELIM); //breaks string(line) into a series of tokens using the delimiter(one or more characters that separate text strings) delim.
  while (token != NULL) {
    tokens[pos] = token;
    pos++;

    if (pos >= buffersize) {
      buffersize += TOK_BUFFERSIZE;
      tokens = realloc(tokens, buffersize * sizeof(char*));
      if (!tokens) {
        fprintf(stderr, "func: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, TOK_DELIM);
  }
  tokens[pos] = NULL;
  total = pos;
  return tokens;
}

void func_loop(void){
  char *line;
  char **args;
  int status;
  char buffer[MAX_SIZE], buffer2[100];
  gethostname(buffer,MAX_SIZE + 1);
  char* user = getenv("USER");

  do {
    printf("\033[1;32m%s@%s\033[1;34m:%s\033[1;31m>\033[0;m ",user,buffer,getcwd(buffer2, 100));
    line = func_read_line();
    args = func_split_line(line);
    status = func_search(total, args);
    //printf("\n");
    free(line);
    free(args);
  } while (status);
}

int main(int argc, char **argv){
  func_loop();
  return EXIT_SUCCESS;
}
