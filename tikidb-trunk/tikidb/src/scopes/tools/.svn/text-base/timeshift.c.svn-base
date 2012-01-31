/* to extract the log file of each node from the serialdump into
   its own file, run:

   for i in {20..49};
   do
   grep "\[$i\]" serialdump-file | cut -d ' ' -f 7- > "log-$i.txt";
   done

   to apply the timeshift program to all log files, run:

   for file in log-*;
   do
   ./timeshift $file $file.out;
   done
*/

#include <stdio.h>
#include <string.h>
#include <errno.h>

#define MAXLINE 300             /* maximum input line length */

int main(int argc, char *argv[])
{
  /* check number of arguments */
  if (argc != 3) {
    fprintf(stderr, "usage: timeshift input-file output-file\n");
    return -1;
  }

  /* open input file */
  FILE *infile;
  if ((infile = fopen(argv[1], "r")) == NULL) {
    perror("failed to open input file");
    return -1;
  }

  /* open output file */
  FILE *outfile;
  if ((outfile = fopen(argv[2], "w")) == NULL) {
    perror("failed to open output file");
    return -1;
  }

  char buffer[MAXLINE];         /* each input line goes in here */
  int start = 0;                /* set to non-zero when a global timestamp is found */
  long int offset;              /* offset between global time and local time */
  unsigned short node_id;       /* node id found in the log message */
  unsigned long local_time;     /* local time found in the log message */
  unsigned long global_time;    /* global time found in the log message */

  /* read a line */
  while (fgets(buffer, MAXLINE, infile) != NULL) {
    /* scan the input line */
    if (sscanf(buffer, " [%hu] %lu:", &node_id, &local_time) != 2) {
      fprintf(stderr, "unexpected input line format: %s\n", buffer);
      continue;
    }

    /* determine the message position */
    char *msg;
    if ((msg = strchr(buffer, ':')) == NULL) {
      fprintf(stderr, "no colon in input line: %s\n", buffer);
      continue;
    }

    /* skip colon and blank */
    msg += 2;

    /* examine the message */
    if (sscanf(msg, "time received from (%*u:%*u): %lu", &global_time) == 1) {
      /* calculate the time offset */
      offset = local_time - global_time;
      printf("calculated time offset: %ld ms\n", offset);
      start = 1;
    }

    /* check if a global timestamp was seen */
    if (start) {
      /* print message with global time */
      fprintf(outfile, "[%hu] %lu: %s", node_id, local_time - offset, msg);
    }
  }

  /* close input and output files */
  fclose(infile);
  fclose(outfile);

  return 0;
}
