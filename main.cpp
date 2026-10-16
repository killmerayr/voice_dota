#include <iostream>
#include "audio_capture.h"
#include "vosk_api.h"
using namespace std;

int main(){
    cout << "/// Arc Warden voice control ///\n";
    cout << "To exit use 'exit'\n";
    cout << "Input command (midas, flux) |for Clone must use 'clone' before command| \n";

    AudioCapture audio;
    if (!audio.init()) {
        cerr << "[Error] Failed to initialize audio capture\n";
        return 1;
    }

    VoskModel* model = vosk_model_new("model");
    if(!model){
        cerr << "[Error] Failed to create Vosk model\n";
        return 1;
    }
    
    VoskRecognizer* recognizer = vosk_recognizer_new(model, 16000.0f);
    if(!recognizer){
        cerr << "[Error] Failed to create Vosk recognizer\n";
        vosk_model_free(model);
        return 1;
    }

    string input;

    while (true){
        cout << "> ";

        if(!getline(cin, input)){
            break;
        }

        if (input == "exit"){
            cout << "Exit...\n";
            break;
        }

        // Дополнительный код для тестирования
        char buffer[1024];
        if (audio.read(buffer, sizeof(buffer))) {
            string recognition = audio.recognize(sizeof(buffer), recognizer);
            cout << "[Recognition] " << recognition << endl;
        } else {
            cerr << "[Error] Failed to read audio data\n";
        }
    }

    // Очистка памяти
    vosk_recognizer_free(recognizer);
    vosk_model_free(model);
    audio.~AudioCapture();

    return 0;
}