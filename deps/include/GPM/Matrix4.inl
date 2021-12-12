/* ======== Static methods, pseudo-constructors ======== */
inline constexpr Matrix4 Matrix4::zero() noexcept
{
    return
    {
        .0f, .0f, .0f, .0f,
        .0f, .0f, .0f, .0f,
        .0f, .0f, .0f, .0f,
        .0f, .0f, .0f, .0f
    };
}


inline constexpr Matrix4 Matrix4::identity() noexcept
{
    return
    {
        1.f, .0f, .0f, .0f,
        .0f, 1.f, .0f, .0f,
        .0f, .0f, 1.f, .0f,
        .0f, .0f, .0f, 1.f
    };
}




/* =================== Constructors =================== */
inline constexpr Matrix4::Matrix4(const f32 e0,  const f32 e1,  const f32 e2,  const f32 e3,
                                  const f32 e4,  const f32 e5,  const f32 e6,  const f32 e7,
                                  const f32 e8,  const f32 e9,  const f32 e10, const f32 e11,
                                  const f32 e12, const f32 e13, const f32 e14, const f32 e15) noexcept
    : e{e0, e1, e2, e3, e4, e5, e6, e7, e8, e9, e10, e11, e12, e13, e14, e15}
{}


inline constexpr Matrix4::Matrix4(const Vec4& c0, const Vec4& c1,
                                  const Vec4& c2, const Vec4& c3) noexcept
    : c{c0, c1, c2, c3}
{}




/* =================== Methods =================== */
inline constexpr Matrix4 Matrix4::cofactor() const noexcept
{
    // Repeated values
    const f32 a22a33{e[5]  * e[10]}, a23a34{e[9]  * e[14]}, a24a32{e[13] * e[6]},
              a24a33{e[13] * e[10]}, a23a32{e[9]  * e[6]},  a22a34{e[5]  * e[14]},
              a33a42{e[10] * e[7]},  a32a44{e[6]  * e[15]}, a34a43{e[14] * e[11]},
              a33a44{e[10] * e[15]}, a32a42{e[14] * e[7]},  a32a43{e[6]  * e[11]},
              a12a23{e[4]  * e[9]},  a13a24{e[8]  * e[13]}, a14a22{e[12] * e[5]},
              a14a23{e[12] * e[9]},  a13a22{e[8]  * e[5]},  a12a24{e[4]  * e[13]},
              a31a44{e[2]  * e[15]}, a31a43{e[2]  * e[11]}, a13a21{e[8]  * e[1]},
              a14a21{e[12] * e[1]},  a31a42{e[2]  * e[7]},  a11a24{e[0] * e[13]},
              a12a21{e[4] * e[1]},   a34a41{e[14] * e[3]},  a33a41{e[10] * e[3]},
              a32a41{e[6] * e[3]},   a11a23{e[0] * e[9]},   a11a22{e[0] * e[5]};

    return
    {
        (a22a33 * e[15]) + (a23a34 * e[7]) + (a24a32 * e[11])
        - (a24a33 * e[7])- (a23a32 * e[15]) - (a22a34 * e[11]),
        (e[12] * a33a42) + (e[8] * a32a44) + (e[4] * a34a43)
        - (e[4] * a33a44) - (e[8] * a32a42) - (e[12] * a32a43),
        (a12a23 * e[15]) + (a13a24 * e[7]) + (a14a22 * e[11])
        - (a14a23 * e[7]) - (a13a22 * e[15]) - (a12a24 * e[11]),
        (e[12] * a23a32) + (e[8] * a22a34) + (e[4] * a24a33)
        - (e[4] * a23a34) - (e[8] * a24a32) - (e[12] * a22a33),
        (a24a33 * e[3]) + (e[9] * a31a44) + (e[1] * a34a43)
        - (e[1] * a33a44) - (a23a34 * e[3]) - (e[13] * a31a43),
        (e[0] * a33a44) + (e[8] * a34a41) + (e[12] * a31a43)
        - (e[12] * a33a41) - (e[8] * a31a44) - (e[0] * a34a43),
        (a14a23 * e[3]) + (a13a21 * e[15]) + (a11a24 * e[11])
        - (a11a23 * e[15]) - (a13a24 * e[3]) - (a14a21 * e[11]),
        (e[0] * a23a34) + (a13a24 * e[2]) + (a14a21 * e[10])
        - (a14a23 * e[2]) - (a13a21 * e[14]) - (e[0] * a24a33),
        (e[1] * a32a44) + (a22a34 * e[3]) + (e[13] * a31a42)
        - (a24a32 * e[3]) - (e[5] * a31a44) - (e[1] * a32a42),
        (e[12] * a32a41) + (e[4] * a31a44) + (e[0] * a32a42)
        - (e[0] * a32a44) - (e[4] * a34a41) - (e[12] * a31a42),
        (a11a22 * e[15]) + (a12a24 * e[3]) + (a14a21 * e[7])
        - (a14a22 * e[3]) - (a12a21 * e[15]) - (a11a24 * e[7]),
        (a14a22 * e[2]) + (a12a21 * e[14]) + (e[0] * a24a32)
        - (e[0] * a22a34) - (a12a24 * e[2]) - (a14a21 * e[6]),
        (a23a32 * e[3]) + (e[5] * a31a43) + (e[1] * a33a42)
        - (e[1] * a32a43) - (a22a33 * e[3]) - (e[9] * a31a42),
        (e[0] * a32a43) + (e[4] * a33a41) + (e[8] * a31a42)
        - (e[8] * a32a41) - (e[4] * a31a43) - (e[0] * a33a42),
        (a13a22 * e[3]) + (a12a21 * e[11]) + (a11a23 * e[7])
        - (a11a22 * e[11]) - (a12a23 * e[3]) - (a13a21 * e[7]),
        (e[0] * a22a33) + (a12a23 * e[2]) + (a13a21 * e[6])
        - (a13a22 * e[2]) - (a12a21 * e[10]) - (e[0] * a23a32)
    };
}


