/* Copyright (c) 2021, Mechanical Simulation Corporation
All rights reserved.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include "vs_connect_live_animation.hpp"

#include "vs_connect_api.h"
#include "vs_vehicle.h"

#include <assert.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>

#include <string>
#include <algorithm>
#include <cctype>
#include <iostream>
#include <chrono>
#include <thread>
#include <sstream>


# if defined(_WIN32) || defined (_WIN64)
  #include <Windows.h>
# endif


#ifdef _DEBUG
  #define BREAKPOINT_LINE   do { int bpl = 0; } while (false)
#else
  #define BREAKPOINT_LINE
#endif


using namespace std;
using namespace std::chrono;


namespace vsla
{


const char  CL_FLAG   = '-';


struct SolverVarInfo
{
  const double*const  mPtr;
  const double        mScale;

  SolverVarInfo(  const double* ptr, double scale = 1.0 )
    : mPtr    ( ptr   )
    , mScale  ( scale )
  {
  }
};


/// Default log callback handler, which does nothing.
static int    DiscardLogMessage( const char * format, ... )
{
  // Ignore log entry request.
  // Do nothing.
  return 0;
}


VsLiveAnimation::LogCallback_t  sLogFunc                    = printf;
const vsc_Api_t*                VsLiveAnimation::msVscApi   = NULL; ///< VS Connect API pointer.



/// Function for use by VS Connect to report log messages to this application.
static int    VscLogCallback(   enum  VscLogLevel   logLevel
                              , const VscNode       node
                              , const VscLink       link
                              , const VscContract   contract
                              , const char*         format
                              , va_list             argptr    )
{
  int   retCharsOutput = 0;

  stringstream ss;

  ss << "VS Connect ";

  if ( logLevel == VSC_LOG_ERROR )
  {
    ss << "ERROR";
  }
  else if ( logLevel == VSC_LOG_WARNING )
  {
    ss << "WARNING";
  }
  else
  {
    ss << "Log";
  }

  if ( node || link || contract )
  {
    ss << ",";
    if ( node )
    {
      ss << " Node-" << std::hex << (uint64_t)node;
    }

    if ( link )
    {
      ss << " Link-" << std::hex << (uint64_t)link;
    }

    if ( contract )
    {
      ss << " Contract-" << std::hex << (uint64_t)contract;
    }
  }

  ss << ": ";

  char      buff[4096];
  vsnprintf( buff, sizeof(buff)/sizeof(char), format, argptr );

  ss << buff << endl;

  return sLogFunc( ss.str().c_str() );
}


/**
 * Cleans up resources allocated by this program that are attached to or
 * associated with the Field that is about to be destroyed.
 */
void      FieldPreDestroyCallback( VscField field )
{
  /// Clean up stuff we (may have) attached to the appData of this Field:
  auto*   varInfo = (SolverVarInfo*)VsLiveAnimation::GetVscApi()->Field_GetAppData( field );
  VsLiveAnimation::GetVscApi()->Field_SetAppData( field, NULL );

  delete varInfo;
  varInfo = nullptr;
}


/// Callback called by VS Connect to inform The Application that a Link
/// has connected.
VscResult   LinkConnectCallback( VscLink connectingLink )
{
  VscResult   acceptLink = VSC_RES_OK;

  sLogFunc("LinkConnectCallback for link: %llx... ", (uint64_t)connectingLink);

  // ...

  if ( VSC_RES_OK == acceptLink )
  {
    sLogFunc("Accepting Link.\n");
  }
  else
  {
    sLogFunc("Rejecting Link.\n");
  }

  return VSC_RES_OK;
}


/// Callback called by VS Connect to inform The Application that a Link
/// has disconnected.
void        LinkDisconnectCallback(   VscLink disconnectedLink
                                    , VscResult disconnectReason )
{
  sLogFunc("Link disconnected: %llx\n", (uint64_t)disconnectedLink);
}


