/*
 * Copyright (C) 2021 Amara Sami, Dallard Thomas, Nardone William, Six Jonathan
 * This file is subject to the LGNU license terms in the LICENSE file
 * found in the top-level directory of this distribution.
 */

#pragma once

#include "Vector3.hpp"

namespace GPM
{
    /*isBetween(5, 0, 10)*/
    template<class T = float>
    T isBetween(T number, T low, T up)
    {
        return number >= low && number <= up;
    }

    /*isBetween<0, 10>(5)*/
    template<typename T = float, T Min, T Max>
    auto isBetween(T value) -> std::enable_if_t<Min <= Max, bool>
    {
        return value >= Min && value <= Max;
    }
} /*/namespace GPM*/
