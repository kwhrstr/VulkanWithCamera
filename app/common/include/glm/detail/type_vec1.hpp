/// @ref core
/// @file glm/detail/type_vec1.hpp

#pragma once

#include "fwd.hpp"
#include "type_vec.hpp"
#if GLM_SWIZZLE == GLM_SWIZZLE_ENABLED
#	if GLM_HAS_UNRESTRICTED_UNIONS
#		include "_swizzle.hpp"
#	else
#		include "_swizzle_func.hpp"
#	endif
#endif //GLM_SWIZZLE
#include <cstddef>

namespace glm
{
	template<typename T, qualifier Q>
	struct vec<1, T, Q>
	{
		// -- Implementation detail --

		typedef T value_type;
		typedef vec type;
		typedef vec<1, bool, Q> bool_type;

		// -- Data --

#		if GLM_HAS_ONLY_XYZW
			T x;

#		elif GLM_HAS_ALIGNED_TYPE
#			if GLM_COMPILER & GLM_COMPILER_GCC
#				pragma GCC diagnostic push
#				pragma GCC diagnostic ignored "-Wpedantic"
#			endif
#			if GLM_COMPILER & GLM_COMPILER_CLANG
#				pragma clang diagnostic push
#				pragma clang diagnostic ignored "-Wgnu-anonymous-struct"
#				pragma clang diagnostic ignored "-Wnested-anon-types"
#			endif
		
			union
			{
				T x;
				T r;
				T s;
/*
#				if GLM_SWIZZLE == GLM_SWIZZLE_ENABLED
					_GLM_SWIZZLE1_2_MEMBERS(T, Q, tvec2, x)
					_GLM_SWIZZLE1_2_MEMBERS(T, Q, tvec2, r)
					_GLM_SWIZZLE1_2_MEMBERS(T, Q, tvec2, s)
					_GLM_SWIZZLE1_3_MEMBERS(T, Q, tvec3, x)
					_GLM_SWIZZLE1_3_MEMBERS(T, Q, tvec3, r)
					_GLM_SWIZZLE1_3_MEMBERS(T, Q, tvec3, s)
					_GLM_SWIZZLE1_4_MEMBERS(T, Q, tvec4, x)
					_GLM_SWIZZLE1_4_MEMBERS(T, Q, tvec4, r)
					_GLM_SWIZZLE1_4_MEMBERS(T, Q, tvec4, s)
#				endif//GLM_SWIZZLE*/
			};
		
#			if GLM_COMPILER & GLM_COMPILER_CLANG
#				pragma clang diagnostic pop
#			endif
#			if GLM_COMPILER & GLM_COMPILER_GCC
#				pragma GCC diagnostic pop
#			endif
#		else
			union {T x, r, s;};
/*
#			if GLM_SWIZZLE == GLM_SWIZZLE_ENABLED
				GLM_SWIZZLE_GEN_VEC_FROM_VEC1(T, P, tvec2, tvec2, tvec3, tvec4)
#			endif//GLM_SWIZZLE*/
#		endif

		// -- Component accesses --

		/// Return the count of components of the vector
		typedef length_t length_type;
		GLM_FUNC_DECL static GLM_CONSTEXPR length_type length(){return 1;}

		GLM_FUNC_DECL T & operator[](length_type i);
		GLM_FUNC_DECL T const& operator[](length_type i) const;

		// -- Implicit basic constructors --

		GLM_FUNC_DECL GLM_CONSTEXPR_CTOR vec() GLM_DEFAULT;
		GLM_FUNC_DECL GLM_CONSTEXPR_CTOR vec(vec const& v) GLM_DEFAULT;
		template<qualifier P>
		GLM_FUNC_DECL GLM_CONSTEXPR_CTOR vec(vec<1, T, P> const& v);

		// -- Explicit basic constructors --

		GLM_FUNC_DECL GLM_CONSTEXPR_CTOR explicit vec(T scalar);

		// -- Conversion vector constructors --

		/// Explicit conversions (From section 5.4.1 Conversion and scalar constructors of GLSL 1.30.08 specification)
		template<typename U, qualifier P>
		GLM_FUNC_DECL GLM_CONSTEXPR_CTOR GLM_EXPLICIT vec(vec<2, U, P> const& v);
		/// Explicit conversions (From section 5.4.1 Conversion and scalar constructors of GLSL 1.30.08 specification)
		template<typename U, qualifier P>
		GLM_FUNC_DECL GLM_CONSTEXPR_CTOR GLM_EXPLICIT vec(vec<3, U, P> const& v);
		/// Explicit conversions (From section 5.4.1 Conversion and scalar constructors of GLSL 1.30.08 specification)
		template<typename U, qualifier P>
		GLM_FUNC_DECL GLM_CONSTEXPR_CTOR GLM_EXPLICIT vec(vec<4, U, P> const& v);

