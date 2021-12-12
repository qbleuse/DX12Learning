/* ================== Transform static methods ================== */
inline constexpr Mat4 Transform::translation(const Vec3& t) noexcept
{
    return
    {
        1.f, .0f, .0f, t.x,
        .0f, 1.f, .0f, t.y,
        .0f, .0f, 1.f, t.z,
        .0f, .0f, .0f, 1.f
    };
}


inline Mat4 Transform::rotationX(const f32 angle) noexcept
{
    const f32 cosAngle{cosf(angle)}, sinAngle{sinf(angle)};

    return
    {
        1.f,    .0f,        .0f,       .0f,
        .0f,    cosAngle,   -sinAngle,  .0f,
        .0f,    sinAngle,  cosAngle,  .0f,
        .0f,    .0f,        .0f,       1.f
    };
}


inline Mat4 Transform::rotationY(const f32 angle) noexcept
{
    const f32 cosAngle{cosf(angle)}, sinAngle{sinf(angle)};

    return
    {
        cosAngle,   .0f,    sinAngle,  .0f,
        .0f,        1.f,    .0f,        .0f,
        -sinAngle,   .0f,    cosAngle,   .0f,
        .0f,        .0f,    .0f,        1.f
    };
}


inline Mat4 Transform::rotationZ(const f32 angle) noexcept
{
    const f32 cosAngle{cosf(angle)}, sinAngle{sinf(angle)};

    return
    {
        cosAngle,   -sinAngle,   .0f,    .0f,
        sinAngle,  cosAngle,   .0f,    .0f,
        .0f,        .0f,        1.f,    .0f,
        .0f,        .0f,        .0f,    1.f
    };
}


// Y * X * Z
inline Mat4 Transform::rotation(const Vec3& r) noexcept
{
    const f32 cx{cosf(r.x)}, sx{sinf(r.x)},
              cy{cosf(r.y)}, sy{sinf(r.y)},
              cz{cosf(r.z)}, sz{sinf(r.z)};

    return
    {
        cy * cz + sx * sy * sz, cx * sz, sx * cy * sz - sy * cz, .0f,
        sx * sy * cz - cy * sz, cx * cz, sy * sz + sx * cy * cz, .0f,
        cx * sy,                -sx,     cx * cy,                .0f,
        .0f,                    .0f,     .0f,                    1.f
    };
}


inline Mat4 Transform::rotationAround(const Vec3& axis, const f32 angle) noexcept
{
    const f32 c{cosf(angle)}, s{sinf(angle)}, t{1.f - c},
              x{axis.x}, y{axis.y}, z{axis.z};

    return
    {
        t * x * x + c,     t * x * y - s * z, t * x * z + s * y, .0f,
        t * x * y + s * z, t * y * y + c,     t * y * z - s * x, .0f,
        t * x * z - s * y, t * y * z + s * x, t * z * z + c,     .0f,
        .0f,               .0f,               .0f,               1.f
    };
}


inline constexpr Mat4 Transform::scaling(const Vec3& s) noexcept
{
    return
    {
        s.x, .0f, .0f, .0f,
        .0f, s.y, .0f, .0f,
        .0f, .0f, s.z, .0f,
        .0f, .0f, .0f, 1.f,
    };
}


inline Mat4 Transform::TRS(const Vec3& t, const Vec3& r, const Vec3& s) noexcept
{
    return scaling(s) * rotation(r) * translation(t);
}

 
inline Mat4 Transform::lookAt(const Vec3& eyePos,
                              const Vec3& targetPos,
                              const Vec3& normalizedUp) noexcept
{
    const Vec4 backward{(eyePos - targetPos).normalized(), .0f},
               right   {normalizedUp.cross(backward.xyz),  .0f},
               up      {backward.xyz.cross(right.xyz),     .0f};

    return {right, up, backward, eyePos};
}


