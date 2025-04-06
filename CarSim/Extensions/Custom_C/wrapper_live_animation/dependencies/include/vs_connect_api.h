/* Copyright (c) 2017-2021, Mechanical Simulation Corporation
All rights reserved.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#pragma once
#ifndef vs_connect_api_h_INCLUDED
#define vs_connect_api_h_INCLUDED


#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>


# ifdef _WIN32
#   if defined(VSC_BUILDING_DLL)
#     define VSC_DECLSPEC     __declspec(dllexport)
#   elif defined(VSC_BUILDING_STATIC)
#     define VSC_DECLSPEC
#   else
#     if !defined(VSC_USING_DLL)
#       define VSC_USING_DLL
#     endif
#     define VSC_DECLSPEC     __declspec(dllimport)
#   endif
#   define VSC_DECLSPEC_2
# else
#     define VSC_DECLSPEC
#     define VSC_DECLSPEC_2 __attribute__((visibility("default")))
#     define __cdecl
# endif


/// \deprecated items. Do not use these.
///@{
/// \deprecated - Instead, use VscRecordPreDestroyFunc_t
#define VscUpdateDataPreDestroyFunc_t     VscRecordPreDestroyFunc_t
/// \deprecated - Instead, use VscRecord_VSCLASS
#define VscUpdateData_VSCLASS             VscRecord_VSCLASS
/// \deprecated - Instead, use VscRecord
#define VscUpdateData                     VscRecord

/// \deprecated - Instead, use Contract_GetRecord
#define Contract_GetUpdateData            Contract_GetRecord
/// \deprecated - Instead, use Record_CopyData
#define UpdateData_CopyData               Record_CopyData
/// \deprecated - Instead, use Record_CreateFromSchema
#define UpdateData_CreateFromSchema       Record_CreateFromSchema
/// \deprecated - Instead, use Record_GetAppData
#define UpdateData_GetAppData             Record_GetAppData
/// \deprecated - Instead, use Record_GetFieldValue
#define UpdateData_GetFieldValue          Record_GetFieldValue
/// \deprecated - Instead, use Record_GetSchema
#define UpdateData_GetSchema              Record_GetSchema
/// \deprecated - Instead, use Record_GetSerialNumber
#define UpdateData_GetSerialNumber        Record_GetSerialNumber
/// \deprecated - Instead, use Record_GetSimtime
#define UpdateData_GetSimtime             Record_GetSimtime
/// \deprecated - Instead, use Record_GetSimtimeI
#define UpdateData_GetSimtimeI            Record_GetSimtimeI
/// \deprecated - Instead, use Record_GetTimeDilation
#define UpdateData_GetTimeDilation        Record_GetTimeDilation
/// \deprecated - Instead, use Record_GetWritableFieldValue
#define UpdateData_GetWritableFieldValue  Record_GetWritableFieldValue
/// \deprecated - Instead, use Record_HandleCopy
#define UpdateData_HandleCopy             Record_HandleCopy
/// \deprecated - Instead, use Record_HandleRelease
#define UpdateData_HandleRelease          Record_HandleRelease
/// \deprecated - Instead, use Record_SetAppData
#define UpdateData_SetAppData             Record_SetAppData
/// \deprecated - Instead, use Record_SetPreDestroyCallback
#define UpdateData_SetPreDestroyCallback  Record_SetPreDestroyCallback
/// \deprecated - Instead, use Record_SetSimtime
#define UpdateData_SetSimtime             Record_SetSimtime
/// \deprecated - Instead, use Record_SetSimtimeI
#define UpdateData_SetSimtimeI            Record_SetSimtimeI
///@}


typedef enum VscResult
{
  VSC_RES_UNDEFINED             = -128,   ///< Unknown/undefined error.
  VSC_RES_ERROR                 = -127,   ///< Generic error.
  //--------------------------
  VSC_RES_ERROR_MESSAGE_TYPE    = -15,    ///< Unsupported message type.
  VSC_RES_ERROR_LICENSE         = -14,    ///< There is no license to allow the action.
  VSC_RES_ERROR_APP_DENIED      = -13,    ///< The Application denied the operation.
  VSC_RES_ERROR_TIMING          = -12,    ///< The operation or message type cannot be processed right now.
  VSC_RES_ERROR_INTERNAL        = -11,    ///< Indicates an unexpected internal failure in the library (primarily used for debugging).
  VSC_RES_ERROR_UNINITIALIZED   = -10,    ///< The library is not initialized.
  VSC_RES_ERROR_TERMINATED      = -9,     ///< The referenced operation or object was explicitly canceled or destroyed.
  VSC_RES_ERROR_INVALID_DATA    = -8,     ///< The data provided is not valid.
  VSC_RES_ERROR_INVALID_PARAM   = -7,     ///< One or more of the parameters is invalid, or the parameters are incompatible with each other.
  VSC_RES_ERROR_RESOURCE        = -6,     ///< A resource needed to complete the request is not available.
  VSC_RES_ERROR_MEMORY          = -5,     ///< Memory could not be allocated to complete the request (out of memory).
  VSC_RES_ERROR_UNSUPPORTED     = -4,     ///< The request is not supported by this version.
  VSC_RES_ERROR_UNAVAILABLE     = -3,     ///< The request is not available at this time.
  VSC_RES_ERROR_TIMEOUT         = -2,     ///< A timeout expired before the request could be completed.
  VSC_RES_ERROR_VERSION         = -1,     ///< The specified version is not supported.
  VSC_RES_OK                    = 0,      ///< Success! No error!
  VSC_RES_OK_ASYNC              = 1,      ///< Successfully initiated asynchronous operation.
  VSC_RES_WARNING               = 2,      ///< Generic warning (non-fatal).
  VSC_RES_WARNING_NOOP          = 3,      ///< No-op (non-fatal).
} VscResult;


/// Returns true if the passed result code represents some type of error.
static inline bool      vsc_IsError( VscResult res )    { return res < VSC_RES_OK;        }
static inline bool      vsc_IsWarning( VscResult res )  { return res >=  VSC_RES_WARNING; }
static inline bool      vsc_IsOk( VscResult res )       { return !vsc_IsError(res) && !vsc_IsWarning(res); }


typedef enum VscLogLevel
{
  VSC_LOG_DISABLED  = -1, ///< Log setting that disables all log messages.
  VSC_LOG_DEFAULT   = 0,  ///< Log setting that instructs the library to use its default log level.
  VSC_LOG_ALWAYS    = 1,  ///< Log level for messages that are always logged (when logging is enabled), regardless of log level setting.
  VSC_LOG_ERROR     = 2,  ///< Something unexpected has occurred that prevents proper functioning.
  VSC_LOG_WARNING   = 3,  ///< Something unusual has occurred which could cause unexpected behavior.
  VSC_LOG_INFO      = 4,  ///< General informational message.
  VSC_LOG_HELPFUL   = 5,  ///< Helpful details that may assist with troubleshooting unexpected behavior.
  VSC_LOG_VERBOSE   = 6,  ///< Excessive text for those that want to see all available information.
  VSC_LOG_DEBUG     = 7,  ///< Includes log messages only available in _DEBUG builds of this library.
} VscLogLevel;


typedef enum VscConnectionStatus
{
  VSC_CONNSTAT_UNDEFINED        = 0,  ///< The connection state is undefined. This indicates an error (e.g. use of an uninitialized object).
  //==========================
  VSC_CONNSTAT_UNCONNECTED      = 1,  ///< We are not connected, and we are not attempting to connect. This is the initial/default connection state of a Peer.
  VSC_CONNSTAT_CONNECTING       = 2,  ///< We are not connected, but we are currently attempting to connect.
  VSC_CONNSTAT_CONNECTED        = 3,  ///< We are currently connected.
  VSC_CONNSTAT_ERROR            = 4,  ///< We are not connected and not attempting to connect because An unexpected error occurred.
  VSC_CONNSTAT_DISCONNECTING    = 5,  ///< We are in the process of disconnecting from the Peer.
} VscConnectionStatus;


/**
Indicates a participant in a connection between two peers.
Identifies a party relative to the current program instance (process).
*/
typedef enum VscParty
{
  /// !!! IMPORTANT: Values of elements must be representable by uint8_t (unsigned char).
  VSC_PARTY_UNDEFINED = 0,
  //==========================
  VSC_PARTY_LOCAL     = 1,  ///< "Us"
  VSC_PARTY_REMOTE    = 2,  ///< "Them"
} VscParty;


/// Handle types for VS Connect objects.
///@{
struct VscNode_VSCLASS;             ///< Forward declaration of internal structure.
struct VscLink_VSCLASS;             ///< Forward declaration of internal structure.
struct VscSchema_VSCLASS;           ///< Forward declaration of internal structure.
struct VscField_VSCLASS;            ///< Forward declaration of internal structure.
struct VscContract_VSCLASS;         ///< Forward declaration of internal structure.
struct VscUpdateDefinition_VSCLASS; ///< Forward declaration of internal structure.
struct VscRecord_VSCLASS;           ///< Forward declaration of internal structure.