		/// Explicit conversions (From section 5.4.1 Conversion and scalar constructors of GLSL 1.30.08 specification)
		template<typename U, qualifier P>
		GLM_FUNC_DECL GLM_CONSTEXPR_CTOR GLM_EXPLICIT vec(vec<1, U, P> const& v);

		// -- Swizzle constructors --
/*
#		if(GLM_HAS_UNRESTRICTED_UNIONS && (GLM_SWIZZLE == GLM_SWIZZLE_ENABLED))
			template<int E0>
			GLM_FUNC_DECL tvec(detail::_swizzle<1, T, Q, tvec1, E0, -1,-2,-3> const& that)
			{
				*this = that();
			}
#		endif//(GLM_HAS_UNRESTRICTED_UNIONS && (GLM_SWIZZLE == GLM_SWIZZLE_ENABLED))
*/
		// -- Unary arithmetic operators --

		GLM_FUNC_DECL vec & operator=(vec const& v) GLM_DEFAULT;

		template<typename U>
		GLM_FUNC_DECL vec & operator=(vec<1, U, Q> const& v);
		template<typename U>
		GLM_FUNC_DECL vec & operator+=(U scalar);
		template<typename U>
		GLM_FUNC_DECL vec & operator+=(vec<1, U, Q> const& v);
		template<typename U>
		GLM_FUNC_DECL vec & operator-=(U scalar);
		template<typename U>
		GLM_FUNC_DECL vec & operator-=(vec<1, U, Q> const& v);
		template<typename U>
		GLM_FUNC_DECL vec & operator*=(U scalar);
		template<typename U>
		GLM_FUNC_DECL vec & operator*=(vec<1, U, Q> const& v);
		template<typename U>
		GLM_FUNC_DECL vec & operator/=(U scalar);
		template<typename U>
		GLM_FUNC_DECL vec & operator/=(vec<1, U, Q> const& v);

		// -- Increment and decrement operators --

		GLM_FUNC_DECL vec & operator++();
		GLM_FUNC_DECL vec & operator--();
		GLM_FUNC_DECL vec operator++(int);
		GLM_FUNC_DECL vec operator--(int);

		// -- Unary bit operators --

		template<typename U>
		GLM_FUNC_DECL vec & operator%=(U scalar);
		template<typename U>
		GLM_FUNC_DECL vec & operator%=(vec<1, U, Q> const& v);
		template<typename U>
		GLM_FUNC_DECL vec & operator&=(U scalar);
		template<typename U>
		GLM_FUNC_DECL vec & operator&=(vec<1, U, Q> const& v);
		template<typename U>
		GLM_FUNC_DECL vec & operator|=(U scalar);
		template<typename U>
		GLM_FUNC_DECL vec & operator|=(vec<1, U, Q> const& v);
		template<typename U>
		GLM_FUNC_DECL vec & operator^=(U scalar);
		template<typename U>
		GLM_FUNC_DECL vec & operator^=(vec<1, U, Q> const& v);
		template<typename U>
		GLM_FUNC_DECL vec & operator<<=(U scalar);
		template<typename U>
		GLM_FUNC_DECL vec & operator<<=(vec<1, U, Q> const& v);
		template<typename U>
		GLM_FUNC_DECL vec & operator>>=(U scalar);
		template<typename U>
		GLM_FUNC_DECL vec & operator>>=(vec<1, U, Q> const& v);
	};

	// -- Unary operators --

	template<typename T, qualifier Q>
	GLM_FUNC_DECL vec<1, T, Q> operator+(vec<1, T, Q> const& v);

	template<typename T, qualifier Q>
	GLM_FUNC_DECL vec<1, T, Q> operator-(vec<1, T, Q> const& v);

	// -- Binary operators --

	template<typename T, qualifier Q>
	GLM_FUNC_DECL vec<1, T, Q> operator+(vec<1, T, Q> const& v, T scalar);

	template<typename T, qualifier Q>
	GLM_FUNC_DECL vec<1, T, Q> operator+(T scalar, vec<1, T, Q> const& v);

	template<typename T, qualifier Q>
	GLM_FUNC_DECL vec<1, T, Q> operator+(vec<1, T, Q> const& v1, vec<1, T, Q> const& v2);

	template<typename T, qualifier Q>
	GLM_FUNC_DECL vec<1, T, Q> operator-(vec<1, T, Q> const& v, T scalar);

	template<typename T, qualifier Q>
	GLM_FUNC_DECL vec<1, T, Q> operator-(T scalar, vec<1, T, Q> const& v);

	template<typename T, qualifier Q>
	GLM_FUNC_DECL vec<1, T, Q> operator-(vec<1, T, Q> const& v1, vec<1, T, Q> const& v2);

	template<typename T, qualifier Q>
	GLM_FUNC_DECL vec<1, T, Q> operator*(vec<1, T, Q> const& v, T scalar);

