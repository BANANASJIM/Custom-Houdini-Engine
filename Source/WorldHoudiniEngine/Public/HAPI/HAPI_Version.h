/*
 * PROPRIETARY INFORMATION.  This software is proprietary to
 * Side Effects Software Inc., and is not to be reproduced,
 * transmitted, or disclosed in any way without written permission.
 *
 * COMMENTS:
 *      Generated version information to be used when linking for
 *      sanity checks.
 */

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// WARNING! This file is GENERATED by Make.
// DO NOT modify manually or commit to the repository!
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

#ifndef __HAPI_VERSION_h__
#define __HAPI_VERSION_h__

// The three components of the Houdini version that HAPI is
// expecting to compile against.
#define HAPI_VERSION_HOUDINI_MAJOR 18
#define HAPI_VERSION_HOUDINI_MINOR 5
#define HAPI_VERSION_HOUDINI_BUILD 696
#define HAPI_VERSION_HOUDINI_PATCH 0

// The two components of the Houdini Engine (marketed) version.
#define HAPI_VERSION_HOUDINI_ENGINE_MAJOR 3
#define HAPI_VERSION_HOUDINI_ENGINE_MINOR 7

// This is a monotonously increasing API version number that can be used
// to lock against a certain API for compatibility purposes. Basically,
// when this number changes code compiled against the HAPI.h methods
// might no longer compile. Semantic changes to the methods will also
// cause this version to increase. This number will be reset to 0
// every time the Houdini Engine version is bumped.
#define HAPI_VERSION_HOUDINI_ENGINE_API 1

#endif // __HAPI_VERSION_h__
