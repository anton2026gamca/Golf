# Golf

To run the game download the [latest release](https://github.com/anton2026gamca/Golf/releases).

This project was made using my library for raylib, go check it out [here](https://github.com/anton2026gamca/BetterRaylib).

## Golf Level Editor (Raylib required)

To run the level editor you will need the [golf-levelbuilder.exe](https://github.com/anton2026gamca/Golf/blob/main/golf-levelbuilder/golf-levelbuilder.exe) file and the [resources](https://github.com/anton2026gamca/Golf/tree/main/golf-levelbuilder/resources) folder.<br>
The level editor loads/saves the level from/to the [level.dat](https://github.com/anton2026gamca/Golf/blob/main/golf-levelbuilder/level.dat) file.<br>
To load the level in game:<br>
 ### 1. save it
 ![Screenshot 2025-01-05 205346](https://github.com/user-attachments/assets/a2a8f661-b890-4308-a809-a040b3b8f004)
 ### 2. Copy the file hash
 ![Screenshot 2025-01-05 210314](https://github.com/user-attachments/assets/88ef918d-fd5d-4324-a37f-04613ec2688c)
 ### 3. copy it to the resources folder of the game
 ![Screenshot 2025-01-05 205714](https://github.com/user-attachments/assets/3c44a110-5d2c-42b1-bb86-7630e2ead835)
 ### 4. rename it to l[level number].dat (replace "[level number]" with the level number you want it to be)
 ![Screenshot 2025-01-05 205943](https://github.com/user-attachments/assets/92f87fd9-5821-4e6d-bb84-5296c8140842)
 ### 4. edit the levels_hash array in the main.cpp and recompile
   1. 
