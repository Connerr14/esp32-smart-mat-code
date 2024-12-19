// Including the lcd library
#include <LiquidCrystal.h>

// Define the FSR pins
const int fsrPin1 = A0;
const int fsrPin2 = A1;

// Define the Button pins
const int powerButton  = 5;
const int leftButton = 2;
const int rightButton = 3;

// Interupt variable declaration (corresponds to the plank challenge)
const byte interruptPin = 2;

// Interupt variable declaration (corresponds to the situp challenge)
const byte secondInterruptPin = 3;

// Variables for counting plank time
unsigned long plankStartTime = 0;
unsigned long plankEndTime = 0;
unsigned long totalPlankTime = 0;

// Count variables (for program flow)
volatile int plankCountVar = 0;
volatile int sitUpsCountVar = 0;

bool power = false;


// Initialize the LCD
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

void setup() 
{
  // Initialize Serial communication
  Serial.begin(9600);

  Serial.println("Setup started");

  // Initialize the LCD's number of columns and rows
  lcd.begin(16, 2);

  // Set the buttons as inputs with internal pull-up resistors
  pinMode(powerButton, INPUT_PULLUP);
  pinMode(leftButton, INPUT_PULLUP);  

  // Set the fsr's as inputs
  pinMode(fsrPin1, INPUT);
  pinMode(fsrPin2, INPUT);

  // Declaring ISR's
  attachInterrupt(digitalPinToInterrupt(interruptPin), challenge1Isr, RISING);
  attachInterrupt(digitalPinToInterrupt(secondInterruptPin),challenge2Isr , RISING);
}

void loop() 
{
  delay(300);

  // Read the state of the button into a local variable:
  int reading = digitalRead(powerButton);

  if (digitalRead(powerButton) == HIGH)
  {
    power = !power;
  }
  
  if (power) 
  {
    // Clear the lcd
    lcd.clear();

    // Reset plank times
    plankStartTime = 0;
    plankEndTime = 0;
    totalPlankTime = 0;

    // Set the cursor to the first line, first character
    lcd.setCursor(0, 0);
    // Corresponds with the left Button 
    lcd.print("btn 1: plank");
    delay(200);
    lcd.setCursor(0, 1);
    // Corresponds with the right button
    lcd.print("btn 2: sit-ups");
    delay(2000);

    // Calling the function that corresponds to the button that was clicked
    if (plankCountVar == 1)
    {
      plankTimeChallenge();
    }
    if (sitUpsCountVar == 1)
    {
      sitUpsChallenge();
    }
  }
  // If power is not on, keep the lcd clear
  else 
  {
    lcd.clear();
    plankCountVar = 0;
    sitUpsCountVar = 0;
  }
}

// Corresponds to a left button click
// Tracks time spent on mat
void plankTimeChallenge ()
{
  // Local variables for pressure sensor input
  int fsrValue1;
  int fsrValue2;

  // Outputting message to user telling them to click the button to start
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Click to start");

  // Wait for user to click button to start the challenge
  while (plankCountVar == 1)
  {
  }

  Serial.println("Plank challenge started");

  // Start the timer
  plankStartTime = millis();

  Serial.print("Plank Start Time: ");
  Serial.println(plankStartTime);

  delay(100);


  // Clear the lcd
  lcd.clear();

  // Delay to wait for user to go onto the mat (subtracted from final time at the end)
  delay(10000);

  // Loop until user leaves the mat
  do 
  {
    // Countinually read the pressure sensor values
    fsrValue1 = analogRead(fsrPin1);
    fsrValue2 = analogRead(fsrPin2);

    Serial.print("FSR1: ");
    Serial.print(fsrValue1);
    Serial.print(", FSR2: ");
    Serial.println(fsrValue2);

    // Output motivational message to the user
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Keep Going");
    delay(1000); // Update every second
  } 
  // When fsrValues drop (user gets off mat), exit the loop and stop counting the time
  while (fsrValue1 > 500 && fsrValue2 > 500);

  Serial.println("Plank challenge ended");

  // Calculate total time
  plankEndTime = millis();

  Serial.print("Plank End Time: ");
  Serial.println(plankEndTime);

  // Calculate the total plank time
  totalPlankTime = (plankEndTime - plankStartTime) / 1000; // Convert to seconds

    // After calculating the total time
  Serial.print("Total Plank Time: ");
  Serial.println(totalPlankTime);
 
  // Subtract the 5 seconds delay only if the total time is greater than or equal to 5 seconds
  if (totalPlankTime >= 5) {
    totalPlankTime -= 10;
  } else {
    totalPlankTime = 0;
  }

  // Display total time
  lcd.clear();
  lcd.setCursor(0, 0); // Start at the first column of the first row
  lcd.print("Time: ");
  lcd.setCursor(0, 1);

  if (totalPlankTime > 0)
  {
  lcd.print(totalPlankTime);
  lcd.print(" sec");
  }
  else
  {
    lcd.print("Try Again!");
  }

  // Wait for user to click the button again
  while (plankCountVar == 2)
  {

  }

    // Reset the count variable for the plank challenge back to 0
  plankCountVar = 0;
}