inline constexpr Mat4 Transform::symFrustrum(const f32 right, const f32 top,
                                             const f32 near_, const f32 far_) noexcept
{
    const f32 depthInv{1.f / (far_ - near_)};

    return
    {
        1.0f / right, .0f,        .0f,                          .0f,
        .0f,          1.0f / top, .0f,                          .0f,
        .0f,          .0f,        -(far_ + near_) * depthInv,   -2.f * far_ * near_ * depthInv ,
        .0f,          .0f,        -1.f, .0f
    };
}


inline Mat4 Transform::perspective(const f32 fovY, const f32 aspect,
                                   const f32 near_, const f32 far_) noexcept
{
    const f32 top{tanf(fovY / 2.f)};
    
    return symFrustrum(top * aspect, top, near_, far_);
}


inline constexpr Mat4 Transform::orthographic(const f32 right, const f32 left, const f32 top, const f32 bottom,
                                              const f32 nearVal, const f32 farVal) noexcept
{
    // will be optimisate by compilator
    const float a11 = 2.f / (right - left);
    const float a22 = 2.f / (top - bottom);
    const float a33 = -2.f / (farVal - nearVal);
    
    const float tx = -(right + left) / (right - left);
    const float ty = -(top + bottom) / (top - bottom);
    const float tz = -(farVal + nearVal) / (farVal - nearVal);

    return {a11, 0.f, 0.f, tx,
            0.f, a22, 0.f, ty,
            0.f, 0.f, a33, tz,
            0.f, 0.f, 0.f, 1.f};
}


inline constexpr Mat4 Transform::viewport(const f32 x,     const f32 y,
                                          const f32 width, const f32 height) noexcept
{
    const f32 halfWidth{width * .5f}, halfHeight{height * .5f};

    return
    {
        halfWidth,     .0f,            .0f, x + halfWidth,
        .0f,           halfHeight,     .0f,  y + halfHeight,
        .0f,           .0f,            .5f, .5f,
        .0f,            .0f,            .0f, 1.f
    };
}




/* ================== Transform constructors ================== */
inline Transform::Transform(const Vec3& t, const Vec3& r, const Vec3& s) noexcept
{
    setScale(s);
    setRotation(r);
    setGlobalTranslation(t);
}


inline constexpr Transform::Transform(const Mat4& m) noexcept
    : model{m}
{}




/* ================== Transform non-static methods ================== */
inline Mat4 Transform::normalMat() const noexcept
{
    return model.cofactor();
}


inline constexpr Vec3 Transform::right() const noexcept
{
    return model.c[0].xyz;
}


inline constexpr Vec3 Transform::up() const noexcept
{
    return model.c[1].xyz;
}


inline constexpr Vec3 Transform::backward() const noexcept
{
    return model.c[2].xyz;
}

inline constexpr Vec3 Transform::forward() const noexcept
{
    return -model.c[2].xyz;
}


inline constexpr Vec3 Transform::translation() const noexcept
{
    return model.c[3].xyz;
}



inline Mat4 Transform::rotation() const noexcept
{
    const Vec3 invScale{1.f / right().length(), 1.f / up().length(), 1.f / backward().length()};

    return
    {
        {model.c[0].xyz * invScale.x, .0f},
        {model.c[1].xyz * invScale.y, .0f},
        {model.c[2].xyz * invScale.x, .0f},
        {Vec3::zero(),                .0f}
    };
}


inline Vec3 Transform::eulerAngles() const noexcept
{
    if (model.e[9] < 1.f)
    {
        if (model.e[9] > -1.f)
        {
            return
            {
                asinf(-model.e[9]),
                atan2f(model.e[8], model.e[10]),
                atan2f(model.e[1], model.e[5])
            };
        }

        return {HALF_PI, -atan2f(-model.e[4], model.e[0]), .0f};
    }

    return {-HALF_PI, atan2f(-model.e[4], model.e[0]), .0f};
}


inline Vec3 Transform::scaling() const noexcept
{
    return {right().length(), up().length(), backward().length()};
}




// Setters
inline constexpr void Transform::setLocalTranslation(const Vec3& t) noexcept
{
    model.e[12] = (t.x * model.e[0]) + (t.y * model.e[4]) + (t.z * model.e[8]);
    model.e[13] = (t.x * model.e[1]) + (t.y * model.e[5]) + (t.z * model.e[9]);
    model.e[14] = (t.x * model.e[2]) + (t.y * model.e[6]) + (t.z * model.e[10]);
}


