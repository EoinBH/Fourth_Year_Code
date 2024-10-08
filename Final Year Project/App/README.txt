GUI for generating and analysing graphs. The DynamicEM algorithm developed by Dr. Martin Emms and Dr. Arun Jayapal is used to infer the parameters of their model to detect diachronic word sense emergence.

Note: the application currently only allows mouse, gay, jet, tank, tanks, plane, planes, transcribe, transcribes, transcribed and transcribing as target words.
 
Compilation:
In subdiretory Animation:
-Type "make animation"
In main diretory App:
-Type "qmake" (possibly not necessary)
-Type "make"
-Run executable, (on macOS, this can be done by typing "./App.app/Contents/MacOS/App")

Usage:
When opened, this GUI application displays several buttons and text entry boxes along the top of the window. Their purpose is to allow the user to specify what arguments to pass to the DynamicEM algorithm. The list of parameters that can be specified is not exhaustive, that is to say that not all possible parameters can be specified.

Once all parameters have been specified according to the user's wishes, the DynamicEM algorithm may be run by clicking on the "Run Algorithm" button. In some cases, if illegal arguments are passed, the code inserts relevant default values instead.

For example, the Sense Values entry box expects a string of floats delimited with a "/" character. The number of floats must be equal to the number of senses specified and they must some to 1. If these conditions are not met, the code initialises each sense probability to an equal value.

Assuming the arguments are permissible, the DynamicEM algorithm is started in a new thread. This is advantageous in that the application will not block and may still be used during the machine learning process, which usually takes some time.

The "Generate Animation" button generates animated graphs based on the current target word number of senses and number of iterations. When clicked, the code checks if an animated graph has already been generated for the word and number of senses. If not, it searches for relevant data in the subdirectory Output. If this data is found, it begins the animation generation in a new thread, (again to avoid blocking).

The "Start Animation" button displays an animated graph based on the current target word and number of senses, (provided such an animation exists).

The "Which graphs?" button pops up a window displaying all the animations available so far. The user should be able to use this button to keep track of which graphs they have generated.

The "Display Final Iteration" button freezes the .gif file at its final frame. This static image represents the results of the experiment without any animations.

The "Control Speed of Graphs" slider allows the speed of the animations to be adjusted.