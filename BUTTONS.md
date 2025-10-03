# Button API

The button API aims to modularize the functionality of both buttons available on the board while to make them configurable per app

It's built around 4 essentials components:

- Button (struct): This aims to store all the variables requireds to make the button work
- Button_Init(): Initializes the button struct and **starts the button RTOS task**
- Button_Update(): This function is the one in charge of the detection of all the gestures:
  - Short press
  - Long press
  - Double tap
- Button_Handle(): This function must be implemented by the user or the app, and should handle all logic related to the button events. Must the passed to the init function as an argument.
