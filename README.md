
A MQTT-Controlled Display with 2 Lines

Hardware:
    - Arduino UNO (First Tests)
    - ESP32 DEVKITV1 (Final)
    - 4x Sure Electronix LED Matrix Displays DE-DP14112
    

About the serial interface:

        First Byte is the Command:
        L<num>      Select Line <num>
        S<Text>     Sets the Text directly on the selected Line
        A<Text>     Appends the Text on the selected Line
        I<num>      Prints debug info of Line <num>
        C<num>      Clear Line <nun>

        Possible control-codes in the Text:
        \g      Select color green
        \r      Select color red
        \o      Select color orange

            NOTE:
            When using "S", the string is parsed from right to left! So the codes must then be BEHIND the text and with switched chars.
            Example: "This in REDr\This in Greeng\"



File information:

        MqttLauftext.ino        The main loop
                                Initialises the stuff, then read from serial in.
                                Serial Input will be replaced with MQTT

        TextHandling.ino        The handling of input strings
                                Input-Strings are parsed and with a FIFO the BitBuffer is filled with data. Basically a FIFO-Buffer for chars

        BitBuffer.ino           The bit buffer handling
                                The BitBuffer takes chars from the TextHandling, creates bit masks from the letters and creates the bitdata for the matrix display.
                                It can also autoscroll the lines if necessary (not used at the moment)                        

        HT1632.ino              The functions to control the HT1632 Controlles. Sending commands and data to them and stuff like that.

        ht1632.h                Some defines, mostly for the HT1632 Controllers. But also HW dependend stuff like number of display modules

        letters.h               The Letters for the Font from SPACE to '~'. 
                                The width of each char can  be different and is specified in the first value.
                                The Databytes are the bit values for each LED column
