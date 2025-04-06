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

#include <assert.h>
#include <string.h>
#include <algorithm>
#include <iostream>
#include <chrono>
#include <thread>

#if defined(_WIN32) || defined (_WIN64)
  #include <Windows.h>
  #include <conio.h>
  #include <direct.h>
#endif


using namespace   std;
using namespace   std::chrono;
using namespace   vsla; // The VehicleSim Live Animation namespace.


static const char*    DEFAULT_SIMFILE = "simfile.sim";


/// Outputs a newline to the console before outputting the log message.
/// We need this so log entries are on their own line (not after "......").
/// If we didn't need the newline, we could delete this and use printf().
int   OutputLog( const char* format, ... )
{
  cout << '\n';

  va_list   argptr;
  va_start( argptr, format );
  return  vprintf( format, argptr );
}


static void   DisplayUsage()
{
  cout << "wrapper_live_animation [[" << CL_FLAG << "sim] <filename>]"                                          << '\n'
      <<  "                       [" << CL_FLAG << "host <name or I.P.>] [" << CL_FLAG << "port <integer>]"     << '\n'
      <<  "                       [" << CL_FLAG << "minconn <integer>] [" << CL_FLAG << "maxconn <integer>]"    << '\n'
      <<  "                       [" << CL_FLAG << "td <real number>] [" << CL_FLAG << "l <integer>]"           << '\n'
      <<  "  Example:" << '\n'
      <<  "    wrapper_live_animation \"simfile.sim\" " << CL_FLAG << "maxconn 3 " << CL_FLAG << "minconn 1 " << CL_FLAG << "td 2.0 " << CL_FLAG << "l 1"   << '\n'
      <<  '\n'
  //      "================================================================================"
      <<  "    sim       VehicleSim simfile to be loaded."                              << '\n'
      <<  "    host      Host name or IP address for server to listen on, or empty"     << '\n'
      <<  "              string \"\" for system default."                               << '\n'
      <<  "    port      Positive integer <= 65535 specifies the port for VS Connect"   << '\n'
      <<  "              server to listen on, or 0 to use VS Connect default."          << '\n'
      <<  "    minconn   Minimum number of connections (simulation pauses with fewer)." << '\n'
      <<  "    maxconn   Maximum number of connections that server will accept."        << '\n'
      <<  "    td        Time dilation factor (ex: 2 = run simulation 2x real time)."   << '\n'
      <<  "    l         Log level. If > 0, extra VS Connect log info will be output."  << '\n'
      <<  endl;
}