#define VscNode               struct VscNode_VSCLASS*             ///< Represents a local communication endpoint. More than one Node may be created.
#define VscLink               struct VscLink_VSCLASS*             ///< Represents a communication channel to a remote Node. A Node may have many Links associated with it.
#define VscSchema             struct VscSchema_VSCLASS*           ///< An ordered collection of VscFields. Describes the structure of Data Updates that are sent or received. Analogous to a a database table definition.
#define VscField              struct VscField_VSCLASS*            ///< Describes the content of a single entry in a Schema. A Field may contain a single value, or a homogeneous array of values.
#define VscContract           struct VscContract_VSCLASS*         ///< An agreement between two connected Nodes that describes what data is to be sent and when.
#define VscUpdateDefinition   struct VscUpdateDefinition_VSCLASS* ///< Description of an update (e.g. schedule, data, etc.).
#define VscRecord             struct VscRecord_VSCLASS*           ///< A packet of data at a singular time (simulation time). Analogous to a database record.
///@}


/**
\deprecated
All of the items in this group are in a preview/alpha state and may change or
removed without warning.

It is recommended that they NOT be used in production code.
*/
///@{
typedef   int64_t   VscTime;  ///< Type used to store time, time scale, and duration.

#define   VSC_TIME_INVALID            (INT64_MIN)
#define   VSC_TIME_NOW                (VSC_TIME_INVALID + 1)
#define   VSC_TIME_LATEST             (VSC_TIME_INVALID + 2)
//...
#define   VSC_TIME_NEGATIVE_INFINITY  (VSC_TIME_INVALID + 1000)
#define   VSC_TIME_VALID_MIN          (VSC_TIME_INVALID + 1001)
#define   VSC_TIME_VALID_MAX          (-VSC_TIME_VALID_MIN) 
#define   VSC_TIME_INFINITY           (-VSC_TIME_NEGATIVE_INFINITY)

/// Returns a value that will fail VscIsValidTime().
inline VscTime          VscGetInvalidTime()                       { return VSC_TIME_INVALID; }
inline bool             VscIsValidTime( VscTime time )            { return VSC_TIME_VALID_MIN <= time && time <= VSC_TIME_VALID_MAX; }
inline void             VscInvalidateTime ( VscTime* out_time )   { *out_time = VSC_TIME_INVALID; }

#define   VSC_INT64_INVALID             VSC_TIME_INVALID
#define   VSC_INT64_NEGATIVE_INFINITY   VSC_TIME_NEGATIVE_INFINITY
#define   VSC_INT64_VALID_MIN           VSC_TIME_VALID_MIN
#define   VSC_INT64_VALID_MAX           VSC_TIME_VALID_MAX
#define   VSC_INT64_INFINITY            VSC_TIME_INFINITY

/// Returns a value that will fail VscIsValidTime().
inline int64_t          VscGetInvalidInt64()                      { return VSC_INT64_INVALID; }
inline bool             VscIsValidInt64( int64_t val )            { return VSC_INT64_VALID_MIN <= val && val <= VSC_INT64_VALID_MAX; }
inline void             VscInvalidateInt64( int64_t* out_val  )   { *out_val = VSC_INT64_INVALID; }
///@}


/**
Types of data that may be sent/received via VS Connect Contract Updates.
Note: Only VSC_DATATYPE_FLOAT and VSC_DATATYPE_INT are supported by this
version of VS Connect.
*/
typedef enum VscDataType
{
  VSC_DATATYPE_UNKNOWN  = 0,
  VSC_DATATYPE_FLOAT    = 1,
  VSC_DATATYPE_INT      = 2,
  // Reserved for future use:
  //    VSC_DATATYPE_UINT     = 3,
  //    VSC_DATATYPE_BLOB     = 4,
  //    VSC_DATATYPE_TEXT     = 5,
  VSC_DATATYPE_TIME     = 6,
  // Reserved for future use:

} VscDataType;


typedef enum VscInterp
{
  VSC_INTERP_DEFAULT      = 0,  ///< Use the default interpolation method for the data type.
  //------------------------------
  //  RESERVED FOR FUTURE USE
  //    VSC_INTERP_LINEAR       = 1,  ///< Linear interpolation.
  //    VSC_INTERP_FLOOR        = 2,  ///< The lower of the two samples is used.
  //    VSC_INTERP_NEAREST      = 3,  ///< Output value steps from the first value to the second at the midpoint between the samples.
  //    VSC_INTERP_CEIL         = 4,  ///< The higher of the two samples is used.
  //    VSC_INTERP_LINEAR_STEP  = 5,  ///< Value is linearly interpolated, then converted to an integer based on the VscRounding setting.
  //==============================
  VSC_INTERP_COUNT,
} VscInterp;


typedef enum VscExtrap
{
  VSC_EXTRAP_DEFAULT    = 0,  ///< Use the default extrapolation method for the data type.
  //------------------------------
  //  RESERVED FOR FUTURE USE
  //    VSC_EXTRAP_NONE       = 1,
  //    VSC_EXTRAP_FLAT       = 2,
  //    VSC_EXTRAP_STRAIGHT   = 3,
  //==============================
  VSC_EXTRAP_COUNT,
} VscExtrap;


typedef enum VscRounding
{
  VSC_ROUNDING_DEFAULT  = 0,  ///< Use the default rounding method for the data type.
  //------------------------------
  //  RESERVED FOR FUTURE USE
  //    VSC_ROUNDING_FLOOR    = 1,
  //    VSC_ROUNDING_NEAREST  = 2,
  //    VSC_ROUNDING_CEIL     = 3,
  //    VSC_ROUNDING_TRUNCATE = 4,
  //==============================
  VSC_ROUNDING_COUNT,
} VscRounding;


/**
Type of schedule used to determine when VS Connect should send an update.
*/
typedef enum VscScheduleType
{
  VSC_SCHED_UNDEFINED                     = 0,
  VSC_SCHED_PERIODIC_SIMTIME_MILLISECONDS = 1,  ///< Periodic updates w/ a period specified in milliseconds of simulation time.
  VSC_SCHED_TRIGGERED                     = 2,  ///< Updates are sent only sent when explicitly triggered by The Application.
} VscScheduleType;


/**
Identifies a party relative to a VscContract.
*/
typedef enum VscRole
{
  VSC_ROLE_UNDEFINED  = 0,
  VSC_ROLE_SENDER     = 1,
  VSC_ROLE_RECEIVER   = 2,
} VscRole;


typedef uint32_t  VscSerialNum_t;   ///< Type used to store the serial number of a VscRecord.


/// Attributes of a moment in time.
typedef enum TimeMomentAttr
{
  // IMPORTANT: Be sure to keep these in sync w/ the named members of the MomentInTime struct.
  TMA_STEADY_TIME         = 0,  ///< The current/immediate system time.
  TMA_SYSTEM_TIME         = TMA_STEADY_TIME,  ///< \deprecated 2020.0 Use TMA_STEADY_TIME instead.
  TMA_SIMTIME,                  ///< The simulation time.
  TMA_SIMTIME_STEADY_TIME,      ///< The system time that corresponds to the simtime.
  TMA_SIMTIME_SYSTEM_TIME = TMA_SIMTIME_STEADY_TIME,  ///< \deprecated 2020.0 Use TMA_SIMTIME_STEADY_TIME instead.
  TMA_SIMTIME_DILATION,         ///< The time scale/multiplier.
  //==============================
  TMA_COUNT,
} TimeMomentAttr;


// A moment in the ping Request-Response-Result process timeline.
typedef enum PingMilestone
{
  PM_REQUESTER_REQUEST_INITIATED  = 0,  ///< Moment the request was initiated by the Requester.
  PM_REQUESTER_REQUEST_QUEUED,          ///< Moment that VS Connect processed the request and queued it for transmission.
  //----------------------------
  PM_RESPONDER_REQUEST_RECEIVED,        ///< Moment that VS Connect received the request from the Requester.
  PM_RESPONDER_REQUEST_PROCESSED,       ///< Moment that VS Connect processed the incoming Request.
  PM_RESPONDER_RESPONSE_QUEUED,         ///< Moment that VS Connect queued the Response to be sent back to the Requester.
  //----------------------------
  PM_REQUESTER_RESPONSE_RECEIVED,       ///< Moment that the Requester received the Response from the Responder.
  //==============================
  PM_COUNT,
} PingMilestone;


/**
Time Sync (TS) mode.
*/
typedef enum VscTimeSyncMode
{
  VSC_TSM_NONE      = 0,  ///< Time Sync disabled.
  VSC_TSM_ABSOLUTE  = 1,  ///< Synchronize simulation time of Peers absolutely.
  //==============================
  VSC_TSM_COUNT,          ///< Number of TS Modes.
} VscTimeSyncMode;


/**
Bitmask of Log targets
*/
typedef enum VscLogTargetMask
{
  VSC_LOGTARGET_NONE      = 0,        ///< Discard all log requests.
  VSC_LOGTARGET_REGFUNC   = (1 << 0), ///< Call the VscLogFunc_t registered by The Application.
  VSC_LOGTARGET_STDOUT    = (1 << 1), ///< Write log output to stdout.
  VSC_LOGTARGET_STDERR    = (1 << 2), ///< Write log output to stderr.
} VscLogTargetMask;


