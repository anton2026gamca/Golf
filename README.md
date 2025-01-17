# Golf

To run the game download it or play the web build on [itch.io](https://anton2026.itch.io/golf).

This project was made using my library for raylib, go check it out [here](https://github.com/anton2026gamca/BetterRaylib).
I originally made this project as a present (sercret santa) for my classmate. 

## Golf Level Editor (Raylib required)

To run the level editor you will need the [golf-levelbuilder-windows.exe](https://github.com/anton2026gamca/Golf/blob/main/golf-levelbuilder/golf-levelbuilder-windows.exe) (or [golf-levelbuilder-linux](https://github.com/anton2026gamca/Golf/blob/main/golf-levelbuilder/golf-levelbuilder-linux)) file and the [resources](https://github.com/anton2026gamca/Golf/tree/main/golf-levelbuilder/resources) folder.<br>
Also dowload the [golf](https://github.com/anton2026gamca/Golf/blob/main/golf) folder because you'l need to recompile

### How to make your own levels:<br>
  1. Run the [golf-levelbuilder-windows.exe](https://github.com/anton2026gamca/Golf/blob/main/golf-levelbuilder/golf-levelbuilder-windows.exe) (or [golf-levelbuilder-linux](https://github.com/anton2026gamca/Golf/blob/main/golf-levelbuilder/golf-levelbuilder-linux))
  2. Load the `l.dat` file in the resources folder od the game (To create a new file just create a blank text document, name it `l.dat` and load it)
  ![Screenshot 2025-01-12 210941](https://github.com/user-attachments/assets/d98c7440-b980-4d7e-b93c-322a02af66e7)
  3. Add your levels
  ![Screenshot 2025-01-12 210225](https://github.com/user-attachments/assets/d752de99-9d3b-455d-99fd-34612d7b521a)
  4. Save it
  5. Copy the file hash
  ![Screenshot 2025-01-05 210314](https://github.com/user-attachments/assets/88ef918d-fd5d-4324-a37f-04613ec2688c)
  6. Change items in script and recompile<br>
    1. Open the project in visual studio code by double-clicking the `main.code-workspace` file
    ![Screenshot 2025-01-05 211440](https://github.com/user-attachments/assets/0fad3b9b-6373-4526-b2dd-abd6d58aa767)
    2. Edit the `levels_hash` variable
    ![Screenshot 2025-01-12 210555](https://github.com/user-attachments/assets/b226093f-0ea2-467f-aab7-3354b7cddccb)
    3. Recompile!  Press F5
