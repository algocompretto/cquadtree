#include "quadtree.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h> /* OpenGL functions */
#endif

unsigned int first = 1;
char desenhaBorda = 1;

QuadNode *newNode(int x, int y, int width, int height)
{
    QuadNode *n = malloc(sizeof(QuadNode));
    n->x = x;
    n->y = y;
    n->width = width;
    n->height = height;
    n->NW = n->NE = n->SW = n->SE = NULL;
    n->color[0] = n->color[1] = n->color[2] = 0;
    n->id = first++;
    return n;
}

/**
 * Calculates the average color of a region in an image.
 *
 * @param node The quadtree node representing the region.
 * @param original_pic The original image.
 * @return 0 if successful.
 */
int calculate_average_color(QuadNode *node, Img *original_pic)
{
    RGBPixel(*pixels)[original_pic->width] = (RGBPixel(*)[original_pic->width])original_pic->img;
    unsigned int totalR = totalG = totalB = 0;

    size_t node_height = node->height;
    size_t node_width = node->width;
    size_t nodeY = node->y;
    size_t nodeX = node->x;
    for (size_t i = nodeY; i < node_height + nodeY; i++)
    {
        for (size_t j = nodeX; j < node_width + nodeX; j++)
        {
            RGBPixel *pixel = &pixels[i][j];
            totalR += pixel->r;
            totalG += pixel->g;
            totalB += pixel->b;
        }
    }
    unsigned int num_pixels = node_height * node_width;
    node->color[0] = totalR / num_pixels;
    node->color[1] = totalG / num_pixels;
    node->color[2] = totalB / num_pixels;
    return 0;
}

/**
 * Calculates the error of a region in an image.
 * @param avg_intensity The average intensity of the region.
 * @param node The quadtree node representing the region.
 * @param original_pic The original image.
 * @return The error value.
 */

int calculate_region_error(unsigned int avg_intensity, QuadNode *node, Img *original_pic)
{
    float error = sum = difference = 0.0;
    int size = node->width * node->height;
    RGBPixel(*pixels)[original_pic->width] = (RGBPixel(*)[original_pic->width])original_pic->img;
    for (int i = node->y; i < node->y + node->height; i++)
    {
        for (int j = node->x; j < node->x + node->width; j++)
        {
            RGBPixel *pixel = &pixels[i][j];
            difference = pow((int)(pixel->r - avg_intensity), 2);
            sum += difference;
        }
    }
    error = sqrt(sum / size);
    return error;
}
//Img *grayscale_img = convert_to_grayscale(original_pic);
    Img *convert_to_grayscale(Img *original_pic){
    Img *new_pic = malloc(sizeof(Img));
    new_pic->width = original_pic->width;
    new_pic->height = original_pic->height;
    size_t size = original_pic->width * original_pic->height;
    new_pic->img = malloc(size * sizeof(RGBPixel));
    RGBPixel *pixels = (RGBPixel *)original_pic->img;
    RGBPixel *new_pixels = (RGBPixel *)new_pic->img;
  
for (size_t i = 0; i < size; i++){
    unsigned int intensity = (unsigned int)(0.3 * pixels[i].r + 0.59 * pixels[i].g + 0.11 * pixels[i].b);
    new_pixels[i].r = intensity;
    new_pixels[i].g = intensity;
    new_pixels[i].b = intensity;
}
return new_pic;
}
/*
 * Calculates the histogram of a region in an image.
 * @param node The quadtree node representing the region.
 * @param original_pic The original image.
 * @param histogram An array to store the histogram.
 */
void calculate_histogram(QuadNode *node, Img *original_pic, long long *histogram)
{
    const int NUM_GRAY = 256;
    RGBPixel(*pixels)[original_pic->width] = (RGBPixel(*)[original_pic->height])original_pic->img;
    memset(histogram, 0, sizeof(long long) * NUM_GRAY);
    for (size_t i = node->y; i < node->y + node->height; i++)
    {
        for (size_t j = node->x; j < node->x + node->width; j++)
        {
            RGBPixel *pixel = &pixels[i][j];
            histogram[pixel->r]++;
        }
    }
}
/**
 * Calculates the average intensity of a histogram.
 *
 * @param histogram The histogram.
 * @param size The size of the histogram.
 * @return The average intensity value.
 */

unsigned int calculate_average_intensity(long long *histogram, int size)
{
    int sum = 0;
    for (size_t i = 0; i < 256; i++)
        sum += histogram[i] * i;
    sum /= size;
    return (unsigned int)sum;
}
QuadNode *geraQuadtree(Img *original_pic, float minError)
{
    return NULL;
}

// Limpa a memória ocupada pela árvore
void clearTree(QuadNode *n)
{
    if (n == NULL)
        return;
    if (n->status == PARCIAL)
    {
        clearTree(n->NE);
        clearTree(n->NW);
        clearTree(n->SE);
        clearTree(n->SW);
    }
    free(n);
}

// Ativa/desativa o desenho das bordas de cada região
void toggleBorder()
{
    desenhaBorda = !desenhaBorda;
    printf("Desenhando borda: %s\n", desenhaBorda ? "SIM" : "NÃO");
}

// Desenha toda a quadtree
void drawTree(QuadNode *raiz)
{
    if (raiz != NULL)
        drawNode(raiz);
}

// Grava a árvore no formato do Graphviz
void writeTree(QuadNode *raiz)
{
    FILE *fp = fopen("quad.dot", "w");
    fprintf(fp, "digraph quadtree {\n");
    if (raiz != NULL)
        writeNode(fp, raiz);
    fprintf(fp, "}\n");
    fclose(fp);
    printf("\nFim!\n");
}

void writeNode(FILE *fp, QuadNode *n)
{
    if (n == NULL)
        return;

    if (n->NE != NULL)
        fprintf(fp, "%d -> %d;\n", n->id, n->NE->id);
    if (n->NW != NULL)
        fprintf(fp, "%d -> %d;\n", n->id, n->NW->id);
    if (n->SE != NULL)
        fprintf(fp, "%d -> %d;\n", n->id, n->SE->id);
    if (n->SW != NULL)
        fprintf(fp, "%d -> %d;\n", n->id, n->SW->id);
    writeNode(fp, n->NE);
    writeNode(fp, n->NW);
    writeNode(fp, n->SE);
    writeNode(fp, n->SW);
}

// Desenha todos os nodos da quadtree, recursivamente
void drawNode(QuadNode *n)
{
    if (n == NULL)
        return;

    glLineWidth(0.1);

    if (n->status == CHEIO)
    {
        glBegin(GL_QUADS);
        glColor3ubv(n->color);
        glVertex2f(n->x, n->y);
        glVertex2f(n->x + n->width - 1, n->y);
        glVertex2f(n->x + n->width - 1, n->y + n->height - 1);
        glVertex2f(n->x, n->y + n->height - 1);
        glEnd();
    }

    else if (n->status == PARCIAL)
    {
        if (desenhaBorda)
        {
            glBegin(GL_LINE_LOOP);
            glColor3ubv(n->color);
            glVertex2f(n->x, n->y);
            glVertex2f(n->x + n->width - 1, n->y);
            glVertex2f(n->x + n->width - 1, n->y + n->height - 1);
            glVertex2f(n->x, n->y + n->height - 1);
            glEnd();
        }
        drawNode(n->NE);
        drawNode(n->NW);
        drawNode(n->SE);
        drawNode(n->SW);
    }
}