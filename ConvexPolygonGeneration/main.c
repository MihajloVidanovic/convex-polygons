#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tgmath.h>
#include <time.h>
#include "raylib.h"

typedef struct Vector2d {
    double x, y;
} Vector2d;

Vector2d AddVectors(const Vector2d a, const Vector2d b) {
    return (Vector2d){a.x + b.x, a.y + b.y};
}

double VectorAngle(Vector2d vec) {
    return ((atan2(vec.y, vec.x) < 0.0) ? atan2(vec.y, vec.x) + 2 * PI : atan2(vec.y, vec.x));
}

int ComparisonByX(const void * a, const void * b)
{
    const Vector2d aValue = *(Vector2d*)a, bValue = *(Vector2d*)b;
    if (aValue.x > bValue.x) return 1;
    return -1;
} // Code taken from StackOverflow, thanks rerun

int ComparisonByY(const void * a, const void * b)
{
    const Vector2d aValue = *(Vector2d*)a, bValue = *(Vector2d*)b;
    if (aValue.y > bValue.y) return 1;
    return -1;
}

int ComparisonByAngle(const void * a, const void * b) {
    const Vector2d aValue = *(Vector2d*)a;
    const Vector2d bValue = *(Vector2d*)b;
    if (VectorAngle(aValue) < VectorAngle(bValue)) {
        return 1;
    }
    return -1;
}

int GetIntegerFromString(const char* str) {
    if (str == NULL) return 0;
    int returnValue = 0;
    for (int i = 0; i < strlen(str); i++) {
        returnValue *= 10;
        returnValue += str[i] - '0';
    }
    return returnValue;
}

void GeneratePolygon(Vector2d** polygon, int* polygonSize, int requestedSize, Rectangle bounds) {
    /* The algorithm used in this code is outlined in a StackOverflow post,
     * https://stackoverflow.com/questions/6758083/how-to-generate-a-random-convex-polygon
     * the algorithm goes as follows: */

    Vector2d vertices[requestedSize];
    Vector2d xvectors1[requestedSize], xvectors2[requestedSize];
    Vector2d yvectors1[requestedSize], yvectors2[requestedSize];
    Vector2d vectors[requestedSize];
    double minX, maxX;
    double minY, maxY;
    Vector2d *x1, *x2, *y1, *y2;

    for (int i = 0; i < requestedSize; i++) {
        vertices[i].x = rand() % bounds.width;
        vertices[i].y = rand() % bounds.height;
    }

    // x section
    qsort(&vertices[0], requestedSize, sizeof(Vector2d), ComparisonByX);
    minX = vertices[0].x;
    maxX = vertices[requestedSize - 1].x;
    xvectors1[0] = (Vector2d){minX, 0.0}, xvectors2[0] = (Vector2d){minX, 0.0};
    x1 = &xvectors1[1], x2 = &xvectors2[1];
    for (int i = 1; i < requestedSize - 1; i++) {
        if (rand() % 2 == 1) {
            *x1++ = (Vector2d){vertices[i].x, 0.0};
        }
        else {
            *x2++ = (Vector2d){vertices[i].x, 0.0};
        }
    }
    *x1 = (Vector2d){maxX, 0.0}, *x2 = (Vector2d){maxX, 0.0};
    for (Vector2d* i = &xvectors1[0]; i < x1; i++) {
        *i = (Vector2d){(i + 1)->x - i->x, 0.0};
    }
    for (Vector2d* i = &xvectors2[0]; i < x2; i++) {
        *i = (Vector2d){i->x - (i + 1)->x, 0.0};
    }

    // y section
    qsort(&vertices[0], requestedSize, sizeof(Vector2d), ComparisonByY);
    minY = vertices[0].y, maxY = vertices[requestedSize - 1].y;
    yvectors1[0] = (Vector2d){0.0, minY}, yvectors2[0] = (Vector2d){0.0, minY};
    y1 = &yvectors1[1], y2 = &yvectors2[1];
    for (int i = 1; i < requestedSize - 1; i++) {
        if (rand() % 2 == 1) {
            *y1++ = (Vector2d){0.0, vertices[i].y};
        }
        else {
            *y2++ = (Vector2d){0.0, vertices[i].y};
        }
    }
    *y1 = (Vector2d){0.0, maxY}, *y2 = (Vector2d){0.0, maxY};
    for (Vector2d* i = &yvectors1[0]; i < y1; i++) {
        *i = (Vector2d){0.0, (i + 1)->y - i->y};
    }
    for (Vector2d* i = &yvectors2[0]; i < y2; i++) {
        *i = (Vector2d){0.0, i->y - (i + 1)->y};
    }

    // combine
    Vector2d *cx1 = &xvectors1[0], *cx2 = &xvectors2[0], *cv = &vectors[0];
    // combine x1 x2 with y1
    for(Vector2d *i = &yvectors1[0]; i < y1; i++) {
        if(cx1 == x1) {
            *cv = AddVectors(*cx2++, *i);
            continue;
        }
        if(cx2 == x2) {
            *cv = AddVectors(*cx1++, *i);
            continue;
        }
        if(rand() % 2 == 1) {
            *cv = AddVectors(*cx1++, *i);
        }
        else {
            *cv = AddVectors(*cx2++, *i);
        }
    }
    // combine x1 x2 with y2
    for(Vector2d *i = &yvectors2[0]; i < y2; i++) {
        if(cx1 == x1) {
            *cv = AddVectors(*cx2++, *i);
            continue;
        }
        if(cx2 == x2) {
            *cv = AddVectors(*cx1++, *i);
            continue;
        }
        if(rand() % 2 == 1) {
            *cv = AddVectors(*cx1++, *i);
        }
        else {
            *cv = AddVectors(*cx2++, *i);
        }
    }
    
    // sort
    qsort(&vectors[0], requestedSize, sizeof(Vector2d), ComparisonByAngle);
    
    // memory allocation
    if(*polygonSize > 0) {
        free(*polygon);
    }
    *polygon = malloc(sizeof(Vector2d) * requestedSize);
    
    // make the polygon
    
}

