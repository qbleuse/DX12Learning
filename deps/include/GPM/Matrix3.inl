/* ======== Static methods, pseudo-constructors ======== */
inline constexpr Matrix3 Matrix3::zero() noexcept
{
    return
    {
        .0f, .0f, .0f,
        .0f, .0f, .0f,
        .0f, .0f, .0f
    };
}


inline constexpr Matrix3 Matrix3::identity() noexcept
{
    return
    {
        1.f, .0f, .0f,
        .0f, 1.f, .0f,
        .0f, .0f, 1.f
    };
}




/* =================== Methods =================== */
inline constexpr Matrix3 Matrix3::cofactor() const noexcept
{
    // Repeated values
    return
    {
        (e[4] * e[8]) - (e[7] * e[5]), (e[7] * e[2]) - (e[1] * e[8]), (e[1] * e[5]) - (e[4] * e[2]),
        (e[6] * e[5]) - (e[3] * e[8]), (e[0] * e[8]) - (e[6] * e[2]), (e[3] * e[2]) - (e[0] * e[5]),
        (e[0] * e[7]) - (e[6] * e[4]), (e[6] * e[1]) - (e[0] * e[7]), (e[0] * e[4]) - (e[3] * e[1])
    };
}


inline constexpr Matrix3 Matrix3::transposed() const noexcept
{
    return
    {
        e[0], e[3], e[6],
        e[1], e[4], e[7],
        e[2], e[5], e[8]
    };
}


inline constexpr Matrix3 Matrix3::adjugate() const noexcept
{ return cofactor().transposed(); }


inline constexpr f32 Matrix3::det() const noexcept
{
    return   e[0] * (e[4] * e[8] - e[7] * e[5])
           - e[1] * (e[3] * e[8] - e[6] * e[5])
           + e[2] * (e[3] * e[7] + e[6] * e[4]);
}


inline constexpr Matrix3 Matrix3::inversed() const noexcept
{ return adjugate() / det(); }



inline constexpr f32 Matrix3::trace() const noexcept
{ return e[0] + e[4] + e[8]; }



/* ================ Operator overloads ================= */
// This actually does m * *this
inline constexpr Matrix3& Matrix3::operator*=(const Matrix3& m) noexcept
{
    e[0] = (m.e[0] * e[0]) + (m.e[3] * e[1]) + (m.e[6] * e[2]);
    e[1] = (m.e[0] * e[3]) + (m.e[3] * e[4]) + (m.e[6] * e[5]);
    e[2] = (m.e[0] * e[6]) + (m.e[3] * e[7]) + (m.e[6] * e[8]);
    e[3] = (m.e[0] * e[0]) + (m.e[3] * e[1]) + (m.e[6] * e[2]);
    e[4] = (m.e[0] * e[3]) + (m.e[3] * e[4]) + (m.e[6] * e[5]);
    e[5] = (m.e[0] * e[6]) + (m.e[3] * e[7]) + (m.e[6] * e[8]);
    e[6] = (m.e[0] * e[0]) + (m.e[3] * e[1]) + (m.e[6] * e[2]);
    e[7] = (m.e[0] * e[3]) + (m.e[3] * e[4]) + (m.e[6] * e[5]);
    e[8] = (m.e[0] * e[6]) + (m.e[3] * e[7]) + (m.e[6] * e[8]);

    return *this;
}


inline constexpr Matrix3& Matrix3::operator/=(const f32 k) noexcept
{
    const f32 reciprocal{1.f / k};

    e[0] *= reciprocal;
    e[1] *= reciprocal;
    e[2] *= reciprocal;
    e[3] *= reciprocal;
    e[4] *= reciprocal;
    e[5] *= reciprocal;
    e[6] *= reciprocal;
    e[7] *= reciprocal;
    e[8] *= reciprocal;

    return *this;
}


inline constexpr Vec3 Matrix3::operator*(const Vec3& v) const noexcept
{
    return
    {
        (e[0] * v.x) + (e[3] * v.y) + (e[6] * v.z),
        (e[1] * v.x) + (e[4] * v.y) + (e[7] * v.z),
        (e[2] * v.x) + (e[5] * v.y) + (e[8] * v.z)
    };
}


inline constexpr Matrix3 Matrix3::operator*(const Matrix3& m) const noexcept
{
    return
    {
        (e[0] * m.e[0]) + (e[3] * m.e[1]) + (e[6] * m.e[2]),
        (e[0] * m.e[3]) + (e[3] * m.e[4]) + (e[6] * m.e[5]),
        (e[0] * m.e[6]) + (e[3] * m.e[7]) + (e[6] * m.e[8]),
        (e[1] * m.e[0]) + (e[4] * m.e[1]) + (e[7] * m.e[2]),
        (e[1] * m.e[3]) + (e[4] * m.e[4]) + (e[7] * m.e[5]),
        (e[1] * m.e[6]) + (e[4] * m.e[7]) + (e[7] * m.e[8]),
        (e[2] * m.e[0]) + (e[5] * m.e[1]) + (e[8] * m.e[2]),
        (e[2] * m.e[3]) + (e[5] * m.e[4]) + (e[8] * m.e[5]),
        (e[2] * m.e[6]) + (e[5] * m.e[7]) + (e[8] * m.e[8])
    };
}


inline constexpr Matrix3 Matrix3::operator/(const f32 k) const noexcept
{
    const f32 reciprocal{1.f / k};
    
    return
    {
        e[0] * reciprocal, e[1] * reciprocal, e[2] * reciprocal,
        e[3] * reciprocal, e[4] * reciprocal, e[5] * reciprocal,
        e[6] * reciprocal, e[7] * reciprocal, e[8] * reciprocal
    };
}