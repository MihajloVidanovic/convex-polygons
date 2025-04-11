#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>
#include <math.h>

typedef struct Vector2Node {
    Vector2 value;
    struct Vector2Node *next, *prev;
} Vector2Node;

typedef enum mode { PlaceVertex = 0, ConnectStartingVertex = 1 } mode;

bool WithinCircle(const Vector2 check, const Vector2 pos, const float r) {
    if (sqrtf((check.x - pos.x) * (check.x - pos.x) + (check.y - pos.y) * (check.y - pos.y)) <= r) {
        return true;
    }
    return false;
}

void DeletePolygon(Vector2Node* polygon) {
    while (polygon != NULL) {
        Vector2Node *p = polygon;
        polygon = p->prev;
        free(p);
    }
    free(polygon);
}

int main() {

    InitWindow(800, 600, "Convex Polygon");

    Vector2Node* polygon = NULL;
    int polygonSize = 0;

    Vector2Node* selectedVertex = NULL;
    Vector2Node* tmp = NULL;

    mode m = PlaceVertex;

    while (!WindowShouldClose()) {

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            // get the position of the mouse click
            const int x = GetMouseX(), y = GetMouseY();
            switch (m) {
                case PlaceVertex:
                    if (polygonSize == 0) {
                        // this is the first vertex of the polygon
                        polygon = (Vector2Node*)malloc(sizeof(Vector2Node));
                        polygon->value.x = (float)x;
                        polygon->value.y = (float)y;
                        // homemade either-side linked list
                        polygon->next = polygon;
                        polygon->prev = polygon;
                    }
                    else if (selectedVertex != NULL) {
                        // we are adding a new vertex
                        // make the polygon vertices be in a loop structure
                        // the assumption is the polygon vertices are already in a loop structure
                        tmp = selectedVertex->next;
                        selectedVertex->next = (Vector2Node*)malloc(sizeof(Vector2Node));
                        tmp->prev = selectedVertex->next;
                        selectedVertex->next->value.x = (float)x;
                        selectedVertex->next->value.y = (float)y;
                        selectedVertex->next->next = tmp;
                        selectedVertex->next->prev = selectedVertex;
                    }
                    polygonSize++;
                    selectedVertex = NULL;
                    m = ConnectStartingVertex;
                    break;
                case ConnectStartingVertex:
                    if (WithinCircle((Vector2){(float)x, (float)y}, (Vector2){polygon->value.x, polygon->value.y}, 5.0f)) {
                        selectedVertex = polygon;
                        m = PlaceVertex;
                    }
                    else if (polygon != NULL) {
                        for (Vector2Node* i = polygon->next; i != polygon; i = i->next) {
                            if (WithinCircle((Vector2){(float)x, (float)y}, (Vector2){i->value.x, i->value.y}, 5.0f)) {
                                selectedVertex = i;
                                m = PlaceVertex;
                                break;
                            }
                        }
                    }
                    break;
            }
        }
        /*if (IsKeyPressed(KEY_R)) {
            DeletePolygon(polygon);
            polygon = NULL;
            polygonSize = 0;
        }*/ // TODO

        BeginDrawing();

            ClearBackground(BLACK);

            // polygon drawing
            if(polygonSize > 0) {
                if(selectedVertex == polygon) {
                    DrawCircleV(polygon->value, 5.0f, RED);
                }
                else {
                    DrawCircleV(polygon->value, 5.0f, WHITE);
                }
                for (const Vector2Node* i = polygon->next; i != polygon; i = i->next) {
                    if(selectedVertex == i) {
                        DrawCircleV(i->value, 5.0f, RED);
                    }
                    else {
                        DrawCircleV(i->value, 5.0f, WHITE);
                    }
                    DrawLineEx(i->prev->value, i->value, 3.0f, WHITE);
                }
                DrawLineEx(polygon->prev->value, polygon->value, 3.0f, WHITE);
            }

            // UI
            // TODO

        EndDrawing();
    }

    DeletePolygon(polygon);

    CloseWindow();
    return 0;
}
