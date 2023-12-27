#pragma once 

struct Scene {
    void(*init)();
    void(*input)();
    void(*logic)();
    void(*graphics)();
    struct Scene(*destruct)();
};