/** Callback for VS Connect to inform The Application that a Contract request
has been received.
*/
VscResult   ContractRequestCallback( const VscLink link, VscContract contract )
{
  VscResult   retRes = VSC_RES_UNDEFINED;

  if ( VSC_ROLE_SENDER != VsLiveAnimation::GetVscApi()->Contract_GetLocalRole(contract) )
  {
    // We only accept outgoing Contracts... we have no processing for incoming data.
    sLogFunc("Unexpected Contract type received. Rejecting.\n");
    retRes = VSC_RES_ERROR_UNSUPPORTED; // Returning an error rejects this Contract request.
  }
  else
  {
    sLogFunc("Outgoing Contract request received... ");

    const auto  node = VsLiveAnimation::GetVscApi()->Link_GetNode(link);

    auto  veh   = (VSEW_VEHICLE_HANDLE)VsLiveAnimation::GetVscApi()->Node_GetAppData(node);
    
    if ( !veh   ||   !vsew_IsOk(veh) )
    {
      sLogFunc("Contract cannot be processed because vehicle is not OK. ");
      retRes = VSC_RES_ERROR_UNAVAILABLE;
    }
    else
    {
      // Find each solver variable named in the Schema of this contract.
      auto        schema    = VsLiveAnimation::GetVscApi()->Contract_GetSchema( contract );
      const auto  numFields = VsLiveAnimation::GetVscApi()->Schema_GetNumFields( schema );

      retRes = VSC_RES_ERROR_INVALID_DATA;  // We'll return this error (rejecting the Contract) if we don't find any solver variables to send.

      for ( int i = 0; i < numFields; ++i )
      {
        auto field = VsLiveAnimation::GetVscApi()->Schema_GetField( schema, i );

        const auto  dataType    = VsLiveAnimation::GetVscApi()->Field_GetDataType( field );
        const auto  sizeInBits  = VsLiveAnimation::GetVscApi()->Field_GetElementSizeInBits( field );
        const auto  numElements = VsLiveAnimation::GetVscApi()->Field_GetNumElements( field );

        // We only support fields that are a single 64-bit floating point value. Ignore others:

        if (      VSC_DATATYPE_FLOAT == dataType
              &&  64 == sizeInBits
              &&  1 == numElements
          )
        {
          const auto objectName   = VsLiveAnimation::GetVscApi()->Field_GetObjectName( field );
          const auto propertyName = VsLiveAnimation::GetVscApi()->Field_GetPropertyName( field );

          // Note that for simple (one vehicle) live animation, we can ignore the "object name" (it should be blank, anyway).

          const auto  varPtr   = vsew_GetVarPtr( veh, propertyName );

          if ( varPtr )
          {
            const auto  scale = vsew_GetVarOutputGain( veh, propertyName );
            assert( scale != 0 );

            // Store the pointer to the variable and the output scale to use with it in an object that we can attach to the Field:
            auto  varInfo = new SolverVarInfo(varPtr, scale);

            // Save the variable pointer in the schema for use later in SendUpdateCallback():
            VsLiveAnimation::GetVscApi()->Field_SetAppData( field, (void*)(varInfo) );
            VsLiveAnimation::GetVscApi()->Field_SetPreDestroyCallback( field, FieldPreDestroyCallback );

            retRes = VSC_RES_OK;  // Returning "OK" accepts this Contract request.
          }
        }
      }
    }
  }

  if ( VSC_RES_OK == retRes )
  {
    sLogFunc("Accepting Contract.\n");
  }
  else
  {
    sLogFunc("Rejecting Contract.\n");
  }

  return retRes;  
}


void      ContractCanceledCallback( const VscLink link, VscContract contract )
{
  BREAKPOINT_LINE;  
}


