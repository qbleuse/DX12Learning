/* ================= Inlined conversion methods ================= */
inline constexpr Mat3 toMatrix3(const Mat4& m) noexcept
{
    return
    {
        m.e[0], m.e[1], m.e[2],
        m.e[4], m.e[5], m.e[6],
        m.e[8], m.e[9], m.e[10]
    };
}


inline constexpr Mat4 toMatrix4(const Mat3& m) noexcept
{
    return
    {
        m.e[0], m.e[1], m.e[2], .0f,
        m.e[3], m.e[4], m.e[5], .0f,
        m.e[6], m.e[7], m.e[8], .0f,
        .0f,    .0f,    .0f,    1.f
    };
}


inline constexpr Mat3 toMatrix3(const Quat& q) noexcept
{
    const f32 x2{q.x * q.x}, y2{q.y * q.y}, z2{q.z * q.z},
              xy{q.x * q.y}, yz{q.y * q.z}, xz{q.x * q.z},
              wx{q.w * q.x}, wy{q.w * q.y}, wz{q.w * q.z},
              s_2{2.f / q.sqrLength()};

    // Column-major
    return
    {
        1.f - s_2 * (y2 + z2), s_2 * (xy + wz),       s_2 * (xz - wy),
        s_2 * (xy - wz),       1.f - s_2 * (x2 + z2), s_2 * (yz + wx),
        s_2 * (xz + wy),       s_2 * (yz - wx),       1.f - s_2 * (x2 + y2)
    };
}


inline constexpr Mat4 toMatrix4(const Quat& q) noexcept
{
    return toMatrix4(toMatrix3(q));
}


inline Quat toQuaternion(const Mat3& m) noexcept
{
    Quat q;
    const f32 trace = m.trace();

    if (trace > .0f)
    {
        f32 tmp = sqrtf(trace + 1.f);

        q.w = tmp * .5f;

        tmp = .5f / tmp;

        q.v.x = (m.e[5] - m.e[7]) * tmp;
        q.v.y = (m.e[6] - m.e[2]) * tmp;
        q.v.z = (m.e[1] - m.e[3]) * tmp;
    }

    else
    {
        u8 i = 0;
        if (m.e[4] > m.e[0])      i = 1u;
        if (m.e[8] > m.c[i].e[i]) i = 2u;

        constexpr u8 NEXT[3]{1u, 2u, 0u};
        const     u8 j = NEXT[i], k = NEXT[j];

        f32 tmp = sqrtf(m.c[i].e[i] - (m.c[j].e[j] + m.c[k].e[k]) + 1.f);

        q.e[i] = tmp * .5f;

        if (tmp != .0f)
            tmp = .5f / tmp;

        q.w    = (m.c[j].e[k] - m.c[k].e[j]) * tmp;
        q.e[j] = (m.c[i].e[j] + m.c[j].e[i]) * tmp;
        q.e[k] = (m.c[i].e[k] + m.c[k].e[i]) * tmp;
    }

    return q;
}


inline Quat toQuaternion(const Mat4& m) noexcept
{
    return toQuaternion(toMatrix3(m));
}


inline SplitTransform toSplitTransform(const Transform& transfo) noexcept
{
    return {toQuaternion(transfo.rotation()), transfo.translation(), transfo.scaling()};
}


inline Transform toTransform(const SplitTransform& transfo) noexcept
{
    return
    {
        Transform::translation(transfo.position) *
        toMatrix4(transfo.rotation) *
        Transform::scaling(transfo.scale)
    };
}