inline constexpr Matrix4 Matrix4::transposed() const noexcept
{
    return
    {
        e[0], e[4], e[8],  e[12],
        e[1], e[5], e[9],  e[13],
        e[2], e[6], e[10], e[14],
        e[3], e[7], e[11], e[15]
    };
}


inline constexpr Matrix4 Matrix4::adjugate() const noexcept
{ return cofactor().transposed(); }


inline constexpr f32 Matrix4::det() const noexcept
{
    const f32 det1{(e[10] * e[15]) - (e[14] * e[11])},
              det2{(e[2]  * e[15]) - (e[14] * e[3])},
              det3{(e[6]  * e[15]) - (e[14] * e[7])},
              det4{(e[6]  * e[11]) - (e[10] * e[7])},
              det5{(e[2]  * e[11]) - (e[10] * e[3])},
              det6{(e[2]  * e[7])  - (e[6]  * e[3])};

    return *e      * ((e[5] * det1) - (e[9] * det3) + (e[13] * det4))
           - e[4]  * ((e[1] * det1) - (e[9] * det2) + (e[13] * det5))
           + e[8]  * ((e[1] * det3) - (e[5] * det2) + (e[13] * det6))
           - e[12] * ((e[1] * det4) - (e[5] * det5) + (e[9]  * det6));
}


inline constexpr Matrix4 Matrix4::inversed() const noexcept
{ return adjugate() / det(); }


inline constexpr f32 Matrix4::trace() const noexcept
{ return e[0] + e[5] + e[10] + e[15]; }


inline bool Matrix4::isEqualTo(const Matrix4& m, const f32 eps) const noexcept
{
    return c[0].isEqualTo(m.c[0], eps) && c[1].isEqualTo(m.c[1], eps) &&
           c[2].isEqualTo(m.c[2], eps) && c[3].isEqualTo(m.c[3], eps);
}




