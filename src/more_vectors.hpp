namespace MoreVectors {

struct vector3 {
    vector3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}

    float x, y, z;
};

struct vector5 {
    vector5(float x_, float y_, float z_, float w_, float q_)
        : x(x_), y(y_), z(z_), w(w_), q(q_) {}

    float x, y, z, w, q;
};

}  // namespace MoreVectors