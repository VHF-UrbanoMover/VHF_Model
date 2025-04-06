/* Copyright (c) 2014-2021, Mechanical Simulation Corporation.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

/**
 * This library is meant to be used as a wrapper to simplify integration of a VehicleSim solver
 * into another simulation environment. It hides many of the intricacies of using the vs_api
 * directly, and helps with creating multiple instances of vehicles.
 *
 * The 'vsew' prefix stands for VehicleSim Encapsulating Wrapper.
 *
 */


#pragma once
#ifndef _vs_vehicle_hpp_INCLUDED
#define _vs_vehicle_hpp_INCLUDED

#include <stdbool.h>

/// Opaque pointer (handle) for a vehicle instance.
typedef struct vsew_Vehicle*   VSEW_VEHICLE_HANDLE;


#if defined(_WIN32) || defined(_WIN64)
  #if defined(VSEW_BUILDING_DLL)
    #define VS_API   __declspec(dllexport)
  #elif defined(VSEW_STATIC)
    #define VS_API
  #else
    #define VS_API   __declspec(dllimport)
  #endif
  #define VS_API_2
#elif defined(_VS_LINUX) || defined(__unix__)
  #define VS_API
  #define VS_API_2 __attribute__((visibility("default")))
  #define __stdcall
  #define DeleteFile remove
  
  #include <dlfcn.h>
#else
  #error Unknown platform
#endif

#ifndef NULL
#define NULL (0)
#endif


