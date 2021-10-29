#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "err.h"

#define BUF_SIZE 1024

void ClearBuffer(char *buf) {

  for(int i = 0; i<=BUF_SIZE; i++){
    if(buf[i]=='\n'){
      buf[i]='\0';
      break;
    }
  }
}
int main(int argc, char *argv[])
{
  int read_dsc, buf_len;
  char buf[BUF_SIZE];

  if (argc != 2)
  {
    fatal("Usage: %s <read_fd>\n", argv[0]);
  }

  read_dsc = atoi(argv[0]);
  printf("Reading data from descriptor %d\n", read_dsc);

  if ((buf_len = read(read_dsc, buf, BUF_SIZE - 1)) == -1)
  {
    syserr("Error in read\n");
  }

  // When data is not null terminated or longer then BUF_SIZE - 1
  buf[buf_len < BUF_SIZE - 1 ? buf_len : BUF_SIZE - 1] = '\0';

  // Unexpected end-of-file
  if (buf_len == 0)
  {
    fatal("read: unexpected end-of-file\n");
  }
  // Successful read
  else
  {
    ClearBuffer(buf);
    printf("Read %d byte(s): \"%s\"\n", buf_len-1, buf);
  }
  exit(0);
}
