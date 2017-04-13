/**
 * Copies a BMP piece by piece, just because.
 */
       
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "bmp.h"

int main(int argc, char *argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        fprintf(stderr, "Usage: ./resize n infile outfile\n");
        return 1;
    }

    int x=0,len;
    len=strlen(argv[1]);
    
    while(x<len)
    {
        if (!isdigit(argv[1][x]))
        {
            printf("n is an integer\n");
            return 5;
        }
        x++;
    }    
    
    int n=atoi(argv[1]);
    if(n<1 || n>100)
    {
        printf("n should be from 1 to 100\n");
        return 6;
    }
    
    // remember filenames
    char *infile = argv[2];
    char *outfile = argv[3];

    // open input file 
    FILE *inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", infile);
        return 2;
    }

    // open output file
    FILE *outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
    }

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 || 
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }

    // determine padding for scanlines
    int padding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;    
    int biHeight = abs(bi.biHeight);
    int biWidth = bi.biWidth;
    
    bi.biWidth=bi.biWidth*n;
    bi.biHeight=bi.biHeight*n;

    int padout = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    
    bi.biSizeImage = abs(bi.biHeight)*(bi.biWidth*sizeof(RGBTRIPLE) + padout);
    
    bf.bfSize = bi.biSizeImage + bf.bfOffBits;
    
    // write outfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    int line = biWidth*3 + padding;                     //width in bytes of input line
    
    // iterate over infile's scanlines
    for (int i = 0; i < biHeight; i++)
    {
        for (int l=0;l<n;l++)
        {
            // iterate over pixels in scanline
            for (int j = 0; j < biWidth; j++)
            {
            
                // temporary storage
                RGBTRIPLE triple;

                
                // read RGB triple from infile
                fread(&triple, sizeof(RGBTRIPLE), 1, inptr);

                for (int w = 0; w<n ; w++)
                {
                    // write RGB triple to outfile
                    fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
                }
                
            }

            // skip over padding, if any
            fseek(inptr, padding, SEEK_CUR);

            // then add it back (to demonstrate how)
            for (int k = 0; k < padout; k++)
            {
                fputc(0x00, outptr);
            }
            
            //go back to start of the line
            fseek(inptr, -line, SEEK_CUR);
        
        }
        
        //go to start of the next line
        fseek(inptr, line, SEEK_CUR);
        
    }

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // success
    return 0;
}
