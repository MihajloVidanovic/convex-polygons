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

    Vector2Node* selectedVertex = polygon;

    mode m = PlaceVertex;

    while (!WindowShouldClose()) {

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && m == PlaceVertex) {
            // get the position of the mouse click
            const int x = GetMouseX(), y = GetMouseY();
            if (selectedVertex == NULL) {
                // this is the first vertex of the polygon
                selectedVertex = (Vector2Node*)malloc(sizeof(Vector2Node));
                selectedVertex->value.x = (float)x;
                selectedVertex->value.y = (float)y;
                // homemade either-side linked list
                selectedVertex->next = NULL;
                selectedVertex->prev = NULL;
                polygon = selectedVertex;
            }
            else {
                // we are adding a new vertex
                Vector2Node* tmp = selectedVertex->next;
                selectedVertex->next = (Vector2Node*)malloc(sizeof(Vector2Node));
                selectedVertex->next->value.x = (float)x;
                selectedVertex->next->value.y = (float)y;
                selectedVertex->next->next = NULL;
                selectedVertex->next->prev = selectedVertex;
                selectedVertex = selectedVertex->next;
            }
            polygonSize++;
            if (polygonSize == 1) {
                // if this is the first vertex: the next step is to pick the starting vertex for the next one
                m = ConnectStartingVertex;
            }
            else {
                // if this is not the first vertex: the next step is to pick the ending vertex for this one
                m = ConnectEndVertex;
            }
        }
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && m == ConnectStartingVertex) {
            const int x = GetMouseX(), y = GetMouseY();
            for (Vector2Node* i = polygon; i != NULL; i = i->prev) {
                if (withinCircle((Vector2){(float)x, (float)y}, (Vector2){i->value.x, i->value.y}, 5.0f)) {
                    selectedVertex = i;
                    break;
                }
            }
        }
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && m == ConnectEndVertex) {}

        BeginDrawing();

            ClearBackground(BLACK);

            for (int i = 0; i < );

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
