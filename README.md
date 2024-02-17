**Link to project journal:**
https://docs.google.com/document/d/e/2PACX-1vQcXveFR99JMTzXYMM4WUpWo1_QDfGM6EWKZiZiR87u00E669bl-GTeatbMJ6cxyqIU5RKKhmYG8dh4/pub

**Link to YouTube Video:** https://youtu.be/F3r-G4Q3Tuc 


**Overview:**
</p>
The goal of this project was to offer users an innovative experience akin to drawing lots, which then creatively composes a poem. I developed an interactive system using NFC tags containing texts of keywords. Users can then draw the tags from a bucket randomly. Through the hardware, the ESP32 board will send the detected keywords to the ChatGPT API to initiate poem generation. The returned result will be displayed through a Python script UI.

**Components:**
</p>
NFC Tags: Used to store predefined keywords related to various themes or emotions.
RFID Reader via SPI connection: Reads the data stored on NFC tags when they are brought into proximity.
ESP32 Board: Serves as the central processing unit, managing data flow between the NFC tags, the AI algorithm, and the output mechanisms.
</p>
ChatGPT API: Processes the keywords extracted from the NFC tags to generate poems based on the given themes.
Google TTS (Text-to-Speech) API: Converts the generated poems into audible speech, enhancing user interaction.
Python-Based UI: Displays the keywords and the generated poems, offering a visual component to accompany the auditory output.
Speaker via I2S connection (from the original object).

![DSC00139](https://github.com/PikaG/PikaG/assets/74200423/5d06c5cc-e064-4114-a9b3-7cfd0f587886)
![DSC00137](https://github.com/PikaG/PikaG/assets/74200423/cad85c3c-26c4-4660-bc21-635abed899ca)
![DSC00131](https://github.com/PikaG/PikaG/assets/74200423/364fa5a2-f3c2-4ade-b2b5-72bdfbd72e20)
