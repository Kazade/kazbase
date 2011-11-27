#ifndef COLOUR_H_INCLUDED
#define COLOUR_H_INCLUDED

class RGBColour;

class HSVColour {
public:
    HSVColour():h_(0), s_(0), v_(0) {}
    HSVColour(double h, double s, double v);
    HSVColour(const HSVColour& c) {
        h_ = c.h_;
        s_ = c.s_;
        v_ = c.v_;
    }

    HSVColour& operator=(const HSVColour& rhs) {
        h_ = rhs.h_;
        s_ = rhs.s_;
        v_ = rhs.v_;
        return *this;
    }

    RGBColour to_rgb() const;

    double hue() const { return h_; }
    double saturation() const { return s_; }
    double value() const { return v_; }

    void set_saturation(double s) { s_ = s; }
    void set_value(double v) { v_ = v; }
private:
    double h_;
    double s_;
    double v_;
};

class RGBColour {
public:
    RGBColour():r_(0), g_(0), b_(0) {}
    RGBColour(double r, double g, double b);
    RGBColour(const RGBColour& c) {
        r_ = c.r_;
        g_ = c.g_;
        b_ = c.b_;
    }

    RGBColour& operator=(const RGBColour& rhs) {
        r_ = rhs.r_;
        g_ = rhs.g_;
        b_ = rhs.b_;
        return *this;
    }

    HSVColour to_hsv() const;

    double red() const { return r_; }
    double green() const { return g_; }
    double blue() const { return b_; }

private:
    double r_;
    double g_;
    double b_;
};

#endif // COLOUR_H_INCLUDED
