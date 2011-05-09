/* Open and close files for bison,
   Copyright (C) 1984, 1986, 1989 Free Software Foundation, Inc.

This file is part of Bison, the GNU Compiler Compiler.

Bison is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

Bison is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Bison; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */


#ifdef VMS
#include <ssdef.h>
#define unlink delete
#ifndef XPFILE
#define XPFILE "GNU_BISON:[000000]BISON.CC"
#endif
#ifndef XPFILE1
#define XPFILE1 "GNU_BISON:[000000]BISON.HAIRY"
#endif
#ifndef XHFILE
#define XHFILE "GNU_BISON:[000000]BISON.H"
#endif
#else
#ifndef XPFILE
#define XPFILE "bison.cc"
#endif
#ifndef XPFILE1
#define XPFILE1 "bison.hairy"
#endif
#ifndef XHFILE
#define XHFILE "bison.h"
#endif
#endif

#include <stdio.h>
#include "system.h"
#include "files.h"
#include "new.h"
#include "gram.h"

FILE *finput = NULL;
FILE *foutput = NULL;
FILE *fdefines = NULL;
FILE *ftable = NULL;
FILE *fattrs = NULL;
FILE *fguard = NULL;
FILE *faction = NULL;
FILE *fparser = NULL;

/* File name specified with -o for the output file, or 0 if no -o.  */
char *spec_outfile;

char *infile;
char *outfile;
char *defsfile;
char *tabfile;
char *attrsfile;
char *guardfile;
char *actfile;
char *tmpattrsfile;
char *tmptabfile;
char *tmpdefsfile;
/* AC added */
char *hskelfile=NULL;
char *cparserfile=NULL;
FILE *fhskel=NULL;
char *parser_name="parse"; 
int parser_defined=0;
int line_fparser=1;
int line_fhskel=1;
char *parser_fname="bison.cc";
char *hskel_fname="bison.h";
char *header_name=NULL;
/* AC added end*/



extern char     *mktemp();      /* So the compiler won't complain */
extern char     *getenv();
extern void     perror();
FILE    *tryopen();     /* This might be a good idea */
void done();

extern char *program_name;
extern int verboseflag;
extern int definesflag;
int fixed_outfiles = 0;

static char *c_suffixes[]=
   {".tab.c",".tab.cc",".tab.cpp",".tab.cxx",".tab.C",
    ".c",".cc",".cpp",".cxx",".C",".CPP",".CXX",".CC",(char *)0};



char*
stringappend(string1, end1, string2)
char *string1;
int end1;
char *string2;
{
  register char *ostring;
  register char *cp, *cp1;
  register int i;

  cp = string2;  i = 0;
  while (*cp++) i++;

  ostring = NEW2(i+end1+1, char);

  cp = ostring;
  cp1 = string1;
  for (i = 0; i < end1; i++)
    *cp++ = *cp1++;

  cp1 = string2;
  while (*cp++ = *cp1++) ;

  return ostring;
}


/* JF this has been hacked to death.  Nowaday it sets up the file names for
   the output files, and opens the tmp files and the parser */
