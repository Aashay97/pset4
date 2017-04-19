/*recovers jpegs
from a memory card*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef uint8_t BYTE;

int main(int argc,char *argv[])
{
    //ensure proper usage
    if(argc!=2)
    {
        fprintf(stderr,"Usage: ./recover image\n");
        return 1;
    }
    
    FILE *inptr = fopen(argv[1],"r");
    if(inptr==NULL)
    {
        fprintf(stderr, "Could not open %s.\n", argv[1]);
        return 2;
    }
    
    int count=0,flag=0;
    
    BYTE buffer[512];
    
    char name[8];
    
    FILE *outptr;
    
    while(fread(buffer,512,1,inptr))
    {
        if(buffer[0]==0xff && buffer[1]==0xd8 && buffer[2]==0xff && (buffer[3] & 0xf0)==0xe0)
        {
            if(outptr!=NULL)
            {
                fclose(outptr);
            }
            
            sprintf(name,"%03i",count);
            count++;
            
            outptr=fopen(name,"w");
            
            fwrite(buffer,sizeof(buffer),1,outptr);
        }
    }
}