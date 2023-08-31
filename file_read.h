#include <string.h>
#include <stdio.h>
#include <stdlib.h>

struct prt_f
{
    long int* prt;
    long int n;
}prt_f_words;
int file_read_lines(FILE *fn)
{
	//FILE *fn = fopen("cet4.txt","r");
	char c;
	long int count = 1;
	//计算行数
	prt_f_words.n = 0;
	
	while( (c = fgetc(fn)) != EOF)
	{
		prt_f_words.n += 1;
	    //printf("%8X\n",c);
		if(c == 10)
		    count += 1;
	}
	//文件行数和字符数
	printf("file_lines-count:%ld prt_f_words.n:%ld\n",count,prt_f_words.n);
	rewind(fn);
	prt_f_words.prt = (long int *)malloc(sizeof(long int) * (count + 1));
	prt_f_words.n = 1;
	prt_f_words.prt[0] = 0;
	while((c = fgetc(fn)) != EOF)
	{
	    if(c == 10)
	    {
			//printf("c:%d prt_f_words.n:%ld num:%ld\n",c,prt_f_words.n,ftell(fn));
			//printf("count:%ld,n:%ld\n",count,prt_f_words.n);
			prt_f_words.prt[prt_f_words.n] = ftell(fn);
			prt_f_words.n += 1;
	    }
	    else
	    {
			//printf("%8X\n",c);
	    }
	}
	if(ftell(fn) != prt_f_words.prt[prt_f_words.n - 1])
	{
		//printf("c:%d prt_f_words.n:%ld num:%ld\n",c,prt_f_words.n,ftell(fn));
		prt_f_words.prt[prt_f_words.n] = count;
	}
	//fclose(fn);
    
	return 0;
}

void file_read_free()
{
    free(prt_f_words.prt);
}