void
openfiles()
{
  char *name_base;
  register char *cp;
  char *filename;
  int base_length;
  int short_base_length;

#ifdef VMS
  char *tmp_base = "sys$scratch:b_";
#else
  char *tmp_base = "/tmp/b.";
#endif
  int tmp_len;

#ifdef _MSDOS
  tmp_base = "";
  strlwr (infile);
#endif /* MSDOS */

  tmp_len = strlen (tmp_base);

  if (spec_outfile)
    {
      /* -o was specified.  The precise -o name will be used for ftable.
	 For other output files, remove the ".c" or ".tab.c" suffix.  */
      name_base = spec_outfile;
#ifdef _MSDOS
      strlwr (name_base);
#endif /* MSDOS */
      base_length = strlen (name_base);
      /* SHORT_BASE_LENGTH includes neither ".tab" nor ".c".  */
       {char **suffix;
	for(suffix=c_suffixes;*suffix;suffix++)
	/* try to detect .c .cpp .tab.c ... options */
	  {if(strlen(name_base)>strlen(*suffix) 
	      && strcmp(name_base+base_length-strlen(*suffix),*suffix)==0)
	    { base_length -= strlen(*suffix);
	     break;}
	  };
	}
       short_base_length=base_length;
    }
  else if (spec_file_prefix)
    {
      /* -b was specified.  Construct names from it.  */
      /* SHORT_BASE_LENGTH includes neither ".tab" nor ".c".  */
      short_base_length = strlen (spec_file_prefix);
      /* Count room for `.tab'.  */
      base_length = short_base_length + 4;
      name_base = (char *) xmalloc (base_length + 1);
      /* Append `.tab'.  */
      strcpy (name_base, spec_file_prefix);
#ifdef VMS
      strcat (name_base, "_tab");
#else
      strcat (name_base, ".tab");
#endif
#ifdef _MSDOS
      strlwr (name_base);
#endif /* MSDOS */
    }
  else
    {
      /* -o was not specified; compute output file name from input
	 or use y.tab.c, etc., if -y was specified.  */

      name_base = fixed_outfiles ? "y.y" : infile;

      /* BASE_LENGTH gets length of NAME_BASE, sans ".y" suffix if any.  */

      base_length = strlen (name_base);
      if (!strcmp (name_base + base_length - 2, ".y"))
	base_length -= 2;
      short_base_length = base_length;

#ifdef VMS
      name_base = stringappend(name_base, short_base_length, "_tab");
#else
#ifdef _MSDOS
      name_base = stringappend(name_base, short_base_length, "_tab");
#else
      name_base = stringappend(name_base, short_base_length, ".tab");
#endif /* not MSDOS */
#endif
      base_length = short_base_length + 4;
    }

  finput = tryopen(infile, "r");

  filename=cparserfile;
  if(filename==NULL)
    filename = getenv("BISON_SIMPLE");
#ifdef _MSDOS
  /* File doesn't exist in current directory; try in INIT directory.  */
  cp = getenv("INIT");
  if (filename == 0 && cp != NULL)
    {FILE *tst;
      filename = (char *)xmalloc(strlen(cp) + strlen(PFILE) + 2);
      strcpy(filename, PFILE);
      if((tst=fopen(filename,"r"))!=NULL)
       {fclose(tst);}
      else 
      {
      strcpy(filename, cp);
      cp = filename + strlen(filename);
      *cp++ = '/';
      strcpy(cp, PFILE);
      }
    }
#endif /* MSDOS */
  {char *p=filename ? filename : PFILE;
   
  parser_fname=(char *)xmalloc(strlen(p)+1);
  strcpy(parser_fname,p);
  }
  fparser = tryopen(parser_fname, "r");

  filename=hskelfile;
  if(filename==NULL)
    filename = getenv("BISON_SIMPLE_H");
#ifdef _MSDOS
  /* File doesn't exist in current directory; try in INIT directory.  */
  cp = getenv("INIT");
  if (filename == 0 && cp != NULL)
    {FILE *tst;
      filename = (char *)xmalloc(strlen(cp) + strlen(HFILE) + 2);
      strcpy(filename, HFILE);
      if((tst=fopen(filename,"r"))!=NULL)
       {fclose(tst);}
      else 
      {
      strcpy(filename, cp);
      cp = filename + strlen(filename);
      *cp++ = '/';
      strcpy(cp, HFILE);
      }

    }
#endif /* MSDOS */
  {char *p=filename ? filename : HFILE;
   
  hskel_fname=(char *)xmalloc(strlen(p)+1);
  strcpy(hskel_fname,p);
  }

  fhskel = tryopen(hskel_fname, "r");

  if (verboseflag)
    {
#ifdef _MSDOS
      outfile = stringappend(name_base, short_base_length, ".out");
#else
      if (spec_name_prefix)
	outfile = stringappend(name_base, short_base_length, ".out");
      else
	outfile = stringappend(name_base, short_base_length, ".output");
#endif
      foutput = tryopen(outfile, "w");
    }

#ifdef _MSDOS
  actfile = _tempnam(".","b_ac");
  tmpattrsfile = _tempnam(".","b_at");
  tmptabfile = _tempnam(".","b_ta");
  tmpdefsfile = _tempnam(".","b_de");
#else
  actfile = mktemp(stringappend(tmp_base, tmp_len, "act.XXXXXX"));
  tmpattrsfile = mktemp(stringappend(tmp_base, tmp_len, "attrs.XXXXXX"));
  tmptabfile = mktemp(stringappend(tmp_base, tmp_len, "tab.XXXXXX"));
  tmpdefsfile = mktemp(stringappend(tmp_base, tmp_len, "defs.XXXXXX"));
#endif /* not MSDOS */

  faction = tryopen(actfile, "w+");
  fattrs = tryopen(tmpattrsfile,"w+");
  ftable = tryopen(tmptabfile, "w+");

  if (definesflag)
    { if(header_name)
       defsfile=header_name;
      else 
       defsfile = stringappend(name_base, base_length, ".h");
      fdefines = tryopen(tmpdefsfile, "w+");
    }

#ifndef _MSDOS
  unlink(actfile);
  unlink(tmpattrsfile);
  unlink(tmptabfile);
  unlink(tmpdefsfile);
#endif

	/* These are opened by `done' or `open_extra_files', if at all */
  if (spec_outfile)
    tabfile = spec_outfile;
  else
    tabfile = stringappend(name_base, base_length, ".c");

#ifdef VMS
  attrsfile = stringappend(name_base, short_base_length, "_stype.h");
  guardfile = stringappend(name_base, short_base_length, "_guard.c");
#else
#ifdef _MSDOS
  attrsfile = stringappend(name_base, short_base_length, ".sth");
  guardfile = stringappend(name_base, short_base_length, ".guc");
#else
  attrsfile = stringappend(name_base, short_base_length, ".stype.h");
  guardfile = stringappend(name_base, short_base_length, ".guard.c");
#endif /* not MSDOS */
#endif /* not VMS */
}