// Fill the passed VscRecord with the latest data available.
VscResult  SendUpdateCallback(  const VscLink       link
                              , const VscContract   contract
                              , VscRecord           out_data )
{
  VscResult  retRes = VSC_RES_UNDEFINED;

  const auto  node = VsLiveAnimation::GetVscApi()->Link_GetNode(link);

  auto  veh   = (VSEW_VEHICLE_HANDLE)VsLiveAnimation::GetVscApi()->Node_GetAppData(node);

  if ( !veh   ||   !vsew_IsOk(veh) )
  {
    retRes = VSC_RES_ERROR_UNAVAILABLE;
  }
  else
  {

    // R&D&D:
    {
      const auto  vscServiceTime  = VsLiveAnimation::GetVscApi()->Record_GetSimtime(out_data);
      const auto  vsVehicleTime   = vsew_GetTime(veh);

      if ( vscServiceTime != vsVehicleTime )
      {
        BREAKPOINT_LINE;
      }
      else
      {
        BREAKPOINT_LINE;
      }
    }

    // We previously stored binding data in the Fields of this Contract's Schema, we'll dig it back out.
    const auto  schema      = VsLiveAnimation::GetVscApi()->Contract_GetSchema( contract );
    const auto  numFields   = VsLiveAnimation::GetVscApi()->Schema_GetNumFields(schema);

    for ( int i = 0; i < numFields; ++i )
    {
      const auto      fieldInfo = VsLiveAnimation::GetVscApi()->Schema_GetField( schema, i );
      double*         fieldData = (double*)VsLiveAnimation::GetVscApi()->Record_GetWritableFieldValue(out_data, i);

      const auto  varInfo = (SolverVarInfo*)VsLiveAnimation::GetVscApi()->Field_GetAppData( fieldInfo );


      if ( !varInfo )
      {
        // This field is not bound to a solver variable. We are obligated to invalidate the
        // value of this field in out_data:

        /// TODO Set all elements of this field to "invalid" values.
        
        BREAKPOINT_LINE;
      }
      else
      {
        // This field is bound.
        const double*   varPtr = varInfo->mPtr;
        assert( varPtr );

        // Based on our binding code when we accepted this contract, the
        // following must be true:
        assert( VSC_DATATYPE_FLOAT == VsLiveAnimation::GetVscApi()->Field_GetDataType( fieldInfo ) );
        assert( 64 == VsLiveAnimation::GetVscApi()->Field_GetElementSizeInBits( fieldInfo ) );
        assert( 1 == VsLiveAnimation::GetVscApi()->Field_GetNumElements( fieldInfo ) );

        *fieldData = *varPtr * varInfo->mScale;

        retRes = VSC_RES_OK;
      }
    }
  }

  return  retRes;
}
static bool   ContainsOnlyDigits( const std::string& s )
{
  return      !s.empty()
          &&  std::find_if( s.begin(), s.end(), [](const unsigned char c) { return !std::isdigit(c); }) == s.end();
}


/// Returns the index where the specified command line option resides, or a
/// negative value if the specified option does not appear in the command line.
/// If the option is specified more than once on the command line, the index
/// of the last occurrence is returned.
int   GetCommandLineOptionPosition( int argc, const char*const argv[], const char* option )
{
  int     retIndex  = -1;

  assert( option && strlen(option) > 0 );

  for ( int i = (argc - 1); i > 0 && retIndex < 0; --i )
  {
    if ( argv[i][0] == CL_FLAG )
    {
      if ( 0 == strcmp( option, argv[i] + 1 ) )
      {
        retIndex = i;
      }
    }
  }

  return retIndex;
}


/// Returns the index where the value of the specified option resides, or a
/// negative value if the option does not appear on the command line or the
/// specified option appears, but is not followed by any value.
/// Note that this function does not do ANY validity checking of the value,
/// it only returns the location where a valid value should be.
int   GetCommandLineOptionValuePosition( int argc, const char*const argv[], const char* option )
{
  const auto  flagIndex = GetCommandLineOptionPosition( argc, argv, option );

  return  (flagIndex > 0 && (flagIndex < argc - 1)) ? flagIndex + 1 : -1;
}


/// Sets the output parameter to the value of the specified command line option.
/// Returns 0 if the command line option value was successfully parsed and the
/// output parameter was set.
/// Returns a non-zero value if the option value could not be determined. If
/// non-zero is returned, the output parameter's value is not modified by this
/// function.
int    GetCommandLineBool( int argc, const char*const argv[], const char* option, bool* out_val )
{
  assert( out_val );

  int   retError = 0;   // Default to success.

  const auto  valueIndex = GetCommandLineOptionValuePosition( argc, argv, option );
  
  if ( valueIndex < 0 )
  {
    retError = -1;
  }
  else
  {
    assert( valueIndex > 1 );
    if ( 0 == strcmp( argv[valueIndex], "true") )
    {
      *out_val = true;
      retError = 0;
    }
    else if ( 0 == strcmp( argv[valueIndex], "false") )
    {
      *out_val = false;
      retError = 0;
    }
    else
    {
      retError = -2;
    }
  }

  return retError;
}