/**
Callback alerting The Application (your program that uses this API) that
VS Connect has a message to be logged.

Implement this function in your application, then register your function using
SetLogFunc(). When VS Connect has a message (including warnings and
errors) to be logged, it will call your registered function.

When VS Connect calls your function, it will specify the Node, Link, and
Contract that are related to the log entry. Any of these may be NULL (if there
is no such object related to the log entry).

Any VS Connect function call may trigger a callback to your registered log
function.

Your function should treat the format and argptr parameters in the same
manner as vprintf().
*/
typedef int       (*VscLogFunc_t)(    VscLogLevel         logLevel
                                    , const VscNode       node
                                    , const VscLink       link
                                    , const VscContract   contract
                                    , const char*         format
                                    , va_list             argptr    );

/**
Callback function prototypes for functions used by VS Connect to call back
into The Application (your program that uses this API). In order to process
these events, The Application should define functions with these signatures
and register them with either the VS Connect in the Node_Create_UDPIP() call,
or one of the Node_SetCallback_*() functions.

These callbacks are triggered by events within VS Connect that occur while it
is processing a call to Node_Service().
*/
///@{
/**
Event callback alerting The Application that a Link has become connected.

Return VSC_RES_OK to allow the connection to be made.

Returning anything other than VSC_RES_OK will reject the connection. If the
connection is rejected, VS Connect will release it's handle to the
Link, and the Link will be disassociated from it's Node.
*/
typedef VscResult (*VscLinkConnectedFunc_t)( VscLink connectingLink );

/**
Event callback alerting The Application that a Link has become disconnected.

After returning from this function, VS Connect will release it's handle to
the Link, and the Link will be disassociated from the Node to which is was
associated. If The Application holds any handles to this Link, it will not
be destroyed, and it can be reused (e.g. to reconnect in the future).
*/
typedef void      (*VscLinkDisconnectedFunc_t)(   VscLink   disconnectedLink
                                                , VscResult disconnectReason );

/**
Event callback alerting The Application that a new Contract request has
been received. Use Contract_GetLocalRole() to determine if this is an
"incoming" (we receive Updates) or "outgoing" (we send Updates) Contract.

Return VSC_RES_OK to accept the Contract - any other return value will cause
the Contract to be rejected.
*/
typedef VscResult (*VscProcessContractRequestFunc_t)(   const VscLink link
                                                      , VscContract   contract );

/**
Event callback alerting The Application that the specified contract is
being canceled.

The Application should destroy any handles it holds to the specified contract
and clean up any additional resources allocated by The Application that are
associated with this contract.

When this callback function returns, VS Connect will disassociate the
Contract from its Link and release all internal handles to the Contract.
When all handles have been released from the Contract, it will be deleted
from memory automatically.
*/
typedef void      (*VscContractCanceledFunc_t)(   const VscLink link
                                                , VscContract   contract );

/**
Event callback called by VS Connect when it needs new data to send.
When called, The Application is expected to fill the data buffer (out_data)
with the current/latest data.

If The Application returns a non-error (i.e. vsc_IsError() == false), then
VS Connect will send the data to the remote peer.
If The Application returns a result that is an error (i.e. vsc_IsError() ==
true), VS Connect will send nothing to the remote peer.

When this call is received, The Application MUST update ALL field values.
If data is not available for a field, The Application should invalidate
the value of the field using the appropriate GetInvalid<type>() or
Invalidate<type>() function (e.g. GetInvalidDouble() or InvalidateDouble()).
*/
typedef VscResult (*VscSendUpdateFunc_t)(   const VscLink     link
                                          , const VscContract contract
                                          , VscRecord         out_data );

/**
Callback by VS Connect when new data has been received from the specified
Incoming Contract.
*/
typedef VscResult (*VscReceiveUpdateFunc_t)(  const VscLink     link
                                            , const VscContract contract
                                            , const VscRecord   data      );

/**
Callback from VS Connect when a response from our Ping request has been
received for the specified Link. Use the Link_PingGetStat() function to
examine the ping timing data.
*/
typedef void      (*VscPingResultsFunc_t)( const VscNode vscNode, const VscLink link );

/**
Object Deallocation Callback function types. These callbacks can be used to
inform The Application that an object is about to be deleted. If registered,
the callback is called immediately before the object's memory is freed.

Copying or releasing the handle parameter, e.g. Node_HandleCopy(objectToBeDestroyed),
is prohibited and will result in undefined behavior (which may include crashing
The Application).
*/
///@{
typedef void      (*VscNodePreDestroyFunc_t)              ( VscNode             objectToBeDestroyed );
typedef void      (*VscLinkPreDestroyFunc_t)              ( VscLink             objectToBeDestroyed );
typedef void      (*VscSchemaPreDestroyFunc_t)            ( VscSchema           objectToBeDestroyed );
typedef void      (*VscFieldPreDestroyFunc_t)             ( VscField            objectToBeDestroyed );
typedef void      (*VscContractPreDestroyFunc_t)          ( VscContract         objectToBeDestroyed );
typedef void      (*VscUpdateDefinitionPreDestroyFunc_t)  ( VscUpdateDefinition objectToBeDestroyed );
typedef void      (*VscRecordPreDestroyFunc_t)            ( VscRecord           objectToBeDestroyed );
///@}
///@}