/* ================ Operator overloads ================= */
inline constexpr bool Matrix4::operator==(const Matrix4& m) const noexcept
{
    return (c[0] == m.c[0]) && (c[1] == m.c[1]) &&
           (c[2] == m.c[2]) && (c[3] == m.c[3]);
}


// This actually does m * *this
inline constexpr Matrix4& Matrix4::operator*=(const Matrix4& m) noexcept
{
    e[0]  = (e[0] * m.e[0]) + (e[4] * m.e[4]) + (e[8]  * m.e[8])  + (e[12] * m.e[12]);
    e[1]  = (e[1] * m.e[0]) + (e[5] * m.e[4]) + (e[9]  * m.e[8])  + (e[13] * m.e[12]);
    e[2]  = (e[2] * m.e[0]) + (e[6] * m.e[4]) + (e[10] * m.e[8])  + (e[14] * m.e[12]);
    e[3]  = (e[3] * m.e[0]) + (e[7] * m.e[4]) + (e[11] * m.e[8])  + (e[15] * m.e[12]);
    e[4]  = (e[0] * m.e[1]) + (e[4] * m.e[5]) + (e[8]  * m.e[9])  + (e[12] * m.e[13]);
    e[5]  = (e[1] * m.e[1]) + (e[5] * m.e[5]) + (e[9]  * m.e[9])  + (e[13] * m.e[13]);
    e[6]  = (e[2] * m.e[1]) + (e[6] * m.e[5]) + (e[10] * m.e[9])  + (e[14] * m.e[13]);
    e[7]  = (e[3] * m.e[1]) + (e[7] * m.e[5]) + (e[11] * m.e[9])  + (e[15] * m.e[13]);
    e[8]  = (e[0] * m.e[2]) + (e[4] * m.e[6]) + (e[8]  * m.e[10]) + (e[12] * m.e[14]);
    e[9]  = (e[1] * m.e[2]) + (e[5] * m.e[6]) + (e[9]  * m.e[10]) + (e[13] * m.e[14]);
    e[10] = (e[2] * m.e[2]) + (e[6] * m.e[6]) + (e[10] * m.e[10]) + (e[14] * m.e[14]);
    e[11] = (e[3] * m.e[2]) + (e[7] * m.e[6]) + (e[11] * m.e[10]) + (e[15] * m.e[14]);
    e[12] = (e[0] * m.e[3]) + (e[4] * m.e[7]) + (e[8]  * m.e[11]) + (e[12] * m.e[15]);
    e[13] = (e[1] * m.e[3]) + (e[5] * m.e[7]) + (e[9]  * m.e[11]) + (e[13] * m.e[15]);
    e[14] = (e[2] * m.e[3]) + (e[6] * m.e[7]) + (e[10] * m.e[11]) + (e[14] * m.e[15]);
    e[15] = (e[3] * m.e[3]) + (e[7] * m.e[7]) + (e[11] * m.e[11]) + (e[15] * m.e[15]);

    return *this;
}


inline constexpr Matrix4& Matrix4::operator/=(const f32 k) noexcept
{
    const f32 reciprocal{1.f / k};

    c[0] *= reciprocal;
    c[1] *= reciprocal;
    c[2] *= reciprocal;
    c[3] *= reciprocal;

    return *this;
}


