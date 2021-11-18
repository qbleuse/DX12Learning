/*
 * Copyright (C) 2021 Amara Sami, Dallard Thomas, Nardone William, Six Jonathan
 * This file is subject to the LGNU license terms in the LICENSE file
 * found in the top-level directory of this distribution.
 */
 
#pragma once

namespace GPM
{
    class Volume
    {
        public:

        Volume ()					                = default;
        Volume (const Volume& other)			    = default;
        Volume (Volume&& other)				        = default;
        virtual ~Volume ()				            = default;
        Volume& operator=(Volume const& other)		= default;
        Volume& operator=(Volume && other)			= default;

        //float getArea () = 0;
    };

} /*namespace GPM*/