#ifdef __cplusplus
extern "C" {
#endif

/**
  * The tire values index in the same method used by the solver.
 */

typedef enum AxleSide
{
  VSEW_AXLESIDE_LEFT = 0, VSEW_AXLESIDE_RIGHT = 1,
}
AxleSide;

typedef enum WheelSide
{
  VSEW_WHEELSIDE_INNER = 0, VSEW_WHEELSIDE_OUTER = 1,
}
WheelSide;

/**
  Specifies if and how import values are applied when a "SetValue" function is called.
*/
typedef enum ImportValueApplication
{
  VSEW_IVA_UNDEFINED  = -1,
  VSEW_IVA_DEFAULT    = 0,
  VSEW_IVA_USE_LAST   = 1,  ///<  Always applies the last value specified.
  VSEW_IVA_USE_FIRST  = 2,  ///<  Uses the first value specified per-frame.
  VSEW_IVA_ACCUMULATE = 3,  ///<  Each time vsew_Import_SetValue() is called, the value specified is added to the existing value.
} ImportValueApplication;

/**
  Specifies whether or not import values are carried over from one frame to the
  next (i.e. are import values retained after they are applied to the solver?).
*/
typedef enum ImportValueCarryOver
{
  VSEW_IVCO_UNDEFINED = -1,
  VSEW_IVCO_DEFAULT   = 0,  ///<  Use default for the specified ImportValueApplication setting.
  VSEW_IVCO_NO        = 1,  ///<  Values are cleared after each frame. The import will be set to initialValue at the beginning of the frame.
  VSEW_IVCO_YES       = 2,  ///<  Values are NOT cleared after each frame. The import will retain its final value for a frame at the beginning of the next frame.
} ImportValueCarryOver;

struct VsRoadInfo
{
  double  mCarSimLocX;  ///< X (CarSim), Z (Unity)
  double  mCarSimLocY;  ///< Y (CarSim), -X (Unity)
  double  mCarSimLocZ;  ///< Z (CarSim), Y (Unity)
  double  mCarSimDzDx;  ///< Slope in global X direction.
  double  mCarSimDzDy;  ///< Slope in global Y direction.
  double  mCarSimMu;    ///< Surface Mu.

  /** Pointer/handle to additional engine-specific (e.g. Unity/Unreal) data
    about the terrain and/or collision/contact point. This is not used
    directly within this project (because we don't know what it is!), but it
    can be used by the engine-specific code which uses this wrapper.
  */
  void*   mCollisionInfo;

#ifdef __cplusplus
  VsRoadInfo()
  {
    Clear();
  }

  void Clear()
  {
    mCarSimLocX     = 0.0;
    mCarSimLocY     = 0.0;
    mCarSimLocZ     = 0.0;
    mCarSimDzDx     = 0.0;
    mCarSimDzDy     = 0.0;
    mCarSimMu       = 0.0;
    mCollisionInfo  = NULL;
  }
#endif
};


/** Prototype for the road info callback function. A function of this type should
  be implemented by the engine-specific code that uses this wrapper. The engine-
  specific code (e.g. Unity/Unreal plug-in) registers it's function of this type
  with this wrapper in the vsew_CreateVehicle() function.
*/
typedef bool (__stdcall *GetRoadInfo)(    void*               applicationData
                                        , double              carSimX0
                                        , double              carSimY0
                                        , double              carSimZ0
                                        , double              carSimX1
                                        , double              carSimY1
                                        , double              carSimZ1
                                        , struct VsRoadInfo*  out_roadInfo    );

// Exported functions that can be called from the simulation environment (Unreal, Unity, etc.):
VS_API bool VS_API_2         vsew_IsValid( double valueToCheck );
VS_API void VS_API_2         vsew_Invalidate( double* varToInvalidate );
VS_API void VS_API_2         vsew_Invalidate_f( float* varToInvalidate );

/// Returns an invalid value (one that will return false from vsew_IsValid()).
VS_API double VS_API_2  vsew_GetInvalidValue();
/// Returns an invalid value (one that will return false from vsew_IsValid()).
VS_API float VS_API_2   vsew_GetInvalidValue_f();

VS_API ImportValueApplication VS_API_2
                        vsew_GetDefaultImportValueApplication();

/**
  Returns the default ImportValueCarryOver setting for the specified
  ImportValueApplication.
*/
VS_API ImportValueCarryOver VS_API_2
                        vsew_GetDefaultImportValueCarryOver( ImportValueApplication iva );

/**
  Creates a new instance of a vehicle.

  \param simfile                  The Simfile to load.
  \param simfileMacros            Two-dimensional array of key-value pairs. Each pair is a macro
                                  and its value.
  \param numSimfileMacros         The number of key-value pairs in the simfileMacros array.
  \param startTime                The initial simulation time. If this parameter is <= -FLT_MAX,
                                  the start time specified by the simfile/parsfile will be used.
  \param useExternalDriverInput   If true, driver inputs can be specified (via functions). If false,
                                  inputs will be ignored and the internal driver model will be used.
  \param roadInfoCallback         Pointer to an external GetRoadInfo function the Solver will use
                                  to query the ground. If NULL, the internal road model will be
                                  used.
  \param roadInfoApplicationData  A value that this vehicle instance will pass to the
                                  roadInfoCallback function whenever it is called.
  \param out_errorMsg             A string buffer to be populated with any error message from the
                                  Solver encountered during creation of the vehicle.
  \param errMsgSize               The size of the out_errorMsg string.
*/
#if defined(VSEW_NO_DEFAULT_PARAMS) || !defined(__cplusplus)
/**
  \deprecated
  DEPRECATED: This function may be removed for VehicleSim 2022.1 or later.
  Use vsew_CreateVehicle2() instead.
*/
VS_API VSEW_VEHICLE_HANDLE VS_API_2
                        vsew_CreateVehicle( const char* simfile
                                          , const char* simfileMacros[][2]
                                          , size_t      numSimfileMacros
                                          , double      startTime
                                          , bool        useExternalDriverInput
                                          , GetRoadInfo roadInfoCallback
                                          , void*       roadInfoApplicationData
                                          , char*       out_errorMsg
                                          , size_t      errMsgSize
                                          );

VS_API VSEW_VEHICLE_HANDLE VS_API_2
                        vsew_CreateVehicle2(  const char* simfile
                                            , const char* simfileMacros[][2]
                                            , size_t      numSimfileMacros
                                            , double      startTime
                                            , GetRoadInfo roadInfoCallback
                                            , void*       roadInfoApplicationData
                                            , char*       out_errorMsg
                                            , size_t      errMsgSize
                                          );

#else
/**
  \deprecated
  DEPRECATED: This function may be removed for VehicleSim 2022.1 or later.
  Use vsew_CreateVehicle2() instead.
*/
#if (__cplusplus >= 201103)
[[deprecated]]
#endif
VS_API VSEW_VEHICLE_HANDLE VS_API_2
                        vsew_CreateVehicle( const char* simfile
                                          , const char* simfileMacros[][2]      = NULL
                                          , size_t      numSimfileMacros        = 0
                                          , double      startTime               = 0.0
                                          , bool        useExternalDriverInput  = true
                                          , GetRoadInfo roadInfoCallback        = NULL
                                          , void*       roadInfoApplicationData = NULL
                                          , char*       out_errorMsg            = NULL
                                          , size_t      errMsgSize              = 0
                                          );
VS_API VSEW_VEHICLE_HANDLE VS_API_2
                        vsew_CreateVehicle2(  const char* simfile
                                            , const char* simfileMacros[][2]      = NULL
                                            , size_t      numSimfileMacros        = 0
                                            , double      startTime               = 0.0
                                            , GetRoadInfo roadInfoCallback        = NULL
                                            , void*       roadInfoApplicationData = NULL
                                            , char*       out_errorMsg            = NULL
                                            , size_t      errMsgSize              = 0
                                          );
#endif

/** Runs solver initialization, which should (normally) be done after moving or
    reorienting the car, modifying the road/terrain callback.

    This may only be called if vsew_IsOk() is true for the specified vehicle.

    It is good practice to check that the vehicle initialization succeeded by
    calling vsew_IsOk() after a call to this function.
*/
VS_API void VS_API_2    vsew_ReinitializeVehicle(VSEW_VEHICLE_HANDLE vehicle);

/// Returns true if the passed handle refers to a valid vehicle.
VS_API bool VS_API_2    vsew_IsValidVehicle(const VSEW_VEHICLE_HANDLE vehicle);

/// Returns true if the specified vehicle is running without any errors. Returns false otherwise.
VS_API bool VS_API_2    vsew_IsOk(const VSEW_VEHICLE_HANDLE veh);

/** Returns a pointer to the VS Solver's error message buffer. Do not free the
returned pointer.
*/
VS_API const char VS_API_2
                        *vsew_GetErrorMessage(const VSEW_VEHICLE_HANDLE veh);

/** Clears the VS Solver's error message buffer.
*/
VS_API void VS_API_2    vsew_ClearErrorMessage(const VSEW_VEHICLE_HANDLE veh);

/** Returns the total number of imports currently configured.
  This may only be called if vsew_IsOk() is true for the specified vehicle.
*/
VS_API size_t VS_API_2  vsew_Import_Count( const VSEW_VEHICLE_HANDLE veh );

/// Returns true if the specified import is available to be added.
VS_API bool VS_API_2    vsew_Import_IsAvailable(VSEW_VEHICLE_HANDLE veh, const char* name);

/**
  This may only be called if vsew_IsOk() is true for the specified vehicle.
*/
VS_API int VS_API_2     vsew_Import_Add3( VSEW_VEHICLE_HANDLE     veh,
                                          const char*             name,
                                          double                  initialValue,
                                          bool                    lerp,
                                          const char*             mode,
                                          ImportValueApplication  iva,
                                          ImportValueCarryOver    ivco
                                        );

/**
  \deprecated
  DEPRECATED: This function may be removed for VehicleSim 2022.1 or later.
  Use the following instead:

      vsew_Import_Add3( veh, name, initialValue, lerp, mode, VSEW_IVA_DEFAULT, VSEW_IVCO_DEFAULT );

  This may only be called if vsew_IsOk() is true for the specified vehicle.
*/
#if (__cplusplus >= 201103)
[[deprecated]]
#endif
VS_API int VS_API_2     vsew_Import_Add2( VSEW_VEHICLE_HANDLE veh,
                                          const char*         name,
                                          double              initialValue,
                                          bool                lerp,
                                          const char*         mode    );

/**
  \deprecated
  DEPRECATED: This function may be removed for VehicleSim 2022.1 or later.
  Use the following instead:

      vsew_Import_Add3( veh, name, initialValue, lerp, "REPLACE", VSEW_IVA_DEFAULT, VSEW_IVCO_DEFAULT );

  This may only be called if vsew_IsOk() is true for the specified vehicle.
*/
#if (__cplusplus >= 201103)
[[deprecated]]
#endif
VS_API int VS_API_2     vsew_Import_Add(  VSEW_VEHICLE_HANDLE veh,
                                          const char*         name,
                                          double              initialValue,
                                          bool                lerp    );

/**
  This may only be called if vsew_IsOk() is true for the specified vehicle.
*/
VS_API int VS_API_2     vsew_Import_GetId( const VSEW_VEHICLE_HANDLE veh, const char* name );

/**
  \deprecated
  DEPRECATED: This function may be removed for VehicleSim 2022.1 or later.
  Use something like this instead:

      int   myImportIndex = vsew_Import_GetId( veh, name );
      if ( myImportIndex < 0 )
      {
        vsew_Import_Add3( veh, name, initialValue, lerp, mode, VSEW_IVA_DEFAULT, VSEW_IVCO_DEFAULT );
      }

  This may only be called if vsew_IsOk() is true for the specified vehicle.
*/
#if (__cplusplus >= 201103)
[[deprecated]]
#endif
VS_API int VS_API_2     vsew_Import_GetOrAdd2(  VSEW_VEHICLE_HANDLE veh,
                                                const char*         name,
                                                double              initialValue,
                                                bool                lerp,
                                                const char*         mode    );

/**
  \deprecated
  DEPRECATED: This function may be removed for VehicleSim 2022.1 or later.
  Use something like this instead:

      int   myImportIndex = vsew_Import_GetId( veh, name );
      if ( myImportIndex < 0 )
      {
        myImportIndex = vsew_Import_Add3( veh, name, initialValue, lerp, "REPLACE", VSEW_IVA_DEFAULT, VSEW_IVCO_DEFAULT );
      }

  This may only be called if vsew_IsOk() is true for the specified vehicle.
*/
#if (__cplusplus >= 201103)
[[deprecated]]
#endif
VS_API int VS_API_2     vsew_Import_GetOrAdd( VSEW_VEHICLE_HANDLE veh,
                                              const char*         name,
                                              double              initialValue,
                                              bool                lerp    );

/**
  This may only be called if vsew_IsOk() is true for the specified vehicle.
*/
VS_API const char VS_API_2
                        *vsew_Import_GetName( const VSEW_VEHICLE_HANDLE veh, int id );

/**
  This may only be called if vsew_IsOk() is true for the specified vehicle.
*/
VS_API bool VS_API_2    vsew_Import_GetLerp( const VSEW_VEHICLE_HANDLE veh, int id );

/**
  This may only be called if vsew_IsOk() is true for the specified vehicle.
*/
VS_API void VS_API_2    vsew_Import_SetValue( VSEW_VEHICLE_HANDLE veh, int id, double val );

/**
  This may only be called if vsew_IsOk() is true for the specified vehicle.

  This call is equivalent to vsew_Import_SetValue(), but the ImportValueApplication
  setting for the import may be overridden. Specify VSEW_IVA_DEFAULT to use the
  import's current setting.
*/
VS_API void VS_API_2    vsew_Import_SetValue2( VSEW_VEHICLE_HANDLE veh, int id, double val, ImportValueApplication iva );

/**
  This may only be called if vsew_IsOk() is true for the specified vehicle.

  Returns the value that was passed to the solver during the previous integration,
  or the imports "initial value" if an integrate function has not yet been called.
*/
VS_API double VS_API_2  vsew_Import_GetPrevValue( VSEW_VEHICLE_HANDLE veh, int id );

/**
  This may only be called if vsew_IsOk() is true for the specified vehicle.

  Returns the value that has been explicitly set for this import since the last
  call to integrate, or an invalid value if this import has not been explicitly
  set since the last call to integrate.
*/
VS_API double VS_API_2  vsew_Import_GetCurrentValue( VSEW_VEHICLE_HANDLE veh, int id );

/**
  This may only be called if vsew_IsOk() is true for the specified vehicle.

  Returns the value that will be passed to the solver during the next integraion.
*/
VS_API double VS_API_2  vsew_Import_GetNextValue( VSEW_VEHICLE_HANDLE veh, int id );

/** Returns a pointer to the specified solver variable, or NULL if the specified variable cannot be
 *  found. This may only be called if vsew_IsOk() is true for the specified vehicle.
*/
VS_API const double VS_API_2
                        *vsew_GetVarPtr(  const VSEW_VEHICLE_HANDLE vehicleHandle
                                        , const char*               varName);

/** Returns a pointer to the specified integer variable, or NULL if the specified variable cannot
 *  be found. This may only be called if vsew_IsOk() is true for the specified vehicle.
*/
VS_API const int VS_API_2
                        *vsew_GetVarPtrInt(   const VSEW_VEHICLE_HANDLE vehicleHandle
                                            , const char*               varName);

/**
Returns the gain that is applied to this variable when it is output (e.g. to an
ERD or VS data file), or 0 if a floating point variable with the specified
name cannot be found (integer variables do not have units or output gains).

This may only be called if vsew_IsOk() is true for the specified vehicle.
*/
VS_API double VS_API_2  vsew_GetVarOutputGain(  const VSEW_VEHICLE_HANDLE vehicleHandle
                                              , const char*               varName);

/** Executes a VS command, or sets a VS Solver parameter.
    If executing a VS Command, specify the entire string in the key parameter and
    pass NULL for the buffer parameter.

    If setting a VS Solver parameter, specify the parameter name with the key
    parameter and specify its value with the buffer parameter.

    This may only be called if vsew_IsOk() is true for the specified vehicle.
*/
VS_API int VS_API_2     vsew_ExecuteVsCommand(  const VSEW_VEHICLE_HANDLE   vehicleHandle
                                              , const char*                 key
                                              , const char*                 buffer);

/**
Returns the number of forward gears the transmission has, or a negative value if
the number of gears cannot be determined.
*/
VS_API int VS_API_2     vsew_GetNumForwardGears( const VSEW_VEHICLE_HANDLE vehicleHandle );

/** Returns the number of units this vehicle has. */
VS_API int VS_API_2     vsew_GetNumUnits(   const VSEW_VEHICLE_HANDLE vehicleHandle);

/** Returns the number of axles this vehicle has on given unit. */
VS_API int VS_API_2     vsew_GetNumAxles(   const VSEW_VEHICLE_HANDLE vehicleHandle
                                          , int                       unitIndex);

/** Returns the offset in the forward direction (VehicleSim +X), in meters, of
    the specified axle relative to the vehicle origin.

    Typically, the first axle of a VS Vehicle is the front axle, and it is
    located longitudinally at the origin, so its offset will be zero. Subsequent
    axles (i.e. a car's rear axle) are typically located behind the first axle,
    so their offsets will be negative.

    The axleIndex parameter is zero-based; it must be >= 0 and < the number of
    axles. Specifying an index that does not meet these requirements may result in
    unexpected and undefined behavior, including program crashes.
*/
VS_API double VS_API_2  vsew_GetAxleOffsetForward(  const VSEW_VEHICLE_HANDLE vehicleHandle
                                                  , int                       unitIndex
                                                  , int                       axleIndex);

/** Returns the track width of the specified axle in meters.

    The axleIndex parameter is zero-based; it must be >= 0 and < the number of
    axles. Specifying an index that does not meet these requirements may result in
    unexpected and undefined behavior, including program crashes.
*/
VS_API double VS_API_2  vsew_GetAxleTrack(  const VSEW_VEHICLE_HANDLE vehicleHandle
                                          , int                       unitIndex
                                          , int                       axleIndex);

/** Returns the width between tire centerlines for dual tires (in meters).
    Returns zero if the specified axle does not have dual tires.

    The unitIndex parameter is zero-based; it must be >= 0 and < the number of units.

    The axleIndex parameter is zero-based; it must be >= 0 and < the number of axles.

    Specifying an index that is out of range will result in undefined behavior,
    including program crashes.
*/
VS_API double VS_API_2  vsew_GetDualWidth(  const VSEW_VEHICLE_HANDLE vehicleHandle
                                          , int                       unitIndex
                                          , int                       axleIndex);

/** Returns the world position of the specified vehicle unit's origin via
    output parameters, in meters, in the VehicleSim coordinate system.

    Output parameters must be valid (i.e. not NULL pointers).

    Returns true on success, returns false (and leaves output parameters
    unchanged) if position is unavailable.

    This may only be called if vsew_IsOk() is true for the specified vehicle.
*/
VS_API bool VS_API_2    vsew_GetWorldPosition(  const VSEW_VEHICLE_HANDLE vehicleHandle
                                              , int                       unitIndex
                                              , double*                   out_posForward
                                              , double*                   out_posLeft
                                              , double*                   out_posUp);

/** Returns the orientation of the specified vehicle unit, in the VehicleSim
  coordinate system, in radians, via output parameters.

  Output parameters must be valid (i.e. not NULL pointers).

  Returns true on success, returns false (and leaves output parameters
  unchanged) if position is unavailable.

  This may only be called if vsew_IsOk() is true for the specified vehicle.
*/

VS_API bool VS_API_2    vsew_GetWorldOrientation( const VSEW_VEHICLE_HANDLE vehicleHandle
                                                , int                       unitIndex
                                                , double*                   out_rollRight
                                                , double*                   out_pitchDown
                                                , double*                   out_yawLeft);

/** Returns the world position of the vehicle's tire via output parameters,
    in meters, in the VehicleSim coordinate system. Parameters must NOT be
    null pointers. Returns true on success, returns false (and leaves output
    parameters unchanged) if position is unavailable.
    This may only be called if vsew_IsOk() is true for the specified vehicle.
*/
VS_API bool VS_API_2    vsew_GetTireWorldPosition(  const VSEW_VEHICLE_HANDLE vehicleHandle
                                                  , int     unitIndex
                                                  , int     axleIndex
                                                  , int     wheelIndex
                                                  , double* out_posForward
                                                  , double* out_posLeft
                                                  , double* out_posUp);

/** Returns the orientation in radians for a specified tire via output parameters.
    Parameters must NOT be null pointers. Returns true on success, returns false (and leaves output
    parameters unchanged) if orientation is unavailable.
    This may only be called if vsew_IsOk() is true for the specified vehicle.

    Tire is specified by the TireIndex
*/
VS_API bool VS_API_2    vsew_GetTireWorldOrientation( const VSEW_VEHICLE_HANDLE vehicleHandle
                                                    , int     unitIndex
                                                    , int     axleIndex
                                                    , int     wheelIndex
                                                    , double* out_rollRight
                                                    , double* out_pitchDown
                                                    , double* out_yawLeft);

/** Returns the Rotation in radians for a specified tire via output parameters.
    Parameters must NOT be null pointers. Returns true on success, returns false (and leaves output
    parameters unchanged) if orientation is unavailable.
    This may only be called if vsew_IsOk() is true for the specified vehicle.
*/
VS_API bool VS_API_2    vsew_GetTireSolverRotation( const VSEW_VEHICLE_HANDLE vehicleHandle
                                                  , int       unitIndex
                                                  , int       axleIndex
                                                  , int       wheelIndex
                                                  , double*   out_rotPitchRad);

/// This may only be called if vsew_IsOk() is true for the specified vehicle.
VS_API void VS_API_2    vsew_SetWorldPosition(  VSEW_VEHICLE_HANDLE vehicleHandle
                                              , double              posForward
                                              , double              posLeft
                                              , double              posUp);

/** Sets the orientation via Euler angles in radians.
    This may only be called if vsew_IsOk() is true for the specified vehicle.
*/
VS_API void VS_API_2    vsew_SetWorldOrientation(   VSEW_VEHICLE_HANDLE vehicleHandle
                                                  , double              rollRightRad
                                                  , double              pitchDownRad
                                                  , double              yawLeftRad   );

/**
  \deprecated
  DEPRECATED: This function may be removed for VehicleSim 2022.1 or later.
  Use the following instead:

      int throtInp = vsew_Import_Add3( vehicleHandle, "IMP_THROTTLE_ENGINE", 0, true, "REPLACE", VSEW_IVA_DEFAULT, VSEW_IVCO_DEFAULT );
      ...
      vsew_Import_SetValue( vehicleHandle, throtInp, newThrottleValue );

  This may only be called if vsew_IsOk() is true for the specified vehicle.

  Specify the normalized throttle pedal position in the range 0.0 to 1.0.
  
  Calls to this function are ignored unless the vehicle was created with
  vsew_CreateVehicle() with the useExternalDriverInput set to true.
*/
#if (__cplusplus >= 201103)
[[deprecated]]
#endif
VS_API void VS_API_2    vsew_SetDriverThrottle(   VSEW_VEHICLE_HANDLE vehicleHandle
                                                  /// Normalized (0.0 to 1.0) throttle pedal position
                                                , double              throttleNorm);

/**
  \deprecated
  DEPRECATED: This function may be removed for VehicleSim 2022.1 or later.
  Use the following to create an import instead:

      vsew_Import_Add3( vehicleHandle, "IMP_FBK_PDL", 0, true, "REPLACE", VSEW_IVA_USE_LAST, VSEW_IVCO_YES );

  This may only be called if vsew_IsOk() is true for the specified vehicle.

  Specify the amount of force (in Newtons) to apply to the brake pedal.

  Calls to this function are ignored unless the vehicle was created with
  vsew_CreateVehicle() with the useExternalDriverInput set to true.
*/
#if (__cplusplus >= 201103)
[[deprecated]]
#endif
VS_API void VS_API_2    vsew_SetDriverBrakePedalNewtons(  VSEW_VEHICLE_HANDLE vehicleHandle
                                                          /// Brake pedal force in Newtons
                                                        , double              brakePedalNewtons);

/**
  \deprecated
  DEPRECATED: This function may be removed for VehicleSim 2022.1 or later.
  Use the following to create imports instead:

      vsew_Import_Add3( vehicleHandle, "IMP_STEER_SW", 0, true, "REPLACE", VSEW_IVA_DEFAULT, VSEW_IVCO_DEFAULT );
      vsew_Import_Add3( vehicleHandle, "IMP_DSTEER_SW", 0, true, "REPLACE", VSEW_IVA_DEFAULT, VSEW_IVCO_DEFAULT );

  This may only be called if vsew_IsOk() is true for the specified vehicle.

  Specify the angle of the steering wheel in degrees to the left of center
  (i.e. turning to the left is a positive angle, steering to the right is a
  negative angle).

  Calls to this function are ignored unless the vehicle was created with
  vsew_CreateVehicle() with the useExternalDriverInput set to true.
*/
#if (__cplusplus >= 201103)
[[deprecated]]
#endif
VS_API void VS_API_2    vsew_SetDriverSteerLeftDegrees(   VSEW_VEHICLE_HANDLE vehicleHandle
                                                          /// Steering angle to the left (degrees)
                                                        , double              steerDeg);

/** This may only be called if the vehicleHandle parameter is valid.
    Returns the approximate current time of the solver (the time to which it has
    been run), or ((double)(-FLT_MAX)) if the solver has not been run.
*/
VS_API double VS_API_2  vsew_GetTime( VSEW_VEHICLE_HANDLE vehicleHandle );

/** This may only be called if vsew_IsOk() is true for the specified vehicle.
    Returns 0 if the solver is successfully run to (near) the specified time.
    Returns a negative value if there was an error.
    Returns 1 if the solver has requested that the run be stopped (e.g.
    run has hit solver's configured end time, or a run-ending event has
    occurred in the solver). 
*/
VS_API int VS_API_2     vsew_Integrate(   VSEW_VEHICLE_HANDLE vehicleHandle
                                          /// Time period to integrate, in Seconds.
                                        , double              deltaTimeSec  );

/** This may only be called if vsew_IsOk() is true for the specified vehicle.
    Returns 0 if the solver is successfully run to (near) the specified time.
    Returns a negative value if there was an error.
    Returns 1 if the solver has requested that the run be stopped (e.g.
    run has hit solver's configured end time, or a run-ending event has
    occurred in the solver). 
*/
VS_API int VS_API_2     vsew_IntegrateTo(   VSEW_VEHICLE_HANDLE vehicleHandle
                                          , double              timeLimit     );

VS_API void VS_API_2    vsew_DestroyVehicle(VSEW_VEHICLE_HANDLE vehicle);

/* Advanced Functions:
   -------------------

   The functions below should be used only by advanced
   users who have a thorough understanding of the side-effects/consequences.
   These functions are dangerous and can lead to unexpected/undefined behavior
   and/or program crashes if used incorrectly.
*/

/** Returns a non-const pointer to the specified solver variable, or NULL if
  the specified variable cannot be found.

  Modifying the returned value may be dangerous (i.e. result in unexpected or
  undefined behavior and/or cause the program to crash).

  This may only be called if vsew_IsOk() is true for the specified vehicle.
*/
VS_API double VS_API_2  *vsew_GetVarPtr_NC(   const VSEW_VEHICLE_HANDLE vehicleHandle
                                            , const char* varName);

/** Returns a non-const pointer to the specified integer variable, or NULL
  if the specified variable cannot be found.

  Modifying the returned value may be dangerous (i.e. result in unexpected or
  undefined behavior and/or cause the program to crash).

  This may only be called if vsew_IsOk() is true for the specified vehicle.
*/
VS_API int VS_API_2     *vsew_GetVarPtrInt_NC(  const VSEW_VEHICLE_HANDLE vehicleHandle
                                              , const char* varName);

#ifdef __cplusplus
}
#endif


#endif  // _vs_vehicle_hpp_INCLUDED
