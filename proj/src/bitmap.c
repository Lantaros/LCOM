    #include "bitmap.h"
    #include <stdlib.h>
    #include <stdio.h>
    //#include "Graphics.h"
    #include "video_gr.h"
    #include "utilities.h"

    Bitmap* loadBitmap(const char* filename) {
        // allocating necessary size
        Bitmap* bmp = (Bitmap*) malloc(sizeof(Bitmap));

        // open filename in read binary mode
        FILE *filePtr;
        filePtr = fopen(filename, "rb");
        if (filePtr == NULL)
            return NULL;

        // read the bitmap file header
        BitmapFileHeader bitmapFileHeader;
        fread(&bitmapFileHeader, 2, 1, filePtr);

        // verify that this is a bmp file by check bitmap id
        if (bitmapFileHeader.type != 0x4D42) {
            fclose(filePtr);
            return NULL;
        }

        int rd;
        do {
            if ((rd = fread(&bitmapFileHeader.size, 4, 1, filePtr)) != 1)
                break;
            if ((rd = fread(&bitmapFileHeader.reserved, 4, 1, filePtr)) != 1)
                break;
            if ((rd = fread(&bitmapFileHeader.offset, 4, 1, filePtr)) != 1)
                break;
        } while (0);

        if (rd != 1) {
            fprintf(stderr, "Error reading file\n");
            exit(-1);
        }

        // read the bitmap info header
        BitmapInfoHeader bitmapInfoHeader;
        fread(&bitmapInfoHeader, sizeof(BitmapInfoHeader), 1, filePtr);

        // move file pointer to the begining of bitmap data
        fseek(filePtr, bitmapFileHeader.offset, SEEK_SET);

        // allocate enough memory for the bitmap image data
        unsigned char* bitmapImage = (unsigned char*) malloc(
                bitmapInfoHeader.imageSize);

        // verify memory allocation
        if (!bitmapImage) {
            free(bitmapImage);
            fclose(filePtr);
            return NULL;
        }

        // read in the bitmap image data
        fread(bitmapImage, bitmapInfoHeader.imageSize, 1, filePtr);

        // make sure bitmap image data was read
        if (bitmapImage == NULL) {
            fclose(filePtr);
            return NULL;
        }

        // close file and return bitmap image data
        fclose(filePtr);

        bmp->bitmapData = bitmapImage;
        bmp->bitmapInfoHeader = bitmapInfoHeader;

        return bmp;
    }


    void drawBitmap(char* video_buff, Bitmap* bmp, int x, int y, Alignment alignment) {
        if (bmp == NULL)
            return;

       unsigned int width = bmp->bitmapInfoHeader.width;
       unsigned int drawWidth = width;
        int height = bmp->bitmapInfoHeader.height;

        if (alignment == ALIGN_CENTER)
        {
            x -= width / 2;
            y -= height / 2;
        }
        else if (alignment == ALIGN_RIGHT)
            x -= width;

        if (x + width < 0 || x > getHorResolution() || y + height < 0 || y > getVerResolution())
            return;

        int xCorrection = 0;
        if (x < 0) {
            xCorrection = -x;
            drawWidth -= xCorrection;
            x = 0;

            if (drawWidth > getHorResolution())
                drawWidth = getHorResolution();
        } else if (x + drawWidth >= getHorResolution()) {
            drawWidth = getHorResolution() - x;
        }

        char* bufferStartPos;
       unsigned char* imgStartPos;

        int i;
        for (i = 0; i < height; i++) {
            int pos = y + height - 1 - i;

            if (pos < 0 || pos >= getVerResolution())
                continue;

            bufferStartPos = video_buff;
            bufferStartPos += x * 2 + pos * getHorResolution() * 2;

            imgStartPos = bmp->bitmapData + xCorrection * 2 + i * width * 2;
//            short * shotPtr = (short *) imgStartPos;
            //printf("shortPtr %x\n", shotPtr);
            //printf("A cor a ignorar é %x\n", *shotPtr);//((*imgStartPos) | ((*(imgStartPos + 1)) << 8))) ;

        unsigned char* pixel =0;
        for(pixel = imgStartPos; pixel <= (imgStartPos + drawWidth*2); pixel+=2){
                if((*(short*)pixel) != TRANSPARENT_PIXEL)
                {
//                    *bufferStartPos = (*z);
//                    *(bufferStartPos + 1) = (*(z+1));
                	  *(short*)bufferStartPos = (*(short*)pixel);
                }

                bufferStartPos +=2;
            }
            //memcpy(bufferStartPos, imgStartPos, drawWidth * 2);
        }
    }

    void deleteBitmap(Bitmap* bmp) {
        if (bmp == NULL)
            return;

        free(bmp->bitmapData);
        free(bmp);
    }

    void delete_bitmap_array(Bitmap** array)
    {
        unsigned int i;

        for(i = 0; i < sizeof(array)/sizeof(Bitmap*); i++)
        {
            deleteBitmap(array[i]);
        }
    }
