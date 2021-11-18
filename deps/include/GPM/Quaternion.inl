/* =================== Static methods (pseudo-constructors) =================== */
inline constexpr Quaternion Quaternion::identity() noexcept
{
    return {Vec3::zero(), 1.f};
}


inline Quaternion Quaternion::angleAxis(const f32 angle, const Vec3& axis) noexcept
{
    const f32 halfAngle{angle * .5f};
    return {axis.normalized() * sinf(halfAngle), cosf(halfAngle)};
}


inline Quaternion Quaternion::fromEuler(const Vec3& angles) noexcept
{
    // Half angles
    const Vec3 halfAngles{angles * .5f};

    // Cosinus and sinus of half angles
    const f32 cosX{cosf(halfAngles.x)}, sinX{sinf(halfAngles.x)},
              cosY{cosf(halfAngles.y)}, sinY{sinf(halfAngles.y)},
              cosZ{cosf(halfAngles.z)}, sinZ{sinf(halfAngles.z)};

    // Results used more than once
    const f32 cosYcosZ{cosY * cosZ}, sinYsinZ{sinY * sinZ},
              sinYcosZ{sinY * cosZ}, cosYsinZ{cosY * sinZ};

    const Vec3 imAxis
    {
        sinX * cosYcosZ - cosX * sinYsinZ,
        cosX * sinYcosZ - sinX * cosYsinZ,
        cosX * cosYsinZ + sinX * sinYcosZ
    };

    return {imAxis, cosX * cosYcosZ + sinX * sinYsinZ};
}

// TODO: simplify, add const, remove casts, remove ifs, pass a "up" Vec3 parameter
inline Quaternion Quaternion::lookAt(const Vec3& sourcePoint, const Vec3& destPoint) noexcept
{
    Vec3 forwardVector = (destPoint - sourcePoint).normalized();

    float dot = Vec3::dot(Vec3::forward(), forwardVector);

    if (std::abs(dot - (-1.0f)) < 0.000001f)
    {
        return Quaternion::angleAxis(PI, Vec3::up());
    }
    if (std::abs(dot - (1.0f)) < 0.000001f)
    {
        return Quaternion::identity();
    }

    float rotAngle = (float)std::acos(dot);
    Vec3  rotAxis  = Vec3::cross(Vec3::forward(), forwardVector);
    rotAxis        = rotAxis.normalized();
    return Quaternion::angleAxis(rotAngle, rotAxis);
}




/* =================== Methods =================== */
inline constexpr Vec3 Quaternion::axis() const noexcept
{
    return v;
}


inline Vec3 Quaternion::unitAxis() const noexcept
{
    return v.normalized();
}


inline f32 Quaternion::angle() const noexcept
{
    return 2.f * acosf(w);
}


inline f32 Quaternion::angleWith(const Quaternion& q) const noexcept
{
    return 2.f * acosf(dot(q));
}


inline constexpr Vec3 Quaternion::rotate(const Vec3& p) const noexcept
{
    return p * (2.f * w * w - 1.f) + (v * v.dot(p) * 2.f) + (v.cross(p) * (2.f * w));
}


inline Vec3 Quaternion::eulerAngles() const noexcept
{
    Vec3 angles;

    { // X and Y
        const f32 z2 = z * z;

        { // X
            const f32 sinXcosZ = 2.f * ((w * x) + (z * y));
            const f32 cosXcosZ = 1.f - 2.f * ((x * x) + z2);
            angles.x = atan2f(sinXcosZ, cosXcosZ);
        }

        { // Y
            const f32 sinYcosZ = 2.f * ((w * y) + (x * z));
            const f32 cosYcosZ = 1.f - 2.f * (z2 + (y * y));
            angles.y = atan2f(sinYcosZ, cosYcosZ);
        }
    }

    { // Z
        const f32 sinZ = 2.f * ((w * z) - (y * x));
        
        if (fabsf(sinZ) >= 1.f)
        {
            angles.z = copysignf(HALF_PI, sinZ);
        }
        else
        {
            angles.z = asinf(sinZ);
        }
    }

    return angles;
}


