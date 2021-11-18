/*
 * Copyright (C) 2021 Amara Sami, Dallard Thomas, Nardone William, Six Jonathan
 * This file is subject to the LGNU license terms in the LICENSE file
 *	found in the top-level directory of this distribution.
 */

#pragma once

#include <cstdlib>
#include <type_traits>

#include "Vector2.hpp"
#include "Vector3.hpp"

//in inl
#include <algorithm>
#include <limits>
#include <time.h>
#include <cmath>

#include "Constants.hpp"

namespace GPM::Random
{

/**
 * @brief Init random seed with the current time
 * 
 */
inline void initSeed();

/**
 * @brief Initialize random number generator
 * 
 * @param seed The pseudo-random number generator is initialized using the argument passed as seed.
 */
inline void initSeed(const u32 seed);

/**
 * @brief This will generate a number from 0.0 to 1.0, inclusive.
 * 
 * @tparam floating point type 
 * @return T 
 */
template<typename T = float> 
inline auto unitValue() -> std::enable_if_t<std::is_floating_point<T>::value, T>;


/**
 * @brief This will generate a number from 0 to 1, inclusive.
 * 
 * @tparam integral type 
 * @return T 
 */
template<typename T = int> 
inline auto unitValue() -> std::enable_if_t<std::is_integral<T>::value, T>;

/**
 * @brief This will generate a number from 0.0 to some arbitrary float, max:
 * 
 * @tparam float 
 * @param max : exclude
 * @return T 
 */
template<typename T = float>
inline auto ranged(T max) -> std::enable_if_t<std::is_floating_point<T>::value, T>;

template<typename T = int>
inline auto ranged(T max) -> std::enable_if_t<std::is_integral<T>::value, T>;

/**
 * @brief This will generate a number from some arbitrary min to some arbitrary max:
 * @tparam float 
 * @param min : include
 * @param max : exclude
 * @return T 
 */
template<typename T = float>
inline auto ranged(T min, T max) -> std::enable_if_t<std::is_floating_point<T>::value, T>;

template<typename T = int>
inline auto ranged(T min, T max) -> std::enable_if_t<std::is_integral<T>::value, T>;

//Cicular
inline Vec2 circularCoordinate(const Vec2& center, float range);

inline Vec2 peripheralCircularCoordinate(const Vec2& center, float range);

inline Vec2 unitPeripheralCircularCoordinate();

//Spherique
inline Vec3 unitPeripheralSphericalCoordonate();

inline Vec3 sphericalCoordinate(const Vec3& center, float range);

inline Vec3 peripheralSphericalCoordinate(const Vec3& center, float range);

//Square
inline Vec2 peripheralSquareCoordinate(const Vec2& center, float extX, float extY);

/**
 * @brief return square coordonate
 * 
 * @tparam float 
 * @param center the center of the square
 * @param extX half saquare extension on x axis
 * @param extY half saquare extension on y axis
 * @return Vec2 
 */
inline Vec2 squareCoordinate(const Vec2& center, float extX, float extY);

/**
 * @brief return unit sqare with value between 0 and 1
 * 
 * @tparam float 
 * @return Vec2 
 */
inline Vec2 unitPeripheralSquareCoordinate();

//Cubique
inline Vec3 peripheralCubiqueCoordinate(const Vec3& center, float extX, float extY, float extZ);

/**
 * @brief return cubique coordonate
 * 
 * @tparam float 
 * @param center the center of the square
 * @param extX half saquare extension on x axis
 * @param extY half saquare extension on y axis
 * @param extZ half saquare extension on z axis
 * @return Vec3 
 */
inline Vec3 cubiqueCoordinate(const Vec3& center, float extX, float extY, float extZ);

/**
 * @brief return unit cubic with value between 0 and 1
 * 
 * @tparam float 
 * @return Vec3 
 */
inline Vec3 unitPeripheralCubiqueCoordinate();

//pourcent
/**
 * @brief return if purcent value is respect in randome case. Inclusive test
 * 
 * @tparam float 
 * @param percent 
 * @return true 
 * @return false 
 */
inline bool isRandNumberInPercentage(float percent);

#include "Random.inl"

} //namespace GPM