// Corresponds to a right button click
// Counts how many situps an indivdual does
void sitUpsChallenge ()
{
  // Variable for keeping track of the amonut of reps the user has done
  int reps = 0;

  // Boolean variable for ethics checking
  bool wasOnMat = false;

  // Declaring local variables for pressure sensor input
  int fsrValue1;
  int fsrValue2;

  // Clearing the screen, then telling the user to click the button to start
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Click to start");

  // Wait for user to click button to start the challenge
  while (sitUpsCountVar == 1)
  {
  }

  Serial.println("Sit ups challenge started");

  delay(100);

  // Clear the lcd
  lcd.clear();

  // Stay in loop, tracking reps untill the button is clicked confirming user is done
   do 
  {
    // Read pressure sensors continually
    fsrValue1 = analogRead(fsrPin1);
    fsrValue2 = analogRead(fsrPin2);

    // Bool variable for tracking whether or not the user is on the mat
    bool isOnMat = fsrValue1 > 600 || fsrValue2 > 600;

    // Check if user just got off the mat
    if (wasOnMat && !isOnMat)
    {
      reps++;  // Increment the rep count
      wasOnMat = false;
      Serial.println("Rep completed");
      delay(500); // Short delay to debounce the rep counting
    }

    // Check if user just got on the mat
    if (!wasOnMat && isOnMat) {
      wasOnMat = true;
    }
    
    Serial.print("FSR1: ");
    Serial.print(fsrValue1);
    Serial.print(", FSR2: ");
    Serial.println(fsrValue2);
    delay(1000);

    // Output motivational message to the user doing the challenge
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Keep Going!");
    delay(200);
  } 
  // When user clicks the button again, end the challenge (stop counting reps)
  while (sitUpsCountVar == 2);

  // Output the anount of reps the user did
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Reps: ");
  lcd.setCursor(0, 1);
  lcd.print(reps);
  
  // Wait for button click
  while (sitUpsCountVar == 3)
  {

  }

  // Reset the counter variable
  sitUpsCountVar = 0;
}


// ISR, waits for left button to be clicked, increments varible when triggered
void challenge1Isr () {

  Serial.print("ISR Triggered, plankCountVar: ");
  Serial.println(plankCountVar);

  if (plankCountVar == 0)
  {
    plankCountVar = 1;
    Serial.println(plankCountVar);
  }
  else if (plankCountVar == 1)
  {
    plankCountVar = 2;
    Serial.println(plankCountVar);
  }
  else if (plankCountVar == 2)
  {
    plankCountVar = 3;
    Serial.println(plankCountVar);
  }
}

// ISR, waits for right button to be clicked, increments varible when triggered
void challenge2Isr () 
{
  Serial.print("ISR Triggered, sitUpsCountVar: ");
  Serial.println(sitUpsCountVar);

  if (sitUpsCountVar == 0)
  {
    sitUpsCountVar = 1;
    Serial.println(sitUpsCountVar);
  }
  else if (sitUpsCountVar == 1)
  {
    sitUpsCountVar = 2;
    Serial.println(sitUpsCountVar);
  }
  else if (sitUpsCountVar == 2)
  {
    sitUpsCountVar = 3;
    Serial.println(sitUpsCountVar);
  }
  else if (sitUpsCountVar == 3)
  {
    sitUpsCountVar = 4;
     Serial.println(sitUpsCountVar);
  }
}
