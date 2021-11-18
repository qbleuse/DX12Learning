/* =================== Constructors =================== */
inline constexpr Vector3::Vector3(const f32 k) noexcept
    : x{k}, y{k}, z{k}
{}


inline constexpr Vector3::Vector3(const f32 x_, const f32 y_, const f32 z_) noexcept
    : x{x_}, y{y_}, z{z_}
{}


inline constexpr Vector3::Vector3(const Vec2 v, const f32 z_) noexcept
    : xy{v}, z{z_}
{}


inline constexpr Vector3::Vector3(const f32 coef[3]) noexcept
    : e{coef[0], coef[1], coef[2]}
{}




/* =================== Static methods (pseudo-constructors) =================== */
inline constexpr Vector3 Vector3::zero() noexcept
{
    return {.0f, .0f, .0f}; 
}


inline constexpr Vector3 Vector3::one() noexcept
{
    return {1.f, 1.f, 1.f}; 
}


inline constexpr Vector3 Vector3::right() noexcept
{
    return {1.f, .0f, .0f}; 
}


inline constexpr Vector3 Vector3::up() noexcept
{
    return {.0f, 1.f, .0f}; 
}


inline constexpr Vector3 Vector3::forward() noexcept
{
    return {.0f, .0f, 1.f}; 
}


inline constexpr f32 Vector3::dot(const Vector3& lhs, const Vector3& rhs) noexcept
{
    return lhs.dot(rhs);
}


inline constexpr Vector3 Vector3::cross(const Vector3& lhs, const Vector3& rhs) noexcept
{
    return lhs.cross(rhs);
}




/* =================== Methods =================== */
inline constexpr f32 Vector3::sqrLength() const noexcept
{
    return (x * x) + (y * y) + (z * z);
}


inline f32 Vector3::length() const noexcept
{
    return sqrtf(sqrLength());
}


inline constexpr f32 Vector3::dot(const Vector3& v) const noexcept
{
    return (x * v.x) + (y * v.y) + (z * v.z);
}


inline constexpr Vector3 Vector3::cross(const Vector3& v) const noexcept
{
    return {(v.z * y) - (z * v.y),
            (v.x * z) - (x * v.z),
            (v.y * x) - (y * v.x)};
}


inline constexpr bool Vector3::isNull() const noexcept
{
    return !x && !y && !z;
}


inline constexpr bool Vector3::isOrthogonalTo(const Vector3& v) const noexcept
{
    return !dot(v);
}


inline constexpr bool Vector3::isNormalized() const noexcept
{
    return !(sqrLength() - 1.f);
}


inline constexpr bool Vector3::isOrthonormalTo(const Vector3& v) const noexcept
{
    return !dot(v) && isNormalized() && v.isNormalized();
}


inline constexpr bool Vector3::isColinearTo(const Vector3& v) const noexcept
{
    return cross(v).isNull();
}


inline bool Vector3::isEqualTo(const Vector3& v, const f32 eps) const noexcept
{
    return fabs(x - v.x) <= eps && fabs(y - v.y) <= eps && fabs(z - v.z) <= eps;
}


inline bool Vector3::isNotEqualTo(const Vector3& v, const f32 eps) const noexcept
{
    return fabs(x - v.x) > eps || fabs(y - v.y) > eps || fabs(z - v.z) > eps;
}


inline void Vector3::normalize() noexcept
{
    *this /= length();
}

inline void Vector3::safelyNormalize() noexcept
{
    const f32 sqrLen{sqrLength()};

    if (sqrLen)
    {
        *this /= sqrtf(sqrLen);
    }
}


inline constexpr f32 Vector3::sqrDistanceTo(const Vector3& v) const noexcept
{
    return (*this - v).sqrLength();
}


inline f32 Vector3::distanceTo(const Vector3& v) const noexcept
{
    return (*this - v).length();
}


inline f32 Vector3::angleWithUnitary(const Vector3& v) const noexcept
{
    const f32 cosAngle{dot(v) / length()};

    return acosf(clamp(cosAngle, -1.f, 1.f));
}


inline f32 Vector3::angleWith(const Vector3& v) const noexcept
{
    const f32 cosAngle{dot(v) / sqrtf(sqrLength() * v.sqrLength())};

    return acosf(clamp(cosAngle, -1.f, 1.f));
}


inline f32 Vector3::signedAngleWithUnitary(const Vector3& v, const Vector3& planeNormal) const noexcept
{
    return atan2f((cross(v)).dot(planeNormal), dot(v));
}


inline f32 Vector3::triangleArea(const Vector3& v) const noexcept
{
    return cross(v).length() * .5f;
}


inline constexpr Vector3 Vector3::projectedOnUnitary(const Vector3& v) const noexcept
{
    return v * dot(v);
}


inline constexpr Vector3 Vector3::projectedOn(const Vector3& v) const noexcept
{
    return v * (dot(v) / v.sqrLength());
}


