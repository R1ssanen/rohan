#ifndef ROHAN_TYPE_H_
#define ROHAN_TYPE_H_

#if defined _WIN32 || defined __CYGWIN__
#ifdef ROHAN_BUILD
#define ROHAN_API __declspec(dllexport)
#else
#define ROHAN_API __declspec(dllimport)
#endif
#elif defined __GNUC__ || defined __clang__
#define ROHAN_API __attribute__((visibility("default")))
#else
#define ROHAN_API
#endif

#if defined __cplusplus
#define ROHAN_RESTRICT
#else
#define ROHAN_RESTRICT restrict
#endif

#endif