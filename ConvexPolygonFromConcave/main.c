#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>
#include <string.h>

typedef struct PolygonNode {
    Vector2 value;
    struct PolygonNode *next, *prev;
} PolygonNode;

Vector2 GetVectorFromString(const char* str) {
    Vector2 vector = (Vector2){0.0f, 0.0f};
    int i;
    for(i = 0; ; i++) {
        if(str[i] == ' ') {
            i++;
            break;
        }
        else if(str[i] == '\0') {
            return (Vector2){-1.0f, -1.0f};
        }
        vector.x *= 10;
        vector.x += (float)(str[i] - '0');
    }
    for(; ; i++) {
        if(str[i] == ' ' || str[i] == '\0') {
            break;
        }
        vector.y *= 10;
        vector.y += (float)(str[i] - '0');
    }
    return vector;
}

void DeletePolygon(PolygonNode** polygon, size_t* polygonSize) {
    if(*polygonSize > 0 && *polygon != NULL) {
        for(PolygonNode* i = (*polygon)->next; i != *polygon; ) {
            PolygonNode* tmp = i;
            i = i->next;
            free(tmp);
        }
        free(*polygon);
    }
    *polygonSize = 0;
    *polygon = NULL;
}

int main() {
    PolygonNode* polygon = NULL;
    size_t polygonSize = 0;

    char str[50]; // not as if somebody's gonna use the whole 50... and if they do, well good on you...
    str[0] = '\0';

    InitWindow(800, 600, "Create Convex Polygons from a Concave polygon");

    printf("Create Convex Polygons from a Concave polygon!\n");
    printf("Type in coordinates to place a vertex\n");
    printf("To place the vertex, press Enter\n");
    printf("To delete the newest vertex, press B\n");
    printf("To delete the whole polygon, press R\n");

    while(!WindowShouldClose()) {

        char c = (char)GetCharPressed();
        while(c && strlen(str) != 49) {
            if('0' <= c && c <= '9' || c == ' ') {
                size_t strLength = strlen(str);
                str[strLength + 1] = '\0';
                str[strLength] = c;
            }
            c = (char)GetCharPressed();
        }
        if(IsKeyPressed(KEY_BACKSPACE)) {
            str[strlen(str) - 1] = '\0';
        }
        if(IsKeyPressed(KEY_ENTER)) {
            if(GetVectorFromString(str).x != -1.0f) {
                if (polygonSize == 0) {
                    polygon = (PolygonNode*)malloc(sizeof(PolygonNode));
                    polygon->next = polygon;
                    polygon->prev = polygon;
                    polygon->value = GetVectorFromString(str);
                } else if (polygon != NULL) {
                    polygon->next->prev = (PolygonNode*)malloc(sizeof(PolygonNode));
                    polygon->next->prev->next = polygon->next;
                    polygon->next->prev->prev = polygon;
                    polygon->next = polygon->next->prev;
                    polygon->next->value = GetVectorFromString(str);
                    polygon = polygon->next;
                }
                polygonSize++;
            }
            str[0] = '\0';
        }
        if(IsKeyPressed(KEY_B) && polygonSize > 0 && polygon != NULL) {
            PolygonNode* tmp = polygon;
            polygon->prev->next = polygon->next;
            polygon->next->prev = polygon->prev;
            polygon = polygon->prev;
            free(tmp);
            polygonSize--;
        }
        if(IsKeyPressed(KEY_R)) {
            DeletePolygon(&polygon, &polygonSize);
        }

        BeginDrawing();

            ClearBackground(BLACK);
            DrawText("To delete the newest vertex press B", 0, 0, 20, WHITE);
            DrawText("To delete the whole polygon press R", 0, 20, 20, WHITE);
            DrawText("To place the vertex press Enter", 0, 60, 20, WHITE);
            DrawText("Enter vertex coordinates: ", 0, 60, 20, WHITE);
            DrawText(str, MeasureText("Enter vertex coordinates: ", 20), 60, 20, WHITE);

            if(polygon != NULL && polygonSize > 0) {
                DrawCircleV(polygon->value, 5.0f, WHITE);
                DrawLineEx(polygon->value, polygon->next->value, 3.0f, (GetVectorFromString(str).x != -1.0f) ? RED : WHITE);
                for (PolygonNode *i = polygon->next; i != polygon; i = i->next) {
                    DrawCircleV(i->value, 5.0f, WHITE);
                    DrawLineEx(i->value, i->next->value, 3.0f, WHITE);
                }
            }

            if(GetVectorFromString(str).x != -1.0f) {
                DrawCircleV(GetVectorFromString(str), 5.0f, WHITE);
                if(polygon != NULL) {
                    DrawLineEx(polygon->value, GetVectorFromString(str), 3.0f, WHITE);
                    DrawLineEx(polygon->next->value, GetVectorFromString(str), 3.0f, WHITE);
                }
            }

        EndDrawing();
    }

    DeletePolygon(&polygon, &polygonSize);

    CloseWindow();
    return 0;
}