inline constexpr Vec4 Matrix4::operator*(const Vec4& v) const noexcept
{
    //return
    //{
    //    (e[0] * v.xyz.x) + (e[4] * v.xyz.y) + (e[8] * v.xyz.z) + (e[12] * v.w),
    //    (e[1] * v.xyz.x) + (e[5] * v.xyz.y) + (e[9] * v.xyz.z) + (e[13] * v.w),
    //    (e[2] * v.xyz.x) + (e[6] * v.xyz.y) + (e[10] * v.xyz.z) + (e[14] * v.w),
    //    (e[3] * v.xyz.x) + (e[7] * v.xyz.y) + (e[11] * v.xyz.z) + (e[15] * v.w)
    //};

    return
    {
        (e[0] * v.xyz.x) + (e[1] * v.xyz.y) + (e[2] * v.xyz.z) + (e[3] * v.w),
        (e[4] * v.xyz.x) + (e[5] * v.xyz.y) + (e[6] * v.xyz.z) + (e[7] * v.w),
        (e[8] * v.xyz.x) + (e[9] * v.xyz.y) + (e[10] * v.xyz.z) + (e[11] * v.w),
        (e[12] * v.xyz.x) + (e[13] * v.xyz.y) + (e[14] * v.xyz.z) + (e[15] * v.w)
    };
}


inline constexpr Matrix4 Matrix4::operator*(const Matrix4& m) const noexcept
{
    return
    {
        (e[0] * m.e[0])  + (e[4] * m.e[1])  + (e[8]  * m.e[2])  + (e[12] * m.e[3]),
        (e[1] * m.e[0])  + (e[5] * m.e[1])  + (e[9]  * m.e[2])  + (e[13] * m.e[3]),
        (e[2] * m.e[0])  + (e[6] * m.e[1])  + (e[10] * m.e[2])  + (e[14] * m.e[3]),
        (e[3] * m.e[0])  + (e[7] * m.e[1])  + (e[11] * m.e[2])  + (e[15] * m.e[3]),
        (e[0] * m.e[4])  + (e[4] * m.e[5])  + (e[8]  * m.e[6])  + (e[12] * m.e[7]),
        (e[1] * m.e[4])  + (e[5] * m.e[5])  + (e[9]  * m.e[6])  + (e[13] * m.e[7]),
        (e[2] * m.e[4])  + (e[6] * m.e[5])  + (e[10] * m.e[6])  + (e[14] * m.e[7]),
        (e[3] * m.e[4])  + (e[7] * m.e[5])  + (e[11] * m.e[6])  + (e[15] * m.e[7]),
        (e[0] * m.e[8])  + (e[4] * m.e[9])  + (e[8]  * m.e[10]) + (e[12] * m.e[11]),
        (e[1] * m.e[8])  + (e[5] * m.e[9])  + (e[9]  * m.e[10]) + (e[13] * m.e[11]),
        (e[2] * m.e[8])  + (e[6] * m.e[9])  + (e[10] * m.e[10]) + (e[14] * m.e[11]),
        (e[3] * m.e[8])  + (e[7] * m.e[9])  + (e[11] * m.e[10]) + (e[15] * m.e[11]),
        (e[0] * m.e[12]) + (e[4] * m.e[13]) + (e[8]  * m.e[14]) + (e[12] * m.e[15]),
        (e[1] * m.e[12]) + (e[5] * m.e[13]) + (e[9]  * m.e[14]) + (e[13] * m.e[15]),
        (e[2] * m.e[12]) + (e[6] * m.e[13]) + (e[10] * m.e[14]) + (e[14] * m.e[15]),
        (e[3] * m.e[12]) + (e[7] * m.e[13]) + (e[11] * m.e[14]) + (e[15] * m.e[15])
    };
}


inline constexpr Matrix4 Matrix4::operator/(const f32 k) const noexcept
{
    const f32 reciprocal{1.f / k};

    return
    {
        e[0]  * reciprocal, e[1]  * reciprocal, e[2]  * reciprocal, e[3]  * reciprocal,
        e[4]  * reciprocal, e[5]  * reciprocal, e[6]  * reciprocal, e[7]  * reciprocal,
        e[8]  * reciprocal, e[9]  * reciprocal, e[10] * reciprocal, e[11] * reciprocal,
        e[12] * reciprocal, e[13] * reciprocal, e[14] * reciprocal, e[15] * reciprocal,
    };
}