inline constexpr void Transform::setGlobalTranslation(const Vec3& t) noexcept
{
    model.c[3].xyz = t;
}


inline void Transform::setRotation(const Vec3& r) noexcept
{
    const Mat4 rot{rotation(r)};
    const Vec3 s  {scaling()};

    model.c[0].xyz = rot.c[0].xyz * s.x;
    model.c[1].xyz = rot.c[1].xyz * s.y;
    model.c[2].xyz = rot.c[2].xyz * s.z;
}


inline constexpr void Transform::setScale(const Vec3& s) noexcept
{
    scale(s / scaling());
}




// Transformations
inline constexpr void Transform::translateX(const f32 shift) noexcept
{
    model.e[12] += shift * model.e[0];
    model.e[13] += shift * model.e[1];
    model.e[14] += shift * model.e[2];
}


inline constexpr void Transform::translateY(const f32 shift) noexcept
{
    model.e[12] += shift * model.e[4];
    model.e[13] += shift * model.e[5];
    model.e[14] += shift * model.e[6];
}


inline constexpr void Transform::translateZ(const f32 shift) noexcept
{
    model.e[12] += shift * model.e[8];
    model.e[13] += shift * model.e[9];
    model.e[14] += shift * model.e[10];
}


inline constexpr void Transform::translate(const Vec3& t) noexcept
{
    model.e[12] += (t.x * model.e[0]) + (t.y * model.e[4]) + (t.z * model.e[8]);
    model.e[13] += (t.x * model.e[1]) + (t.y * model.e[5]) + (t.z * model.e[9]);
    model.e[14] += (t.x * model.e[2]) + (t.y * model.e[6]) + (t.z * model.e[10]);
}


inline void Transform::rotateX(const f32 angle) noexcept
{
    const f32 cosAngle{cosf(angle)}, sinAngle{sinf(angle)};

    f32 tmp1{(model.e[1]  * cosAngle) - (model.e[2]  * sinAngle)},
        tmp2{(model.e[1]  * sinAngle) + (model.e[2]  * cosAngle)};
    model.e[1]  = tmp1;
    model.e[2]  = tmp2;

    tmp1 = (model.e[5]  * cosAngle) - (model.e[6]  * sinAngle);
    tmp2 = (model.e[5]  * sinAngle) + (model.e[6]  * cosAngle);
    model.e[5]  = tmp1;
    model.e[6]  = tmp2;

    tmp1 = (model.e[9]  * cosAngle) - (model.e[10] * sinAngle);
    tmp2 = (model.e[9]  * sinAngle) + (model.e[10] * cosAngle);
    model.e[9]  = tmp1;
    model.e[10] = tmp2;
}


inline void Transform::rotateY(const f32 angle) noexcept
{
    const f32 cosAngle{cosf(angle)}, sinAngle{sinf(angle)};

    f32 tmp1{(model.e[0]  * cosAngle) + (model.e[2]  * sinAngle)},
        tmp2{(model.e[2]  * cosAngle) - (model.e[0]  * sinAngle)};
    model.e[0]  = tmp1;
    model.e[2]  = tmp2;

    tmp1 = (model.e[4]  * cosAngle) + (model.e[6]  * sinAngle);
    tmp2 = (model.e[6]  * cosAngle) - (model.e[4]  * sinAngle);
    model.e[4]  = tmp1;
    model.e[6]  = tmp2;

    tmp1 = (model.e[8]  * cosAngle) + (model.e[10] * sinAngle);
    tmp2 = (model.e[10] * cosAngle) - (model.e[8]  * sinAngle);
    model.e[8]  = tmp1;
    model.e[10] = tmp2;
}


