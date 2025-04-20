#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>
#include <string.h>

typedef struct PolygonNode {
    Vector2 value;
    struct PolygonNode *next, *prev;
} PolygonNode;

typedef struct Triangle {
    Vector2 v1, v2, v3;
} Triangle;

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

void AddVertexToPolygon(PolygonNode** polygon, size_t* polygonSize, const Vector2 vertex) {
    if (*polygonSize == 0) {
        *polygon = (PolygonNode*)malloc(sizeof(PolygonNode));
        (*polygon)->next = *polygon;
        (*polygon)->prev = *polygon;
        (*polygon)->value = vertex;
    } else if (*polygon != NULL) {
        (*polygon)->next->prev = (PolygonNode*)malloc(sizeof(PolygonNode));
        (*polygon)->next->prev->next = (*polygon)->next;
        (*polygon)->next->prev->prev = *polygon;
        (*polygon)->next = (*polygon)->next->prev;
        (*polygon)->next->value = vertex;
        *polygon = (*polygon)->next;
    }
    (*polygonSize)++;
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

void MakeTrianglesFromConcave(const PolygonNode* polygon, const size_t polygonSize, Triangle** returnTriangles, size_t* trianglesSize) {
    *returnTriangles = (Triangle*)malloc(sizeof(Triangle) * polygonSize);
    PolygonNode *a = polygon, *b = polygon->prev;
    for(int i = 0; b != a->next; i++, a = a->next) {
        (*returnTriangles)[i] = (Triangle){a->value, a->next->value, b->value};
        (*trianglesSize)++;
    }
}

void MakeConvexPolygonsFromConcave(PolygonNode* polygon, size_t polygonSize, PolygonNode** returnPolygons) {
    // d = (x - x1)(y2 - y1) - (y - y1)(x2 - x1)
    // x, y = vertex
    // x1, y1 -> x2, y2 = edge
    // https://math.stackexchange.com/questions/274712/calculate-on-which-side-of-a-straight-line-is-a-given-point-located
    // big thanks
    
}

int main() {
    PolygonNode* polygon = NULL;
    size_t polygonSize = 0;

    char str[50]; // not as if somebody's gonna use the whole 50... and if they do, well good on you...
    str[0] = '\0';

    Triangle* triangles = NULL;
    size_t trianglesSize = 0;

    Color colorArray[21] = {DARKGRAY, GRAY, LIGHTGRAY, MAROON, RED, PINK, ORANGE, GOLD, YELLOW, DARKGREEN, LIME, GREEN,
                            DARKBLUE, BLUE, SKYBLUE, DARKPURPLE, VIOLET, PURPLE, DARKBROWN, BROWN, BEIGE};

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
                AddVertexToPolygon(&polygon, &polygonSize, GetVectorFromString(str));
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
        if(IsKeyPressed(KEY_T)) {
            MakeTrianglesFromConcave(polygon, polygonSize, &triangles, &trianglesSize);
        }

        BeginDrawing();

            ClearBackground(BLACK);

            if(trianglesSize > 0 && triangles != NULL) {
                for(int i = 0; i < trianglesSize; i++) {
                    DrawTriangle(triangles[i].v1, triangles[i].v2, triangles[i].v3, colorArray[i % 21]);
                    DrawTriangle(triangles[i].v2, triangles[i].v3, triangles[i].v1, colorArray[i % 21]);
                    DrawTriangle(triangles[i].v3, triangles[i].v1, triangles[i].v2, colorArray[i % 21]);
                }
            }

            DrawText("To delete the newest vertex press B", 0, 0, 20, WHITE);
            DrawText("To delete the whole polygon press R", 0, 20, 20, WHITE);
            DrawText("To place the vertex press Enter", 0, 40, 20, WHITE);
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

    free(triangles);

    DeletePolygon(&polygon, &polygonSize);

    CloseWindow();
    return 0;
}
