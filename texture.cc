#include "texture.hh"
#include <string>
#include <cstring>
#include <unistd.h>
#include <GL/gl.h>

Texture::Texture(const char*fname, int x1, int y1, int z1, int w1, int h1):w(w1), h(h1), x(x1), y(y1), z(z1){
	loadImage(fname);
    glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0,
		GL_RGB, GL_UNSIGNED_BYTE, data);
}

void Texture::Display_Picture(int xres, int yres){
    int width = xres/2;
	int height = yres/2;
    glPushMatrix();
    glColor3f(1.0,1.0,1.0);
    glBindTexture(GL_TEXTURE_2D, id);
    glBegin(GL_QUADS);
        glTexCoord2f(0, 0);
        glVertex2i(-width,-height); 
        glTexCoord2f(0, 1);
        glVertex2i(-width,height); 
        glTexCoord2f(1, 1);
        glVertex2i(width, height);      
        glTexCoord2f(1,0);
        glVertex2i(width,-height);
    glEnd();
    glPopMatrix();
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_ALPHA_TEST);
}

void Texture::loadImage(const char * fname){
    if (fname[0] == '\0')
        return;

    int ppmFlag = 0;

    char name[40];

    strcpy(name, fname);

    int slen = strlen(name);

    char ppmname[80];

    if (strncmp(name+(slen-4), ".ppm", 4) == 0)

        ppmFlag = 1;

    if (ppmFlag) {

        strcpy(ppmname, name);

    } else {

        name[slen-4] = '\0';

        sprintf(ppmname,"%s.ppm", name);

        char ts[100];

        sprintf(ts, "convert %s %s", fname, ppmname);

        system(ts);

    }

    FILE *fpi = fopen(ppmname, "r");

    if (fpi) {

        char line[200];

        fgets(line, 200, fpi);

        fgets(line, 200, fpi);

        while (line[0] == '#' || strlen(line) < 2)

            fgets(line, 200, fpi);

        sscanf(line, "%i %i", &w, &h);

        fgets(line, 200, fpi);

        int n = w * h * 3;
        data = new unsigned char[n];
        for (int i=0; i<n; i++)
            data[i] = fgetc(fpi);

        fclose(fpi);

    } else {

        printf("ERROR opening image: %s\n",ppmname);

        exit(0);

    }

    if (!ppmFlag)
        unlink(ppmname);

}
