#include "game_core.h"
#include "Arduino.h"
#include "config.h"
#include "led_board.h"
#include "user_console.h"
#include <avr/sleep.h>

/* current index of generated target sequence */
int seqIndex;

/*  target sequence to reproduce */
int currentTargetSequence[NLEDS];

/* previous input seq length */
int previousInputSeqLen;

/* current max time that can be used to input the sequence */
long max_time_to_form_sequence;

/* current max time to display the sequence */
long max_time_to_display_sequence;

/* difficulty level */
int difficulty_level;

/* difficulty scaling factor */
double difficulty_scaling_factor = 1.5;

/* the score */
long score = 0;

/* current game state */
int game_state;

/* time in which the game entered in the game state */
long entered_state_time;

/* how long it the game is in current state */
long current_time_in_state;

/* =========== procedures about game state ======== */


void change_game_state(int new_state){
  game_state = new_state;
  entered_state_time = millis();
}

void update_game_state_time(){
  current_time_in_state = millis() - entered_state_time;
}

void check_difficulty_level(){
  int new_difficulty_level = read_difficulty_level();
  if (new_difficulty_level != difficulty_level){
    difficulty_level = new_difficulty_level;
    print_on_console(String("New difficulty Level: ") + difficulty_level);  
  } 
}

void game_intro(){
  reset_led_board();   
  print_on_console("Welcome to the Restore the Lights! Game. Press Key B1 to Start");
  reset_pulsing();
  reset_player_input();
  change_game_state(GAME_WAIT_TO_START);
}

void game_wait_to_start(){
  if (current_time_in_state < MAX_IDLE_TIME){
    go_on_pulsing();
    check_difficulty_level();
    if (player_input_started()){
      change_game_state(GAME_INIT);
    }
  } else {
    change_game_state(GAME_SLEEP);
  }
}

void game_sleep(){
  log("Going to sleep..");
  reset_pulsing();
  delay(500);
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);  
  sleep_enable();
  sleep_mode();  
  sleep_disable();   
  change_game_state(GAME_INTRO);
}

void game_init(){
  reset_pulsing();

  difficulty_level = read_difficulty_level();
  difficulty_scaling_factor = 1.0 + difficulty_level*0.1;
  log(String("Scaling F: ") + difficulty_scaling_factor);
  
  score = 0;
  
  print_on_console("Go!");  
  max_time_to_display_sequence = T2_TIME; 
  max_time_to_form_sequence = T3_TIME;

  srandom(1);
  change_game_state(GAME_LOOP_GENERATE_AND_SHOW_SEQUENCE);
}

void reset_target_sequence(){
  seqIndex = 0;
}

int genNewSeqElem(){
  int newElem = random(4);
  bool foundGoodOne = false;
  while (!foundGoodOne){
    bool duplicated = false;
    for (int i = 0; i < seqIndex; i++){
      if (currentTargetSequence[i] == newElem){
        newElem = (newElem + 1) % NLEDS;
        duplicated = true;
        break;
      }
    }
    if (!duplicated){
      foundGoodOne = true;
    }
  } 
  return newElem; 
}

void addNewElemInSequence(int elem){
  currentTargetSequence[seqIndex] = elem;
  seqIndex++;
}

void game_loop_generate_and_show_sequence(){
  turn_all_leds_on();
  delay(T1_TIME);
  
  reset_target_sequence();
  for (int i = 0; i < NLEDS; i++){
    int newSeqElem = genNewSeqElem();
    turn_off_led(newSeqElem);
    addNewElemInSequence(newSeqElem);
    delay(max_time_to_display_sequence/NLEDS);
  }  

  reset_player_input();
  log("Waiting for player input");
  previousInputSeqLen = 0;
  change_game_state(GAME_LOOP_WAITING_PLAYER_SEQUENCE);
}

String getTargetSequenceAsString(){
  String st = "";
  for (int i = 0; i < NLEDS; i++){
    st = st + currentTargetSequence[i] + " ";   
  }
  return st;
}

void change_to_game_over(){
  print_on_console(String("Game Over - Final Score: ") + score);  
  change_game_state(GAME_OVER);
}

void game_loop_waiting_player_sequence(){

  int currentInputSeqLen = getCurrentInputSequenceLength();
  if (currentInputSeqLen > previousInputSeqLen){
    // log(String(currentInputSeqLen));
    for (int i = previousInputSeqLen; i < currentInputSeqLen; i++){
      // log(String("-> on: ") + String(getCurrentInputSequenceElem(i)));
      turn_on_led(getCurrentInputSequenceElem(i));
    }
    previousInputSeqLen = currentInputSeqLen;
  }

  if (current_time_in_state >= max_time_to_form_sequence){
    log("Player input timeout");
    log(String("input sequence: ") + getCurrentInputSequenceAsString());
    log(String("the target sequence was: ") + getTargetSequenceAsString());
  
    /* checking */ 
    bool inputSeqCorrect = true;
    if (getCurrentInputSequenceLength() != NLEDS){
      inputSeqCorrect = false;
    } else {
      for (int i = 0; i < NLEDS; i++){
        if (getCurrentInputSequenceElem(i) != currentTargetSequence[(NLEDS - i) - 1]){
          inputSeqCorrect = false;
          break;
        }
      }
    }
    
    if (!inputSeqCorrect){
      change_to_game_over();
    } else {
      score++;

      max_time_to_display_sequence /= difficulty_scaling_factor; 
      max_time_to_form_sequence /= difficulty_scaling_factor;

      print_on_console(String("New Point! Score ") + score);
      change_game_state(GAME_LOOP_GENERATE_AND_SHOW_SEQUENCE);
    }
  } 
}

void game_over(){
  if (current_time_in_state > TIME_IN_GAME_OVER){
     change_game_state(GAME_INTRO);
  }
}

