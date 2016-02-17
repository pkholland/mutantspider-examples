/*
 Copyright (c) 2016 Mutantspider authors, see AUTHORS file.
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
*/

#include "file_system.h"
#include "persistent_tests.h"
#include "resource_tests.h"

#include <sys/stat.h>

// defining this skips the fuse-based implementation of the
// persistent storage file system, allowing you to see what
// the test code does when running on a "normal" file system
//#define NO_FUSE

extern "C" {

void MS_Init(const char* init_args)
{
  mutantspider::init_fs();

  // the set of directories that we want to be "persistent"
  // that is, file io done in these directories (or subdirectories of them)
  // will persist across page loads in the browser.  It is recommended
  // that all such directories have a high-level directory name that is
  // reasonably unique to the web app itself.  So here we use
  // "file_system_example".  This will result in the directory
  // "/persistent/file_system_example" being our main persistent directory.
  std::vector<std::string> persistent_dirs;
  #if !defined(NO_FUSE)
  persistent_dirs.push_back("file_system_example");
  #endif
  mutantspider::mount_fs(persistent_dirs);
}

void MS_AsyncStartupComplete()
{
    #if defined(NO_FUSE)
    mkdir("/persistent", 0777);
    mkdir("/persistent/file_system_example", 0777);
    #endif

    // file system, and in particular the _async_ persistent part,
    // is now ready to use.  So start the testing code.
    int num_tests_run = 0;
    int num_tests_failed = 0;
    
    auto ret = persistent_tests();
    num_tests_run += ret.first;
    num_tests_failed += ret.second;
    
    ret = resource_tests();
    num_tests_run += ret.first;
    num_tests_failed += ret.second;
    
    fs_postLine("");
    fs_postHeading("File System Tests Completed: " + std::to_string(num_tests_run) + " tests run, " + std::to_string(num_tests_failed) + " tests failed");
}

}