/// Sets the output parameter to the value of the specified command line option.
/// Returns 0 if the command line option value was successfully parsed and the
/// output parameter was set.
/// Returns a non-zero value if the option value could not be determined. If
/// non-zero is returned, the output parameter's value is not modified by this
/// function.
int    GetCommandLineInt( int argc, char* argv[], const char* option, int* out_val )
{
  assert( out_val );

  int   retError = 0;   // Default to success.

  const auto  valueIndex = GetCommandLineOptionValuePosition( argc, argv, option );

  if ( valueIndex < 0 )
  {
    retError = -1;
  }
  else
  {
    errno = 0;
    const auto  intVal  = atoi( argv[valueIndex] );
    if (      0 != errno
          ||  !ContainsOnlyDigits( argv[valueIndex] )
      )
    {
      retError = -2;
    }
    else
    {
      *out_val = intVal;
      retError = 0;
    }
  }

  return retError;
}


/// Sets the output parameter to the value of the specified command line option.
/// Returns 0 if the command line option value was successfully parsed and the
/// output parameter was set.
/// Returns a non-zero value if the option value could not be determined. If
/// non-zero is returned, the output parameter's value is not modified by this
/// function.
int    GetCommandLineDouble( int argc, char* argv[], const char* option, double* out_val )
{
  assert( out_val );

  int   retError = 0;   // Default to success.

  const auto  valueIndex = GetCommandLineOptionValuePosition( argc, argv, option );

  if ( valueIndex < 0 )
  {
    retError = -1;
  }
  else
  {
    errno = 0;
    const auto  val   = atof( argv[valueIndex] );
    if ( 0 != errno )
    {
      retError = -2;
    }
    else
    {
      *out_val = val;
      retError = 0;
    }
  }

  return retError;
}


/// Sets the output parameter to the value of the specified command line option.
/// Returns 0 if the command line option value was successfully parsed and the
/// output parameter was set.
/// Returns a non-zero value if the option value could not be determined. If
/// non-zero is returned, the output parameter's value is not modified by this
/// function.
int    GetCommandLineString( int argc, char* argv[], const char* option, string* out_val )
{
  assert( out_val );

  int   retError = 0;   // Default to success.

  const auto  valueIndex = GetCommandLineOptionValuePosition( argc, argv, option );

  if ( valueIndex < 0 )
  {
    retError = -1;
  }
  else
  {
    *out_val = argv[valueIndex];
    retError = 0;
  }

  return retError;
}


void  VsLiveAnimation::Builder::ProcessCommandLine( int argc, char* argv[] )
{
  int   retExitCode = 0;

  if ( argc > 1 && CL_FLAG != argv[1][0] )
  {
    // First 
    mSimfile = argv[1];

  }

  GetCommandLineString( argc, argv, "host", &mListenHost );
  GetCommandLineInt(    argc, argv, "port", &mListenPort );
  GetCommandLineInt(    argc, argv, "maxconn", &mMaxConnections );
  GetCommandLineString( argc, argv, "sim", &mSimfile );
}


void  VsLiveAnimation::SetLogCallback( LogCallback_t logFunc )
{
  if ( !logFunc )
  {
    sLogFunc = DiscardLogMessage;
  }
  else
  {
    sLogFunc = logFunc;
  }
}


VsLiveAnimation::LogCallback_t    VsLiveAnimation::GetLogCallback()
{
  LogCallback_t   retLogCallbackFunc = nullptr;

  if ( DiscardLogMessage != sLogFunc )
  {
    retLogCallbackFunc = sLogFunc;
  }

  return retLogCallbackFunc;
}


void  VsLiveAnimation::Init()
{
  if ( !msVscApi )
  {
    msVscApi = (vsc_Api_t*)VsConnectApi_Get( VSC_DEFAULT_API_VERSION );

    if ( !msVscApi )
    {
      sLogFunc( "ERROR - Unable to retrieve VS Connect API.\n" );
    }
    else
    {
      msVscApi->SetLogFunc( VscLogCallback );

      msVscApi->Init();

      if ( !msVscApi->IsInitialized() )
      {
        sLogFunc(  "VS Connect initialization failed. Note that VS Connect"
                    " requires an active VehicleSim Solver license to operate. Run"
                    " the VechicleSim Browser or License Manager to enable your"
                    " VehicleSim Solver license.\n"
                  );
      }
    }
  }
}


