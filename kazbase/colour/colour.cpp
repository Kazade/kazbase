#include <algorithm>
#include <cmath>
#include <cassert>

#include "colour.h"

HSVColour::HSVColour(double h, double s, double v):
    h_(h),
    s_(s),
    v_(v) {

}

int ftrc(double x) {
    return (int) floor(x);
}

RGBColour HSVColour::to_rgb() const {
    RGBColour result;

    float hue6, p, q, t;

    assert(!(h_ < 0.0f || s_ < 0.0f || s_ > 1.0f || v_ < 0.0f || v_ > 1.0f));

    float hue = ftrc(h_ / 60.0) % 6;
    float f = (h_ / 60.0) - floor(h_ / 60.0);

    p = v_ * (1.0f - s_);
    q = v_ * (1.0f - (s_ * f));
    t = v_ * (1.0f - (s_ * (1.0f - f)));

    switch(ftrc(hue)) {
        case 0: // red -> yellow
            result = RGBColour(v_, t, p);
            break;
        case 1: // yellow -> green
            result = RGBColour(q, v_, p);
            break;
        case 2: // green -> cyan
            result = RGBColour(p, v_, t);
            break;
        case 3: // cyan -> blue
            result = RGBColour(p, q, v_);
            break;
        case 4: // blue -> magenta
            result = RGBColour(t, p, v_);
            break;
        case 5: // magenta -> red
            result = RGBColour(v_, p, q);
            break;
    }

    return result;
}

RGBColour::RGBColour(double r, double g, double b):
    r_(r),
    g_(g),
    b_(b) {

}

HSVColour RGBColour::to_hsv() const {
    double rgb_min = std::min(r_, std::min(g_, b_));
    double rgb_max = std::max(r_, std::max(g_, b_));

    double s = 0;
    double h = 0;
    double v = rgb_max;
    if(fabs(v) < 0.00000001) {
        return HSVColour(0, 0, 0);
    }

    float tr = r_ / v;
    float tg = g_ / v;
    float tb = b_ / v;

    rgb_min = std::min(tr, std::min(tg, tb));
    rgb_max = std::max(tr, std::max(tg, tb));

    s = rgb_max - rgb_min;
    if(fabs(s) < 0.00000001) {
        h = 0;
        return HSVColour(h, s, v);
    }

    tr = (tr - rgb_min) / (rgb_max - rgb_min);
    tg = (tg - rgb_min) / (rgb_max - rgb_min);
    tb = (tb - rgb_min) / (rgb_max - rgb_min);

    rgb_min = std::min(tr, std::min(tg, tb));
    rgb_max = std::max(tr, std::max(tg, tb));

    if(rgb_max == tr) {
        h = 0.0 + 60.0 * (tg - tb);
        if(h < 0.0) {
            h += 360.0;
        }
    } else if (rgb_max == tg) {
        h = 120.0 + 60.0 * (tb - tr);
    } else {
        h = 240.0 + 60.0 * (tr - tg);
    }

    return HSVColour(h, s, v);
}
