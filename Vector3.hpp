#ifndef engine_VECTOR3_HPP


#if defined( _MSVC_LANG )
#  define MATH_VECTOR_VERSION _MSVC_LANG
#else
#  define MATH_VECTOR_VERSION __cplusplus
#endif

#if 201103L < MATH_VECTOR_VERSION
#  define MATH_VECTOR_VERSION_CPP 14
#elif 199711L < MATH_VECTOR_VERSION
#  define MATH_VECTOR_VERSION_CPP 11
#else
#define MATH_VECTOR_VERSION_CPP 0
#endif

#if MATH_VECTOR_VERSION_CPP == 14
#define __constexpr__ constexpr
#else
#define __constexpr__
#endif

namespace math {
    template<class T>
    class vec3 {
        typedef vec3<T> v3;
    public:
        T x, y, z;
        __constexpr__ vec3(T _x, T _y, T _z) :
                x(_x), y(_y), z(_z) {}

        //Values +-*/
        __constexpr__ v3 operator+(const T &value) const noexcept {
            return vec3<T>{
                    x + value,
                    y + value,
                    z + value
            };
        }

        __constexpr__ v3 operator-(const T &value) const noexcept {
            return v3{
                    x - value,
                    y - value,
                    z - value
            };
        }

        __constexpr__ v3 operator*(const T &value) const noexcept {
            return v3{
                    x * value,
                    y * value,
                    z * value
            };
        }

        __constexpr__ v3 operator/(const T &value) const noexcept {
            if(value == 0) return v3{0, 0, 0};
            return v3{
                    x / value,
                    y / value,
                    z / value
            };
        }

        //Vectors +-*/
        __constexpr__ v3 operator+(const v3 &value) const noexcept {
            return v3{
                    this->x + value.x,
                    this->y + value.y,
                    this->z + value.z
            };
        }

        __constexpr__ v3 operator-(const v3 &value) const noexcept {
            return v3{
                    this->x - value.x,
                    this->y - value.y,
                    this->z - value.z
            };
        }

        __constexpr__ v3 operator*(const v3 &value) const noexcept {
            return v3{
                    this->x * value.x,
                    this->y * value.y,
                    this->z * value.z
            };
        }

        __constexpr__ v3 operator/(const v3 &value) const noexcept {
            return v3{
                    (value.x == 0 ? 0 : this->x / value.x),
                    (value.y == 0 ? 0 : this->y / value.y),
                    (value.z == 0 ? 0 : this->z / value.z)
            };
        }

        //Values operator s=
        v3 &operator+=(const T &value) noexcept {
            this->x += value;
            this->y += value;
            this->z += value;
            return *this;
        }


        v3 &operator-=(const T &value) noexcept {
            this->x -= value;
            this->y -= value;
            this->z -= value;
            return *this;
        }

        v3 &operator*=(const T &value) noexcept {
            this->x *= value;
            this->y *= value;
            this->z *= value;
            return *this;
        }

        v3 &operator/=(const T &value) noexcept {
            if(value == 0) {
                this->x = this->y = this->z = 0;
            } else {
                this->x /= value;
                this->y /= value;
                this->z /= value;
            }
            return *this;
        }

        //Vec2 operators s=
        v3 &operator+=(const v3 &value) noexcept {
            this->x += value.y;
            this->y += value.x;
            this->z += value.z;
            return *this;
        }

        v3 &operator-=(const v3 &value) noexcept {
            this->x -= value.y;
            this->y -= value.x;
            this->z -= value.z;
            return *this;
        }

        v3 &operator*=(const v3 &value) noexcept {
            this->x *= value.y;
            this->y *= value.x;
            this->z -= value.z;
            return *this;
        }

        v3 &operator/=(const v3 &value) noexcept {
            if(value.x != 0) this->x /= value.y;
            else this->x = 0;
            if(value.y != 0) this->y /= value.x;
            else this->y = 0;
            if(value.z != 0) this->z /= value.z;
            else this->z = 0;
            return *this;
        }
    };

    typedef vec3<float>     vec3f;
    typedef vec3<int>       vec3i;
    typedef vec3<double>    vec3d;
    typedef vec3<unsigned>  vec3u;
}
#undef MATH_VECTOR_VERSION
#undef MATH_VECTOR_VERSION_CPP
#undef __constexpr__
#endif