/**
A structure that contains function pointers to the API functions of the
VS Connect version 6 API. Retrieve a set of function pointers by calling
VsConnectApi_Get(6), then use the returned pointer to call the API
functions contained in this struct. For example:

  \code{.c}
    vsc_Api_V6_t* myApiPtr = VsConnectApi_Get(6);
    myApiPtr->SetLogFunc( myLogFunction );
    myApiPtr->Init();
    VscNode   myNode = myApiPtr->Node_Create_UDPIP(...
    ...
    myApiPtr->Node_ReleaseHandle(&myNode);
    myApiPtr->Shutdown();
  \endcode
*/
typedef struct vsc_Api_V6_t
{
  /**
  Specifies the callback function to be used to output/display log messages
  generated by VS Connect.
  This may be called before calling Init() so that log messages during
  initialization can be captured.
  */
  void                  (__cdecl *SetLogFunc)( VscLogFunc_t logFunc );

  /**
  Specifies the level of log messages that will be sent to the log callback
  function.
  This may be called before calling Init() so that log messages during
  initialization can be captured.
  */
  void                  (__cdecl *SetLogLevel)( VscLogLevel logLevel );

  /**
  Initializes VS Connect.

  You must initialize VS Connect with this function before calling other VS
  Connect functions (unless otherwise noted).

  SetLogFunc() and SetLogLevel() may be called before calling this function to
  ensure log messages during initialization are captured. If SetLogFunc() is
  not called, VS Connect will send all log output to the console (stdout).

  Returns an error if VS Connect was not successfully initialized.
  */
  VscResult             (__cdecl *Init)();

  /**
  Returns true if VS Connect has been successfully initialized.
  */
  bool                  (__cdecl *IsInitialized)();

  /**
  Shuts down VS Connect and frees resources. Be sure to call this when your
  Application is finished using VS Connect (e.g. before exiting your application,
  or before unloading a DLL containing VS Connect).
  */
  void                  (__cdecl *Shutdown)();

  /**
  Returns an English string suitable for display to the user that
  describes the passed result code, e.g. "Error".
  */
  const char*           (__cdecl *DescribeResult)( VscResult resultCode );

  /**
  Returns true if the specified value is a valid VS Connect data value of this
  type (double precision float).
  */
  bool                  (__cdecl *IsValidDouble)( double value );

  /**
  Returns a value that is guaranteed to return false from
  IsValidDouble(). Use this to set a Field value when a VS Connect callback
  requests data that is not currently available. This indicates to the remote
  peer that there is currently no valid value available for the specified Field.
  */
  double                (__cdecl *GetInvalidDouble)();

  /**
  Pass a pointer to a variable to set that variable to an invalid value.
  */
  void                  (__cdecl *InvalidateDouble)( double* varToInvalidate );

  /**
  \deprecated
  This function is in a preview/alpha state and may be change or removed without
  warning.
  
  It is recommended that it NOT be used in production code.
  */
  VscTime               (__cdecl *GetDefaultTimeScale)();

  /**
  Returns true if the specified value is a valid VS Connect Simulation Time
  value.

  If data arrives from a remote peer with a value that returns false from this
  function, it indicates that there is no current Simulation Time (e.g. the
  simulation is not running).
  */
  bool                  (__cdecl *IsValidSimtime)( double time );

  /**
  Returns a value that is guaranteed to return false from
  IsValidSimtime(). Use this to set the time of an outgoing VscRecord update if
  the data is not associated with a specific Simulation Time or there is not
  currently a valid Simulation Time (e.g. the Simulation has not yet started,
  etc.).

  Use the value returned from this function if VS Connect requests the current
  simulation time when the simulation is not running. This will indicate to the
  remote peer that there is currently no valid simulation time (e.g. the
  simulation is not running).
  */
  double                (__cdecl *GetInvalidSimtime)();

  /**
  Pass a pointer to a variable to set that variable to an invalid value.
  */
  void                  (__cdecl *InvalidateTime)( double* varToInvalidate );

  /**
  Returns true if the specified Serial Number is valid.
  */
  bool                  (__cdecl *IsValidSerialNumber)( VscSerialNum_t serialNumber );

  /**
  Returns a value that is guaranteed to cause IsValidSerialNumber()
  to return false. Use this to initialize a Serial Number variable that does
  not yet have a value.
  */
  VscSerialNum_t        (__cdecl *GetInvalidSerialNumber)();

  /**
  Returns the first valid Serial Number value.
  All valid Serial Numbers are >= the value returned by this function.
  */
  VscSerialNum_t        (__cdecl *GetFirstSerialNumber)();

  /**
  Returns the last valid Serial Number value.
  All valid Serial Numbers are <= the value returned by this function.
  */
  VscSerialNum_t        (__cdecl *GetLastSerialNumber)();

  /// Returns the default listen port used for I.P. server Nodes.
  uint16_t              (__cdecl *GetDefaultListenPort)();

  /// Returns a descriptive name for the specified Time Sync Mode.
  const char*           (__cdecl *DescribeTsMode)( VscTimeSyncMode tsMode );

  /**
  Creates a VS Connect Node. A Node is the primary VS Connect object which
  processes communications (when The Application calls Node_Service()).

  If listenAddress is NULL, the Node will not accept incoming connections. This
  type of Node will only allow outgoing connections.

  If listenAddress is not NULL, the Node will open a server ("listen") port to
  accept incoming connections.

  If listenAddress is an empty string, the system will automatically select an
  appropriate address (i.e. the default IPv4 interface) for the server.
  Alternately, you may specify a valid host name or IPv4 address of an
  interface on the local machine on which the Node should listen. For example:
  "localhost", "127.0.0.1", "192.168.0.45", etc.

  The listenPort parameter specifies the UDP port on which the Node should
  should listen for incoming connections. If listenPort parameter is 0, the
  default listen port (GetDefaultListenPort()) will be used. If
  listenAddress is NULL, this parameter is ignored.

  The maxConnections parameter limits the total number of Links that can be
  associated with this Node. The total includes both incoming and outgoing
  connections.

  This function creates a reference to the created object and returns it. The
  calling function is responsible for saving the returned handle/reference and
  for calling the appropriate ReleaseHandle function on it when it is no longer
  in use. For example:

    {
      VscNode myNodeHandle = vscApi->Node_Create_UDPIP(...);
      while ( simulationIsRunning)
      {
        ProcessMySimulation();
        vscApi->Node_Service( myNodeHandle );
      }
      vscApi->Node_HandleRelease( &myNodeHandle );  // Release handle/reference now that we're done with it:
    }

  Failure to release the handle returned from this function will result in
  memory leaks. When all handles have been released, the memory (and any
  other resources) allocated for the object by VS Connect will be automatically
  freed.
  */
  VscNode               (__cdecl *Node_Create_UDPIP)(   const char*                     listenAddress
                                                      , uint16_t                        listenPort
                                                      , int                             maxConnections
                                                      , VscTimeSyncMode                 requiredTsMode
                                                      , VscLinkConnectedFunc_t          linkConnectCallback
                                                      , VscLinkDisconnectedFunc_t       linkdisconnectCallback
                                                      , VscProcessContractRequestFunc_t contractRequestCallback
                                                      , VscContractCanceledFunc_t       contractCanceledCallback
                                                      , VscSendUpdateFunc_t             sendUpdateCallback
                                                      , VscReceiveUpdateFunc_t          receiveUpdateCallback
                                                      , VscPingResultsFunc_t            pingResultsCallback
                                                      , VscResult*                      out_result
                                                    );

  /**
  Specifies a function of The Application to be called before the specified
  object is destroyed/deleted/freed.
  */
  void                  (__cdecl *Node_SetPreDestroyCallback)(  VscNode node
                                                              , VscNodePreDestroyFunc_t func );

  /**
  Makes a new copy of an existing handle.
  The parameters are not checked for validity - it is the responsibility
  of The Application to ensure that:
    - The source handle is valid (i.e. not NULL).
    - The pointer to the destination handle is not NULL.
    - The destination pointer is not valid (i.e. it is NULL).

  Copying a handle in this way increments the reference count of the underlying
  object. The Application must call the corresponding *_HandleRelease()
  function on the destination handle when it is no longer in use to release
  this reference and decrement the reference count.
  */
  void                  (__cdecl *Node_HandleCopy)(   VscNode   source
                                                    , VscNode*  out_destination );

  /**
  Releases a handle reference to an object.
  The parameters are not checked for validity - it is the responsibility
  of The Application to ensure that:
    - The pointer to the handle is not NULL.
    - The handle is valid (i.e. not NULL).

  When all handle references to an object have been released, the object will
  automatically be freed.
  */
  void                  (__cdecl *Node_HandleRelease)( VscNode* inout_handle );

  /**
  Associate app-defined data with the specified object.
  The VS Connect object must be valid.
  VS Connect does not do anything with this value, it is exclusively for the
  use of The Application, and The Application is responsible for any
  management/cleanup of this data.
  */
  void                  (__cdecl *Node_SetAppData)(   VscNode   vscNode
                                                    , void*     appData );

  /**
  Returns the value previously associated with this object using
  the corresponding *_SetAppData() function.
  */
  void*                 (__cdecl *Node_GetAppData)( const VscNode vscNode );

  /**
  Sets, changes, or removes the event callback. Only one callback function
  can be associated with each event type, so if an existing function was
  previously registered, it will be unregistered by this call. To unregister
  the existing function without specifying a new callback, pass NULL as the
  second parameter.
  */
  ///@{
  void                  (__cdecl *Node_SetCallback_LinkConnect)(      VscNode node, VscLinkConnectedFunc_t          linkConnectCallback       );
  void                  (__cdecl *Node_SetCallback_LinkDisconnect)(   VscNode node, VscLinkDisconnectedFunc_t       linkDisconnectCallback    );
  void                  (__cdecl *Node_SetCallback_ContractRequest)(  VscNode node, VscProcessContractRequestFunc_t contractRequestCallback   );
  void                  (__cdecl *Node_SetCallback_ContractCanceled)( VscNode node, VscContractCanceledFunc_t       contractCanceledCallback  );
  void                  (__cdecl *Node_SetCallback_SendUpdate)(       VscNode node, VscSendUpdateFunc_t             sendUpdateCallback        );
  void                  (__cdecl *Node_SetCallback_ReceiveUpdate)(    VscNode node, VscReceiveUpdateFunc_t          receiveUpdateCallback     );
  void                  (__cdecl *Node_SetCallback_PingResults)(      VscNode node, VscPingResultsFunc_t            pingResultsCallback       );
  ///@}

  /**
  Returns information about system and simulation time (from the Node's
  perspective). This can be used to get the following values:

    TMA_STEADY_TIME           The current/immediate system steady time.
    TMA_SIMTIME               The simulation time that was specified
                              when Node_Service() was last called.
    TMA_SIMTIME_STEADY_TIME   The system time when Node_Service()
                              was last called.
    TMA_SIMTIME_DILATION      The time scale/multiplier that was specified
                              when Node_Service() was last called.
  */
  double                (__cdecl *Node_GetTimeInfo)(  const VscNode   vscNode
                                                    , TimeMomentAttr  timeAttr );

  /**
  Returns the number of VscLinks currently associated with the specified
  VscNode. Note that once a VscLink becomes disconnected, its association with
  its VscNode is automatically severed.
  */
  int                   (__cdecl *Node_GetNumLinks)( const VscNode vscNode );

  /**
  Returns the VscLink that currently resides at the specified index. Note
  that when VscLinks become disconnected, they are automatically removed from
  the Node's array of Links, and this may change the indexes at which remaining
  Links reside, so you should not cache indexes for future use; Indexes should
  be used only for immediate iteration through connected Links. To store a
  reference to a link for later use, use Link_HandleCopy() on the returned
  link to retain a reference counted handle (then use Link_HandleRelease()
  when you are done with the handle).

  This function is tuned for performance and thus does not check the validity of
  the parameters, so you can expect The Application to crash if the vscNode
  parameter is invalid, or if linkIndex is out of bounds (linkIndex must be >= 0
  and < Node_GetNumLinks() ).
  */
  VscLink               (__cdecl *Node_GetLink)(    const VscNode   vscNode
                                                  , int             linkIndex );

  /**
  Services the specified Node. This allows queued data to be processed (i.e.
  send/receive data via network). The Links of the Node are also serviced. This
  call may result in registered callback functions being called.

  This is the call that allows VS Connect to perform most of it's processing.

  The Application should call this function periodically, and as frequently as
  is practical to ensure the lowest latency of data transmitted to/from peers.

  @param vscNode The Node to service.
  @param simtime The local simulation time.
  @param timeDilation The rate at which the local simulation is executing
    relative to "real time". E.g. 1.0 means the simulation is
    (trying to) execute at "real time" rate, 0.5 means the simulation is
    updating in "slow motion" (it takes two seconds of realtime to advance
    the simulation one second).
  */
  VscResult             (__cdecl *Node_Service)(    VscNode   vscNode
                                                  , double    simtime
                                                  , double    timeDilation  );

  /**
  When "true" is specified, the specified server node will reject all
  subsequent incoming connection requests. If a link connect callback
  (VscLinkConnectedFunc_t) has been registered for the node, it will not be
  called when incoming connections are attempted.

  When "false" is specified, the specified server Node will resume accepting
  incoming connection requests. If a link connect callback is registered, it
  will be called when an incoming connection request is received. If a link
  connect callback is not registered, all incoming connections will be
  automatically accepted.

  This can be used to simplify application shutdown by automatically rejecting
  connection requests during the shutdown process.
  */
  void                  (__cdecl *Node_IgnoreConnectionRequests)( VscNode node, bool ignoreIncomingConnectionRequests );

  /**
  Indicates if the specified server Node is currently accepting connections.
  Use Node_IgnoreConnectionRequests() to change whether or not a Node will
  accept connections.

  The passed Node must be valid.
  */
  bool                  (__cdecl *Node_ConnectionRequestsIgnored)( const VscNode node );

  /**
  Returns true if this Node contains any Links with Contracts that may
  enter the "blocking" state.
  */
  bool                  (__cdecl *Node_CanBlock)( const VscNode node );

  /**
  Returns true if one or more of this Node's Link's Incoming Contracts is
  in the "blocking" state.
  */
  bool                  (__cdecl *Node_IsBlocked)( const VscNode node );

  /**
  Returns the Time Sync Mode for the specified Node.
  */
  VscTimeSyncMode       (__cdecl *Node_GetTsMode)( const VscNode node );

  /**
  Returns the Contract that defines the Time Sync Mode for the
  specified Node.

  Returns NULL if the Node is not subject to Time Sync, or if the Node is
  subject to Time Sync but there is not currently a TS Contract to manage it.

  This function does NOT increase the reference count on the returned object. If
  you intend to cache the return value for later use, you should use
  *_HandleCopy() and *_HandleRelease() functions for the return object type.
  */
  VscContract           (__cdecl *Node_GetTsContract)( const VscNode node );

  /**
  Defines a new Link that will communicate via UDP/IP.
  If the remote IP address cannot be immediately resolved, the call will fail
  and return a NULL handle.

  This call does not attempt to establish the connection to the remote peer. Use
  Link_Connect_Async() to initiate the Link connection process.

  Returns a reference counted handle to the new VscLink object. The caller of
  this function is responsible for calling Link_HandleRelease() to release
  the returned reference when it is no longer needed. When all handle
  references to the object have been released, the object will automatically
  be deleted/freed.
  */
  VscLink               (__cdecl *Link_Create_UDPIP)(   const char* serverAddress /**< Host name, or I.P. address of remote node. */
                                                      , uint16_t    serverPort    /**< Port on remote node that accepts VS Connect connections. */
                                                      , VscResult*  out_result    /**< Result of call (e.g. detailed error code. */
                                                    );

  /**
  Specifies a function of The Application to be called before the specified
  object is destroyed/deleted/freed.
  */
  void                  (__cdecl *Link_SetPreDestroyCallback)(  VscLink link
                                                              , VscLinkPreDestroyFunc_t func );

  /**
  Makes a new copy of an existing handle.
  The parameters are not checked for validity - it is the responsibility
  of The Application to ensure that:
    - The source handle is valid (i.e. not NULL).
    - The pointer to the destination handle is not NULL.
    - The destination pointer is not valid (i.e. it is NULL).

  Copying a handle in this way increments the reference count of the underlying
  object. The Application must call the corresponding *_HandleRelease()
  function on the destination handle when it is no longer in use to release
  this reference and decrement the reference count.
  */
  void                  (__cdecl *Link_HandleCopy)( VscLink   source
                                                  , VscLink*  out_destination );

  /**
  Releases a handle reference to an object.
  The parameters are not checked for validity - it is the responsibility
  of The Application to ensure that:
    - The pointer to the handle is not NULL.
    - The handle is valid (i.e. not NULL).

  When all handle references to an object have been released, the object will
  automatically be freed.
  */
  void                  (__cdecl *Link_HandleRelease)( VscLink* inout_handle );

  /**
  Associate app-defined data with the specified object.
  VS Connect does not do anything with this value, it is exclusively for the
  use of The Application, and The Application is responsible for any
  management/cleanup of this data.
  */
  void                  (__cdecl *Link_SetAppData)(   VscLink link
                                                    , void*   appData );

  /**
  Returns the value previously associated with this object using
  the corresponding *_SetAppData() function.
  */
  void*                 (__cdecl *Link_GetAppData)( const VscLink link );

  /**
  Returns the VS Connect Node that the specified link belongs to, or NULL if
  the Link is disconnected.

  This function does NOT increase the reference count on the returned object. If
  you intend to cache the return value for later use, you should use
  *_HandleCopy() and *_HandleRelease() functions for the return object type.
  */
  VscNode               (__cdecl *Link_GetNode)( const VscLink vscLink );

  /**
  Asynchronously initiates an attempt to connect the specified Link. This
  function will return immediately and the connection request will be queued for
  processing during the next call to Node_Service().

  Note that the connection process requires some back-and-forth communication
  with the remote peer, and it will take several calls to Node_Service() to
  complete this handshake process.

  When the connection is established, The Application's connect callback
  function will be called (i.e. the VscLinkConnectedFunc_t parameter specified
  when the Node was created).

  Returns an error (vsc_IsError()) if the connection cannot be initiated (e.g.
  invalid or unreachable address/port).

  Returns VSC_RES_OK if the connection is already established.

  Returns VSC_RES_OK_ASYNC if the connection attempt has been successfully
  initiated.
  */
  VscResult             (__cdecl *Link_Connect_Async)(  VscNode   vscNode
                                                      , VscLink   vscLink );

  /**
  Asynchronously initiates the disconnection process for the Link.
  This function returns immediately (i.e. does not block during disconnection
  process). Subsequent calls to Node_Service() perform the actual
  disconnection process.

  Subsequent calls to Link_GetConnectionStatus() may be used to determine
  if/when the disconnection process has completed. Returns VSC_RES_OK_ASYNC if
  the disconnection process was successfully initiated.

  If a disconnect callback has been registered for the Node, it will be called
  when the disconnection is completed.
  */
  VscResult             (__cdecl *Link_Disconnect)( VscLink vscLink );

  /**
  Returns the current connection status of the specified Link.
  */
  VscConnectionStatus   (__cdecl *Link_GetConnectionStatus)( const VscLink vscLink );

  /**
  Returns the Party that initiated this Link.
  */
  VscParty              (__cdecl *Link_GetInitiator)( const VscLink vscLink );

  /**
  Initiates a request to Receive data from the remote peer. If the Link
  is not currently connected, the request is stored and will be transmitted to
  the remote peer once a connection is established.

  If the call is successful, and the out_contractHandle parameter is non-null,
  it will be populated with a reference-counted handle to the newly created
  VscContract. The caller of this function is responsible for calling
  Contract_HandleRelease() on this handle when it is no longer in use. When
  all handle references to the Contract have been released, it will
  automatically be deleted/freed. The VscLink that this Contract belongs to
  will retain its own internal reference to the object, so it is not necessary
  for The Application to use the optional output parameter (i.e. you can pass
  a NULL pointer for the output parameter).

  To disable Overdue Data Detection, specify -1 for overdueDataTimeoutSec.
  */
  VscResult             (__cdecl *Link_CreateIncomingContract)(   VscLink                 vscLink
                                                                , VscUpdateDefinition     updateDef
                                                                , VscReceiveUpdateFunc_t  receiveUpdateCallback     ///< Function that will be called when new data arrives.
                                                                , VscTimeSyncMode         tsMode
                                                                , double                  overdueDataTimeoutSec
                                                                , VscContract*            out_contractHandle        ///< Optional parameter to receive a reference-counted handle to the new Contract.
                                                                );

  /**
  Initiates a request for the remote peer to accept data that we will send
  to it.

  If the call is successful, and the out_contractHandle parameter is non-null,
  it will be populated with a reference-counted handle to the newly created
  VscContract. The caller of this function is responsible for calling
  Contract_HandleRelease() on this handle when it is no longer in use. When
  all handle references to the Contract have been released, it will
  automatically be deleted/freed. The VscLink that this Contract belongs to
  will retain its own internal reference to the object, so it is not necessary
  for The Application to use the optional output parameter (i.e. you can pass
  a NULL pointer for the output parameter).

  To disable Overdue Data Detection, specify -1 for overdueDataTimeoutSec.
 */
  VscResult             (__cdecl *Link_CreateOutgoingContract)(   VscLink             vscLink
                                                                , VscUpdateDefinition updateDef
                                                                , VscSendUpdateFunc_t sendUpdateCallback
                                                                , VscTimeSyncMode     tsMode
                                                                , double              overdueDataTimeoutSec
                                                                , VscContract*        out_contractHandle
                                                                );

  /**
  Returns the Time Sync Contract for this Link, if it has one.
  Returns NULL if this Link has no Time Sync Contract.
  */
  VscContract           (__cdecl *Link_GetTsContract)( const VscLink link );

  /**
  Initiates a ping request. When the Peer responds to this ping request,
  the Node's VscPingResultsFunc_t callback function will be called (if
  this function was registered).
  */
  void                  (__cdecl *Link_PingInitiate)(   VscLink     vscLink
                                                      , const void* appData );

  /**
  Sets the period (seconds of system time) for periodic ping requests to be
  automatically sent to our Peer on this Link. To disable periodic automatic
  ping requests, set the period to GetInvalidSimtime().
  */
  void                  (__cdecl *Link_PingSetPeriodSec)(   VscLink     vscLink
                                                          , double      period
                                                          , const void* appData );

  /**
  Returns a value from the a latest ping results.

  For maximum performance, none of the parameters are checked, so you MUST
  ensure that they are all valid (i.e. the Link handle is valid and the indexes
  are within their respective valid ranges).

  The value returned may be "invalid" (false will be returned from
  IsValidSimtime()), indicating that the requested data is not available.

  Example:
    \code{.c}
      double initiatedTime      = vscApi->Link_PingGetStat( myLink, PM_REQUESTER_REQUEST_INITIATED, TMA_STEADY_TIME );
      double resultReceivedTime = vscApi->Link_PingGetStat( myLink, PM_REQUESTER_RESPONSE_RECEIVED, TMA_STEADY_TIME );

      if ( vscApi->IsValidSimtime(initiatedTime)  &&  vscApi->IsValidSimtime(resultReceivedTime) )
      {
        double totalRoundTripTime = resultReceivedTime - initiatedTime;
      }
    \endcode
  */
  double                (__cdecl *Link_PingGetStat)(  const VscLink   vscLink
                                                    , PingMilestone   pingMilestone
                                                    , TimeMomentAttr  momentAttribute );

  /**
  Returns the value that was specified by The Application in the call
  to Link_PingInitiate() or Link_PingSetPeriodSec().
  */
  const void*           (__cdecl *Link_PingGetAppData)( const VscLink vscLink );

  /**
  Returns the average Round Trip Time (RTT) for this Link, in seconds.
  The returned value may be affected by the frequency with which The
  Application calls Node_Service() for the specified peer's Host. I.e, the
  more frequently Node_Service() is called, the more accurate and consistent
  the returned data is likely to be.

  This is a low-level communication subsystem call and its exact meaning may
  be platform and/or system dependent. Use the data returned with caution...
  this is here only to provide an extra tiny bit of information for advanced
  users.
  */
  double                (__cdecl *Link_GetRttMean)( const VscLink link );

  /**
  Returns true if this Link contains any Contracts that may
  enter the "blocking" state.
  */
  bool                  (__cdecl *Link_CanBlock)( const VscLink link );

  /**
  Returns true if one of this Link's Incoming Contracts is in the
  "blocking" state.
  */
  bool                  (__cdecl *Link_IsBlocked)( const VscLink link );

  /**
  Creates a Schema object. A Schema is used to define the structure and
  contents of data exchanged between peers.
  The numFields parameter specifies the number of fields (variables) that the
  Schema is to contain, and must be > 0.

  After creating a Schema object with this function, you must use the
  Schema_SetField() function to set each of the Schema's fields.

  The caller of this function is responsible for calling the *_HandleRelease()
  function for the returned type to release the returned reference when it is no
  longer needed. When all handle references to the object have been released,
  the object will automatically be destroyed (deleted/freed).
  */
  VscSchema             (__cdecl *Schema_Create)( int numFields );

  /**
  Specifies a function of The Application to be called before the specified
  object is destroyed/deleted/freed.
  */
  void                  (__cdecl *Schema_SetPreDestroyCallback)(  VscSchema schema
                                                                , VscSchemaPreDestroyFunc_t func );

  /**
  Makes a new copy of an existing handle.
  The parameters are not checked for validity - it is the responsibility
  of The Application to ensure that:
    - The source handle is valid (i.e. not NULL).
    - The pointer to the destination handle is not NULL.
    - The destination pointer is not valid (i.e. it is NULL).

  Copying a handle in this way increments the reference count of the underlying
  object. The Application must call the corresponding *_HandleRelease()
  function on the destination handle when it is no longer in use to release
  this reference and decrement the reference count.
  */
  void                  (__cdecl *Schema_HandleCopy)(  VscSchema   source
                                                     , VscSchema*  out_destination );
  /**
  Releases a handle reference to an object.
  The parameters are not checked for validity - it is the responsibility
  of The Application to ensure that:
    - The pointer to the handle is not NULL.
    - The handle is valid (i.e. not NULL).

  When all handle references to an object have been released, the object will
  automatically be freed.
  */
  void                  (__cdecl *Schema_HandleRelease)( VscSchema* inout_handle );

  /**
  Associate app-defined data with the specified object.
  VS Connect does not do anything with this value, it is exclusively for the
  use of The Application, and The Application is responsible for any
  management/cleanup of this data.
  */
  void                  (__cdecl *Schema_SetAppData)(   VscSchema   schema
                                                      , void*       appData );

  /**
  Returns the value previously associated with this object using
  the corresponding *_SetAppData() function.
  */
  void*                 (__cdecl *Schema_GetAppData)( const VscSchema schema );

  /**
  Sets a Field in a Schema. The Schema will copy and retain a reference
  to the passed Field handle. This handle will only be released when the
  Schema is destroyed.

  The passed schema handle must be valid (i.e. one created with
  Schema_Create()), but the Schema must not be valid (i.e. Schema_IsValid()
  should return false). A valid Schema is immutable and cannot be modified by
  this function.

  Note: Once a all fields in a Schema have been set, you may not call this
  function to change any of the fields.  Doing so will result in a return value
  of VSC_RES_ERROR_UNSUPPORTED (indicating that changing a valid configured field
  is not a supported operation).
  */
  VscResult             (__cdecl *Schema_SetField)(   VscSchema       schema
                                                    , int             fieldIndex
                                                    , const VscField  field
                                                  );

  /**
  Returns true if the specified schema is completely configured and valid.
  To be valid each Field must be set with Schema_SetField().
  Once a Schema has been fully configured and is valid, it cannot be modified.
  */
  bool                  (__cdecl *Schema_IsValid)( const VscSchema schema );

  /**
  Defines the properties of a Field in a Schema.

  Note: Once a field has been successfully configured, you may not call this
  function to change it's properties. Doing so will result in a return value
  of VSC_RES_ERROR_UNSUPPORTED (indicating that changing a configured field
  is not a supported operation).

  This function is an alternative to using Field_Create() followed by
  Schema_SetField().

  Note: The only currently valid dataElementSizeInBits is 64.
  Note: numElements must be greater than 0.
  */
  VscResult             (__cdecl *Schema_InitField)(  VscSchema
                                                    , int
                                                    , VscDataType
                                                    , int
                                                    , int
                                                    , const char*
                                                    , const char*
                                                    , const char*
                                                    , VscInterp
                                                    , VscExtrap
                                                    , VscRounding 
                                                  );

  /// Returns the number of Fields that the Schema contains.
  int                   (__cdecl *Schema_GetNumFields)( const VscSchema schema);

  /**
  Parameters are not checked for validity, you must ensure that they are
  valid before calling this function.

  This function does NOT increase the reference count on the returned object. If
  you intend to cache the return value for later use, you should use
  *_HandleCopy() and *_HandleRelease() functions for the return object type.
  */
  VscField              (__cdecl *Schema_GetField)( const VscSchema schema
                                                  , int             fieldIndex );

  /**
  Creates a Field with the specified properties.

  Note: The only currently valid dataTypes are: VSC_DATATYPE_FLOAT and VSC_DATATYPE_INT.
  Note: The only currently valid dataElementSizeInBits is 64.
  Note: numElements must be greater than 0.

  The caller of this function is responsible for calling the *_HandleRelease()
  function for the returned type to release the returned reference when it is no
  longer needed. When all handle references to the object have been released,
  the object will automatically be destroyed (deleted/freed).
  */
  VscField              (__cdecl *Field_Create)(    VscDataType   dataType              ///< The type of data the Field will contain.
                                                  , int           dataElementSizeInBits ///< The size of the data the Field will contain.
                                                  , int           numElements           ///< Array size. Must be greater than zero.
                                                  , const char*   objectName            ///< Name of the VS Connect Object this field is associated with.
                                                  , const char*   propertyName          ///< Name of the VS Connect Object property this field's value represents.
                                                  , const char*   propertyParams        ///< Character string containing arbitrary parameters.
                                                  , VscInterp     interpMethod          ///< How this field's data should be interpolated between samples.
                                                  , VscExtrap     extrapMethod          ///< How this field's data should be extrapolated.
                                                  , VscRounding   roundingMethod        ///< How this field's data should be rounded.
                                                  , VscResult     *out_opt_result       ///< Optional output parameter with result code.
                                                );

  /**
  Makes a new copy of an existing handle.
  The parameters are not checked for validity - it is the responsibility
  of The Application to ensure that:
    - The source handle is valid (i.e. not NULL).
    - The pointer to the destination handle is not NULL.
    - The destination pointer is not valid (i.e. it is NULL).

  Copying a handle in this way increments the reference count of the underlying
  object. The Application must call the corresponding *_HandleRelease()
  function on the destination handle when it is no longer in use to release
  this reference and decrement the reference count.
  */
  void                  (__cdecl *Field_HandleCopy)(   VscField  source
                                                     , VscField* out_destination );

  /**
  Releases a handle reference to an object.
  The parameters are not checked for validity - it is the responsibility
  of The Application to ensure that:
    - The pointer to the handle is not NULL.
    - The handle is valid (i.e. not NULL).

  When all handle references to an object have been released, the object will
  automatically be freed.
  */
  void                  (__cdecl *Field_HandleRelease)( VscField* inout_handle );

  /**
  Specifies a function of The Application to be called before the specified
  object is destroyed/deleted/freed.
  */
  void                  (__cdecl *Field_SetPreDestroyCallback)(   VscField field
                                                                , VscFieldPreDestroyFunc_t func );

  /**
  Associate app-defined data with the specified object.
  VS Connect does not do anything with this value, it is exclusively for the
  use of The Application, and The Application is responsible for any
  management/cleanup of this data.
  */
  void                  (__cdecl *Field_SetAppData)(  const VscField  field
                                                    , void*           appData );

  /**
  Returns the value previously associated with this object using
  the corresponding *_SetAppData() function.
  */
  void*                 (__cdecl *Field_GetAppData)( const VscField field );


  /// Returns the name of the Object for the specified Field.
  /// Parameters are not checked for validity, you must ensure that they are valid before calling this function.
  const char*           (__cdecl *Field_GetObjectName)( const VscField field );

  /// Returns the name of the Property for the specified Field.
  /// Parameters are not checked for validity, you must ensure that they are valid before calling this function.
  const char*           (__cdecl *Field_GetPropertyName)( const VscField field );

  /// Returns the parameter string for the specified Field.
  /// Parameters are not checked for validity, you must ensure that they are valid before calling this function.
  const char*           (__cdecl *Field_GetParams)( const VscField field );

  /// Parameters are not checked for validity, you must ensure that they are valid before calling this function.
  VscDataType           (__cdecl *Field_GetDataType)( const VscField field );

  /// Parameters are not checked for validity, you must ensure that they are valid before calling this function.
  int32_t               (__cdecl *Field_GetElementSizeInBits)( const VscField field );

  /// Parameters are not checked for validity, you must ensure that they are valid before calling this function.
  int32_t               (__cdecl *Field_GetNumElements)( const VscField field );

  /// Parameters are not checked for validity, you must ensure that they are valid before calling this function.
  VscInterp             (__cdecl *Field_GetInterpMethod)( const VscField field );

  /// Parameters are not checked for validity, you must ensure that they are valid before calling this function.
  VscExtrap             (__cdecl *Field_GetExtrapMethod)( const VscField field );

  /// Parameters are not checked for validity, you must ensure that they are valid before calling this function.
  VscRounding           (__cdecl *Field_GetRoundingMethod)( const VscField field );

  /**
  Returns a handle to a VscRecord that can hold the data described
  by the Schema, or NULL if the specified schema is not complete and valid.

  If the call succeeds, the returned object retains a handle reference to the
  Schema that was passed in (i.e. it does NOT make its own unique Schema copy).

  The returned handle requires special handling to avoid heap corruption,
  memory leaks, and program crashes.

  You must call Record_HandleRelease() on this handle when it is
  no longer in use. When all handles to the underlying object are destroyed
  (i.e. there are no more references to the object), the resources (memory,
  etc.) associated with the object will be freed.
  */
  VscRecord             (__cdecl *Record_CreateFromSchema)(   const VscSchema schema
                                                            , VscResult*      out_result  /**< Result of call (detailed error code). */
                                                          );

  /**
  Specifies a function of The Application to be called before the specified
  object is destroyed/deleted/freed.
  */
  void                  (__cdecl *Record_SetPreDestroyCallback)(  VscRecord   record
                                                                , VscRecordPreDestroyFunc_t   func );

  /**
  Makes a new copy of an existing handle.
  The parameters are not checked for validity - it is the responsibility
  of The Application to ensure that:
    - The source handle is valid (i.e. not NULL).
    - The pointer to the destination handle is not NULL.
    - The destination pointer is not valid (i.e. it is NULL).

  Copying a handle in this way increments the reference count of the underlying
  object. The Application must call the corresponding *_HandleRelease()
  function on the destination handle when it is no longer in use to release
  this reference and decrement the reference count.
  */
  void                  (__cdecl *Record_HandleCopy)(   VscRecord   source
                                                      , VscRecord*  out_destination );

  /**
  Releases a handle reference to an object.
  The parameters are not checked for validity - it is the responsibility
  of The Application to ensure that:
    - The pointer to the handle is not NULL.
    - The handle is valid (i.e. not NULL).

  When all handle references to an object have been released, the object will
  automatically be freed.
  */
  void                  (__cdecl *Record_HandleRelease)( VscRecord* inout_handle );

  /**
  Associate app-defined data with the specified object.
  VS Connect does not do anything with this value, it is exclusively for the
  use of The Application, and The Application is responsible for any
  management/cleanup of this data.
  */
  void                  (__cdecl *Record_SetAppData)(   VscRecord   record
                                                      , void*       appData     );

  /**
  Returns the value previously associated with this object using
  the corresponding *_SetAppData() function.
  */
  void*                 (__cdecl *Record_GetAppData)( const VscRecord record );

  /**
  Returns the Schema associated with the specified Record.

  The VscRecord parameter must be valid (non-NULL).

  This function does NOT increase the reference count on the returned object. If
  you intend to cache the return value for later use, you should use
  *_HandleCopy() and *_HandleRelease() functions for the return object type.
  */
  VscSchema             (__cdecl *Record_GetSchema)( const VscRecord data );

  /**
  Copies the data from one buffer to the other. Both handles must be valid,
  and both must be created from the same Schema.

  Note that this is different than copying the handle itself using
  Record_HandleCopy().
  */
  VscResult             (__cdecl *Record_CopyData)(   const VscRecord   source
                                                    , VscRecord         dest );

  /// Returns the Serial Number of the data in the buffer.
  VscSerialNum_t        (__cdecl *Record_GetSerialNumber)( const VscRecord data );

  /// Returns the Simulation Time of the data in the buffer.
  double                (__cdecl *Record_GetSimtime)( const VscRecord data );

  /**
  \deprecated
  This function is in a preview/alpha state and may be change or removed without
  warning.

  It is recommended that it NOT be used in production code.
  */
  VscTime               (__cdecl *Record_GetSimtimeI)( const VscRecord data );

  /// Returns the Time Dilation of simulation at the time that the data was gathered.
  double                (__cdecl *Record_GetTimeDilation)( const VscRecord data );

  /**
  Retrieves a pointer to the const data of the given index.

  Use the associated Schema to determine the type/format of the data pointed to
  by the returned pointer and cast the returned pointer to that type.

  For example:

  double field_4_value = *((double*)vscApi->Record_GetWritableFieldValue(myDataBuffer, 4));

    or

  const double* field_4_ptr = (const double*)vscApi->Record_GetWritableFieldValue(myDataBuffer, 4);

  Note that the returned pointer may NOT be used to modify the data. Use the
  function Record_GetWritableFieldValue() to retrieve a pointer to data that may
  be modified.

  Note: The index specified must be valid for the VscRecord instance.
  */
  const void*           (__cdecl *Record_GetFieldValue)(  const VscRecord   data
                                                        , int               fieldIndex );

  /**
  Retrieves a pointer to the writable/non-const data of the given index.
  Use the associated Schema to determine the type/format of the data pointed to
  by the returned pointer and cast the returned pointer to that type.

  This function should be used when you intend to modify the data (e.g. in the
  Send Update Callback of The Application). If you are just retrieving the
  value of the field, use Record_GetFieldValue() instead.

  Examples:

  *((double*)sVscApi->Record_GetWritableFieldValue(myDataBuffer, 3)) = 1.0;

  double* field_4_ptr = (double*)vscApi->Record_GetWritableFieldValue(myDataBuffer, 4);
  *field_4_value = someNewValue;

  Note: The index specified must be valid for the VscRecord instance.
  */
  void*                 (__cdecl *Record_GetWritableFieldValue)(  VscRecord data
                                                                , int       fieldIndex );


  /**
  Sets the Simulation Time for the VscRecord. When this data is transmitted to
  a remote peer, the peer can use Record_GetSimtime() to read this value.
  */
  void                  (__cdecl *Record_SetSimtime)(   VscRecord data
                                                      , double    simtime );

  /**
  \deprecated
  This function is in a preview/alpha state and may be change or removed without
  warning.

  It is recommended that it NOT be used in production code.
  */
  void                  (__cdecl *Record_SetSimtimeI)(  VscRecord data
                                                      , VscTime   simtime );

  /**
  Creates a VscUpdateDefinition, which specifies details about what will be
  sent, and when.

  Returns a handle to the VscUpdateDefinition object which can be used to
  create a Contract.

  The caller of this function is responsible for calling the *_HandleRelease()
  function for the returned type to release the returned reference when it is no
  longer needed. When all handle references to the object have been released,
  the object will automatically be deleted/freed. VS Connect will retain its own
  internal reference to the object as necessary.
  Note that the scheduleValue parameter is a floating point type, so you may
  specify fractional values (e.g. 0.005 milliseconds == 5.0 microseconds).
  */
  VscUpdateDefinition   (__cdecl *UpdateDefinition_Create)(   const VscSchema   schema
                                                            , VscScheduleType   scheduleType
                                                            , double            scheduleValue
                                                          );

  /**
  Specifies a function of The Application to be called before the specified
  object is destroyed/deleted/freed.
  */
  void                  (__cdecl *UpdateDefinition_SetPreDestroyCallback)(    VscUpdateDefinition updateDef
                                                                            , VscUpdateDefinitionPreDestroyFunc_t func );

  /**
  Makes a new copy of an existing handle.
  The parameters are not checked for validity - it is the responsibility
  of The Application to ensure that:
    - The source handle is valid (i.e. not NULL).
    - The pointer to the destination handle is not NULL.
    - The destination pointer is not valid (i.e. it is NULL).

  Copying a handle in this way increments the reference count of the underlying
  object. The Application must call the corresponding *_HandleRelease()
  function on the destination handle when it is no longer in use to release
  this reference and decrement the reference count.
  */
  void                  (__cdecl *UpdateDefinition_HandleCopy)(   VscUpdateDefinition   source
                                                                , VscUpdateDefinition*  out_destination );

  /**
  Releases a handle reference to an object.
  The parameters are not checked for validity - it is the responsibility
  of The Application to ensure that:
    - The pointer to the handle is not NULL.
    - The handle is valid (i.e. not NULL).

  When all handle references to an object have been released, the object will
  automatically be freed.
  */
  void                  (__cdecl *UpdateDefinition_HandleRelease)( VscUpdateDefinition* inout_handle );

  /**
  Retrieves the Schema associated with the specified VscUpdateDefinition.

  This function does NOT increase the reference count on the returned object. If
  you intend to cache the return value for later use, you should use
  *_HandleCopy() and *_HandleRelease() functions for the return object type.
  */
  VscSchema             (__cdecl *UpdateDefinition_GetSchema)( const VscUpdateDefinition updateDef );

  VscScheduleType       (__cdecl *UpdateDefinition_GetScheduleType)( const VscUpdateDefinition updateDef );
  
  double                (__cdecl *UpdateDefinition_GetScheduleValue)( const VscUpdateDefinition updateDef );

  /**
  Specifies a function of The Application to be called before the specified
  object is destroyed/deleted/freed.
  */
  void                  (__cdecl *Contract_SetPreDestroyCallback)(  VscContract contract
                                                                  , VscContractPreDestroyFunc_t func );

  /**
  Makes a new copy of an existing handle.
  The parameters are not checked for validity - it is the responsibility
  of The Application to ensure that:
    - The source handle is valid (i.e. not NULL).
    - The pointer to the destination handle is not NULL.
    - The destination pointer is not valid (i.e. it is NULL).

  Copying a handle in this way increments the reference count of the underlying
  object. The Application must call the corresponding *_HandleRelease()
  function on the destination handle when it is no longer in use to release
  this reference and decrement the reference count.
  */
  void                  (__cdecl *Contract_HandleCopy)(   VscContract   source
                                                        , VscContract*  out_destination );

  /**
  Releases a handle reference to an object.
  The parameters are not checked for validity - it is the responsibility
  of The Application to ensure that:
    - The pointer to the handle is not NULL.
    - The handle is valid (i.e. not NULL).

  When all handle references to an object have been released, the object will
  automatically be freed.
  */
  void                  (__cdecl *Contract_HandleRelease)( VscContract* inout_handle );

  /**
  Associate app-defined data with the specified object.
  VS Connect does not do anything with this value, it is exclusively for the
  use of The Application, and The Application is responsible for any
  management/cleanup of this data.
  */
  void                  (__cdecl *Contract_SetAppData)(   VscContract contract
                                                        , void*       appData );

  /**
  Returns the value previously associated with this object using
  the corresponding *_SetAppData() function.
  */
  void*                 (__cdecl *Contract_GetAppData)( const VscContract contract );

  /// Returns the role that we play in this contract (i.e. sender or receiver).
  VscRole               (__cdecl *Contract_GetLocalRole)( const VscContract contract );

  /**
  Retrieves the Link that owns the specified Contract.

  This function does NOT increase the reference count on the returned object. If
  you intend to cache the return value for later use, you should use
  *_HandleCopy() and *_HandleRelease() functions for the return object type.
  */
  VscLink               (__cdecl *Contract_GetLink)( const VscContract contract );

  /**
  Retrieves the Schema associated with the specified Contract.

  This function does NOT increase the reference count on the returned object. If
  you intend to cache the return value for later use, you should use
  *_HandleCopy() and *_HandleRelease() functions for the return object type.
  */
  VscSchema             (__cdecl *Contract_GetSchema)( const VscContract contract );


  /**
  Retrieves the VscUpdateDefinition associated with the specified Contract.

  This function does NOT increase the reference count on the returned object. If
  you intend to cache the return value for later use, you should use
  *_HandleCopy() and *_HandleRelease() functions for the return object type.
  */
  VscUpdateDefinition   (__cdecl *Contract_GetUpdateDefinition)( const VscContract contract );

  /**
  Returns a value that can be added to the remote simulation time to
  compute the approximate corresponding local simulation time.
  This is value is only an estimate, and is continuously adjusted to reflect
  the best guess of the current/immediate time offset between the local and
  remote simulations.
  */
  double                (__cdecl *Contract_GetReceiveTimeOffset)( const VscContract contract );

  /**
  If the specified Contract is an Incoming Contract, this function returns the
  last (most recently received) Record. If not NULL, out_time will be set to
  the local simulation time that the VscRecord was received. If no valid data has
  been received, returns NULL, and out_receiveTime is set to an invalid value.

  If the specified Contract is an Outgoing Contract, this function returns the
  Record this Contract uses to store data that is sent to remote Peers. If a
  valid handle is returned, it identifies the VscRecord that will be passed
  back to The Application in subsequent calls to the registered "send update
  callback" (VscSendUpdateFunc_t) which The Application will use to supply data
  to VS Connect to be sent to remote Peers.

  This function does NOT increase the reference count on the returned object. If
  you intend to cache the return value for later use, you should use
  *_HandleCopy() and *_HandleRelease() functions for the return object type.
  */
  VscRecord             (__cdecl *Contract_GetRecord)( const VscContract contract, double* out_time );

  /**
  Returns the Time Sync Mode for the specified Contract.
  */
  VscTimeSyncMode       (__cdecl *Contract_GetTsMode)( const VscContract contract );

  /**
  Returns true if this Contracts may enter the "blocking" state.
  */
  bool                  (__cdecl *Contract_CanBlock)( const VscContract contract );

  /**
  Returns the time at which the specified Contract will enter the "blocked"
  state.
  Pass the current local simulation time for context; a value <= the passed
  simtime will be returned if the Contract is currently blocked.
  */
  double                (__cdecl *Contract_GetBlockTime)( const VscContract contract, double simtime );

  /**
  Returns true if the specified Contracts is in the "blocking" state.
  Pass the current local simulation time for context.
  */
  bool                  (__cdecl *Contract_IsBlocked)( const VscContract contract, double simtime );

  /**
  If immediateSend is true, data will be immediately gathered (via the
  SendUpdateCallback) and transmitted. If immediateSend is true the simtime
  parameter is used. If simtime is invalid, the time value specified in the
  previous call to Node_Service() is used.

  If immediateSend is false, the simtime parameter is ignored, and a subsequent
  call to Node_Service() will result in a call to the Send Callback (if the
  callback is registered).
  */
  VscResult             (__cdecl *Contract_Trigger)(    const VscContract contract
                                                      , bool              immediateSend
                                                      , double            simtime
                                                      , double            timeDilation  );
} vsc_Api_V6_t;

/// The API structure of the latest version.
typedef vsc_Api_V6_t      vsc_Api_t;

/// The latest/default/preferred version of the API at compile time.
#define VSC_DEFAULT_API_VERSION (6)


#if defined(__cplusplus)
extern "C"
{
#endif

/**
Retrieves a pointer to the API of the specified version, or NULL if the
specified version is not supported/available.

To get the latest/default version of the API (recommended), use:

  vsc_Api_t* apiPtr = (vsc_Api_t*)VsConnectApi_Get( VSC_DEFAULT_API_VERSION );

Note: This is the only function exported from this DLL. Use it to
get function pointers to the API functions.
*/
VSC_DECLSPEC const void VSC_DECLSPEC_2 * __cdecl
                        VsConnectApi_Get( int apiVersion );

#if defined(__cplusplus)
} // extern "C"
#endif


#endif  // vs_connect_api_h_INCLUDED
