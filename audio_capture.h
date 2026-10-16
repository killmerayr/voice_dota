#ifndef AUDIO_CAPTURE_H
#define AUDIO_CAPTURE_H

#include <portaudio.h>
#include <iostream>
#include "vosk_api.h"
#include <vector>
using namespace std;

class AudioCapture{
private:
    PaStream* stream;
    const int sampleRate = 16000;
    const int framesPerBuffer = 8000;

public:
    AudioCapture() : stream(nullptr) {}

    bool init() {
        PaError err = Pa_Initialize();
        if(err != paNoError) {
            cerr << "[Error] PortAudio init failed: " << Pa_GetErrorText(err) << endl;
            return false;
        }

        err = Pa_OpenDefaultStream(&stream,
                                    1,
                                    0,
                                    paInt16,
                                    sampleRate,
                                    framesPerBuffer,
                                    nullptr,
                                    nullptr);
        if (err != paNoError) {
            cerr << "[Error] Failed to open audio stream: " << Pa_GetErrorText(err) << endl;
            Pa_Terminate();
            return false;
        }

        err = Pa_StartStream(stream);
        if(err != paNoError){
            cerr << "[Error] Failed to start audio stream: " << Pa_GetErrorText(err) << endl;
            Pa_CloseStream(stream);
            Pa_Terminate();
            return false;
        }

        cout << "[Success] Audio capture started successfully!\n";
        return true;
    }

    bool read(char* buffer, int sizeInBytes) {
        if (!stream) return false;
        PaError err = Pa_ReadStream(stream, buffer, sizeInBytes / 2);
        if (err != paNoError) {
            return false; // Игнор переполнения буфера
        }
        return true;
    }

    ~AudioCapture() {
        if (stream) {
            Pa_StopStream(stream);
            Pa_CloseStream(stream);
        }
        Pa_Terminate();
    }

    string recognize(int bufferSize, VoskRecognizer* recognizer) {
        vector<char> buffer(bufferSize);
        if (!read(buffer.data(), bufferSize)) return "";
        vosk_recognizer_accept_waveform(recognizer, buffer.data(), bufferSize / 2);
        const char* result = vosk_recognizer_partial_result(recognizer);
        return string(result);
    }
};

#endif
