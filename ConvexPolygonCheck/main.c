#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>
#include <math.h>

// current program mode
typedef enum mode { PlaceVertex = 0, ConnectStartingVertex = 1 } mode;

// doubly linked list node
typedef struct Vector2Node {
    Vector2 value;
    struct Vector2Node *next, *prev;
} Vector2Node;

// simple function to check whether xy is within circle pos, r
bool WithinCircle(const Vector2 xy, const Vector2 pos, const float r) {
    // pythagorean theorem
    if (sqrtf((xy.x - pos.x) * (xy.x - pos.x) + (xy.y - pos.y) * (xy.y - pos.y)) <= r) {
        return true;
    }
    return false;
}

// a function for freeing the polygon data
void DeletePolygon(Vector2Node* polygon, int polygonSize) {
    while (polygonSize--) {
        // simple iteration through the polygon
        Vector2Node *p = polygon;
        // without the 2 lines below, the program crashes
        polygon->prev->next = polygon->next;
        polygon->next->prev = polygon->prev;
        polygon = polygon->next;
        free(p);
    }
}

int main() {

    // window creation
    InitWindow(800, 600, "Convex Polygon");

    // this is our polygon
    Vector2Node* polygon = NULL;
    int polygonSize = 0;

    // these are temporary (?) variables that are used for selecting/adding vertices
    Vector2Node* selectedVertex = NULL;
    Vector2Node* tmp = NULL;

    // variable that indicates what mode the program is in
    mode m = PlaceVertex;

    while (!WindowShouldClose()) {

        // adding a vertex/selecting a vertex is done with the left mouse button
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            // get the position of the mouse click
            const int x = GetMouseX(), y = GetMouseY();

            // since there's nothing to select
            if (polygonSize == 0) {
                m = PlaceVertex;
            }

            switch (m) {
                case PlaceVertex:
                    // a new vertex is to be placed and added to the polygon
                    if (polygonSize == 0) {
                        // this is the first vertex of the polygon
                        polygon = (Vector2Node*)malloc(sizeof(Vector2Node));
                        polygon->value.x = (float)x;
                        polygon->value.y = (float)y;
                        // homemade doubly linked list
                        polygon->next = polygon;
                        polygon->prev = polygon;
                    }
                    else if (selectedVertex != NULL) {
                        // we are adding a new vertex
                        // make the polygon vertices be in a loop structure
                        // the assumption is the polygon vertices are already in a loop structure
                        if (WithinCircle((Vector2){(float)x, (float)y}, selectedVertex->value, 5.0f)) {
                            // the same vertex was selected again, unselect it
                            selectedVertex = NULL;
                            m = ConnectStartingVertex;
                            break;
                        }
                        // tmp is the vertex that selectedVertex was originally connected to,
                        // we are going to connect the new vertex to selectedVertex and tmp
                        tmp = selectedVertex->next;
                        selectedVertex->next = (Vector2Node*)malloc(sizeof(Vector2Node));
                        // so that the doubly linked list is preserved
                        tmp->prev = selectedVertex->next;
                        selectedVertex->next->value.x = (float)x;
                        selectedVertex->next->value.y = (float)y;
                        selectedVertex->next->next = tmp;
                        selectedVertex->next->prev = selectedVertex;
                    }
                    // the polygon has another vertex added, reset the select variable
                    polygonSize++;
                    selectedVertex = NULL;
                    m = ConnectStartingVertex;
                    break;
                case ConnectStartingVertex:
                    // this statement is necessary because the for loop skips over polygon
                    if (WithinCircle((Vector2){(float)x, (float)y}, (Vector2){polygon->value.x, polygon->value.y}, 5.0f)) {
                        selectedVertex = polygon;
                        m = PlaceVertex;
                        break;
                    }
                    // checking each vertex to see if any are selected
                    if (polygon != NULL) {
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
        else if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
            // get the mouse click position
            const int x = GetMouseX(), y = GetMouseY();

            if (polygon != NULL && polygonSize > 0) { // because of clangd screaming at me...
                if (WithinCircle((Vector2){(float)x, (float)y}, (Vector2){polygon->value.x, polygon->value.y}, 5.0f)) {
                    // vertex found
                    polygon->prev->next = polygon->next;
                    polygon->next->prev = polygon->prev;
                    Vector2Node* p = polygon;
                    polygon = polygon->next;
                    free(p);
                    polygonSize--;
                }
                for (Vector2Node* i = polygon->next; i != polygon; i = i->next) {
                    if (WithinCircle((Vector2){(float)x, (float)y}, (Vector2){i->value.x, i->value.y}, 5.0f)) {
                        // vertex found
                        i->prev->next = i->next;
                        i->next->prev = i->prev;
                        Vector2Node* p = i;
                        i = i->next;
                        free(p);
                        polygonSize--;
                    }
                }
            }
        }
        if (IsKeyPressed(KEY_R)) {
            DeletePolygon(polygon, polygonSize);
            polygon = NULL;
            polygonSize = 0;
        }

        BeginDrawing();

            ClearBackground(BLACK);

            // drawing the polygon
            if(polygonSize > 0) {
                // required because the for loop skips over polygon
                if(polygon != NULL && selectedVertex == polygon) { // because of clangd screaming at me...
                    // if polygon is selected, draw it red
                    DrawCircleV(polygon->value, 5.0f, RED);
                }
                else if (polygon != NULL) { // because of clangd screaming at me...
                    DrawCircleV(polygon->value, 5.0f, WHITE);
                }
                // draw each vertex and a line connecting them
                if (polygon != NULL) { // because of clangd screaming at me...
                    // iteration through the polygon
                    for (const Vector2Node* i = polygon->next; i != polygon; i = i->next) {
                        if(selectedVertex == i) {
                            // if i is selected, draw it red
                            DrawCircleV(i->value, 5.0f, RED);
                        }
                        else {
                            DrawCircleV(i->value, 5.0f, WHITE);
                        }
                        // draw a line between the current vertex and the previous one
                        DrawLineEx(i->prev->value, i->value, 3.0f, WHITE);
                    }
                }
                if (polygon != NULL) { // because of clangd screaming at me...
                    // the final line between polygon->prev and polygon, skipped in for loop
                    DrawLineEx(polygon->prev->value, polygon->value, 3.0f, WHITE);
                }
            }

            // UI
            // TODO

        EndDrawing();
    }

    // the polygon data is freed
    DeletePolygon(polygon, polygonSize);

    // window closed
    CloseWindow();
    return 0;
}