/// Static cleanup. Call this before exiting your application.
void  VsLiveAnimation::Shutdown()
{
  if ( msVscApi )
  {
    msVscApi->Shutdown();
  }
}


VsLiveAnimation::~VsLiveAnimation()
{
  msVscApi->Node_IgnoreConnectionRequests( mNode, true );

  // Cleanly disconnect any existing Links.
  msVscApi->Node_SetAppData( mNode, NULL );   // Disassociate our VS Vehicle from the VS Connect Node.

  vsew_DestroyVehicle( mVehicle );     mVehicle = NULL;

  {
    auto  numConnectedLinks = msVscApi->Node_GetNumLinks(mNode);

    for ( int i = 0; i < numConnectedLinks; ++i )
    {
      msVscApi->Link_Disconnect( msVscApi->Node_GetLink(mNode, i) );
    }

    // Now service the Node for a short bit to cleanly process
    // disconnections:
    bool  shutdownComplete  = false;
        
    /// Time when we will give up trying to disconnect cleanly.
    const auto    giveUpTime = steady_clock::now() + duration<double>( mShutdownTimeout_sec );

    while (     steady_clock::now() < giveUpTime
            &&  (numConnectedLinks = msVscApi->Node_GetNumLinks(mNode)) > 0 )
    {
      msVscApi->Node_Service(   mNode
                              , msVscApi->GetInvalidSimtime()
                              , msVscApi->GetInvalidSimtime()  );

      this_thread::sleep_for( milliseconds(1) );
    }

    if ( numConnectedLinks > 0 )
    {
      sLogFunc( "Failed to cleanly disconnect %d connections.\n", numConnectedLinks );
    }
  }

  msVscApi->Node_HandleRelease( &mNode );
}


std::shared_ptr<VsLiveAnimation> VsLiveAnimation::Builder::Build()
{
  std::shared_ptr<VsLiveAnimation>  retVsla;

  VsLiveAnimation::Init();

  if ( msVscApi )
  {
    static const int  ERROR_MSG_SIZE = 4096;
    char              errorMsg[ERROR_MSG_SIZE];

    auto  vehicle = vsew_CreateVehicle2(    mSimfile.c_str()
                                          , NULL
                                          , 0
                                          , -DBL_MAX
                                          , NULL
                                          , NULL
                                          , errorMsg
                                          , ERROR_MSG_SIZE
                                        );

    if ( !vehicle )
    {
      const bool    showSolverError = strlen(errorMsg) > 0;

      sLogFunc(    "Failed to create VS Vehicle instance.%s%s\n"
                  , showSolverError ? " Error reported: " : ""
                  , showSolverError ? errorMsg : ""
                );
    }
    else
    {
      VscResult   tempResult;
      VscNode node  = msVscApi->Node_Create_UDPIP(  mListenHost.c_str()       // listenAddress
                                                  , mListenPort               // listenPort
                                                  , mMaxConnections           // maxConnections
                                                  , VSC_TSM_NONE              // requiredTsMode
                                                  , LinkConnectCallback       // linkConnectCallback
                                                  , LinkDisconnectCallback    // linkdisconnectCallback
                                                  , ContractRequestCallback   // contractRequestCallback
                                                  , ContractCanceledCallback  // contractCanceledCallback
                                                  , SendUpdateCallback        // sendUpdateCallback
                                                  , NULL                      // receiveUpdateCallback
                                                  , NULL                      // pingResultsCallback
                                                  , &tempResult               // out_result
                                                );

      if ( !node )
      {
        sLogFunc(     "ERROR: Unable to create Server Node. This will happen if two"
                      " instances of this program are run at the same time - the"
                      " Server Node of the second instance cannot be created because"
                      " the first instance is already bound to the UDP port.\n"
                  );

        vsew_DestroyVehicle( vehicle );   vehicle = NULL;
      }
      else
      {
        msVscApi->Node_SetAppData( node, vehicle );  // Save our VS Vehicle handle in Node's appData so we can easily access it in VS Connect callbacks.

        retVsla.reset( new VsLiveAnimation() );

        if ( retVsla )
        {
          retVsla->mNode            = node;
          retVsla->mVehicle         = vehicle;
          retVsla->mMaxConnections  = mMaxConnections;
        }
      }
    }
  }

  return retVsla;
}