inline void Transform::rotateZ(const f32 angle) noexcept
{
    const f32 cosAngle{cosf(angle)}, sinAngle{sinf(angle)};
    f32 tmp1{(model.e[0]  * cosAngle) - (model.e[1]  * sinAngle)},
        tmp2{(model.e[0]  * sinAngle) + (model.e[1]  * cosAngle)};
    model.e[0]  = tmp1;
    model.e[1]  = tmp2;

    tmp1 = (model.e[4]  * cosAngle) - (model.e[5]  * sinAngle);
    tmp2 = (model.e[4]  * sinAngle) + (model.e[5]  * cosAngle);
    model.e[4]  = tmp1;
    model.e[5]  = tmp2;

    tmp1 = (model.e[8]  * cosAngle) - (model.e[9]  * sinAngle);
    tmp2 = (model.e[8]  * sinAngle) + (model.e[9]  * cosAngle);
    model.e[8]  = tmp1;
    model.e[9]  = tmp2;
}


inline void Transform::rotate(const Vec3& r) noexcept
{
    const Mat4 rot{rotation(r)};

    f32 tmp1{model.e[0]}, tmp2{model.e[1]};
    model.e[0]  = (tmp1 * rot.e[0]) + (tmp2 * rot.e[4]) + (model.e[2]  * rot.e[8]);
    model.e[1]  = (tmp1 * rot.e[1]) + (tmp2 * rot.e[5]) + (model.e[2]  * rot.e[9]);
    model.e[2]  = (tmp1 * rot.e[2]) + (tmp2 * rot.e[6]) + (model.e[2]  * rot.e[10]);

    tmp1 = model.e[4];
    tmp2 = model.e[5];
    model.e[4]  = (tmp1 * rot.e[0]) + (tmp2 * rot.e[4]) + (model.e[6]  * rot.e[8]);
    model.e[5]  = (tmp1 * rot.e[1]) + (tmp2 * rot.e[5]) + (model.e[6]  * rot.e[9]);
    model.e[6]  = (tmp1 * rot.e[2]) + (tmp2 * rot.e[6]) + (model.e[6]  * rot.e[10]);
    
    tmp1 = model.e[8];
    tmp2 = model.e[9];
    model.e[8]  = (tmp1 * rot.e[0]) + (tmp2 * rot.e[4]) + (model.e[10] * rot.e[8]);
    model.e[9]  = (tmp1 * rot.e[1]) + (tmp2 * rot.e[5]) + (model.e[10] * rot.e[9]);
    model.e[10] = (tmp1 * rot.e[2]) + (tmp2 * rot.e[6]) + (model.e[10] * rot.e[10]);
}


inline void Transform::rotateAround(const Vec3& axis, const f32 angle) noexcept
{
    apply(rotationAround(axis, angle));
}

inline constexpr void Transform::setVectorUp(const Vec3& newUp) noexcept
{
	model.c[1].xyz = newUp;
}

inline constexpr void Transform::setVectorRight(const Vec3& newRight) noexcept
{
	model.c[0].xyz = newRight;
}

inline constexpr void Transform::setVectorForward(const Vec3& newForward) noexcept
{
	model.c[2].xyz = -newForward;
}

inline constexpr void Transform::scaleX(const f32 coef) noexcept
{
    model.c[0].xyz *= coef;
}


inline constexpr void Transform::scaleY(const f32 coef) noexcept
{
    model.c[1].xyz *= coef;
}


inline constexpr void Transform::scaleZ(const f32 coef) noexcept
{
    model.c[2].xyz *= coef;
}


inline constexpr void Transform::scale(const Vec3& s) noexcept
{
    scaleX(s.x);
    scaleY(s.y);
    scaleZ(s.z);
}


inline void Transform::lookAt(const Vec3& targetPos) noexcept
{
    const Vec3 s{scaling()};

    model.c[2] = {(model.c[3].xyz - targetPos).normalized() * s, .0f};
    model.c[0] = {model.c[1].xyz.cross(model.c[2].xyz) * s,      .0f};
    model.c[1] = {model.c[2].xyz.cross(model.c[0].xyz) * s,      .0f};
}


inline void Transform::apply(const Vec3& t, const Vec3& r, const Vec3& s) noexcept
{
    scale(s);
    rotate(r);
    translate(t);
}


inline constexpr void Transform::apply(const Mat4& m) noexcept
{
    model *= m;
}