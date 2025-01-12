# Golf

To run the game download it on [itch.io](https://anton2026.itch.io/golf).

This project was made using my library for raylib, go check it out [here](https://github.com/anton2026gamca/BetterRaylib).

## Golf Level Editor (Raylib required)

To run the level editor you will need the [golf-levelbuilder-windows.exe](https://github.com/anton2026gamca/Golf/blob/main/golf-levelbuilder/golf-levelbuilder-windows.exe) (or [golf-levelbuilder-linux](https://github.com/anton2026gamca/Golf/blob/main/golf-levelbuilder/golf-levelbuilder-linux)) file and the [resources](https://github.com/anton2026gamca/Golf/tree/main/golf-levelbuilder/resources) folder.<br>

### How to make your own level:<br>
  #### 1. Run the [golf-levelbuilder-windows.exe](https://github.com/anton2026gamca/Golf/blob/main/golf-levelbuilder/golf-levelbuilder-windows.exe) (or [golf-levelbuilder-linux](https://github.com/anton2026gamca/Golf/blob/main/golf-levelbuilder/golf-levelbuilder-linux))
  #### 2. Add your levels
  ![Screenshot 2025-01-12 210225](https://github.com/user-attachments/assets/d752de99-9d3b-455d-99fd-34612d7b521a)
  #### 3. Save it
  #### 4. Copy the file hash
  ![Screenshot 2025-01-05 210314](https://github.com/user-attachments/assets/88ef918d-fd5d-4324-a37f-04613ec2688c)
  #### 5. Change items in script and recompile
  1. Open the project in visual studio code by double-clicking the `main.code-workspace` file
  ![Screenshot 2025-01-05 211440](https://github.com/user-attachments/assets/0fad3b9b-6373-4526-b2dd-abd6d58aa767)
  2. Add the hash of the level file and update `levels_count` variable

  3. Recompile!  Press F5