void  VsLiveAnimation::AllowConnections( bool allowConnections )
{
  if ( allowConnections )
  {
    if ( msVscApi->Node_ConnectionRequestsIgnored( mNode ) )
    {
      sLogFunc( "Now accepting VS Connect Link connection requests.\n" );

      msVscApi->Node_IgnoreConnectionRequests( mNode, false );
    }
  }
  else
  {
    if ( !msVscApi->Node_ConnectionRequestsIgnored( mNode ) )
    {
      sLogFunc( "Now rejecting new VS Connect Link connection requests.\n" );

      msVscApi->Node_IgnoreConnectionRequests( mNode, true );
    }
  }
}


int VsLiveAnimation::GetNumConnections() const
{
  int   retNumLinks = 0;

  if ( mNode )
  {
    retNumLinks = msVscApi->Node_GetNumLinks( mNode );
  }

  return retNumLinks;
}


void  VsLiveAnimation::DisconnectAllLinks()
{
  const auto    numLinks = msVscApi->Node_GetNumLinks( mNode );

  if ( numLinks > 0 )
  {
    sLogFunc( "Initiating disconnection of %d Links.\n", numLinks );

    for ( int i = 0; i < numLinks; ++i )
    {
      msVscApi->Link_Disconnect( msVscApi->Node_GetLink( mNode, i ) );
    }
  }
}


double  VsLiveAnimation::GetSimTime() const
{
  double  retSimTime = 0;
  if ( mVehicle )
  {
    retSimTime = vsew_GetTime( mVehicle );
  }

  return retSimTime;
}


VscResult VsLiveAnimation::UpdateTo( double simTime, double timeDilation )
{
  VscResult   retRes = VSC_RES_UNDEFINED;

  bool  terminate = false;

  if ( !mVehicle || !vsew_IsOk(mVehicle) )
  {
    retRes = VSC_RES_ERROR_UNAVAILABLE;
  }
  else if ( !vsew_IsOk( mVehicle ) )
  {
    // Run has previously stopped.
    retRes = VSC_RES_ERROR_TERMINATED;
  }
  else if ( msVscApi->IsValidSimtime(simTime) )
  {
    if ( simTime <= vsew_GetTime(mVehicle) )  // + vsew_GetTimeStep(mVehicle) )
    {
      // Time has not progressed far enough for us to integrate the vehicle solver.
      retRes  = VSC_RES_OK;
    }
    else
    {
      const auto  integrateRes = vsew_IntegrateTo( mVehicle, simTime );

      if ( 0 == integrateRes )
      {
        // Everything is OK.
        // Nothing to do.
        // Do nothing.
        assert( !terminate );

        retRes = VSC_RES_OK;
      }
      else if ( 1 == integrateRes )
      {
        // Solver has terminated "normally" (scheduled end of run, or event triggered terminate).
        terminate = true;
        retRes = VSC_RES_WARNING;
      }
      else
      {
        // Solver has encountered an error.

        std::stringstream   logMessage;

        logMessage << "ERROR: Solver integration error (error code: " << integrateRes << ").";

        const auto  solverErrorMessage = vsew_GetErrorMessage(mVehicle);

        if ( solverErrorMessage && strlen(solverErrorMessage) > 0 )
        {
          logMessage << " Error message from solver: " << solverErrorMessage;
        }

        logMessage << endl;

        sLogFunc( logMessage.str().c_str() );

        terminate = true;
        retRes = VSC_RES_ERROR;
      }
    }
  }

  if ( terminate )
  {
    AllowConnections( false );

    // // Set the time for our call to Node_Service() to the solver termination time:
    // simTime = vsew_GetTime(mVehicle);
  }

  msVscApi->Node_Service( mNode, simTime, timeDilation );
  
  if ( terminate )
  {
    DisconnectAllLinks();
  }

  assert( VSC_RES_UNDEFINED != retRes );  // Ensure we set return value.

  return retRes;
}


} // End namespace vsla.