	template<typename T, qualifier Q>
	GLM_FUNC_DECL vec<1, T, Q> operator*(T scalar, vec<1, T, Q> const& v);

	template<typename T, qualifier Q>
	GLM_FUNC_DECL vec<1, T, Q> operator*(vec<1, T, Q> const& v1, vec<1, T, Q> const& v2);

	template<typename T, qualifier Q>
	GLM_FUNC_DECL vec<1, T, Q> operator/(vec<1, T, Q> const& v, T scalar);

	template<typename T, qualifier Q>
	GLM_FUNC_DECL vec<1, T, Q> operator/(T scalar, vec<1, T, Q> const& v);

	template<typename T, qualifier Q>
	GLM_FUNC_DECL vec<1, T, Q> operator/(vec<1, T, Q> const& v1, vec<1, T, Q> const& v2);

	template<typename T, qualifier Q>
	GLM_FUNC_DECL vec<1, T, Q> operator%(vec<1, T, Q> const& v, T scalar);

	template<typename T, qualifier Q>
	GLM_FUNC_DECL vec<1, T, Q> operator%(T scalar, vec<1, T, Q> const& v);

	template<typename T, qualifier Q>
	GLM_FUNC_DECL vec<1, T, Q> operator%(vec<1, T, Q> const& v1, vec<1, T, Q> const& v2);

	template<typename T, qualifier Q>
	GLM_FUNC_DECL vec<1, T, Q> operator&(vec<1, T, Q> const& v, T scalar);

	template<typename T, qualifier Q>
	GLM_FUNC_DECL vec<1, T, Q> operator&(T scalar, vec<1, T, Q> const& v);

	template<typename T, qualifier Q>
	GLM_FUNC_DECL vec<1, T, Q> operator&(vec<1, T, Q> const& v1, vec<1, T, Q> const& v2);

	template<typename T, qualifier Q>
	GLM_FUNC_DECL vec<1, T, Q> operator|(vec<1, T, Q> const& v, T scalar);

	template<typename T, qualifier Q>
	GLM_FUNC_DECL vec<1, T, Q> operator|(T scalar, vec<1, T, Q> const& v);

	template<typename T, qualifier Q>
	GLM_FUNC_DECL vec<1, T, Q> operator|(vec<1, T, Q> const& v1, vec<1, T, Q> const& v2);

	template<typename T, qualifier Q>
	GLM_FUNC_DECL vec<1, T, Q> operator^(vec<1, T, Q> const& v, T scalar);

	template<typename T, qualifier Q>
	GLM_FUNC_DECL vec<1, T, Q> operator^(T scalar, vec<1, T, Q> const& v);

	template<typename T, qualifier Q>
	GLM_FUNC_DECL vec<1, T, Q> operator^(vec<1, T, Q> const& v1, vec<1, T, Q> const& v2);

	template<typename T, qualifier Q>
	GLM_FUNC_DECL vec<1, T, Q> operator<<(vec<1, T, Q> const& v, T scalar);

	template<typename T, qualifier Q>
	GLM_FUNC_DECL vec<1, T, Q> operator<<(T scalar, vec<1, T, Q> const& v);

	template<typename T, qualifier Q>
	GLM_FUNC_DECL vec<1, T, Q> operator<<(vec<1, T, Q> const& v1, vec<1, T, Q> const& v2);

	template<typename T, qualifier Q>
	GLM_FUNC_DECL vec<1, T, Q> operator>>(vec<1, T, Q> const& v, T scalar);

	template<typename T, qualifier Q>
	GLM_FUNC_DECL vec<1, T, Q> operator>>(T scalar, vec<1, T, Q> const& v);

	template<typename T, qualifier Q>
	GLM_FUNC_DECL vec<1, T, Q> operator>>(vec<1, T, Q> const& v1, vec<1, T, Q> const& v2);

	template<typename T, qualifier Q>
	GLM_FUNC_DECL vec<1, T, Q> operator~(vec<1, T, Q> const& v);

	// -- Boolean operators --

	template<typename T, qualifier Q>
	GLM_FUNC_DECL bool operator==(vec<1, T, Q> const& v1, vec<1, T, Q> const& v2);

	template<typename T, qualifier Q>
	GLM_FUNC_DECL bool operator!=(vec<1, T, Q> const& v1, vec<1, T, Q> const& v2);

	template<qualifier Q>
	GLM_FUNC_DECL vec<1, bool, Q> operator&&(vec<1, bool, Q> const& v1, vec<1, bool, Q> const& v2);

	template<qualifier Q>
	GLM_FUNC_DECL vec<1, bool, Q> operator||(vec<1, bool, Q> const& v1, vec<1, bool, Q> const& v2);
}//namespace glm

#ifndef GLM_EXTERNAL_TEMPLATE
#include "type_vec1.inl"
#endif//GLM_EXTERNAL_TEMPLATE
