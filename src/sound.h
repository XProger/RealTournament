#ifndef H_SOUND
#define H_SOUND

#include "utils.h"

#define MAX_SAMPLES 24

namespace Sound {

    struct Frame {
        int16 L;
        int16 R;
    };

    struct SampleData {
        uint8 *data;
        int   size;
    };

    struct Sample {
        uint8 *data;
        int   size;
        int   pos;

        Sample(const char *name) : data(NULL), pos(0) {
            Stream stream(name);

            uint32 fourcc;
            stream.read(&fourcc, sizeof(fourcc));
            if (fourcc == FOURCC("RIFF")) { // wav

                struct {
                    uint16  format;
                    uint16  channels;
                    uint32  samplesPerSec;
                    uint32  bytesPerSec;
                    uint16  block;
                    uint16  sampleBits;
                } waveFmt;

                stream.seek(8);
                while (stream.pos < stream.size) {
                    uint32 type;
                    stream.read(&type, sizeof(type));
                    stream.read(&size, sizeof(size));
                    if (type == FOURCC("fmt ")) {
                        stream.read(&waveFmt, sizeof(waveFmt));
                        stream.seek(size - sizeof(waveFmt));
                    } else if (type == FOURCC("data")) {
                        data = new uint8[size];
                        stream.read(data, size);
                        break;
                    } else
                        stream.seek(size);
                }
            }
        }

        ~Sample() {
            delete[] data;
        }

        Frame getNextFrame() {
            Frame f;
            if (pos >= size) {
                f.L = f.R = 0;
            } else {
                f.L = f.R = *(int16*)(data + pos);
                pos += 2;
            }
            return f;
        }

    } *samples[MAX_SAMPLES];
    int samplesCount;

    void init() {
        samplesCount = 0;
    }

    void deinit() {
        for (int i = 0; i < samplesCount; i++)
            delete samples[i];
    }

    void fill(Frame *frame, int count) {
        for (int i = 0; i < count; i++) {
            int32 L = 0, R = 0;
            for (int j = 0; j < samplesCount; j++) {
                Frame f = samples[j]->getNextFrame();
                L += f.L;
                R += f.R;
            }
            /*
            if (samplesCount) {
                L /= samplesCount;
                R /= samplesCount;
            }
            */
            frame[i].L = clamp(L, -0xFFFF, +0xFFFF);
            frame[i].R = clamp(R, -0xFFFF, +0xFFFF);
        }

        int i = 0;
        while (i < samplesCount) {
            if (samples[i]->pos >= samples[i]->size) {
                delete samples[i];
                samples[i] = samples[--samplesCount];
            } else
                i++;
        }
    }

    void play(const char *name) {
        if (samplesCount >= MAX_SAMPLES) return;
        samples[samplesCount++] = new Sample(name);
    }
}

#endif