int main(int argc, char* argv[])
{
  int   returnError = 0;

  #if defined(_WIN32) || defined (_WIN64)
    // Disable Windows' command window "Quick Edit Mode" which causes the app
    // to freeze if the user clicks the mouse in the window.
    {
      DWORD lpMode;
      HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
      GetConsoleMode( hStdin, &lpMode );
      lpMode &= ~ENABLE_QUICK_EDIT_MODE;
      SetConsoleMode( hStdin, lpMode );
    }
  #endif

  cout << "VS Solver Live Animation Wrapper\n" << endl;

  if ( argc > 1 )
  {
    cout << "Command line parameters: ";
    for ( int i = 1; i < argc; ++i )
    {
      cout << argv[i] << ' ';
    }
    cout << endl;
  }
  else
  {
    cout << "No command line parameters specified." << endl;
  }

  {
    char    cwd[4096];
    const auto  res = _getcwd( cwd, sizeof(cwd) / sizeof(cwd[0]) );
    if ( nullptr != res )
    {
      cout << "Working directory: " << cwd << endl;
    }
    else
    {
      cout << "WARNING: Unable to determine current working directory." << endl;
    }
  }

  // First check if user requested command-line help:
  bool  displayUsageInsteadOfRunning = false;
  if ( argc > 1 )
  {
    static const char*  sParameterSwitchStrings[] = { "-", "/", "--" };
    static const int    sNumParameterSwitches     = sizeof(sParameterSwitchStrings) / sizeof(char*);
    static const char*  sHelpRequestStrings[]     = { "H", "?", "HELP" };
    static const int    sNumHelpRequestStrings    = sizeof(sHelpRequestStrings) / sizeof(char*);

    string   firstParamUpper( argv[1] );

    transform(firstParamUpper.begin(), firstParamUpper.end(), firstParamUpper.begin(), toupper);

    for ( int i = 0; i < sNumParameterSwitches && !displayUsageInsteadOfRunning; ++i )
    {
      for ( int j = 0; j < sNumHelpRequestStrings && !displayUsageInsteadOfRunning; ++j )
      {
        string   checkString( sParameterSwitchStrings[i] );
        checkString += sHelpRequestStrings[j];

        if ( 0 == checkString.compare(firstParamUpper) )
        {
          displayUsageInsteadOfRunning = true;
        }
      }
    }
  }

  if ( displayUsageInsteadOfRunning )
  {
    DisplayUsage();
  }
  else
  {
    int   logLevel = 0;
    GetCommandLineInt( argc, argv, "l", &logLevel );
    VsLiveAnimation::SetLogCallback( (logLevel > 0) ? OutputLog : nullptr );

    VsLiveAnimation::Builder    vslaBuilder;
    vslaBuilder.mMaxConnections = 3;  // Default, unless changed by command line parameters.
    vslaBuilder.mSimfile = "";
    vslaBuilder.ProcessCommandLine( argc, argv );

    if ( vslaBuilder.mSimfile.size() < 1 )
    {
      cout << "Warning: No simfile specified on command line. Using default \"" << DEFAULT_SIMFILE << "\"." << endl;
      vslaBuilder.mSimfile = DEFAULT_SIMFILE;
    }

    double  timeDilation                  = 1;
    int     minConnectionsForSimulation   = 1;

    GetCommandLineDouble( argc, argv, "td", &timeDilation );
    GetCommandLineInt(    argc, argv, "minconn", &minConnectionsForSimulation );

    VsLiveAnimation::Init();

    // No error processing command line. Continue running.
    returnError = 0;

    auto  vsla = vslaBuilder.Build();

    if ( !vsla )
    {
      cout << "Unable to create VS Live Animation instance." << endl;
      cout << "\nUsage:\n";
      DisplayUsage();
      returnError = 2;
    }
    else
    {
      static const double                   localUpdatePeriod_sec   = 0.0055555555555555555555; // 180 Hz
      static const steady_clock::duration   pulsePeriod( seconds(1) );        // How often to output a "pulse" to the console so user knows program is running.
      static const steady_clock::duration   simUiUpdatePeriod( seconds(30) ); // How often to output simulation status update to the console so user knows program is running.

      auto          vehicle                 = vsla->GetVsVehicle(); // Retrieve a VS Vehicle handle for later use.
      auto          node                    = vsla->GetVscNode();   // Retrieve a VS Vehicle handle for later use.

      const auto    startSteadyTime         = steady_clock::now();
      const auto    startSimTime            = vsew_GetTime( vehicle );

      double        localSimTime_sec        = startSimTime;
      double        prevSimTime_sec         = localSimTime_sec;

      int           prevConnections         = -1;

      double        invTimeDilation         = (0 == timeDilation) ? 0 : (1.0 / timeDilation);

      int           throttleBaseFrame       = -1; ///< The frame index when the sim and steady clocks were synced (-1 means before the first frame).
      double        throttleBaseSimTime_sec = localSimTime_sec;
      auto          throttleBaseSteadyTime  = steady_clock::now();

      int           frameIndex              = 0;

      auto          nextPulse               = startSteadyTime + pulsePeriod;
      auto          nextSimUiUpdate         = startSteadyTime + simUiUpdatePeriod;

      bool          resyncClocks            = false;

      bool          paused                  = false;

      bool          showUsage               = false;
      bool          showHelp                = true;
      bool          showConnectionInfo      = true;
      bool          showSimUpdate           = false;

      bool  done  = false;
      while (!done)
      {
        const auto  curNumConnections   = vsla->GetNumConnections();

        if ( _kbhit() )
        {
          auto  rawKey    = _getch();

          if ( 0 == rawKey || 224 == rawKey )
          {
            const auto  escapedKey = _getch();

            // Arrow keys: Up: 72 'H', Left: 75 'K', Right: 77 'M', Down: 80 'P'

            // Not processing any special keys (yet)... Nothing to do.
            // Do nothing.
          }
          else
          {
            auto  upperKey  = (char)toupper( rawKey );
 
            cout << upperKey << endl;

            if ( 'Q' == upperKey )
            {
              done = true;
              returnError = 0;
            }
            else if ( 'H' == upperKey )
            {
              showHelp  = true;
              showUsage = true;
              cout << '\n';
            }
            else if ( 'D' == upperKey )
            {
              cout << "Connections disallowed." << endl;
              // Stop accepting new connection requests:
              vsla->AllowConnections( false );

              // Disconnect any existing Links.
              vsla->DisconnectAllLinks();
            }
            else if ( 'C' == upperKey )
            {
              cout << "Connections allowed." << endl;
              // Allow connections.
              vsla->AllowConnections( true );
            }
            else if ( 'L' == upperKey )
            {
              if ( vsla->GetLogCallback() )
              {
                cout << "Disabling extra log output." << endl;
                vsla->SetLogCallback( nullptr );
              }
              else
              {
                cout << "Enabling extra log output." << endl;
                vsla->SetLogCallback( OutputLog );
              }
            }
            else if ( 'M' == upperKey )
            {
              ++minConnectionsForSimulation;
              minConnectionsForSimulation = min( minConnectionsForSimulation, vsla->GetMaxConnections() );
              showConnectionInfo = true;
            }
            else if ( 'F' == upperKey )
            {
              --minConnectionsForSimulation;
              minConnectionsForSimulation = max( minConnectionsForSimulation, 0 );
              showConnectionInfo = true;
            }
            else if ( 'I' == upperKey )
            {
              showConnectionInfo  = true;
              showSimUpdate       = true;
            }
            else if ( 'P' == upperKey )
            {
              paused = !paused;
              cout << (paused ? "Paused" : "Unpaused");
              cout << (!paused && (minConnectionsForSimulation < curNumConnections) ? " (but waiting for connections)" : " ");
              showSimUpdate = true;
            }
            else if (     '+' == upperKey
                      ||  '-' == upperKey
                      ||  '0' == upperKey
                      ||  '1' == upperKey )
            {
              static const auto   maxDilation   = pow(2, 6);
              static const auto   minDilation   = 1 / maxDilation;

              const auto    prevDilation = timeDilation;

              if ( '0' == upperKey )
              {
                timeDilation = 0;
              }
              else if ( '1' == upperKey )
              {
                timeDilation = 1;
              }
              else if (     '+' == upperKey
                        ||  '-' == upperKey )
              {                   
                if ( '+' == upperKey )
                {
                  timeDilation *= 2;
                }
                else
                {
                  if ( 0 == timeDilation )
                  {
                    timeDilation = maxDilation;
                  }
                  else
                  {
                    timeDilation /= 2;
                  }
                }

                if ( 0.75 < timeDilation && timeDilation < 1.5 )
                {
                  timeDilation = 1.0;
                }

                if ( timeDilation > maxDilation )
                {
                  timeDilation = 0;   // Means "no limit", i.e. no time throttling.
                }
                else if ( 0 < timeDilation   &&   timeDilation < minDilation )
                {
                  timeDilation = minDilation;
                }
              }

              if ( prevDilation != timeDilation )
              {
                invTimeDilation = (0 == timeDilation) ? 0 : (1 / timeDilation);

                resyncClocks  = true;
                showSimUpdate = true;
              }
            }
            else if ( 'W' == upperKey )
            {
              // Clear ("Wipe") the console screen.
              system("cls");

              showHelp            = true;
              showConnectionInfo  = true;
              showSimUpdate       = true;
            }
            else
            {
              // User pressed an unrecognized key.
              cout << '\n' << upperKey << "?\n";
              showHelp = true;
            }
          }
        }

        const auto  connWaitCount       = max( 0, minConnectionsForSimulation - curNumConnections );
        const bool  awaitingConnections = connWaitCount > 0;
        const bool  advanceThisFrame    = !paused && !awaitingConnections;

        if ( showUsage )
        {
          DisplayUsage();
          showUsage = false;
        }

        if ( showHelp )
        {
          cout <<   "Keyboard Commands:"
                    "\n  H  Help                                 C  Accept connections"
                    "\n  L  Enable/disable extra log messages    D  Disconnect/disallow connections"
                    "\n  W  Wipe screen                          M  Require more connections"
                    "\n  P  Pause/unpause simulation             F  Require fewer connections"
                    "\n  I  Show connection info                 +  Increase time dilation"
                    "\n  Q  Quit                                 -  Decrease time dilation"
            << endl;

            showHelp = false;
        }

        showConnectionInfo  = showConnectionInfo || (curNumConnections != prevConnections);

        if ( showConnectionInfo )
        {
          showConnectionInfo = false;

          cout  << "\nConnections: Min: " << minConnectionsForSimulation 
                << " Max: " << vsla->GetMaxConnections()
                << " Current: " << curNumConnections;

          if ( connWaitCount > 0 )
          {
            cout << "\nWaiting for " << connWaitCount << " more connections.";
          }

          prevConnections = curNumConnections;
        }

        const bool  prevFrameAdvanced   = localSimTime_sec > prevSimTime_sec;
        prevSimTime_sec = localSimTime_sec;

        if ( advanceThisFrame )
        {
          localSimTime_sec += localUpdatePeriod_sec;

          if ( !prevFrameAdvanced )
          {
            resyncClocks    = true;
            showSimUpdate   = true;
          }
        }

        if ( resyncClocks )
        {
          resyncClocks            = false;

          // Sync our clocks:
          throttleBaseFrame       = frameIndex;
          throttleBaseSteadyTime  = steady_clock::now();
          throttleBaseSimTime_sec = localSimTime_sec;
        }

        const auto  nowBefore       = steady_clock::now();

        /// This services network connections and runs VS solver integration.
        const auto  updateRes = vsla->UpdateTo( localSimTime_sec, timeDilation );

        showSimUpdate = showSimUpdate || (nextSimUiUpdate <= nowBefore);

        if ( showSimUpdate )
        {
          showSimUpdate = false;

          cout << "\nSim: " << (int)vsla->GetSimTime() << " sec.";

          if ( timeDilation != 1 )
          {
            cout << " Time dilation: ";
            if ( 0 < timeDilation   &&   timeDilation < 1 )
            {
              cout << "1/" << (1 / timeDilation);
            }
            else
            {
              cout << timeDilation;
            }
          }

          if ( paused || awaitingConnections )
          {
            cout  << " ("
                  << (paused ? "paused" : "")
                  << ((paused && awaitingConnections) ? ", " : "")
                  << (awaitingConnections ? "waiting for connections" : "")
                  << ")";
          }

          cout << ' ';

          nextSimUiUpdate = nowBefore + simUiUpdatePeriod;
          nextPulse       = nowBefore + pulsePeriod;
        }
        else if ( nextPulse <= nowBefore )
        {
          cout << '.';
          nextPulse = nowBefore + pulsePeriod;
        }

        if ( vsc_IsWarning(updateRes) )
        {
          // Run has terminated normally/cleanly.
          done = true;
          returnError = 0;
        }
        else if ( vsc_IsError(updateRes) )
        {
          // Run has stopped.
          done = true;
          returnError = 3;
        }
        else if ( advanceThisFrame )
        {
          if ( invTimeDilation > 0 )
          {
            // Time throttling is enabled.

            const auto  elapsedSimTime_sec            = localSimTime_sec - throttleBaseSimTime_sec;
            const auto  desiredElapsedSteadyTime_sec  = duration<double>( elapsedSimTime_sec * invTimeDilation );
            const auto  desiredElapsedSteadyTime      = duration_cast<steady_clock::duration>( desiredElapsedSteadyTime_sec );

            assert( desiredElapsedSteadyTime.count() >= 0 );

            steady_clock::duration    timeAhead;  ///< How far ahead of "real time" we are.
            do 
            {
              const auto now  = steady_clock::now();

              const auto  elapsedSteadyTime   = now - throttleBaseSteadyTime;

              timeAhead   = desiredElapsedSteadyTime - elapsedSteadyTime;

              static const steady_clock::duration  minTimeAheadToSleep( microseconds(500) );

              if ( timeAhead >= minTimeAheadToSleep )
              {
                // Sleep for half the time we are ahead (sleep is not that accurate).
                const auto  timeToSleep = timeAhead / 2;
                this_thread::sleep_for( timeToSleep );
              }
              else if ( timeAhead.count() > 0 )
              {
                // We're getting close... don't sleep, just yield.
                this_thread::yield();
              }
              else if ( timeAhead.count() < 0 )
              {
                // Make sure we're not too far behind "real time".
                const auto  stepDuration  = duration_cast<decltype(timeAhead)>( duration<double>( localUpdatePeriod_sec * invTimeDilation ) );
                const auto  timeBehind    = -timeAhead;

                if ( timeBehind > stepDuration * 2 )
                {
                  // We're falling behind "real time". Reset our time references to avoid excessive rubber-banding.
                  throttleBaseFrame       = frameIndex;
                  throttleBaseSteadyTime  = now;
                  throttleBaseSimTime_sec = localSimTime_sec;
                }
              }
            } while ( timeAhead.count() > 0 );
          }

          ++frameIndex;
        }
        else
        {
          // We did not advance the simulation this frame.
          // Sleep for a bit to wait for new connections.
          this_thread::sleep_for( milliseconds(10) );
        }
      }

      const auto    endSteadyTime = steady_clock::now();
      const auto    endSimTime    = vsew_GetTime( vehicle );

      // Prevent new connections from being accepted/acknowledged.
      vsla->AllowConnections( false );

      const auto    elapsedSteadyTime_sec   = duration<double>(endSteadyTime - startSteadyTime).count();
      const auto    elapsedSimTime_sec      = endSimTime - startSimTime;
      const auto    rTime                   = elapsedSteadyTime_sec == 0 ? 0 : elapsedSteadyTime_sec / elapsedSimTime_sec;

      cout << '\n';
      cout << "Run stopped at t = " << endSimTime << '\n';
      cout << "Elapsed wall time: " << elapsedSteadyTime_sec << " elapsed simulation time: "<< elapsedSimTime_sec << '\n';
      cout << "Computational time ratio: RTIME = " << rTime << " (real time)/(simulation time)" << '\n';
      cout << endl;
    }
  }

  VsLiveAnimation::Shutdown();

  if ( 0 != returnError )
  {
    cout << "\nError occurred during execution.";

    // Comment this out to prevent console window waiting when there is any error:
    {
      cout << "\nPress any key to exit.";
      _getch();
    }
    
    cout << endl;
  }

  return returnError;
}
