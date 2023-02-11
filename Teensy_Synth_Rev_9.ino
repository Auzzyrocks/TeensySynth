// Teensy Synth 
// 2 Waveforms, with volume and Fq control
// By Austin Shouli

/*  Version Notes 
 *  
 *  Master Volume
 *  Master Tune
 *  Osc 1 Vol
 *  Osc 1 Freq
 *  Osc 1 - waveform select button 
 *  Osc 2 Vol
 *  Osc 2 Freq
 *  Osc 2 - waveform select button 
 *  
 */


#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <Bounce.h>

// GUItool: begin automatically generated code
AudioSynthWaveform       lfo2;      //xy=174,373
AudioSynthWaveform       lfo1;      //xy=176.75,270.5
AudioSynthWaveformModulated waveformMod1;   //xy=418.75001525878906,273.75000762939453
AudioSynthWaveformModulated waveformMod2;   //xy=435,362.5
AudioMixer4              mixer1;         //xy=645.75,330
AudioSynthWaveform       lfo3;      //xy=674,442
AudioFilterStateVariable filter1;        //xy=832,332
AudioAmplifier           amp1;           //xy=1010,325
AudioOutputI2S           i2s1;           //xy=1151.75,323
AudioConnection          patchCord1(lfo2, 0, waveformMod2, 0);
AudioConnection          patchCord2(lfo1, 0, waveformMod1, 0);
AudioConnection          patchCord3(waveformMod1, 0, mixer1, 0);
AudioConnection          patchCord4(waveformMod2, 0, mixer1, 1);
AudioConnection          patchCord5(mixer1, 0, filter1, 0);
AudioConnection          patchCord6(lfo3, 0, filter1, 1);
AudioConnection          patchCord7(filter1, 0, amp1, 0);
AudioConnection          patchCord8(amp1, 0, i2s1, 0);
AudioConnection          patchCord9(amp1, 0, i2s1, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=1091.7500114440918,208.25000762939453
// GUItool: end automatically generated code


//Button Bounce Setup 

Bounce button1 = Bounce(8, 15);
int current_waveform1 = WAVEFORM_SAWTOOTH;

Bounce button2 = Bounce(5, 15);
int current_waveform2 = WAVEFORM_SAWTOOTH;

Bounce button3 = Bounce(3, 15);
int current_lfo1 = WAVEFORM_SAWTOOTH;

Bounce button5 = Bounce(0, 15);
int current_lfo2 = WAVEFORM_ARBITRARY;

Bounce button6 = Bounce(2, 15);
int current_lfo3 = WAVEFORM_TRIANGLE_VARIABLE;

Bounce button4 = Bounce(1, 15);
int onOff = 1;

void setup() {
  Serial.begin(9600);
  AudioMemory(20);
  
  // Comment these out if not using the audio adaptor board.
  // This may wait forever if the SDA & SCL pins lack
  // pullup resistors
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.5); // caution: very loud - use oscilloscope only!

  
  waveformMod1.begin(WAVEFORM_SAWTOOTH);
  waveformMod1.amplitude(0.75);
  waveformMod1.frequency(500);
  //waveformMod1.pulseWidth(0.15);
  pinMode(8, INPUT_PULLUP);
  
  waveformMod2.begin(WAVEFORM_SAWTOOTH);
  waveformMod2.amplitude(0.75);
  waveformMod2.frequency(500);
  //waveformMod2.pulseWidth(0.15);
  pinMode(5, INPUT_PULLUP);

  lfo1.begin(WAVEFORM_SAWTOOTH);
  lfo1.amplitude(0.75);
  lfo1.frequency(500);
  lfo1.pulseWidth(0.15);
  pinMode(3, INPUT_PULLUP);

  lfo2.begin(WAVEFORM_SAWTOOTH);
  lfo2.amplitude(0.75);
  lfo2.frequency(500);
  lfo2.pulseWidth(0.15);
  pinMode(0, INPUT_PULLUP);

  lfo3.begin(WAVEFORM_TRIANGLE_VARIABLE);
  lfo3.amplitude(0);
  lfo3.frequency(500);
  lfo3.pulseWidth(0.15);
  pinMode(2, INPUT_PULLUP);

  filter1.frequency(15000);
  filter1.resonance(0.7);
  filter1.octaveControl(2.2);

  pinMode(1, INPUT_PULLUP); //Master Switch
  
}

