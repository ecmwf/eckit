#include "eckit/eckit_version.h"

#ifdef __cplusplus
extern "C" {
#endif

const char * eckit_version()     { return "2.0.0"; }

const char * eckit_version_str() { return "2.0.0"; }

unsigned int eckit_version_int()
{
  return 10000*2 + 100*0 + 1*0;
}

const char * eckit_git_sha1() { return "565b89d1d03f392b4b7aba18c95cfe2eb25f503f"; }

#ifdef __cplusplus
}
#endif
