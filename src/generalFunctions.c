// generalFunctions.c
#include "generalFunctions.h"
#include <string.h>

void InitTypeWriter(TypeWriter* tw, const char* text, float speed) {
    tw->text = text;
    tw->length = strlen(text);
    tw->drawnChars = 0;
    tw->accum = 0.0f;
    tw->speed = speed;
    tw->done = false;
}

void UpdateTypeWriter(TypeWriter* tw, float deltaTime, bool skip) {
    if (tw->drawnChars < tw->length && !tw->done) {
        tw->accum += deltaTime * tw->speed;
        int nextLen = (int)tw->accum;
        if (nextLen > tw->drawnChars) {
            tw->drawnChars = nextLen;
            if (tw->drawnChars > tw->length)
                tw->drawnChars = tw->length;
        }
        if (skip) {
            tw->drawnChars = tw->length;
            tw->done = true;
        }
    } else {
        tw->done = true;
    }
}

void SkipTypeWriter(TypeWriter* tw) {
    tw->drawnChars = tw->length;
    tw->done = true;
}