inline constexpr f32 Quaternion::sqrLength() const noexcept
{
    return v.sqrLength() + (w * w);
}


inline f32 Quaternion::length() const noexcept
{
    return sqrtf(sqrLength());
}


inline constexpr f32 Quaternion::dot(const Quaternion& q) const noexcept
{
    return v.dot(q.v) + w * q.w;
}


inline Quaternion Quaternion::normalized() const noexcept
{
    return *this / length();
}


inline Quaternion Quaternion::safelyNormalized() const noexcept
{
    const f32 sqrLen{sqrLength()};
    return sqrLen ? *this / sqrtf(sqrLen) : *this;
}


inline void Quaternion::normalize() noexcept
{
    const f32 reciprocal_len{1.f / length()};
    v *= reciprocal_len;
    w *= reciprocal_len;
}

inline void Quaternion::safelyNormalize() noexcept
{
    const f32 sqrLen{sqrLength()};

    if (sqrLen)
    {
        *this /= sqrtf(sqrLen);
    }
}


inline constexpr Quaternion Quaternion::conjugate() const noexcept
{
    return {-v, w};
}


inline constexpr Quaternion Quaternion::inversed() const noexcept
{
    return conjugate() / sqrLength();
}


inline constexpr Quaternion Quaternion::to(const Quaternion& target) const noexcept
{
    return target * inversed();
}


inline Quaternion Quaternion::slerp(const Quaternion& target, const f32 t) const noexcept
{
    const f32 dotQaQb = dot(target);
    const f32 angle   = std::acos(std::abs(dotQaQb));
    const f32 sign    = copysignf(1.f, dotQaQb);
    return (*this * sign * std::sin((1.f - t) * angle) + target * std::sin(t * angle)) / std::sin(angle);
}


inline Quaternion Quaternion::nlerp(const Quaternion& target, const f32 t) const noexcept
{
    const Quaternion q{*this * (1.f - t) + target * t};
    return q.normalized();
}


inline bool Quaternion::isEqualTo(const Quaternion& q, const f32 eps) const noexcept
{
    return v.isEqualTo(q.v) && fabs(w - q.w) < eps;
}




/* =================== Operator overloads =================== */
inline constexpr bool Quaternion::operator==(const Quaternion& q) const noexcept
{
    return v == q.v && w == q.w;
}


inline constexpr Quaternion Quaternion::operator*=(const f32 k) const noexcept
{
    return {v * k, w * k};
}


inline constexpr Quaternion Quaternion::operator/=(const f32 k) const noexcept
{
    const f32 reciprocal{1.f / k};

    return {v * reciprocal, w * reciprocal};
}




inline constexpr Quaternion Quaternion::operator+(const Quaternion& q) const noexcept
{
    return {v + q.v, w + q.w};
}


inline constexpr Quaternion Quaternion::operator-(const Quaternion& q) const noexcept
{
    return {v - q.v, w - q.w};
}


inline constexpr Quaternion Quaternion::operator-() const noexcept
{
    return {-v, -w};
}


inline constexpr Quaternion Quaternion::operator*(const Quaternion& q) const noexcept
{
    return {(q.v * w) + (v * q.w) + v.cross(q.v), (w * q.w) - v.dot(q.v)};
}


// The quaternion is assumed normalized
inline constexpr Vec3 Quaternion::operator*(const Vec3& v) const noexcept
{
    return rotate(v);
}


inline constexpr Quaternion Quaternion::operator*(const f32 k) const noexcept
{
    return {v * k, w * k};
}


inline constexpr Quaternion Quaternion::operator/(const f32 k) const noexcept
{
    const f32 reciprocal{1.f / k};

    return {v * reciprocal, w * reciprocal};
}