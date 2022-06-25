namespace MoreVectors {

struct vector3 {
    vector3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}

    float x, y, z;
};

struct vector4 {
    vector4(float x_, float y_, float z_, float q_)
        : x(x_), y(y_), z(z_), q(q_) {}

    uint32_t x, y, z, q;
};

}  // namespace MoreVectors