int main() {

    InitWindow(800, 600, "Generate a convex polygon");

    char* str = calloc(35, sizeof(char));
    str[34] = '\0';

    Vector2d* polygon = NULL;
    int polygonSize = 0;

    srand(time(0));

    while (!WindowShouldClose()) {

        char c = GetCharPressed();
        if (c >= '0' && c <= '9') {
            str[strlen(str)] = c;
            str[strlen(str) + 1] = '\0';
        }
        if (IsKeyPressed(KEY_BACKSPACE) && strlen(str) > 0) {
            str[strlen(str)-1] = 0;
        }
        if (IsKeyPressed(KEY_R) && strlen(str) > 0) {
            free(str);
            str = calloc(35, sizeof(char));
        }
        if (IsKeyPressed(KEY_ENTER)) {
            // make the polygon
            if (polygon != NULL)
                free(polygon);
            GeneratePolygon(&polygon, &polygonSize, GetIntegerFromString(str), (Rectangle){0.0f, 0.0f, 800.0f, 600.0f});
        }

        BeginDrawing();

            ClearBackground(BLACK);

            DrawText("Type the polygon size and press Enter: ", 0, 0, 20, WHITE);
            DrawText(str, MeasureText("Type the polygon size and press enter: ", 20), 0, 20, WHITE);
            DrawText("Press Backspace to delete the last character", 0, 20, 20, WHITE);
            DrawText("Press R to reset the polygon size",0, 40, 20, WHITE);

            if (polygon != NULL) {
                for (int i = 0; i < polygonSize - 1; i++) {
                    DrawCircle((int)polygon[i].x, (int)polygon[i].y, 5.0f, WHITE);
                    DrawLineEx((Vector2){(float)polygon[i].x, (float)polygon[i].y}, (Vector2){(float)polygon[i + 1].x, (float)polygon[i + 1].y}, 3.0f, WHITE);
                }
                DrawCircle((int)polygon[polygonSize - 1].x, (int)polygon[polygonSize - 1].y, 5.0f, WHITE);
                DrawLineEx((Vector2){(float)polygon[polygonSize - 1].x, (float)polygon[polygonSize - 1].y}, (Vector2){(float)polygon[0].x, (float)polygon[0].y}, 3.0f, WHITE);
            }

        EndDrawing();
    }

    free(str);
    free(polygon);

    CloseWindow();
    return 0;
}