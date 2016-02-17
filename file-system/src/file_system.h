/*
 Copyright (c) 2014 Mutantspider authors, see AUTHORS file.
 
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

#pragma once

#include "mutantspider.h"
#include "errno.h"

extern "C" {

void fs_postMsg(const char* msg);

}

template<typename Func>
void fs_output(const char* cls, int line_num, Func func)
{
  std::ostringstream format;
  
  std::ostringstream body;
  func(body);
  std::string body_str = body.str();
  bool heading_or_empty = (strcmp(cls,"heading") == 0) || !body_str.size();
 
  format << "<span class=\"" << cls << "\">";
  if (!heading_or_empty)
    format << "[" << line_num << "]&nbsp;&nbsp;";
  format << body_str << "</span><br>";

  fs_postMsg(format.str().c_str());
}

#define fs_postLine(_body) fs_output("default", __LINE__, [&](std::ostream& formatter) {formatter << _body;})
#define fs_postError(_body) fs_output("error", __LINE__, [&](std::ostream& formatter) {formatter << _body;})
#define fs_postHeading(_body) fs_output("heading", __LINE__, [&](std::ostream& formatter) {formatter << _body;})


inline std::string errno_string()
{
    switch (errno)
    {
        case EPERM:
            return "EPERM";
        case ENOENT:
            return "ENOENT";
        case EBADF:
            return "EBADF";
        case EACCES:
            return "EACCES";
        case ENOTDIR:
            return "ENOTDIR";
        case EROFS:
            return "EROFS";
        default:
            return std::to_string(errno);
    }
}

inline std::string to_octal_string(int i)
{
    char buf[10];
    sprintf(buf, "%o", i);
    return std::string("0") + &buf[0];
}
