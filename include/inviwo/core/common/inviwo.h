/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 * Version 0.6b
 *
 * Copyright (c) 2012-2014 Inviwo Foundation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * Contact: Erik Sund�n
 *
 *********************************************************************************/

#ifndef IVW_INVIWO_H
#define IVW_INVIWO_H

#pragma warning(disable: 4290)

//#pragma warning(default: 4640) // non thread safe construction of object
#pragma warning(default: 4514) // never used function
#pragma warning(default: 4296) // expression is always false
#pragma warning(default: 4061) // enum
#pragma warning(default: 4062) // enum
#pragma warning(default: 4266) // warn if no overide of function in baseclass
#pragma warning(default: 4265) // warn if non-virtual destructor
#pragma warning(default: 4263) // warn for virtual functions that do not overide something in baseclass

#define NOMINMAX

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

// Define a debug flag for inviwo called IVW_DEBUG
#ifdef __clang__ // Clang sets NDEBUG when not debugging
    #ifndef NDEBUG
        #ifndef IVW_DEBUG
            #define IVW_DEBUG
        #endif
    #endif
#endif

#ifdef _MSC_VER
    #ifdef _DEBUG /// MVS sets _DEBUG when debugging
        #ifndef IVW_DEBUG
            #define IVW_DEBUG
        #endif
    #endif
#endif

// Convenience macros for clang warning pragmas
#ifdef __clang__
#define STARTCLANGIGNORESTR(X) #X
#define STARTCLANGIGNORE(X) \
    _Pragma("clang diagnostic push") \
    _Pragma(STARTCLANGIGNORESTR(clang diagnostic ignored X))
#define ENDCLANGIGNORE \
    _Pragma("clang diagnostic pop")
#else
#define STARTCLANGIGNORE(X)
#define ENDCLANGIGNORE
#endif

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <fstream>
#include <stdio.h>

#include <inviwo/core/common/inviwocommondefines.h>

// include glm
#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif
#ifndef GLM_SWIZZLE
#define GLM_SWIZZLE
#endif
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_precision.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/matrix_operation.hpp>
#include <glm/common.hpp>

typedef glm::ivec2 ivec2;
typedef glm::ivec3 ivec3;
typedef glm::ivec4 ivec4;
typedef glm::vec2 vec2;
typedef glm::vec3 vec3;
typedef glm::vec4 vec4;
typedef glm::dvec2 dvec2;
typedef glm::dvec3 dvec3;
typedef glm::dvec4 dvec4;
typedef glm::bvec2 bvec2;
typedef glm::bvec3 bvec3;
typedef glm::bvec4 bvec4;
typedef glm::uvec2 uvec2;
typedef glm::uvec3 uvec3;
typedef glm::uvec4 uvec4;
typedef glm::mat2 mat2;
typedef glm::mat3 mat3;
typedef glm::mat4 mat4;
typedef glm::dmat2 dmat2;
typedef glm::dmat3 dmat3;
typedef glm::dmat4 dmat4;
typedef glm::quat quat;

#include <inviwo/core/util/glmstreamoperators.h>

#include <inviwo/core/util/formats.h>

// error handling
#include <inviwo/core/util/assertion.h>
#include <inviwo/core/util/exception.h>
#include <inviwo/core/util/logcentral.h>
#define IVW_UNUSED_PARAM(param) (void)param

#include <inviwo/core/io/serialization/ivwserialization.h>

//#define IVW_DEPRECATION_WARNINGS

#if defined(IVW_DEPRECATION_WARNINGS)
#define ivwDeprecatedMethod(newFunction) \
    std::cout << __FUNCTION__ << " is deprecated. Use " << newFunction << " instead." << std::endl; \
    std::cout << "(" << __FILE__ << " - Line " << __LINE__ << ")." << std::endl;
#else
#define ivwDeprecatedMethod(newFunction)
#endif

#ifdef _MSC_VER
#ifdef IVW_ENABLE_MSVC_MEM_LEAK_TEST
#include <ext/vld/vld.h>
#endif
#endif

#endif // IVW_INVIWO_H
