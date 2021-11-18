/* =================== Constructors =================== */
inline constexpr Vector2::Vector2(const f32 k) noexcept
	: x{k}, y{k}
{}


inline constexpr Vector2::Vector2(const f32 x_, const f32 y_) noexcept
	: x{x_}, y{y_}
{}


inline constexpr Vector2::Vector2(const f32 coef[2]) noexcept
	: e{coef[0], coef[1]}
{}




/* =================== Static methods (pseudo-constructors) =================== */
inline constexpr Vector2 Vector2::zero() noexcept
{ return {.0f, .0f}; }


inline constexpr Vector2 one() noexcept
{ return {1.f, 1.f}; }


inline constexpr Vector2 Vector2::right() noexcept
{ return {1.f, .0f}; }


inline constexpr Vector2 Vector2::up() noexcept
{ return {.0f, 1.f}; }




/* =================== Methods =================== */
inline constexpr f32 Vector2::sqrLength() const noexcept
{ return (x * x) + (y * y); }


inline f32 Vector2::length() const noexcept
{ return sqrtf(sqrLength()); }


inline constexpr f32 Vector2::dot(const Vector2& v) const noexcept
{ return (x * v.x) + (y * v.y); }


inline constexpr f32 Vector2::cross(const Vector2& v) const noexcept
{ return (x * v.y) - (y * v.x); }


inline constexpr bool Vector2::isNull() const noexcept
{ return !x && !y; }


inline constexpr bool Vector2::isOrthogonalTo(const Vector2& v) const noexcept
{ return !dot(v); }


inline constexpr bool Vector2::isNormalized() const noexcept
{ return !(sqrLength() - 1.f); }


inline constexpr bool Vector2::isOrthonormalTo(const Vector2& v) const noexcept
{ return !dot(v) && isNormalized() && v.isNormalized(); }


inline constexpr bool Vector2::isColinearTo(const Vector2& v) const noexcept
{ return !cross(v); }


inline bool Vector2::isEqualTo(const Vector2& v, const f32 eps) const noexcept
{ return fabs(x - v.x) <= eps && fabs(y - v.y) <= eps; }


inline bool Vector2::isNotEqualTo(const Vector2& v, const f32 eps) const noexcept
{ return fabs(x - v.x) > eps || fabs(y - v.y) > eps; }


inline void Vector2::normalize() noexcept
{
	*this /= length();
}


inline void Vector2::safelyNormalize() noexcept
{
    const f32 sqrLen{sqrLength()};

    if (sqrLen)    
	    *this /= sqrtf(sqrLen);
}


inline constexpr f32 Vector2::sqrDistanceTo(const Vector2& v) const noexcept
{ return (*this - v).sqrLength(); }


inline f32 Vector2::distanceTo(const Vector2& v) const noexcept
{ return (*this - v).length(); }


inline f32 Vector2::angleWithUnitary(const Vector2& v) const noexcept
{ return acosf(dot(v) / length()); }


inline f32 Vector2::angleWith(const Vector2& v) const noexcept
{ return acosf(dot(v) / sqrtf(sqrLength() * v.sqrLength())); }


inline constexpr f32 Vector2::triangleArea(const Vector2& v) const noexcept
{ return cross(v) * .5f; }


inline constexpr Vector2 Vector2::projectedOnUnitary(const Vector2& v) const noexcept
{ return v * dot(v); }


inline constexpr Vector2 Vector2::projectedOn(const Vector2& v) const noexcept
{ return v * (dot(v) / v.sqrLength()); }


inline Vector2 Vector2::normalized() const noexcept
{ return *this / length(); }


inline Vector2 Vector2::safelyNormalized() const noexcept
{
    const f32 sqrLen{sqrLength()};

    return sqrLen ? (*this / sqrtf(sqrLen)) : *this;
}


inline Vector2 Vector2::rotated(const f32 angle) const noexcept
{
	const f32 cosAngle{cosf(angle)}, sinAngle{sinf(angle)};

	return {(cosAngle * x) - (sinAngle * y), (sinAngle * x) + (cosAngle * y)};
}