inline Vector3 Vector3::normalized() const noexcept
{
    return *this / length();
}


inline Vector3 Vector3::safelyNormalized() const noexcept
{
    const f32 sqrLen{sqrLength()};

    return sqrLen ? (*this / sqrtf(sqrLen)) : *this;
}


inline Vector3 Vector3::rotatedAroundUnitary(const Vector3& v, const f32 angle) const noexcept
{
	const Vector3   uxv     {v.cross(*this)};
	const f32	    cosAngle{cosf(angle)}, dp{dot(v)};

    return (*this * cosAngle) + v * (dp * (1.f - cosAngle)) + (uxv * sinf(angle));
}


inline Vector3 Vector3::rotatedAround(const Vector3& v, const f32 angle) const noexcept
{
    return rotatedAroundUnitary(v.normalized(), angle);
}


inline constexpr Vector3 Vector3::lerp(const Vector3& v, const f32 t) const noexcept
{
    const f32 tmp{1.f - t};

    return {(x * tmp) + (v.x * t), (y * tmp) + (v.y * t), (z * tmp) + (v.z * t)};
}


inline constexpr Vector3& Vector3::operator+=(const Vector3& v) noexcept
{
	x += v.x;
	y += v.y;
	z += v.z;

	return *this;
}


inline constexpr Vector3& Vector3::operator+=(const Vector3&& v) noexcept
{
	x += v.x;
	y += v.y;
	z += v.z;

	return *this;
}


inline constexpr Vector3& Vector3::operator-=(const Vector3& v) noexcept
{
	x -= v.x;
	y -= v.y;
	z -= v.z;

	return *this;
}


inline constexpr Vector3& Vector3::operator-=(const Vector3&& v) noexcept
{
	x -= v.x;
	y -= v.y;
	z -= v.z;

	return *this;
}


inline constexpr Vector3& Vector3::operator*=(const Vector3& v) noexcept
{
    x *= v.x;
    y *= v.y;
    z *= v.z;

    return *this;
}


inline constexpr Vector3& Vector3::operator*=(const Vector3&& v) noexcept
{
    x *= v.x;
    y *= v.y;
    z *= v.z;

    return *this;
}


inline constexpr Vector3& Vector3::operator/=(const Vector3& v) noexcept
{
    x /= v.x;
    y /= v.y;
    z /= v.z;

    return *this;
}


inline constexpr Vector3& Vector3::operator/=(const Vector3&& v) noexcept
{
    x /= v.x;
    y /= v.y;
    z /= v.z;

    return *this;
}


inline constexpr Vector3& Vector3::operator*=(const f32 k) noexcept
{
    x *= k;
    y *= k;
    z *= k;

    return *this;
}


inline constexpr Vector3& Vector3::operator/=(const f32 k) noexcept
{
    const f32 reciprocal{1.f / k};
    
    x *= reciprocal;
    y *= reciprocal;
    z *= reciprocal;

    return *this;
}


inline constexpr bool Vector3::operator==(const Vector3& v) const noexcept
{
    return x == v.x && y == v.y && z == v.z;
}


inline constexpr Vector3 Vector3::operator+(const Vector3& v) const noexcept
{
    return {x + v.x, y + v.y, z + v.z};
}


inline constexpr Vector3 Vector3::operator+(const Vector3&& v) const noexcept
{
    return {x + v.x, y + v.y, z + v.z};
}


inline constexpr Vector3 Vector3::operator-(const Vector3& v) const noexcept
{
    return {x - v.x, y - v.y, z - v.z};
}


inline constexpr Vector3 Vector3::operator-(const Vector3&& v) const noexcept
{
    return {x - v.x, y - v.y, z - v.z};
}


inline constexpr Vector3 Vector3::operator-() const noexcept
{
    return {-x, -y, -z};
}


inline constexpr Vector3 Vector3::operator*(const Vector3& v) const noexcept
{
    return {x * v.x, y * v.y, z * v.z};
}


inline constexpr Vector3 Vector3::operator*(const Vector3&& v) const noexcept
{
    return {x * v.x, y * v.y, z * v.z};
}


inline constexpr Vector3 Vector3::operator/(const Vector3& v) const noexcept
{
    return {x / v.x, y / v.y, z / v.z};
}


inline constexpr Vector3 Vector3::operator/(const Vector3&& v) const noexcept
{
    return {x / v.x, y / v.y, z / v.z};
}


inline constexpr Vector3 Vector3::operator*(const f32 k) const noexcept
{
    return {x * k, y * k, z * k};
}


inline constexpr Vector3 Vector3::operator/(const f32 k) const noexcept
{
    const f32 reciprocal{1.f / k};

    return {x * reciprocal, y * reciprocal, z * reciprocal};
}


inline constexpr Vector3 operator*(const f32 k, const Vector3& v) noexcept
{
    return v * k;
}