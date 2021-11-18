inline std::ostream& operator<<(std::ostream& os, const Matrix3& mat) noexcept
{
    std::streamsize original = std::cout.precision();
    os << std::setprecision(2);

    for (u8 i{0u}; i < MAT3_COL; ++i)
    {
        u8 induction{i};
        
        os << "| ";
        for (u8 j{MAT3_COL}; j--; induction += MAT3_COL)
            os << std::setw(6) << mat.e[induction] << ' ';

        os << " |";

        if (i != MAT3_COL - 1u)
            os << '\n';
    }

    os << std::setprecision(original);

    return os;
}


inline std::ostream& operator<<(std::ostream& os, const Matrix4& mat) noexcept
{
    std::streamsize original = std::cout.precision();
    os << std::setprecision(2);

    for (u8 i{0u}; i < MAT4_COL; ++i)
    {
        u8 induction{i};
        
        os << "| ";
        for (u8 j{MAT4_COL}; j--; induction += MAT4_COL)
            os << std::setw(10) << mat.e[induction] << ' ';

        os << " |";
        
        if (i != MAT4_COL - 1u)
            os << '\n';
    }

    os << std::setprecision(original);

    return os;
}


inline std::ostream& operator<<(std::ostream& os, const Quaternion& q) noexcept
{ return os << '[' << q.w << ", " << q.v << ']'; }


inline std::ostream& operator<<(std::ostream& os, const SplitTransform& st) noexcept
{
    return os << "rotation: "   << st.rotation
              << "\nposition: " << st.position
              << "\nscale: "    << st.scale;
}


inline std::ostream& operator<<(std::ostream& os, const Transform& t) noexcept
{ return os << t.model; }


inline std::ostream& operator<<(std::ostream& os, const Vector2& v) noexcept
{ return os << '[' << v.x << ", " << v.y << ']'; }


inline std::ostream& operator<<(std::ostream& os, const Vector3& v) noexcept
{ return os << '[' << v.x << ", " << v.y << ", " << v.z << ']'; }


inline std::ostream& operator<<(std::ostream& os, const Vector4& v) noexcept
{ return os << '[' << v.xyz.x << ", " << v.xyz.y << ", " << v.xyz.z << ", " << v.w << ']'; }