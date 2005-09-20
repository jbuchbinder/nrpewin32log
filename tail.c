/*
 * $Id$
 * $Author$
 *
 * Tail function
 * Modified heavily from logtail.c, written by
 * Craig H. Rowland <crowland@psionic.com>
 *
 */

#include <stdio.h>
#include <string.h>

int tail (char *logfile, char *offsetfile)
{
  FILE *logfh,*offsetfh;
  int offs;
  int previous;
  char buf[256];

  offsetfh=fopen(offsetfile,"r");
  if(!offsetfh)
    {
      offs=0;
    }
  else
    {
      fgets(buf,200,offsetfh);
/*       buf[i+1]=0; */
/*       printf("%d:%s:\n",i,buf); */
      offs=atoi(buf);
      fclose(offsetfh);
    }
  
/*   printf("offset=%d\n",offs); */
  previous = offs;

  logfh=fopen(logfile,"r");
  if(!logfh)
    {
      printf("Can't open %s for reading\n",logfile);
      exit(-1);
    }

  if(offs)
    {
      if(fseek(logfh,offs,SEEK_SET))
	{
	  /* logrotate probably gave us a new file */
	  rewind(logfh);
	}
    }

  while(fgets(buf,256,logfh))
    {
      /* printf("%s",buf); */
    }
  offs=ftell(logfh);
  fclose(logfh);

  offsetfh=fopen(offsetfile,"w+");
  if(!offsetfh)
    {
      printf("Can't open %s for writing\n",offsetfile);
      exit(-1);
    }
  else
    {
      fprintf(offsetfh,"%d",offs);
      fclose(offsetfh);
    }
  return previous;
}

/*
 * char *make_offset_name (char *)
 *
 * Derive offset file name from original file name
 *
 */
char *make_offset_name (char *original) {
  char *output;

  if (strlen(original) == 0) {
    printf("make_offset_name() was passed a null file name\n");
    exit(-1);
  }

  output = (char *)calloc(strlen(original) + 8, sizeof(char *));
  strcpy(output, original);
  strcat(output, ".offset");
  return output;
}

