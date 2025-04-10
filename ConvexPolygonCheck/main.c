#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>
#include <math.h>

typedef struct Vector2Node {
    Vector2 value;
    struct Vector2Node *next, *prev;
} Vector2Node;

typedef enum mode { PlaceVertex = 0, ConnectStartingVertex = 1, ConnectEndVertex = 2 } mode;

bool withinCircle(const Vector2 check, const Vector2 pos, const float r) {
    if (sqrtf((check.x - pos.x) * (check.x - pos.x) + (check.y - pos.y) * (check.y - pos.y)) <= r) {
        return true;
    }
    return false;
}

int main() {

    InitWindow(800, 600, "Convex Polygon");

    Vector2Node* polygon = NULL;
    int polygonSize = 0;

    Vector2Node* selectedVertex = NULL;
    Vector2Node* tmp;

    mode m = PlaceVertex;

    while (!WindowShouldClose()) {

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                if(m == PlaceVertex) {
                    // get the position of the mouse click
                    const int x = GetMouseX(), y = GetMouseY();
                    if (polygonSize == 0) {
                        // this is the first vertex of the polygon
                        polygon = (Vector2Node*)malloc(sizeof(Vector2Node));
                        polygon->value.x = (float)x;
                        polygon->value.y = (float)y;
                        // homemade either-side linked list
                        polygon->next = polygon;
                        polygon->prev = polygon;
                    }
                    else {
                        // we are adding a new vertex
                        // make the polygon vertices be in a loop structure
                        // the assumption is the polygon vertices are already in a loop structure
                        tmp = selectedVertex->next;
                        tmp->prev = selectedVertex->next;
                        selectedVertex->next = (Vector2Node*)malloc(sizeof(Vector2Node));
                        selectedVertex->next->value.x = (float)x;
                        selectedVertex->next->value.y = (float)y;
                        selectedVertex->next->next = tmp;
                        selectedVertex->next->prev = selectedVertex;
                        selectedVertex = selectedVertex->next;
                    }
                    polygonSize++;
                    m = ConnectStartingVertex;
                }
                else if(m == ConnectStartingVertex) {
                    const int x = GetMouseX(), y = GetMouseY();
                    if (withinCircle((Vector2){(float)x, (float)y}, (Vector2){polygon->value.x, polygon->value.y}, 5.0f)) {
                        selectedVertex = polygon;
                        m = PlaceVertex;
                    }
                    else {
                        for (Vector2Node* i = polygon->next; i != polygon; i = i->next) {
                            if (withinCircle((Vector2){(float)x, (float)y}, (Vector2){i->value.x, i->value.y}, 5.0f)) {
                                selectedVertex = i;
                                m = PlaceVertex;
                                break;
                            }
                        }
                    }
                }
            }

        BeginDrawing();

            ClearBackground(BLACK);
            
            if(polygonSize > 0) {
                printf("test\n");
                if(selectedVertex == polygon) {
                    DrawCircleV(polygon->value, 5.0f, RED);
                }
                else {
                    DrawCircleV(polygon->value, 5.0f, WHITE);
                }
                for (Vector2Node* i = polygon->next; i != polygon; i = i->next) {
                    if(selectedVertex == polygon) {
                        DrawCircleV(i->value, 5.0f, RED);
                    }
                    else {
                        DrawCircleV(i->value, 5.0f, WHITE);
                    }
                    DrawLineEx(i->prev->value, i->value, 3.0f, WHITE);
                }
            }

        EndDrawing();
    }

    while (polygon != NULL) {
        Vector2Node *p = polygon;
        polygon = p->prev;
        free(p);
    }
    free(polygon);

    CloseWindow();
    return 0;
}
