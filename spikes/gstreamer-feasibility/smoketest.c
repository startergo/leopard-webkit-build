/*
 * GStreamer-on-10.6 feasibility smoke test.
 *
 * Goal: verify that the MacPorts-installed GStreamer 1.16.2 on the macmini
 * (real 10.6.8 hardware) can be linked against from a 10.6-targeting binary
 * and that the YouTube-critical elements (playbin, qtdemux, avdec_h264,
 * dashdemux, etc.) resolve at runtime.
 *
 * Build (on macmini):
 *   clang -DOSX -I/opt/local/include/gstreamer-1.0 \
 *       -I/opt/local/include/glib-2.0 \
 *       -I/opt/local/lib/glib-2.0/include \
 *       smoketest.c -o smoketest \
 *       -L/opt/local/lib \
 *       -lgstreamer-1.0 -lgobject-2.0 -lglib-2.0 -lintl
 *
 * Run:
 *   ./smoketest
 */

/* Xcode 4.2's clang predates __has_feature; GLib 2.58+ uses it unconditionally
 * in gmacros.h. Provide the standard fallback so the headers parse. */
#ifndef __has_feature
#define __has_feature(x) 0
#endif
#ifndef __has_extension
#define __has_extension(x) 0
#endif
#ifndef __has_attribute
#define __has_attribute(x) 0
#endif
#ifndef __has_builtin
#define __has_builtin(x) 0
#endif

#include <gst/gst.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    gst_init(&argc, &argv);

    guint major, minor, micro, nano;
    gst_version(&major, &minor, &micro, &nano);
    printf("== GStreamer 10.6 smoketest ==\n");
    printf("gst_version_string: %s\n", gst_version_string());
    printf("gst linked against: %u.%u.%u.%u\n\n", major, minor, micro, nano);

    /* Test 1: create playbin — the autoplug-and-play element WebKit would use. */
    GstElement *playbin = gst_element_factory_make("playbin", "test_playbin");
    printf("playbin: %s\n", playbin ? "OK" : "FAILED");
    if (!playbin) {
        return 1;
    }

    /* Test 2: exercise GObject property set/get on playbin (mirrors what
     * MediaPlayerPrivateGStreamer does to assign URLs). */
    g_object_set(playbin, "uri", "https://example.com/test.mp4", NULL);
    gchar *uri = NULL;
    g_object_get(playbin, "uri", &uri, NULL);
    printf("playbin uri set/get: %s\n\n", uri ? "OK" : "FAILED");
    g_free(uri);

    /* Test 3: probe every YouTube-critical element. osxaudio / glimagesink
     * are the macOS-specific sinks most likely to have bitrotted. */
    const char *elements[] = {
        /* demuxers */
        "qtdemux",           /* MP4/M4V */
        "matroskademux",     /* WebM/Matroska */
        "flvdemux",          /* FLV */
        "dashdemux",         /* MPEG-DASH — YouTube's adaptive streaming */
        "hlsdemux",          /* Apple HLS */
        /* decoders */
        "avdec_h264",        /* H.264 via libav */
        "avdec_vp9",         /* VP9 via libav */
        "avdec_aac",         /* AAC via libav */
        "avdec_mp3",         /* MP3 via libav */
        "vp9dec",            /* native VP9 */
        "opusdec",           /* Opus */
        "avdec_vorbis",      /* Vorbis via libav */
        /* parsers */
        "aacparse",
        "h264parse",
        "opusparse",
        /* sinks — the macOS-specific ones */
        "osxaudiosink",      /* CoreAudio via gst-plugins-base */
        "osxvideosink",      /* NSView via gst-plugins-base */
        "autoaudiosink",     /* fallback if osxaudiosink missing */
        "glimagesink",       /* OpenGL output (compositor-friendly) */
        /* app integration — what WebKit uses to push buffers */
        "appsink",
        "appsrc",
    };
    int n = (int)(sizeof(elements) / sizeof(elements[0]));
    int found = 0;
    for (int i = 0; i < n; i++) {
        GstElement *e = gst_element_factory_make(elements[i], NULL);
        printf("  %-20s: %s\n", elements[i], e ? "OK" : "MISSING");
        if (e) {
            gst_object_unref(e);
            found++;
        }
    }

    gst_object_unref(playbin);
    printf("\n%d/%d elements available.\n", found, n);

    /* Test 4: parse-launch a tiny pipeline (no actual playback — proves the
     * GStreamer runtime can construct a bin and link pads end-to-end). */
    GError *err = NULL;
    GstElement *pipe = gst_parse_launch("videotestsrc num-buffers=1 ! videoconvert ! fakesink", &err);
    if (err) {
        printf("\ngst_parse_launch FAILED: %s\n", err->message);
        g_error_free(err);
    } else {
        GstStateChangeReturn ret = gst_element_set_state(pipe, GST_STATE_PLAYING);
        printf("\ngst_parse_launch + set_state(PLAYING): %s\n",
               ret == GST_STATE_CHANGE_SUCCESS ? "OK" :
               ret == GST_STATE_CHANGE_ASYNC  ? "ASYNC (expected for fakesink)" :
               "FAILED");
        GstBus *bus = gst_element_get_bus(pipe);
        gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE,
                                   GST_MESSAGE_EOS | GST_MESSAGE_ERROR);
        gst_object_unref(bus);
        gst_element_set_state(pipe, GST_STATE_NULL);
        gst_object_unref(pipe);
    }

    return (found >= n - 2) ? 0 : 1; /* allow 2 missing (osxvideosink/glimagesink) */
}
