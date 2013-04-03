/*

  Copyright (c) 2012, Samsung R&D Institute Russia
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met: 

  1. Redistributions of source code must retain the above copyright notice, this
     list of conditions and the following disclaimer. 
  2. Redistributions in binary form must reproduce the above copyright notice,
     this list of conditions and the following disclaimer in the documentation
     and/or other materials provided with the distribution. 

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

  The views and conclusions contained in the software and documentation are those
  of the authors and should not be interpreted as representing official policies, 
  either expressed or implied, of the FreeBSD Project.
  
 */

/*! @file death_handler.h
 *  @brief Declaration of the SIGSEGV handler which prints the debug stack
 *  trace.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *  @license Simplified BSD License
 *  @copyright 2012 Samsung R&D Institute Russia
 */
 
/*! @mainpage SIGSEGV handler which prints the debug stack trace.
 *  Example
 *  =======
 *  ~~~~{.cc}
 *  #include "death_handler.h"
 *
 *  int main() {
 *    Debug::DeathHandler dh;
 *    int* p = NULL;
 *    *p = 0;
 *    return 0;
 *  }
 *  ~~~~
 *
 *  Underlying code style is very similar to [Google C++ Style Guide](http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml). It is checked with cpplint.py.
 */

#ifndef DEATH_HANDLER_H_
#define DEATH_HANDLER_H_

#include <stddef.h>

// Comment this out on systems without quick_exit()
#define QUICK_EXIT

namespace Debug {

/// @brief This class installs a SEGFAULT signal handler to print
/// a nice stack trace and (if requested) generate a core dump.
/// @details In DeathHandler's constructor, a SEGFAULT signal handler
/// is installed via sigaction(). If your program encounters a segmentation
/// fault, the call stack is unwinded with backtrace(), converted into
/// function names with line numbers via addr2line (fork() + execlp()).
/// Addresses from shared libraries are also converted thanks to dladdr().
/// All C++ symbols are demangled. Printed stack trace includes the faulty
/// thread id obtained with pthread_self() and each line contains the process
/// id to distinguish several stack traces printed by different processes at
/// the same time.
class DeathHandler {
 public:
  /// @brief Installs the SIGSEGV signal handler.
  DeathHandler();
  /// @brief This is called on normal program termination. Previously installed
  /// SIGSEGV signal handler is removed.
  ~DeathHandler();

  /// @brief Sets the value of cleanup property.
  /// @details If cleanup is set to true, program attempts to run all static
  /// destructors and atexit() callbacks before terminating. If
  /// generateCoreDump is set to true, this property is ignored.
  /// @note Default value of this property is true.
  bool cleanup();

  /// @brief Returns the value of cleanup property.
  /// @details If cleanup is set to true, program attempts to run all static
  /// destructors and atexit() callbacks before terminating. If
  /// generateCoreDump is set to true, this property is ignored.
  /// @note Default value of this property is true.
  void set_cleanup(bool value);

  /// @brief Returns the current value of generateCoreDump property.
  /// @details If generateCoreDump is set to true, a core dump will
  /// be generated when the program terminates. This behavior
  /// is implemented using abort(). cleanup property is ignored.
  /// @note You need to set ulimit to a value different from the default 0
  /// (for example, "ulimit -c unlimited") to enable core dumps generation
  /// on your system.
  /// @note Default value of this property is true.
  bool generateCoreDump();

  /// @brief Sets the value of generateCoreDump property.
  /// @details If generateCoreDump is set to true, a core dump will
  /// be generated when the program terminates. This behavior
  /// is implemented using abort(). cleanup property is ignored.
  /// @note You need to set ulimit to a value different from the default 0
  /// (for example, "ulimit -c unlimited") to enable core dumps generation
  /// on your system.
  /// @note Default value of this property is true.
  void set_generateCoreDump(bool value);

#ifdef QUICK_EXIT
  /// @brief Returns the value of quickExit property.
  /// @details If quick_exit is set to true, program will be terminated with
  /// quick_exit() call. generateCoreDump and cleanup properties are
  /// ignored.
  /// @note Default value is false.
  bool quickExit();

  /// @brief Sets the value of quickExit property.
  /// @details If quick_exit is set to true, program will be terminated with
  /// quick_exit() call. generateCoreDump and cleanup properties are
  /// ignored.
  /// @note Default value is false.
  void set_quickExit(bool value);

  /// @brief Returns the depth of the stack trace.
  /// @note Default value is 16.
  int framesCount();

  /// @brief Sets the depth of the stack trace. Accepted range is 1..100.
  /// @note Default value is 16.
  void set_framesCount(int value);
#endif

  /// @brief Returns the value indicating whether to shorten stack trace paths
  /// by cutting off the common root between each path and the current working
  /// directory.
  /// @note Default value is true.
  bool cutCommonPathRoot();

  /// @brief Sets the value indicating whether to shorten stack trace paths
  /// by cutting off the common root between each path and the current working
  /// directory.
  /// @note Default value is true.
  void set_cutCommonPathRoot(bool value);

  /// @brief Returns the value indicating whether to shorten stack trace paths
  /// by cutting off the relative part (e.g., "../../..").
  /// @note Default value is true.
  bool cutRelativePaths();

  /// @brief Sets the value indicating whether to shorten stack trace paths
  /// by cutting off the relative part (e.g., "../../..").
  /// @note Default value is true.
  void set_cutRelativePaths(bool value);

  /// @brief Returns the value indicating whether to append the process id
  /// to each stack trace line.
  /// @note Default value is false.
  bool appendPid();

  /// @brief Sets the value indicating whether to append the process id
  /// to each stack trace line.
  /// @note Default value is false.
  void set_appendPid(bool value);

  /// @brief Returns the value indicating whether to color the output
  /// with ANSI escape sequences.
  /// @note Default value is true.
  bool colorOutput();

  /// @brief Sets the value indicating whether to color the output
  /// with ANSI escape sequences.
  /// @note Default value is true.
  void set_colorOutput(bool value);

  /// @brief Returns the value indicating whether to do a thread-safe
  /// stack trace printing, stopping all running threads by forking.
  /// @note Default value is true.
  bool threadSafe();

  /// @brief Sets the value indicating whether to do a thread-safe stack trace
  /// printing, stopping all running threads by forking.
  /// @note Default value is true.
  void set_threadSafe(bool value);

 private:
  static void SignalHandler(int sig, void* info,
                            void* secret);

  static bool generateCoreDump_;
  static bool cleanup_;
#ifdef QUICK_EXIT
  static bool quickExit_;
#endif
  static int framesCount_;
  static bool cutCommonPathRoot_;
  static bool cutRelativePaths_;
  static bool appendPid_;
  static bool colorOutput_;
  static bool threadSafe_;
};

}  // namespace Debug
#endif  // DEATH_HANDLER_H_