inline constexpr Vector2 Vector2::rotated90() const noexcept
{ return {y, -x}; }


inline constexpr Vector2 Vector2::lerp(const Vector2& v, const f32 t) const noexcept
{
	const f32 tmp{1.f - t};

    return {(x * tmp) + (v.x * t), (y * tmp) + (v.y * t)};
}


inline constexpr Vector2& Vector2::operator+=(const Vector2& v) noexcept
{
	x += v.x;
	y += v.y;

	return *this;
}


inline constexpr Vector2& Vector2::operator+=(const Vector2&& v) noexcept
{
	x += v.x;
	y += v.y;

	return *this;
}


inline constexpr Vector2& Vector2::operator-=(const Vector2& v) noexcept
{
	x -= v.x;
	y -= v.y;

	return *this;
}


inline constexpr Vector2& Vector2::operator-=(const Vector2&& v) noexcept
{
	x -= v.x;
	y -= v.y;

	return *this;
}


inline constexpr Vector2& Vector2::operator*=(const Vector2& v) noexcept
{
	x *= v.x;
	y *= v.y;

	return *this;
}


inline constexpr Vector2& Vector2::operator*=(const Vector2&& v) noexcept
{
	x *= v.x;
	y *= v.y;

	return *this;
}


inline constexpr Vector2& Vector2::operator/=(const Vector2& v) noexcept
{
	const f32 reciprocal{1.f / (v.x * v.y)};

	x *= v.y * reciprocal;
	y *= v.x * reciprocal;

	return *this;
}


inline constexpr Vector2& Vector2::operator/=(const Vector2&& v) noexcept
{
	const f32 reciprocal{1.f / (v.x * v.y)};
	x *= v.y * reciprocal;
	y *= v.x * reciprocal;

	return *this;
}


inline constexpr Vector2& Vector2::operator*=(const f32 k) noexcept
{
	x *= k;
	y *= k;

	return *this;
}


inline constexpr Vector2& Vector2::operator/=(const f32 k) noexcept
{
	const f32 reciprocal{1.f / k};

	x *= reciprocal;
	y *= reciprocal;

	return *this;
}


inline constexpr Vector2 Vector2::operator+(const Vector2& v) const noexcept
{ return {x + v.x, y + v.y}; }


inline constexpr Vector2 Vector2::operator+(const Vector2&& v) const noexcept
{ return {x + v.x, y + v.y}; }


inline constexpr Vector2 Vector2::operator*(const Vector2& v) const noexcept
{ return {x * v.x, y * v.y}; }


inline constexpr Vector2 Vector2::operator*(const Vector2&& v) const noexcept
{ return {x * v.x, y * v.y}; }


inline constexpr Vector2 Vector2::operator/(const Vector2& v) const noexcept
{
	const f32 reciprocal{1.f / (v.x * v.y)};

	return {x * v.y * reciprocal, y * v.x * reciprocal};
}


inline constexpr Vector2 Vector2::operator/(const Vector2&& v) const noexcept
{
	const f32 reciprocal{1.f / (v.x * v.y)};

	return {x * v.y * reciprocal, y * v.x * reciprocal};
}


inline constexpr Vector2 Vector2::operator-() const noexcept
{ return {-x, -y}; }


inline constexpr Vector2 Vector2::operator-(const Vector2& v) const noexcept
{ return {x - v.x, y - v.y}; }


inline constexpr Vector2 Vector2::operator-(const Vector2&& v) const noexcept
{ return {x - v.x, y - v.y}; }


inline constexpr Vector2 Vector2::operator*(const f32 k) const noexcept
{ return {x * k, y * k}; }


inline constexpr Vector2 Vector2::operator/(const f32 k) const noexcept
{
	const f32 reciprocal{1.f / k};

	return {x * reciprocal, y * reciprocal};
}