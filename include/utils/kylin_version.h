#ifndef _KYLIN_VERSION_H_
#define _KYLIN_VERSION_H_

/*
 * 主版本, 次版本，编译版本
 */
#define KYLIN_VERSION_MAJOR(version) (((unsigned char *)&(version))[2])
#define KYLIN_VERSION_MINOR(version) (((unsigned char *)&(version))[1])
#define KYLIN_VERSION_BUILD(version) (((unsigned char *)&(version))[0])

/*
 * 版本字符的长度，格式
 */
#define KYLIN_VERSION_STRING_LEN       16
#define KYLIN_VERSION_FORMAT           "%u.%u.%02u"
#define KYLIN_VERSION_QUAD(version)       \
        ((unsigned char *)&(version))[2], \
        ((unsigned char *)&(version))[1], \
        ((unsigned char *)&(version))[0]

/*
 * 三种版本类型，alpha->beta->realase.
 */
#define KYLIN_VERSION_TYPE_UNKNOWN      0x00
#define KYLIN_VERSION_TYPE_ALPHA        0x01
#define KYLIN_VERSION_TYPE_BETA         0x02
#define KYLIN_VERSION_TYPE_RELEASE      0x03

#define KYLIN_VERSION_TYPE_UNKNOWN_STRING      "UNKNOWN"
#define KYLIN_VERSION_TYPE_ALPHA_STRING        "ALPHA"
#define KYLIN_VERSION_TYPE_BETA_STRING         "BETA"
#define KYLIN_VERSION_TYPE_RELEASE_STRING      "RELEASE"

static inline const char * kylin_version_type(int type) {
    switch(type) {
        case KYLIN_VERSION_TYPE_RELEASE:
            return KYLIN_VERSION_TYPE_RELEASE_STRING;

        case KYLIN_VERSION_TYPE_BETA:
            return KYLIN_VERSION_TYPE_BETA_STRING;

        case KYLIN_VERSION_TYPE_ALPHA:
            return KYLIN_VERSION_TYPE_ALPHA_STRING;

        case KYLIN_VERSION_TYPE_UNKNOWN:
        default:
            return KYLIN_VERSION_TYPE_UNKNOWN_STRING;
    }
    return KYLIN_VERSION_TYPE_UNKNOWN_STRING;
}

/*
 * 版本生成，一般放在模块实现文件中
 */
#define KYLIN_VERSION_VALUE(major, minor, build) \
        (uint32_t)(((uint32_t)(major) << 16) + ((uint16_t)(minor) << 8) + ((uint16_t)(build)))

#define KYLIN_VERSION_GENERATE(name, major, minor, build, type)              \
static int  name##_version      = KYLIN_VERSION_VALUE((major), (minor), (build));  \
static int  name##_version_type = (type);                                     \
static char name##_description[128];                                                \
static uint8_t name##_version_major(void) {                                         \
    return KYLIN_VERSION_MAJOR(name##_version);                              \
}                                                                            \
                                                                             \
static uint8_t name##_version_minor(void) {                                         \
    return KYLIN_VERSION_MINOR(name##_version);                              \
}                                                                            \
                                                                             \
static uint8_t name##_version_build(void) {                                         \
    return KYLIN_VERSION_BUILD(name##_version);                              \
}                                                                            \
                                                                             \
static char *name##_version_description(void) {                                     \
    return name##_description;                                               \
}                                                                            \

#endif /*_KYLIN_VERSION_H_*/
