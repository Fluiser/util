#ifndef engine_VECTOR2_HPP

#if defined( _MSVC_LANG )
#  define VERSION _MSVC_LANG
#else
#  define VERSION __cplusplus
#endif

#if 201103L < VERSION
#  define VERSION_CPP 14
#elif 199711L < VERSION
#  define VERSION_CPP 11
#else
#  error "Vector3.hpp needs at least c++ standrt version 11"
#endif

#if VERSION_CPP == 14
#define __constexpr__ constexpr
#else
#define __constexpr__
#endif

namespace math {
    template<class T>
    class vec2 {
        typedef vec2<T> v2;
    public:
        T x, y;
        constexpr vec2(T _x, T _y) :
                x(_x), y(_y) {}

        //Values +-*/
        constexpr v2 operator+(const T &value) const noexcept {
            return v2{
                    x + value,
                    y + value
            };
        }

        constexpr v2 operator-(const T &value) const noexcept {
            return v2{
                    x - value,
                    y - value
            };
        }

        constexpr v2 operator*(const T &value) const noexcept {
            return v2{
                    x * value,
                    y * value
            };
        }

        constexpr v2 operator/(const T &value) const noexcept {
        	if(value == 0) return v2{0, 0};
            return v2{
                    x / value,
                    y / value
            };
        }

        //Vectors +-*/
        constexpr v2 operator+(const v2 &value) const noexcept {
            return v2{
                    this->x + value.x,
                    this->y + value.y
            };
        }

        constexpr v2 operator-(const v2 &value) const noexcept {
            return v2{
                    this->x - value.x,
                    this->y - value.y
            };
        }

        constexpr v2 operator*(const v2 &value) const noexcept {
            return v2{
                    this->x * value.x,
                    this->y * value.y
            };
        }

        constexpr v2 operator/(const v2 &value) const noexcept {
            return v2{
                    (value.x == 0 ? 0 : this->x / value.x),
                    (value.y == 0 ? 0 : this->y / value.y)
            };
        }

        //Values operator s=
        __constexpr__ v2 &operator+=(const T &value) noexcept {
            this->x += value;
            this->y += value;
            return *this;
        }

        __constexpr__ v2 &operator-=(const T &value) noexcept {
            this->x -= value;
            this->y -= value;
            return *this;
        }

        __constexpr__ v2 &operator*=(const T &value) noexcept {
            this->x *= value;
            this->y *= value;
            return *this;
        }

        __constexpr__ v2 &operator/=(const T &value) noexcept {
            if(value != 0)
            {
				this->x /= value;
	            this->y /= value;
			}
			else
			{
				this->x = this->y = 0;
			}
			return *this;
        }

        //Vec2 operators s=
        __constexpr__ v2 &operator+=(const v2 &value) noexcept {
            this->x += value.y;
            this->y += value.x;
            return *this;
        }

        __constexpr__ v2 &operator-=(const v2 &value) noexcept {
            this->x -= value.y;
            this->y -= value.x;
            return *this;
        }

        __constexpr__ v2 &operator*=(const v2 &value) noexcept {
            this->x *= value.y;
            this->y *= value.x;
            return *this;
        }

        __constexpr__ v2 &operator/=(const v2 &value) noexcept {
            if(value.y != 0)
            	this->x /= value.y;
            else
            	this->x = 0;
            if(value.y != 0)
            	this->y /= value.x;
            else
            	this->y = 0;
            return *this;
        }
    };

    typedef vec2<float>     vec2f;
    typedef vec2<int>       vec2i;
    typedef vec2<double>    vec2d;
    typedef vec2<unsigned>  vec2u;
}
#undef VERSION
#undef VERSION_CPP
#undef __constexpr__
#endif