/*
 * FFmpeg-on-10.6 feasibility smoke test.
 *
 * Goal: verify that libavcodec.60.dylib / libavutil.58.dylib from the
 * PowerFox.app bundle load and decode-symbol-resolve correctly when
 * run from a 10.6.8 process.
 *
 * Build:
 *   clang -target x86_64-apple-darwin10 \
 *       -isysroot .../MacOSX10.6.sdk \
 *       -O2 -Wall \
 *       -L./lib -Wl,-rpath,@executable_path \
 *       smoketest.c -lavcodec.60 -lavutil.58 -o smoketest
 *
 * Run:
 *   DYLD_LIBRARY_PATH=./lib ./smoketest
 *   (or place libavcodec.60.dylib + libavutil.58.dylib next to the binary)
 */

#include <stdio.h>

/*
 * Manual prototypes — no FFmpeg headers required.
 * This avoids pulling in libavcodec 62 headers (Homebrew) that might
 * disagree with the libavcodec 60 ABI in the PowerFox-bundled dylib.
 *
 * AV_CODEC_ID enum values are stable across FFmpeg 5.x/6.x for the
 * codecs we care about (H.264, VP8/VP9, AV1, MP3, AAC, Vorbis, Opus).
 * Verified against libavcodec/codec_id.h in ffmpeg release/6.x.
 */
#define AV_CODEC_ID_H264    27
#define AV_CODEC_ID_VP8     174
#define AV_CODEC_ID_VP9     167
#define AV_CODEC_ID_AV1     227
#define AV_CODEC_ID_MP3     0x15001
#define AV_CODEC_ID_AAC     0x15002
#define AV_CODEC_ID_VORBIS  165
#define AV_CODEC_ID_OPUS    86076

/* Treat the opaque AVCodec* as void* — we only test for NULL. */
typedef const void *avcodec_t;

extern unsigned      avcodec_version(void);
extern unsigned      avutil_version(void);
extern const char   *avcodec_configuration(void);
extern const char   *avutil_configuration(void);
extern avcodec_t     avcodec_find_decoder(int codec_id);
extern const char   *avcodec_get_name(int codec_id);

int main(void)
{
    unsigned avcv = avcodec_version();
    unsigned avuv = avutil_version();

    printf("== FFmpeg 10.6 smoketest ==\n");
    printf("avcodec_version   = %u (major %u, minor %u, micro %u)\n",
           avcv, avcv >> 16, (avcv >> 8) & 0xff, avcv & 0xff);
    printf("avutil_version    = %u (major %u, minor %u, micro %u)\n",
           avuv, avuv >> 16, (avuv >> 8) & 0xff, avuv & 0xff);
    printf("avcodec_configuration = %s\n", avcodec_configuration());
    printf("avutil_configuration  = %s\n\n", avutil_configuration());

    struct { int id; const char *name; } decoders[] = {
        { AV_CODEC_ID_H264,   "H.264"   },
        { AV_CODEC_ID_VP9,    "VP9"     },
        { AV_CODEC_ID_VP8,    "VP8"     },
        { AV_CODEC_ID_AAC,    "AAC"     },
        { AV_CODEC_ID_MP3,    "MP3"     },
        { AV_CODEC_ID_VORBIS, "Vorbis"  },
        { AV_CODEC_ID_OPUS,   "Opus"    },
        { AV_CODEC_ID_AV1,    "AV1"     },
    };

    int total = (int)(sizeof(decoders) / sizeof(decoders[0]));
    int ok = 0;
    for (int i = 0; i < total; i++) {
        avcodec_t dec = avcodec_find_decoder(decoders[i].id);
        const char *pretty = avcodec_get_name(decoders[i].id);
        printf("  %-8s (id=%-7d): %s  [%s]\n",
               decoders[i].name, decoders[i].id,
               dec ? "FOUND" : "MISSING",
               pretty ? pretty : "?");
        if (dec) ok++;
    }

    printf("\n%d/%d decoders resolve.\n", ok, total);
    return (ok == total) ? 0 : 1;
}
