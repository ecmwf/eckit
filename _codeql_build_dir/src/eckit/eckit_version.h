#ifndef eckit_version_h
#define eckit_version_h

#define eckit_VERSION_STR "2.0.0"
#define eckit_VERSION     "2.0.0"

#define eckit_VERSION_MAJOR 2
#define eckit_VERSION_MINOR 0
#define eckit_VERSION_PATCH 0


#ifdef __cplusplus
extern "C" {
#endif

const char * eckit_version();

unsigned int eckit_version_int();

const char * eckit_version_str();

const char * eckit_git_sha1();

#ifdef __cplusplus
}
#endif

#endif // eckit_version_h