void loop() {

  //Master Controls
  float master_vol = analogRead(A16) / 1023.0 - 1.0;
  float master_tune = analogRead(A13) / 1023.0 * 27.0; //Use similar method to master/sub volume for indipendant/master tuning? Set master_tune = analogRead(tuningKnob); may need to set +/- for tuning variation, somehow. 

//On Off Switch
  button4.update();

  if (button4.fallingEdge()) {
  
      switch (onOff) {
        case 1:
          onOff = 0;
          Serial.println("onOff: Off");
          break;
        case 0:
          onOff = 1.0;
          Serial.println("onOff: On");
          break;
      }
  }
  
  amp1.gain(onOff);
  
  //waveform1 volume control
  float w1_vol = analogRead(A2) / 1023.0; //Use this to control vol for each waveform, and have a master control, when using multiple waveforms
  waveformMod1.amplitude(master_vol + w1_vol );   

  //waveform1 frequency control
  float w1_fq = 100.0 + analogRead(A3) / 1023.0 * 900.0; 
  waveformMod1.frequency(master_tune + w1_fq); 

  //waveform1 waveform select
  button1.update();

   if (button1.fallingEdge()) {
    switch (current_waveform1) {
      case WAVEFORM_SINE:
        current_waveform1 = WAVEFORM_SAWTOOTH;
        Serial.println("OSC 1 : Sawtooth");
        break;
      case WAVEFORM_SAWTOOTH:
        current_waveform1 = WAVEFORM_SAWTOOTH_REVERSE;
        Serial.println("OSC 1 : Reverse Sawtooth");
        break;
      case WAVEFORM_SAWTOOTH_REVERSE:
        current_waveform1 = WAVEFORM_SQUARE;
        Serial.println("OSC 1 : Square");
        break;
      case WAVEFORM_SQUARE:
        current_waveform1 = WAVEFORM_TRIANGLE;
        Serial.println("OSC 1 : Triangle");
        break;
      case WAVEFORM_TRIANGLE:
        current_waveform1 = WAVEFORM_TRIANGLE_VARIABLE;
        Serial.println("OSC 1 : Variable Triangle");
        break;
      case WAVEFORM_TRIANGLE_VARIABLE:
        current_waveform1 = WAVEFORM_ARBITRARY;
        Serial.println("OSC 1 : Arbitary Waveform");
        break;
      case WAVEFORM_ARBITRARY:
        current_waveform1 = WAVEFORM_PULSE;
        Serial.println("OSC 1 : Pulse");
        break;
      case WAVEFORM_PULSE:
        current_waveform1 = WAVEFORM_SAMPLE_HOLD;
        Serial.println("OSC 1 : Sample & Hold");
        break;
      case WAVEFORM_SAMPLE_HOLD:
        current_waveform1 = WAVEFORM_SINE;
        Serial.println("OSC 1 : Sine");
        break;
      }
    }
   waveformMod1.begin(current_waveform1);

  //waveform2 volume control
  float w2_vol = analogRead(A6) / 1023.0; //Use this to control vol for each waveform, and have a master control, when using multiple waveforms
  waveformMod2.amplitude(master_vol + w2_vol);

  //waveform2 frequency control
  float w2_fq = 100.0 + analogRead(A7) / 1023.0 * 900.0; 
  waveformMod2.frequency(master_tune + w2_fq); 

  //waveform2 waveform select
  button2.update();

   if (button2.fallingEdge()) {
    switch (current_waveform2) {
      case WAVEFORM_SINE:
        current_waveform2 = WAVEFORM_SAWTOOTH;
        Serial.println("OSC 2 : Sawtooth");
        break;
      case WAVEFORM_SAWTOOTH:
        current_waveform2 = WAVEFORM_SAWTOOTH_REVERSE;
        Serial.println("OSC 2 : Reverse Sawtooth");
        break;
      case WAVEFORM_SAWTOOTH_REVERSE:
        current_waveform2 = WAVEFORM_SQUARE;
        Serial.println("OSC 2 : Square");
        break;
      case WAVEFORM_SQUARE:
        current_waveform2 = WAVEFORM_TRIANGLE;
        Serial.println("OSC 2 : Triangle");
        break;
      case WAVEFORM_TRIANGLE:
        current_waveform2 = WAVEFORM_TRIANGLE_VARIABLE;
        Serial.println("OSC 2 : Variable Triangle");
        break;
      case WAVEFORM_TRIANGLE_VARIABLE:
        current_waveform2 = WAVEFORM_ARBITRARY;
        Serial.println("OSC 2 : Arbitary Waveform");
        break;
      case WAVEFORM_ARBITRARY:
        current_waveform2 = WAVEFORM_PULSE;
        Serial.println("OSC 2 : Pulse");
        break;
      case WAVEFORM_PULSE:
        current_waveform2 = WAVEFORM_SAMPLE_HOLD;
        Serial.println("OSC 2 : Sample & Hold");
        break;
      case WAVEFORM_SAMPLE_HOLD:
        current_waveform2 = WAVEFORM_SINE;
        Serial.println("OSC 2 : Sine");
        break;
      }
   }
  waveformMod2.begin(current_waveform2);


//LFO 1 Controls
  //waveform1 volume control
  float lfo1_vol = analogRead(A10) / 1023.0 / 2; //Use this to control vol for each waveform, and have a master control, when using multiple waveforms
  lfo1.amplitude(lfo1_vol );   

  //waveform1 frequency control
  float lfo1_fq = analogRead(A12) / 1023.0 * 900.0 / 27; 
  lfo1.frequency(lfo1_fq); 

  //lfo1 waveform select - Change to Assignment button?
  button3.update();

   if (button3.fallingEdge()) {
    switch (current_lfo1) {
      case WAVEFORM_SINE:
        current_lfo1 = WAVEFORM_SAWTOOTH;
        Serial.println("LFO 1 : Sawtooth");
        break;
      case WAVEFORM_SAWTOOTH:
        current_lfo1 = WAVEFORM_SAWTOOTH_REVERSE;
        Serial.println("LFO 1 : Reverse Sawtooth");
        break;
      case WAVEFORM_SAWTOOTH_REVERSE:
        current_lfo1 = WAVEFORM_SQUARE;
        Serial.println("LFO 1 : Square");
        break;
      case WAVEFORM_SQUARE:
        current_lfo1 = WAVEFORM_TRIANGLE;
        Serial.println("LFO 1 : Triangle");
        break;
      case WAVEFORM_TRIANGLE:
        current_lfo1 = WAVEFORM_TRIANGLE_VARIABLE;
        Serial.println("LFO 1 : Variable Triangle");
        break;
      case WAVEFORM_TRIANGLE_VARIABLE:
        current_lfo1 = WAVEFORM_ARBITRARY;
        Serial.println("LFO 1 : Arbitary Waveform");
        break;
      case WAVEFORM_ARBITRARY:
        current_lfo1 = WAVEFORM_PULSE;
        Serial.println("LFO 1 : Pulse");
        break;
      case WAVEFORM_PULSE:
        current_lfo1 = WAVEFORM_SAMPLE_HOLD;
        Serial.println("LFO 1 : Sample & Hold");
        break;
      case WAVEFORM_SAMPLE_HOLD:
        current_lfo1 = WAVEFORM_SINE;
        Serial.println("LFO 1 : Sine");
        break;
      }
    }
   lfo1.begin(current_lfo1);

//LFO 2 Controls
  //lfo2 volume control
  float lfo2_vol = analogRead(A11) / 1023.0 / 2; //Use this to control vol for each waveform, and have a master control, when using multiple waveforms
  lfo2.amplitude(lfo2_vol );   

  //lfo2 frequency control
  float lfo2_fq = analogRead(A17) / 1023.0 * 900.0 / 70; 
  lfo2.frequency(lfo2_fq); 

  //lfo1 waveform select - Change to Assignment button?
  button5.update();

   if (button5.fallingEdge()) {
    switch (current_lfo2) {
      case WAVEFORM_SINE:
        current_lfo2 = WAVEFORM_SAWTOOTH;
        Serial.println("LFO 2 : Sawtooth");
        break;
      case WAVEFORM_SAWTOOTH:
        current_lfo2 = WAVEFORM_SAWTOOTH_REVERSE;
        Serial.println("LFO 2 : Reverse Sawtooth");
        break;
      case WAVEFORM_SAWTOOTH_REVERSE:
        current_lfo2 = WAVEFORM_SQUARE;
        Serial.println("LFO 2 : Square");
        break;
      case WAVEFORM_SQUARE:
        current_lfo2 = WAVEFORM_TRIANGLE;
        Serial.println("LFO 2 : Triangle");
        break;
      case WAVEFORM_TRIANGLE:
        current_lfo2 = WAVEFORM_TRIANGLE_VARIABLE;
        Serial.println("LFO 2 : Variable Triangle");
        break;
      case WAVEFORM_TRIANGLE_VARIABLE:
        current_lfo2 = WAVEFORM_ARBITRARY;
        Serial.println("LFO 2 : Arbitary Waveform");
        break;
      case WAVEFORM_ARBITRARY:
        current_lfo2 = WAVEFORM_PULSE;
        Serial.println("LFO 2 : Pulse");
        break;
      case WAVEFORM_PULSE:
        current_lfo2 = WAVEFORM_SAMPLE_HOLD;
        Serial.println("LFO 2 : Sample & Hold");
        break;
      case WAVEFORM_SAMPLE_HOLD:
        current_lfo2 = WAVEFORM_SINE;
        Serial.println("LFO 2 : Sine");
        break;
      }
    }
   lfo2.begin(current_lfo2);

//Filter / LFO 3 Controls

 /*
  float lfo3_vol = analogRead(A20) / 1023.0 / 2; 
  lfo3.amplitude(lfo3_vol );   
*/

  //Filter frequency control - LFO3
  float lfo3_fq =  analogRead(A20) / 1023.0 * 10.0; 
  lfo3.frequency(lfo3_fq); 

  //Filter cutoff frequency control
  float flt_fq =  analogRead(A18) / 1023.0 * 3000.0; 
  filter1.frequency(flt_fq); 

  //lfo3 waveform select
  button6.update();

   if (button6.fallingEdge()) {
    switch (current_lfo3) {
      case WAVEFORM_TRIANGLE_VARIABLE:
        current_lfo3 = WAVEFORM_PULSE;
        lfo3.amplitude(1);
        Serial.println("LFO 3 : Pulse");
        break;
      case WAVEFORM_SINE:
        current_lfo3 = WAVEFORM_SAWTOOTH;
        lfo3.amplitude(1);
        Serial.println("LFO 3 : Sawtooth");
        break;
      case WAVEFORM_SAWTOOTH:
        current_lfo3 = WAVEFORM_SAWTOOTH_REVERSE;
        lfo3.amplitude(1);
        Serial.println("LFO 3 : Reverse Sawtooth");
        break;
      case WAVEFORM_SAWTOOTH_REVERSE:
        current_lfo3 = WAVEFORM_SQUARE;
        lfo3.amplitude(1);
        Serial.println("LFO 3 : Square");
        break;
      case WAVEFORM_SQUARE:
        current_lfo3 = WAVEFORM_TRIANGLE;
        lfo3.amplitude(1);
        Serial.println("LFO 3 : Triangle");
        break;
      case WAVEFORM_TRIANGLE:
        current_lfo3 = WAVEFORM_TRIANGLE_VARIABLE;
        lfo3.amplitude(0);
        Serial.println("LFO 3 : Filter LFO off");
        break;
      case WAVEFORM_PULSE:
        current_lfo3 = WAVEFORM_SAMPLE_HOLD;
        lfo3.amplitude(1);
        Serial.println("LFO 3 : Sample & Hold");
        break;
      case WAVEFORM_SAMPLE_HOLD:
        current_lfo3 = WAVEFORM_SINE;
        lfo3.amplitude(1);
        Serial.println("LFO 3 : Sine");
        break;
      }
    }
    
   lfo3.begin(current_lfo3);


}
