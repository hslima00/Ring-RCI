
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
typedef struct
{
  char *key;
  char *IP;
  char *PORT;
  char *opt;
} command_s;

command_s string_to_command (char *buf, command_s command)
{

  char delim[] = " ";
  printf ("command_string= %s\n", buf);
  char *ptr = strtok (buf, delim);
  char *ip_copy;
  int arg_counter = 0;
  char *args[4];
  char *ret;
  while (ptr != NULL)
    {
      printf ("'%s'\n", ptr);
      args[arg_counter] = (char *) malloc (strlen (ptr + 1) * sizeof (char));
      strcpy (args[arg_counter], ptr);
      arg_counter++;
      ptr = strtok (NULL, delim);
    }

  for (int i = 0; i < arg_counter; i++)
    {
      printf ("arg[%d]=%s\n", i, args[i]);
    }
  memset (&command, " ", sizeof (command));

  command.opt = (char *) malloc (strlen (args[0] + 1) * sizeof (char));
  command.IP = (char *) malloc (strlen (args[2] + 1) * sizeof (char));
  command.PORT = (char *) malloc (strlen (args[3] + 1) * sizeof (char));
  command.key = (char *) malloc (strlen (args[1] + 1) * sizeof (char));

  strcpy (command.opt, args[0]);
  strcpy (command.IP, args[2]);
  strcpy (command.PORT, args[3]);
  strcpy (command.key, args[1]);

  return command;
}

int
main ()
{
  command_s command;

  char buf[] = "p 3 127.0.0.1 4";

  command = string_to_command (buf, command);
  printf ("HOLA: %s\n", command.IP);
}