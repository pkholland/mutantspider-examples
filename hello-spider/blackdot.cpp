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

#include "blackdot.h"
#include "math.h"   // for sqrt

extern "C"  {

void MS_Init(const char* init_args)
{
  mutantspider::init_fs();
  
  // the set of directories that we want to be "persistent"
  // that is, file io done in these directories (or subdirectories of them)
  // will persist across page loads in the browser.
  std::vector<std::string> persistent_dirs;
  mutantspider::mount_fs(persistent_dirs);
}

void MS_AsyncStartupComplete()
{
}

// called from javascript to generate a "black dot" bitmap and "return" it
//
// The is callable from javascript because this file (blackdot.cpp) is listed
// in the "js_to_c_files" section of blackdot.json, and the "exported_c_functions"
// section lists "BD_*", so BD_GenBlackDot is seen as a match.  The mutantspider
// build is given the blackdot.json file and so generates the needed glue
// code to allow javascript to call this function.
void BD_GenBlackDot(int width, int height, int dot_x, int dot_y)
{
  // allocate the pixel storage for a bitmap that of dimensions <width,height>
  int alloc_size = width *4 * height;
  uint32_t* data = static_cast<uint32_t*>(malloc(alloc_size));
  
  // paint the red field with a black circle around the current mouse position
  size_t offset = 0;
  for (int y=0;y<height;y++)
  {
    for (int x=0;x<width;x++)
    {
      int xdist = x - dot_x;
      int ydist = y - dot_y;
      int dist = (int)(sqrt(xdist*xdist + ydist*ydist));
      if (dist > 255)
        dist = 255;

      // construct one pixel
      uint8_t	px[4];
      px[0] = dist;       // red
      px[1] = px[2] = 0;  // green and blue
      px[3] = 255;        // alpha

      // set it into our bitmap
      data[offset] = *(uint32_t*)&px[0];
      ++offset;
    }
  }
  
  // "return" the bitmap by passing it to this function
  bd_show_bitmap(width, height, data, alloc_size);
  free(data);
}

}

