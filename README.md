# Golf

To run the game download it on [itch.io](https://anton2026.itch.io/golf).

This project was made using my library for raylib, go check it out [here](https://github.com/anton2026gamca/BetterRaylib).

## Golf Level Editor (Raylib required)

To run the level editor you will need the [golf-levelbuilder-windows.exe](https://github.com/anton2026gamca/Golf/blob/main/golf-levelbuilder/golf-levelbuilder-windows.exe) (or [golf-levelbuilder-linux](https://github.com/anton2026gamca/Golf/blob/main/golf-levelbuilder/golf-levelbuilder-linux)) file and the [resources](https://github.com/anton2026gamca/Golf/tree/main/golf-levelbuilder/resources) folder.<br>
To load the level in game:<br>
 ### 1. Save it
 ![Screenshot 2025-01-05 205346](https://github.com/user-attachments/assets/a2a8f661-b890-4308-a809-a040b3b8f004)
 ### 2. Copy the file hash
 ![Screenshot 2025-01-05 210314](https://github.com/user-attachments/assets/88ef918d-fd5d-4324-a37f-04613ec2688c)
 ### 3. Copy the `level.dat` file to the resources folder of the game
 ![Screenshot 2025-01-05 214402](https://github.com/user-attachments/assets/965827bf-8d38-4c09-8cf0-801bc0a428db)
 ### 4. Rename it to `l[level number].dat`   (replace "[level number]" with the level number you want it to be)
 ![Screenshot 2025-01-05 205943](https://github.com/user-attachments/assets/92f87fd9-5821-4e6d-bb84-5296c8140842)
 ### 5. Change items in script and recompile
   1. Open the project in visual studio code by double-clicking the `main.code-workspace` file
   ![Screenshot 2025-01-05 211440](https://github.com/user-attachments/assets/0fad3b9b-6373-4526-b2dd-abd6d58aa767)
   2. Add the hash of the level file and update `levels_count` variable
   ![Screenshot 2025-01-05 211241](https://github.com/user-attachments/assets/0f85edbd-1a9d-4fb1-b58f-c626051d5e2d)
   3. Recompile!  Press F5
