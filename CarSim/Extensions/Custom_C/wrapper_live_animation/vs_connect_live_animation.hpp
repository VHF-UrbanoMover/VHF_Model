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

#ifndef _vs_connect_live_amination_hpp_INCLUDED
#define _vs_connect_live_amination_hpp_INCLUDED


#include "vs_connect_api.h"
#include "vs_vehicle.h"

#include <string>
#include <memory>

/// The VS Live Animation namespace.
namespace vsla
{


extern const char   CL_FLAG;  ///< Prefix character for command line option flags.


/// Utility Functions
///@{
int   GetCommandLineOptionValuePosition( int argc, const char*const argv[], const char* option );
int   GetCommandLineBool( int argc, const char*const argv[], const char* option, bool* out_val );
int   GetCommandLineInt( int argc, char* argv[], const char* option, int* out_val );
int   GetCommandLineDouble( int argc, char* argv[], const char* option, double* out_val );
int   GetCommandLineString( int argc, char* argv[], const char* option, std::string* out_val );
///@}


/// Encapsulates and utilizes both VS Vehicle and VS Connect in order to
/// simultaneously run the VehicleSim solver and manage VS Connect, which can
/// provide live simulation data for VS Visualizer Live Animation, MATLAB and
/// Simulink (via the VS Connect s-function Simulink block), or any other
/// custom application that utilizes the VS Connect library.
class VsLiveAnimation
{
public:

  /** The prototype for a function that an application which uses this class
  can implement so that this class can generate log messages.
  Note that this prototype is compatible with printf().
  */
  typedef int (*LogCallback_t)( const char* format, ... );

  /**
  Specify a function which instances of this class can call to report errors,
  warnings, etc.
   
  This may be called before Init().

  For logging, an application using this class can:
    1) Implement its own custom function to receive log calls from this class.
    2) Explicitly specify an existing function, such as printf.
    3) Not call this function at all (printf() will be used by default).
    4) Specify NULL as the log function (log messages will be ignored/discarded).
  */
  static void           SetLogCallback( LogCallback_t logFunc );
  static LogCallback_t  GetLogCallback();

  /// Static initialization. If not called explicitly, will be called by Builder::Build() as needed.
  static void           Init();

  /// Static cleanup. Call this once before exiting your application.
  static void           Shutdown();

  /// Returns a pointer to the VS Connect API being used, or nullptr if the
  /// VS Connect API has not yet been loaded (by a call to Init()).
  /// Be sure to call Init() before calling this function.
  static const vsc_Api_t* 
                        GetVscApi()         { return msVscApi;  }

  /// Builder class that creates instances of VsLiveAnimation based on settings.
  class Builder
  {
  public:
    std::string   mSimfile;
    std::string   mListenHost;
    int           mListenPort;
    int           mMaxConnections;
    int           mMinConnectionsForSimulation; ///< Number of active connections required for the simulation to progress (sim pauses with less connections).

    Builder()
      : mSimfile                      ( "" )
      , mListenHost                   ( "" )
      , mListenPort                   ( 0 )
      , mMaxConnections               ( 1 )
      , mMinConnectionsForSimulation  ( 0 )
    {
    }

    /// Extracts options from command line.
    void                ProcessCommandLine( int argc, char* argv[] );

    /// Create a VsLiveAnimation instance with the current settings.
    /// Returns an empty pointer if the current settings not valid and/or
    /// a VsLiveAnimation instance cannot be created.
    std::shared_ptr<VsLiveAnimation>
                        Build();
  };

  int                   GetMaxConnections() const   { return mMaxConnections; }
  int                   GetNumConnections() const;

  void                  AllowConnections( bool allowConnections = true );
  void                  DisconnectAllLinks();

  double                GetSimTime() const;
  VscResult             UpdateTo( double simTime, double timeDilation = 1.0 );


  /// Returns the VS Vehicle handle.
  VSEW_VEHICLE_HANDLE   GetVsVehicle() const  { return mVehicle;  }

  /// Returns the VS Connect Node.
  VscNode               GetVscNode() const    { return mNode;     }

  /// Returns the time (in seconds) that *this is allowed to block in it's
  /// destructor while shutting down and disconnecting VS Connect Links.
  double                GetShutdownTimeout_sec() const        { return mShutdownTimeout_sec;  }
  void                  SetShutdownTimeout_sec( double val )  { mShutdownTimeout_sec = val;   }

private:
  static const vsc_Api_t*   msVscApi;             ///< VS Connect API pointer.

  VSEW_VEHICLE_HANDLE       mVehicle;             ///< VS Vehicle vehicle handle.
  VscNode                   mNode;                ///< VS Connect server Node for Live Animation.
  int                       mMaxConnections;      ///< Maximum number of connections we will accept;
  double                    mShutdownTimeout_sec; ///< Time (seconds) that *this is allowed to block in it's destructor while shutting down and disconnecting links.

  /// Private constructor. User Builder instance to construct an instance of this class.
  VsLiveAnimation()
    : mVehicle              ( NULL )
    , mNode                 ( NULL )
    , mMaxConnections       ( 3 )
    , mShutdownTimeout_sec  ( 1 )
  {
  }

public:
  ~VsLiveAnimation();

private:

};


};
#endif
