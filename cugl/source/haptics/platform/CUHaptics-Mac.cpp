//
//  CUHaptics-Mac.cpp
//  Cornell University Game Library (CUGL)
//
//  This module is a singleton providing display information about the device.
//  Originally, we had made this part of Application.  However, we discovered
//  that we needed platform specfic code for this, so we factored it out.
//
//  This singleton is also responsible for initializing (and disposing) the
//  OpenGL context.  That is because that context is tightly coupled to the
//  orientation information, which is provided by this class.
//
//  Because this is a singleton, there are no publicly accessible constructors
//  or intializers.  Use the static methods instead.
//
//  CUGL MIT License:
//      This software is provided 'as-is', without any express or implied
//      warranty.  In no event will the authors be held liable for any damages
//      arising from the use of this software.
//
//      Permission is granted to anyone to use this software for any purpose,
//      including commercial applications, and to alter it and redistribute it
//      freely, subject to the following restrictions:
//
//      1. The origin of this software must not be misrepresented; you must not
//      claim that you wrote the original software. If you use this software
//      in a product, an acknowledgment in the product documentation would be
//      appreciated but is not required.
//
//      2. Altered source versions must be plainly marked as such, and must not
//      be misrepresented as being the original software.
//
//      3. This notice may not be removed or altered from any source distribution.
//
//
//  Created by Marcos Eizayaga on 6/7/22.
//  Copyright © 2022 Game Design Initiative at Cornell. All rights reserved.
//

#include <stdio.h>
#include "CUHaptics-impl.h"

/** 
 * This method returns if haptics is supported for the current device. Not every
 * device has haptics enabled or has a usable API.
 * 
 * @return If haptics is supported. 
 */
bool cugl::impl::HapticsIsSupported() { return false; }

/*
WARNING: Haptics is not available on Mac, all the following functions are no-ops. 
*/

bool cugl::impl::HapticsInit() { return true; }
  
bool cugl::impl::HapticsDispose() { return true; }

bool cugl::impl::HapticsPlayTransient(float intensity, float sharpness) { return false; }
  
bool cugl::impl::HapticsPlayContinuous(float intensity, float sharpness, float duration, bool force) { return false; }
  
bool cugl::impl::HapticsUpdateContinuous(float intensity, float sharpness) { return false; }

