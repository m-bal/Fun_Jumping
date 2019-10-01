#pragma once
#include <GL/glx.h>
class Texture{

    private:
        int w;
        int h;
        int x;
        int y;
        int z;
        GLuint id;
        unsigned char * data;
        public:
        Texture(const char*, int, int, int, int, int);
        void loadImage(const char *fname);        
        void Display_Picture(int, int);
};
