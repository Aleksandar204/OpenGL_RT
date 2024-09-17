#pragma once

class Time
{
private:
    static inline float delta_time;
public:
    static float getDeltaTime() {return delta_time;}
    static void setDeltaTime(float dt) {delta_time = dt;}
};
