#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>
#include <math.h>

// current program mode
typedef enum mode { PlaceVertex = 0, ConnectStartingVertex = 1 } mode;

// vector2 but with double's
typedef struct Vector2d {
    double x, y;
} Vector2d;

// doubly linked list node
typedef struct Vector2dNode {
    Vector2d value;
    struct Vector2dNode *next, *prev;
} Vector2dNode;

// function to subtract a - b
Vector2d SubtractVector(const Vector2d a, const Vector2d b) {
    return (Vector2d){a.x - b.x, a.y - b.y};
}

// simple function to check whether xy is within circle pos, r
bool WithinCircle(const Vector2d xy, const Vector2d pos, const float r) {
    // pythagorean theorem
    if (sqrt((xy.x - pos.x) * (xy.x - pos.x) + (xy.y - pos.y) * (xy.y - pos.y)) <= r) {
        return true;
    }
    return false;
}

// a function for freeing the polygon data
void DeletePolygon(Vector2dNode* polygon, int polygonSize) {
    while (polygonSize--) {
        // simple iteration through the polygon
        Vector2dNode *p = polygon;
        // without the 2 lines below, the program crashes
        polygon->prev->next = polygon->next;
        polygon->next->prev = polygon->prev;
        polygon = polygon->next;
        free(p);
    }
}

double FindAngle(Vector2d vector) {
    vector.x = vector.x / sqrt(vector.x * vector.x + vector.y * vector.y);
    vector.y = vector.y / sqrt(vector.x * vector.x + vector.y * vector.y);
    if (vector.y > 0) { // quadrant 1 and 2
        return atan2(vector.y, vector.x);
    }
    return 2.0*PI + atan2(vector.y, vector.x);
}

bool IsPolygonConvex(Vector2dNode* polygon, int polygonSize) {
    /* Since the method outlined in my previous commit is buggy, i have decided to change it to another.
     * -Find the edge with the smallest angle value.
     * -Iterate through every next edge.
     * -If the angle is smaller than the current angle, the polygon must be concave.
     */
    // Every triangle/line/vertex is convex
    if (polygonSize <= 3) {
        return true;
    }
    Vector2dNode *currAngle = polygon;
    for (Vector2dNode* i = polygon->next; i != polygon; i = i->next) {
        if (FindAngle(SubtractVector(i->value, i->prev->value)) < FindAngle(SubtractVector(currAngle->value, currAngle->prev->value))) {
            currAngle = i;
        }
    }
    polygonSize--;
    if (FindAngle(SubtractVector(currAngle->next->value, currAngle->value)) > FindAngle(SubtractVector(currAngle->prev->value, currAngle->prev->prev->value))) {
        while (--polygonSize) {
            if (FindAngle(SubtractVector(currAngle->prev->value, currAngle->prev->prev->value)) < FindAngle(SubtractVector(currAngle->value, currAngle->prev->value))) {
                return false;
            }
            currAngle = currAngle->prev;
        }
    }
    else {
        while (--polygonSize) {
            if (FindAngle(SubtractVector(currAngle->next->value, currAngle->value)) < FindAngle(SubtractVector(currAngle->value, currAngle->prev->value))) {
                return false;
            }
            currAngle = currAngle->next;
        }
    }
    return true;
}

int main() {

    // window creation
    InitWindow(800, 600, "Check whether a polygon is convex");

    // this is our polygon
    Vector2dNode* polygon = NULL;
    int polygonSize = 0;

    // these are temporary (?) variables that are used for selecting/adding vertices
    Vector2dNode* selectedVertex = NULL;
    Vector2dNode* tmp = NULL;

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
                        polygon = (Vector2dNode*)malloc(sizeof(Vector2dNode));
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
                        if (WithinCircle((Vector2d){(float)x, (float)y}, selectedVertex->value, 5.0f)) {
                            // the same vertex was selected again, unselect it
                            selectedVertex = NULL;
                            m = ConnectStartingVertex;
                            break;
                        }
                        // tmp is the vertex that selectedVertex was originally connected to,
                        // we are going to connect the new vertex to selectedVertex and tmp
                        tmp = selectedVertex->next;
                        selectedVertex->next = (Vector2dNode*)malloc(sizeof(Vector2dNode));
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
                    if (WithinCircle((Vector2d){(double)x, (double)y}, (Vector2d){polygon->value.x, polygon->value.y}, 5.0f)) {
                        selectedVertex = polygon;
                        m = PlaceVertex;
                        break;
                    }
                    // checking each vertex to see if any are selected
                    if (polygon != NULL) {
                        for (Vector2dNode* i = polygon->next; i != polygon; i = i->next) {
                            if (WithinCircle((Vector2d){(double)x, (double)y}, (Vector2d){i->value.x, i->value.y}, 5.0f)) {
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
                if (WithinCircle((Vector2d){(double)x, (double)y}, polygon->value, 5.0f)) {
                    // vertex found
                    polygon->prev->next = polygon->next;
                    polygon->next->prev = polygon->prev;
                    Vector2dNode* p = polygon;
                    polygon = polygon->next;
                    free(p);
                    polygonSize--;
                }
                for (Vector2dNode* i = polygon->next; i != polygon; i = i->next) {
                    if (WithinCircle((Vector2d){(float)x, (float)y}, i->value, 5.0f)) {
                        // vertex found
                        i->prev->next = i->next;
                        i->next->prev = i->prev;
                        Vector2dNode* p = i;
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
                    DrawCircle(polygon->value.x, polygon->value.y, 5.0f, RED);
                }
                else if (polygon != NULL) { // because of clangd screaming at me...
                    DrawCircle(polygon->value.x, polygon->value.y, 5.0f, WHITE);
                }
                // draw each vertex and a line connecting them
                if (polygon != NULL) { // because of clangd screaming at me...
                    // iteration through the polygon
                    for (const Vector2dNode* i = polygon->next; i != polygon; i = i->next) {
                        if(selectedVertex == i) {
                            // if i is selected, draw it red
                            DrawCircle(i->value.x, i->value.y, 5.0f, RED);
                        }
                        else {
                            DrawCircle(i->value.x, i->value.y, 5.0f, WHITE);
                        }
                        // draw a line between the current vertex and the previous one
                        DrawLineEx((Vector2){i->prev->value.x, i->prev->value.y}, (Vector2){i->value.x, i->value.y}, 3.0f, WHITE);
                    }
                }
                if (polygon != NULL) { // because of clangd screaming at me...
                    // the final line between polygon->prev and polygon, skipped in for loop
                    DrawLineEx((Vector2){polygon->prev->value.x, polygon->prev->value.y}, (Vector2){polygon->value.x, polygon->value.y}, 3.0f, WHITE);
                }
            }

            // UI
            DrawText("Use Left Click to place vertices and select them", 0, 0, 20, WHITE);
            DrawText("Use Right Click to delete vertices", 0, 20, 20, WHITE);
            DrawText("Use R to reset the polygon", 0, 40, 20, WHITE);
            DrawText("The current polygon is: ", 0, 60, 20, WHITE);
            if (IsPolygonConvex(polygon, polygonSize)) {
                DrawText("Convex", MeasureText("The current polygon is: ", 20), 60, 20, GREEN);
            }
            else {
                DrawText("Concave", MeasureText("The current polygon is: ", 20), 60, 20, RED);
            }

        EndDrawing();
    }

    // the polygon data is freed
    DeletePolygon(polygon, polygonSize);

    // window closed
    CloseWindow();
    return 0;
}
