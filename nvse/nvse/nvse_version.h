#ifndef __NVSE_VERSION_H__
#define __NVSE_VERSION_H__

// these have to be macros so they can be used in the .rc
#define NVSE_VERSION_INTEGER		5
#define NVSE_VERSION_INTEGER_MINOR	2
#define NVSE_VERSION_INTEGER_BETA	5
#define NVSE_VERSION_VERSTRING		"0, 5, 2, 5"
#define NVSE_VERSION_PADDEDSTRING	"0005"

// build numbers do not appear to follow the same format as with oblivion
#define MAKE_NEW_VEGAS_VERSION_EX(major, minor, build, sub)	(((major & 0xFF) << 24) | ((minor & 0xFF) << 16) | ((build & 0xFFF) << 4) | (sub & 0xF))
#define MAKE_NEW_VEGAS_VERSION(major, minor, build)			MAKE_NEW_VEGAS_VERSION_EX(major, minor, build, 0)

// assume the major version number is 1.x
#define RUNTIME_VERSION_1_0_0_240	MAKE_NEW_VEGAS_VERSION(0, 0, 240)		// 0x00000F00
#define RUNTIME_VERSION_1_1_0_240	MAKE_NEW_VEGAS_VERSION(1, 0, 268)		// 0x010010C0
#define RUNTIME_VERSION_1_1_1_271	MAKE_NEW_VEGAS_VERSION(1, 1, 271)		// 0x010110F0
#define RUNTIME_VERSION_1_1_1_280	MAKE_NEW_VEGAS_VERSION(1, 1, 280)		// 0x01011180
#define RUNTIME_VERSION_1_2_0_285	MAKE_NEW_VEGAS_VERSION(2, 0, 285)		// 0x020011D0
#define RUNTIME_VERSION_1_2_0_314	MAKE_NEW_VEGAS_VERSION(2, 0, 314)		// 0x020013A0
#define RUNTIME_VERSION_1_2_0_352	MAKE_NEW_VEGAS_VERSION(2, 0, 352)		// 0x02001600
#define RUNTIME_VERSION_1_3_0_452	MAKE_NEW_VEGAS_VERSION(3, 0, 452)		// 0x03001C40
#define RUNTIME_VERSION_1_4_0_525	MAKE_NEW_VEGAS_VERSION(4, 0, 525)		// 0x040020D0
#define RUNTIME_VERSION_1_4_0_525ng	MAKE_NEW_VEGAS_VERSION_EX(4, 0, 525, 1)	// 0x040020D1

#define CS_VERSION_1_1_0_262		MAKE_NEW_VEGAS_VERSION(1, 0, 262)		// 0x01001060
#define CS_VERSION_1_3_0_452		MAKE_NEW_VEGAS_VERSION(3, 0, 452)		// 0x03001C40
#define CS_VERSION_1_4_0_518		MAKE_NEW_VEGAS_VERSION(4, 0, 518)		// 0x04002060

#define PACKED_NVSE_VERSION		MAKE_NEW_VEGAS_VERSION(NVSE_VERSION_INTEGER, NVSE_VERSION_INTEGER_MINOR, NVSE_VERSION_INTEGER_BETA)

#endif /* __NVSE_VERSION_H__ */
