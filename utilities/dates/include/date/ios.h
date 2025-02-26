//
//  ios.h
//  datetimelib
//
// the mit license (mit)
//
// copyright (c) 2016 alexander kormanovsky
//
// permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "software"), to deal
// in the software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the software, and to permit persons to whom the software is
// furnished to do so, subject to the following conditions:
//
// the above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the software.
//
// the software is provided "as is", without warranty of any kind, express or
// implied, including but not limited to the warranties of merchantability,
// fitness for a particular purpose and noninfringement. in no event shall the
// authors or copyright holders be liable for any claim, damages or other
// liability, whether in an action of contract, tort or otherwise, arising from,
// out of or in connection with the software or the use or other dealings in the
// software.

#ifndef ios_hpp
#define ios_hpp

#if __APPLE__
# include <TargetConditionals.h>
# if TARGET_OS_IPHONE
#   include <string>

    namespace date
    {
    namespace iOSUtils
    {

    std::string get_tzdata_path();
    std::string get_current_timezone();

    }  // namespace iosutils
    }  // namespace date

# endif  // target_os_iphone
#else   // !__apple__
# define TARGET_OS_IPHONE 0
#endif  // !__apple__
#endif // ios_hpp