/* open the output files needed only for the semantic parser.
This is done when %semantic_parser is seen in the declarations section.  */

void
open_extra_files()
{
  FILE *ftmp;
  int c;
  char *filename, *cp;

  fclose(fparser);
  filename=cparserfile;
  if(filename!=NULL)
    filename = (char *) getenv ("BISON_HAIRY");
#ifdef _MSDOS
  /* File doesn't exist in current directory; try in INIT directory.  */
  cp = getenv("INIT");
  if (filename == 0 && cp != NULL)
    {FILE *tst;
      filename = (char *)xmalloc(strlen(cp) + strlen(PFILE1) + 2);
      strcpy(filename, PFILE1);
      if((tst=fopen(filename,"r"))!=NULL)
       {fclose(tst);}
      else 
      {
      strcpy(filename, cp);
      cp = filename + strlen(filename);
      *cp++ = '/';
      strcpy(cp, PFILE1);
      }

    }
#endif /* MSDOS */
  {char *p=filename ? filename : PFILE1;
   
  parser_fname=(char *)xmalloc(strlen(p)+1);
  strcpy(parser_fname,p);
  }
  fparser = tryopen(parser_fname, "r");


		/* JF change from inline attrs file to separate one */
  ftmp = tryopen(attrsfile, "w");
  rewind(fattrs);
  while((c=getc(fattrs))!=EOF)  /* Thank god for buffering */
    putc(c,ftmp);
  fclose(fattrs);
  fattrs=ftmp;

  fguard = tryopen(guardfile, "w");

}

	/* JF to make file opening easier.  This func tries to open file
	   NAME with mode MODE, and prints an error message if it fails. */
FILE *
tryopen(name, mode)
char *name;
char *mode;
{
  FILE  *ptr;

  ptr = fopen(name, mode);
  if (ptr == NULL)
    {
      fprintf(stderr, "%s: ", program_name);
      perror(name);
      done(2);
    }
  return ptr;
}

void
done(k)
int k;
{
  if (faction)
    fclose(faction);

  if (fattrs)
    fclose(fattrs);

  if (fguard)
    fclose(fguard);

  if (finput)
    fclose(finput);

  if (fparser)
    fclose(fparser);

  if (foutput)
    fclose(foutput);

	/* JF write out the output file */
  if (k == 0 && ftable)
    {
      FILE *ftmp;
      register int c;
      int lftmp=1;
      char *pattern="\n#line @";
      char *pospattern=pattern;
      ftmp=tryopen(tabfile, "w");
/* avoid reloading the definitions of tab.h */
      
      fprintf(ftmp,"#define YY_%s_h_included\n",parser_name);
      lftmp++;
      rewind(ftable);
      while((c=getc(ftable)) != EOF)
	{if(c=='\n') lftmp++;
	 if(c== *pospattern++ && c)
          {if(*pospattern==0)
            {fprintf(ftmp,"%d \"%s\"",lftmp+1, quoted_filename(tabfile));
             pospattern=pattern;continue;}
          }
         else pospattern=pattern;
         putc(c,ftmp);
        }
      fclose(ftmp);
      fclose(ftable);

      if (definesflag)
	{ lftmp=1;
	  ftmp = tryopen(defsfile, "w");
	  fprintf(ftmp,"#ifndef YY_%s_h_included\n",parser_name);
	  fprintf(ftmp,"#define YY_%s_h_included\n",parser_name);
          lftmp+=2;
	  fflush(fdefines);
	  rewind(fdefines);
	  while((c=getc(fdefines)) != EOF)
		{if(c=='\n') lftmp++;
		 if(c==*pospattern++ && c)
		  {if(*pospattern==0)
		    {fprintf(ftmp,"%d \"%s\"",lftmp+1
                                             ,quoted_filename(defsfile));
                     pospattern=pattern;continue;}
		  }
		 else pospattern=pattern;
		 putc(c,ftmp);
		}
	  fclose(fdefines);
	  fprintf(ftmp,"#endif\n");
          lftmp++;
	  fclose(ftmp);
	}
    }

#ifdef VMS
  if (faction)
    delete(actfile);
  if (fattrs)
    delete(tmpattrsfile);
  if (ftable)
    delete(tmptabfile);
  if (fdefines)
    delete(tmpdefsfile);
  if (k==0) sys$exit(SS$_NORMAL);
  sys$exit(SS$_ABORT);
#else
#ifdef _MSDOS
  if (actfile) unlink(actfile);
  if (tmpattrsfile) unlink(tmpattrsfile);
  if (tmptabfile) unlink(tmptabfile);
  if (tmpdefsfile) unlink(tmpdefsfile);
#endif /* MSDOS */
  exit(k);
#endif /* not VMS */
}
