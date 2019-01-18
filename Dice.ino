/* Hardware Setup Information:

   The 7 LEDS are pins 2-8. Outputs are normally low.
   LEDS are arranged in this layout and should be connected to these pins:
   2#  5  #3
   4#  #  #6
   7#     #8
   I chose to connect all the parallel cathodes of the LEDS
   to a single 20 ohm resistor which is connected to GND.

   The one input is on pin 9 and should be pulled low when the button is pressed.
   Internal pullup resistors are used so don't worry about that. :)

   Analog pin 0 should be left floating to provide unpredictable seeds for the number generator.

   A schematic can be found at https://easyeda.com/springtrap.bite87/arduino-dice-wiring
*/

//Configurable settings
//System
#define ROLL_BUTTON_PIN 9 //Pull this pin low to roll the dice
#define SERIAL_ENABLE false //Should serial debug output be enabled? Note that it may be possible to cheat if you can predict the next number based on the seed.
#define SERIAL_BAUD 9600
#define STARTUP_FLASH_LENGTH 200 //Length of startup flash in milliseconds. Not sure why you would really want to change this but whatever lol

//Aesthetic
#define NUMBER_OF_DICE 2 //How many dice are rolled
#define RANDOM_FLASH_LENGTH_MIN 48 //Minimum length of random flashing when 'rolling' the die
#define RANDOM_FLASH_LENGTH_MAX 52 //Maximum length of random flashing when 'rolling' the die
#define NUMBER_SHOW_LENGTH 1000 //How long a number should be shown for before the next one is shown in milliseconds

//LED layout configuration (aka which pins do what)
//Don't touch unless redefining LED layout
const int ledMatrix[7][7] = {
  {0, 0, 0, 0, 0, 0, 0}, //0 (clear)
  {0, 0, 0, 1, 0, 0, 0}, //1
  {1, 0, 0, 0, 0, 0, 1}, //2
  {1, 0, 0, 1, 0, 0, 1}, //3
  {1, 0, 1, 0, 1, 0, 1}, //4
  {1, 0, 1, 1, 1, 0, 1}, //5
  {1, 1, 1, 0, 1, 1, 1}  //6
};

void setup() {
  //Initialise serial
  if (SERIAL_ENABLE) Serial.begin(SERIAL_BAUD);

  //Set all leds as outputs
  for (int i = 2; i <= 8; i++) pinMode(i, OUTPUT);
  //Define roll button as input, using internal pullup resistors to prevent floating
  pinMode(ROLL_BUTTON_PIN, INPUT_PULLUP);

  //startup flash
  for (int i = 2; i <= 8; i++) {
    digitalWrite(i, 1);
  }
  delay(STARTUP_FLASH_LENGTH);
  output_number(0); //clear die
  if (SERIAL_ENABLE) Serial.println("Dice sketch v3.");
}

void loop() {
  //Check if the button is pressed. If so, the pin is pulled low and thus a logical 0 (hence the 'not' operator).
  if (!digitalRead(ROLL_BUTTON_PIN)) {
    if (SERIAL_ENABLE) Serial.println("Button pressed!");

    //Set the random humber generator seed to a random value pulled from analog pin 0 (which should be floating)
    float x = analogRead(0);
    if (SERIAL_ENABLE) {
      Serial.print("Pseudo-random number generator seed is ");
      Serial.println(x);
    }
    randomSeed(x); //Set the seed value

    if (SERIAL_ENABLE) Serial.println("Flashing randomly...");
    //Flash leds randomly for aesthetic purposes
    for (int i = 0; i <= random(RANDOM_FLASH_LENGTH_MIN, RANDOM_FLASH_LENGTH_MAX); i++) { //Loop the following code roughly 50 times. Call to random() is done to add more noise
      for (int j = 2; j <= 8; j++) { //For every led...
        int x = random(1, 10); //choose a random number between 1 and 10.
        if (x <= 5) digitalWrite(j, 0); //If that number was smaller than 5, turn that led off.
        else digitalWrite(j, 1); //Otherwise, turn it on.
      }
      delay(20);
    }

    int previous_number = 0; //0 is an impossible value to roll, so a pause cannot accidentally be triggered for no reason.
    //Run the next bit however many die are configured to roll
    for (int i = 1; i <= NUMBER_OF_DICE; i++) {
      //pick a random number then display it
      int number = random(1, 7);
      if (SERIAL_ENABLE) {
        Serial.print("Chose random number ");
        Serial.println(number);
      }

      //If the previous number chosen was the same as the current chosen number, separate the identical numbers with a small pause where nothing is shown.
      if (number == previous_number) {
        if (SERIAL_ENABLE) Serial.println("Pausing because rolled a double.");
        output_number(0); //Blank die
        delay(100);
      }

      output_number(number);
      delay(NUMBER_SHOW_LENGTH); //Wait 1 second to allow the user to read number.
      previous_number = number; //Update the previously chosen number
    }

    output_number(0); //Clear die
  }
}

//Stuck this in a function because I used it multiple times and overall it makes things a bit cleaner.
void output_number(int rollednumber)
{
  for (int i = 2; i <= 8; i++) { //loop over digital pins 2-8
    digitalWrite(i, ledMatrix[rollednumber][i-2]); //2 is subtracted from i to compensate for the offset of 2
  }
}
