print("Type \"done\" to continue")
levels_paths = []
while True:
    usr_input: str = input(f"Path to level {len(levels_paths) + 1}: ")
    if (usr_input == "done"):
        break
    levels_paths.append(usr_input)
if (len(levels_paths) < 1):
    exit(0)

output_path: str = input("Path to output file: ")
grid_width: int = -1
grid_height: int = -1
grid_size: int = -1
try:
    grid_width = int(input("Grid width: "))
    grid_height = int(input("Grid height: "))
    grid_size = int(input("Grid size: "))
except ValueError:
    print("Please enter an integer!")
    exit(0)

output = open(output_path, "wb")
with open(levels_paths[0], 'rb') as file:
    lvl1data = file.read()

output.write(grid_size.to_bytes())
output.write(grid_width.to_bytes())
output.write(grid_height.to_bytes())
output.write(len(levels_paths).to_bytes())

for level_path in levels_paths:
    print(f"Saving {level_path} ...")
    file = open(level_path, 'rb')
    lvl_data = file.read()
    output.write(lvl_data[:17]) # ball_position, hole_position and shots
    first_data = 20 # skip grid_size, grid_width and grid_height
    last_data = first_data + (grid_width * grid_height)
    output.write(lvl_data[first_data:last_data]) # grid data
    file.close()
output.close()