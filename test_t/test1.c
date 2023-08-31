#include<string.h>
#include<stdio.h>
int main(void)
{
FILE* stream;
char string[]="Thisisatest";
fpos_t filepos;
/*openafileforupdate*/
stream=fopen("DUMMY.FIL","w+");
/*writeastringintothefile*/
fwrite(string,strlen(string),1,stream);
/*reportthefilepointerposition*/
fgetpos(stream,&filepos);
printf("Thefilepointerisatbyte%ld\n",filepos.__pos);
fclose(stream);
return 0;
}
